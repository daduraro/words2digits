#ifndef INCLUDE_GUARD__GRAMMAR_H__GUID_2f67ead557e14b0abbcb5be53288968c
#define INCLUDE_GUARD__GRAMMAR_H__GUID_2f67ead557e14b0abbcb5be53288968c

#include "token_stream.h"

#include <cstdint>

namespace core {

    /**
     * @brief A match of the cardinal numbers grammar.
     *
     * A match is composed both by the number of tokens in the match and the
     * corresponding parsed value. An empty match is represented by a size of 0.
     */
    struct match_t {
        /// Returns whether the match is not empty.
        operator bool() const noexcept { return size != 0; }

        std::uint64_t size; //!< Size of the match in tokens.
        std::uint64_t num;  //!< Parsed number of the match.
    };

    /**
     * @brief Returns if there is a textual number at current token of `it`.
     *
     * Starting by the current token `it`, tries to match a textual number.
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
     * Where a whitespace represent a space token, see token_category_enum_t.
     *
     * @note This function will increment the iterator, which have some side-effects
    *        on the referred token_sequence_t.
    *
    * @param it The token from which the algorithm will try to match a textual number.
    * @returns An empty match (size=0) if no match occurred, the actual match otherwise.
    */
    match_t match_cardinal_number(forward_token_iterator_t it) noexcept;

}

#endif // INCLUDE_GUARD__GRAMMAR_H__GUID_2f67ead557e14b0abbcb5be53288968c