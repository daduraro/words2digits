#ifndef INCLUDE_GUARD__GRAMMAR_H__GUID_2f67ead557e14b0abbcb5be53288968c
#define INCLUDE_GUARD__GRAMMAR_H__GUID_2f67ead557e14b0abbcb5be53288968c

#include "token_stream.h"

#include "absl/types/optional.h"

#include <cstdint>

namespace core {

    /**
     * @brief A match of the cardinal numbers grammar.
     *
     * A match is composed both by a token sequence (i.e. which tokens produced a match)
     * and the corresponding parsed value.
     */
    struct match_result_t {
        token_sequence_t seq;   //!< Token sequence of the match.
        std::uint64_t num;      //!< Parsed number of the match.
    };

    using match_t = absl::optional<match_result_t>;

    /**
     * @brief Returns if there is a textual number at current token of `seq`.
     *
     * Starting by the current token of the token sequence `seq`, tries
     * to match a textual number.
     * Any token before the current one in `seq` is not analyzed and
     * the final matched sequence will contain them.
     *
     * This function analyzes the following grammar:
     *
     *     CardNum      -> 'zero' | Millions | AValue
     *     Digit        -> 'one' | 'two' | 'three' | 'four' | 'five' | 'six' | 'seven' | 'eight' | 'nine'
     *     Teens        ->  'ten' | 'eleven' | 'twelve'  | 'thirteen' | 'fourteen' | 'fifteen' | 'sixteen' | 'seventeen' | 'eighteen' | 'nineteen'
     *     SecDig       -> 'twenty' | 'thirty' | 'forty' | 'fifty' | 'sixty' | 'seventy' | 'eighty' | 'ninety'
     *     Below100     -> Digit | Teens | SecDig | SecDig '-' Digit
     *     HundredSfx   -> 'hundred' | 'hundred and ' Below100
     *     Hundreds     -> Below100 | Digit ' ' HundredSfx
     *     ThousandSfx  -> 'thousand' | 'thousand ' Hundreds
     *     Thousands    -> Hundreds | Hundreds ' ' ThousandSfx
     *     MillionSfx   -> 'million' | 'million ' Thousands
     *     Millions     -> Thousands | Thousands ' ' MillionSfx
     *     AValue       -> 'a ' HundredSfx | 'a ' ThousandSfx | 'a hundred ' ThousandSfx | 'a ' MillionSfx | 'a hundred ' MillionSfx
     *
     * Where a whitespace represent a space token, see token_sequence_t.
     *
     * @note This function will call seq.next_token(), which have some side-effects
    *        on the referred token_sequence_t (namely, it may extract more tokens).
    *
    * @param seq The token sequence from which the algorithm will try
    *            to match from its current token.
    * @returns nullopt no match occurred, a match_t otherwise.
    */
    match_t match_cardinal_number(token_sequence_t seq) noexcept;

}

#endif // INCLUDE_GUARD__GRAMMAR_H__GUID_2f67ead557e14b0abbcb5be53288968c