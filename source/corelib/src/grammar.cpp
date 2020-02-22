#include "core/grammar.h"

namespace {
    using namespace core;

    /**
     * Matches the rule:
     * Digit -> 'one' | 'two' | 'three' | 'four' | 'five' | 'six' | 'seven' | 'eight' | 'nine'
     */
    match_t rule_Digit(forward_token_iterator_t it) noexcept
    {
        const auto& token = it->str();
        if (token == "one")         return { 1, 1 };
        else if (token == "two")    return { 1, 2 };
        else if (token == "three")  return { 1, 3 };
        else if (token == "four")   return { 1, 4 };
        else if (token == "five")   return { 1, 5 };
        else if (token == "six")    return { 1, 6 };
        else if (token == "seven")  return { 1, 7 };
        else if (token == "eight")  return { 1, 8 };
        else if (token == "nine")   return { 1, 9 };
        return {};
    }

    /**
     * Matches the rule:
     * Teens -> 'ten' | 'eleven' | 'twelve'  | 'thirteen' | 'fourteen' | 'fifteen' | 'sixteen' | 'seventeen' | 'eighteen' | 'nineteen'
     */
    match_t rule_Teens(forward_token_iterator_t it) noexcept
    {
        const auto& token = it->str();
        if (token == "ten")             return { 1, 10 };
        else if (token == "eleven")     return { 1, 11 };
        else if (token == "twelve")     return { 1, 12 };
        else if (token == "thirteen")   return { 1, 13 };
        else if (token == "fifteen")    return { 1, 14 };
        else if (token == "fourteen")   return { 1, 15 };
        else if (token == "sixteen")    return { 1, 16 };
        else if (token == "seventeen")  return { 1, 17 };
        else if (token == "eighteen")   return { 1, 18 };
        else if (token == "nineteen")   return { 1, 19 };
        return {};
    }

    /**
     * Matches the rule:
     * SecDig -> 'twenty' | 'thirty' | 'forty' | 'fifty' | 'sixty' | 'seventy' | 'eighty' | 'ninety'
     */
    match_t rule_SecDig(forward_token_iterator_t it) noexcept
    {
        const auto& token = it->str();
        if (token == "twenty")          return { 1, 20 };
        else if (token == "thirty")     return { 1, 30 };
        else if (token == "forty")      return { 1, 40 };
        else if (token == "fifty")      return { 1, 50 };
        else if (token == "sixty")      return { 1, 60 };
        else if (token == "seventy")    return { 1, 70 };
        else if (token == "eighty")     return { 1, 80 };
        else if (token == "ninety")     return { 1, 90 };
        return {};
    }

    /**
     * Matches the rule:
     * Below100 -> Digit | Teens | SecDig | SecDig '-' Digit
     */
    match_t rule_Below100(forward_token_iterator_t start) noexcept
    {
        match_t m;
        if ((m = rule_SecDig(start))) {
            // be greedy and try to match '-' Digit,
            // if not, we need to report current match
            auto it = start + m.size;

            if (it->str() != "-") return m;
            ++it;

            match_t digit;
            if ((digit = rule_Digit(it))) return { m.size + 1 + digit.size, m.num + digit.num };
            return m;
        }
        if ((m = rule_Teens(start))) return m;
        return rule_Digit(start);
    }

    /**
     * Matches the rule:
     * HundredSfx   -> 'hundred' | 'hundred' Space 'and' Space Below100
     */
    match_t rule_HundredSfx(forward_token_iterator_t it) noexcept
    {
        if (it->str() != "hundred") return {};

        match_t m = { 1, 100 };
        ++it;

        // be greedy and try to match Space 'and' Space Below100,
        // however, if cannot match, return current match
        if (!it->is_space()) return m;
        ++it;

        if (it->str() != "and") return m;
        ++it;

        if (!it->is_space()) return m;
        ++it;

        match_t inner_match;
        if ((inner_match = rule_Below100(it))) return { inner_match.size + m.size + 3, 100 + inner_match.num };

        return m;
    }

    /**
     * Matches the rule:
     * Hundreds -> Below100 | Digit Space HundredSfx
     */
    match_t rule_Hundreds(forward_token_iterator_t it) noexcept
    {
        // warning: Below100 and Digit share prefix
        //          try to match Below100, and if the number is below 10
        //          try to match HundredSfx
        match_t m;
        if ((m = rule_Below100(it))) {

            // Check if Digit Space HundedSfx rule is still valid.
            if (m.num < 10) {
                match_t hundredsfx_match;
                it += m.size;

                if (!it->is_space()) return m;
                ++it;

                if ((hundredsfx_match = rule_HundredSfx(it)))
                    return { m.size + 1 + hundredsfx_match.size, m.num * 100 + (hundredsfx_match.num - 100) };
            }
            return m;
        }

        return {};
    }

    /**
     * Matches the rule:
     * ThousandSfx  -> 'thousand' | 'thousand' Space Hundreds
     */
    match_t rule_ThousandSfx(forward_token_iterator_t it) noexcept
    {
        if (it->str() != "thousand") return {};
        match_t m = { 1, 1000 };
        ++it;

        // be greedy and try to match Hundreds
        if (!it->is_space()) return m;
        ++it;

        match_t hundreds_match;
        if ((hundreds_match = rule_Hundreds(it)))
            return { m.size + 1 + hundreds_match.size, 1000 + hundreds_match.num };

        return m;
    }

    /**
     * Matches the rule:
     * Thousands    -> Hundreds | Hundreds Space ThousandSfx
     */
    match_t rule_Thousands(forward_token_iterator_t it) noexcept
    {
        match_t m;
        if ((m = rule_Hundreds(it))) {

            // try to match the rule Hundreds Space ThousandSfx
            it += m.size;

            if (!it->is_space()) return m;
            ++it;

            match_t m2;
            if ((m2 = rule_ThousandSfx(it)))
                return { m.size + 1 + m2.size, 1000 * m.num + (m2.num-1000) };

            return m;
        }

        return {};
    }

    /**
     * Matches the rule:
     * MillionSfx   -> 'million' | 'million' Space Thousands
     */
    match_t rule_MillionSfx(forward_token_iterator_t it) noexcept
    {
        if (it->str() != "million") return {};
        match_t m = { 1, 1000000 };
        ++it;

        // be greedy and try to match Space Thousands
        if (!it->is_space()) return m;
        ++it;

        match_t m2;
        if ((m2 = rule_Thousands(it)))
            return { m.size + 1 + m2.size, 1000000 + m2.num };

        return m;
    }

    /**
     * Matches the rule:
     * Millions    -> Thousands | Thousands Space MillionSfx
     */
    match_t rule_Millions(forward_token_iterator_t it) noexcept
    {
        match_t m;
        if ((m = rule_Thousands(it))) {

            // try to match Space MillionSfx
            it += m.size;

            if (!it->is_space()) return m;
            ++it;

            match_t m2;
            if ((m2 = rule_MillionSfx(it)))
                return { m.size + 1 + m2.size, 1000000 * m.num + (m2.num-1000000) };

            return m;
        }

        return {};
    }

    /**
     * Matches the rule:
     * AValue -> 'a' Space HundredSfx | 'a' Space 'hundred' Space ThousandSfx | 'a' Space 'hundred' Space MillionSfx  | 'a' Space ThousandSfx | 'a' Space MillionSfx
     */
    match_t rule_AValue(forward_token_iterator_t it) noexcept
    {
        // 'a'
        if (it->str() != "a") return {};
        ++it;

        // 'a' Space
        if (!it->is_space()) return {};
        ++it;

        // treat all the 'a' Space 'hundred' cases
        // 'a' Space HundredSfx | 'a' Space 'hundred' Space ThousandSfx | 'a' Space 'hundred' Space MillionSfx
        match_t m;
        if ((m = rule_HundredSfx(it))) {
            it += m.size;
            m.size += 2; // include 'a' Space

            // check that the text is actually 'hundred' alone in order to match
            // 'a' Space 'hundred' Space ThousandSfx | 'a' Space 'hundred' Space MillionSfx
            if (m.num == 100) {

                // 'a' Space 'hundred' Space
                if (!it->is_space()) return m;
                ++it;

                match_t m2;

                // 'a' Space 'hundred' Space ThousandSfx
                if ((m2 = rule_ThousandSfx(it))) return { m.size + 1 + m2.size, (m2.num - 1000) + 100000 };

                // 'a' Space 'hundred' Space MillionSfx
                if ((m2 = rule_MillionSfx(it))) return { m.size + 1 + m2.size, (m2.num - 1000000) + 100000000 };
            }

            // matched rule was 'a' Space HundredSfx
            return m;
        }

        // handle rule 'a' Space ThousandSfx
        if ((m = rule_ThousandSfx(it))) return { m.size + 2, m.num };

        // handle rule 'a' Space MillionSfx
        if ((m = rule_MillionSfx(it))) return { m.size + 2, m.num };

        return {};
    }
}

namespace core {
    match_t match_cardinal_number(forward_token_iterator_t it) noexcept
    {
        // CardNum -> 'zero' | Millions | AValue
        match_t m;
        if (it->str() == "zero") return { 1, 0 };
        else if ((m = rule_AValue(it))) return m;
        return rule_Millions(it);
    }
}
