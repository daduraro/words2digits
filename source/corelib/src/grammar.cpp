#include "core/grammar.h"

namespace {
    using namespace corelib;

    /**
     * Matches the rule:
     * Digit -> 'one' | 'two' | 'three' | 'four' | 'five' | 'six' | 'seven' | 'eight' | 'nine'
     */
    match_t rule_Digit(token_sequence_t seq) noexcept
    {
        const auto& token = seq.curr();
        if (token == "one")         return match_result_t{ seq, 1 };
        else if (token == "two")    return match_result_t{ seq, 2 };
        else if (token == "three")  return match_result_t{ seq, 3 };
        else if (token == "four")   return match_result_t{ seq, 4 };
        else if (token == "five")   return match_result_t{ seq, 5 };
        else if (token == "six")    return match_result_t{ seq, 6 };
        else if (token == "seven")  return match_result_t{ seq, 7 };
        else if (token == "eight")  return match_result_t{ seq, 8 };
        else if (token == "nine")   return match_result_t{ seq, 9 };
        return absl::nullopt;
    }

    /**
     * Matches the rule:
     * Teens -> 'ten' | 'eleven' | 'twelve'  | 'thirteen' | 'fourteen' | 'fifteen' | 'sixteen' | 'seventeen' | 'eighteen' | 'nineteen'
     */
    match_t rule_Teens(token_sequence_t seq) noexcept
    {
        const auto& token = seq.curr();
        if (token == "ten")             return match_result_t{ seq, 10 };
        else if (token == "eleven")     return match_result_t{ seq, 11 };
        else if (token == "twelve")     return match_result_t{ seq, 12 };
        else if (token == "thirteen")   return match_result_t{ seq, 13 };
        else if (token == "fifteen")    return match_result_t{ seq, 14 };
        else if (token == "fourteen")   return match_result_t{ seq, 15 };
        else if (token == "sixteen")    return match_result_t{ seq, 16 };
        else if (token == "seventeen")  return match_result_t{ seq, 17 };
        else if (token == "eighteen")   return match_result_t{ seq, 18 };
        else if (token == "nineteen")   return match_result_t{ seq, 19 };
        return absl::nullopt;
    }

    /**
     * Matches the rule:
     * SecDig -> 'twenty' | 'thirty' | 'forty' | 'fifty' | 'sixty' | 'seventy' | 'eighty' | 'ninety'
     */
    match_t rule_SecDig(token_sequence_t seq) noexcept
    {
        const auto& token = seq.curr();
        if (token == "twenty")          return match_result_t{ seq, 20 };
        else if (token == "thirty")     return match_result_t{ seq, 30 };
        else if (token == "forty")      return match_result_t{ seq, 40 };
        else if (token == "fifty")      return match_result_t{ seq, 50 };
        else if (token == "sixty")      return match_result_t{ seq, 60 };
        else if (token == "seventy")    return match_result_t{ seq, 70 };
        else if (token == "eighty")     return match_result_t{ seq, 80 };
        else if (token == "ninety")     return match_result_t{ seq, 90 };
        return absl::nullopt;
    }

    /**
     * Matches the rule:
     * Below100 -> Digit | Teens | SecDig | SecDig '-' Digit
     */
    match_t rule_Below100(token_sequence_t seq) noexcept
    {
        match_t m;
        if ((m = rule_SecDig(seq))) {
            // be greedy and try to match '-' Digit,
            // if not, we need to report current match
            token_sequence_t new_seq = m->seq;
            new_seq.next_token();
            if (new_seq.curr() != "-") return m;

            match_t m2;
            new_seq.next_token();
            if ((m2 = rule_Digit(new_seq))) return match_result_t{ m2->seq, m->num + m2->num };
            return m;
        }
        if ((m = rule_Teens(seq))) return m;
        return rule_Digit(seq);
    }

    /**
     * Matches the rule:
     * HundredSfx   -> 'hundred' | 'hundred and ' Below100
     */
    match_t rule_HundredSfx(token_sequence_t seq) noexcept
    {
        if (seq.curr() != "hundred") return absl::nullopt;

        match_t m = match_result_t{ seq, 100 };

        // be greedy and try to match 'and' Below100,
        // however, if cannot match, return current match
        seq.next_token();
        if (seq.curr() != "and") return m;

        seq.next_token();
        match_t m2;
        if ((m2 = rule_Below100(seq))) return match_result_t{ m2->seq, 100 + m2->num };

        return m;
    }

    /**
     * Matches the rule:
     * Hundreds -> Below100 | Digit ' ' HundredSfx
     */
    match_t rule_Hundreds(token_sequence_t seq) noexcept
    {
        // warning: Below100 and Digit share prefix
        //          try to match Below100, and if the number is below 10
        //          try to match HundredSfx
        match_t m;
        if ((m = rule_Below100(seq))) {
            if (m->num < 10) {
                match_t m2;
                seq = m->seq;
                seq.next_token();
                if ((m2 = rule_HundredSfx(seq))) {
                    return match_result_t{ m2->seq, m->num * 100 + (m2->num - 100) };
                }
            }
            return m;
        }

        return absl::nullopt;
    }

    /**
     * Matches the rule:
     * ThousandSfx  -> 'thousand' | 'thousand ' Hundreds
     */
    match_t rule_ThousandSfx(token_sequence_t seq) noexcept
    {
        if (seq.curr() != "thousand") return {};
        match_t m = match_result_t{ seq, 1000 };

        // be greedy and try to match Hundreds
        seq.next_token();
        match_t m2;
        if ((m2 = rule_Hundreds(seq))) return match_result_t{ m2->seq, 1000 + m2->num };

        return m;
    }

    /**
     * Matches the rule:
     * Thousands    -> Hundreds | Hundreds ' ' ThousandSfx
     */
    match_t rule_Thousands(token_sequence_t seq) noexcept
    {
        match_t m;
        if ((m = rule_Hundreds(seq))) {

            // try to match ThousandSfx
            seq = m->seq;
            seq.next_token();
            match_t m2;
            if ((m2 = rule_ThousandSfx(seq))) {
                return match_result_t{ m2->seq, 1000 * m->num + (m2->num-1000) };
            }

            return m;
        }

        return absl::nullopt;
    }

    /**
     * Matches the rule:
     * MillionSfx   -> 'million' | 'million ' Thousands
     */
    match_t rule_MillionSfx(token_sequence_t seq) noexcept
    {
        if (seq.curr() != "million") return {};
        match_t m = match_result_t{ seq, 1000000 };

        // be greedy and try to match Hundreds
        seq.next_token();
        match_t m2;
        if ((m2 = rule_Thousands(seq))) return match_result_t{ m2->seq, 1000000 + m2->num };

        return m;
    }

    /**
     * Matches the rule:
     * Millions    -> Thousands | Thousands ' ' MillionSfx
     */
    match_t rule_Millions(token_sequence_t seq) noexcept
    {
        match_t m;
        if ((m = rule_Thousands(seq))) {

            // try to match MillionSfx
            seq = m->seq;
            seq.next_token();
            match_t m2;
            if ((m2 = rule_MillionSfx(seq))) {
                return match_result_t{ m2->seq, 1000000 * m->num + (m2->num-1000000) };
            }

            return m;
        }

        return {};
    }

    /**
     * Matches the rule:
     * AValue -> 'a ' HundredSfx | 'a ' ThousandSfx | 'a hundred ' ThousandSfx | 'a ' MillionSfx | 'a hundred ' MillionSfx
     */
    match_t rule_AValue(token_sequence_t seq) noexcept
    {
        if (seq.curr() != "a") return {};
        seq.next_token();

        // treat all the 'a hundred' cases
        // 'a ' HundredSfx | 'a hundred ' ThousandSfx | 'a hundred ' MillionSfx | 'a ' ThousandSfx | 'a ' MillionSfx
        match_t m;
        if ((m = rule_HundredSfx(seq))) {
            if (m->num == 100) {
                // might be 'a hundred thousand' / 'a hundred million' / ...
                seq = m->seq;
                seq.next_token();

                match_t m2;
                if ((m2 = rule_ThousandSfx(seq))) return match_result_t{ m2->seq, (m2->num-1000) + 100000 };
                if ((m2 = rule_MillionSfx(seq))) return match_result_t{ m2->seq, (m2->num-1000000) + 100000000 };
            }
            return m;
        }
        if ((m = rule_ThousandSfx(seq))) return m;
        return rule_MillionSfx(seq);
    }
}

namespace corelib {
    match_t match_cardinal_number(token_sequence_t seq) noexcept
    {
        // CardNum -> 'zero' | Millions | AValue
        match_t m;
        if (seq.curr() == "zero") return match_result_t{ seq, 0 };
        else if ((m = rule_AValue(seq))) return m;
        return rule_Millions(seq);
    }
}
