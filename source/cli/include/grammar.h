#ifndef INCLUDE_GUARD__GRAMMAR_H__GUID_2f67ead557e14b0abbcb5be53288968c
#define INCLUDE_GUARD__GRAMMAR_H__GUID_2f67ead557e14b0abbcb5be53288968c

#include "token_stream.h"

#include <cstdint>

// TODO implement optional<T> to better encapsulate the semantics of match_t
struct match_t {
    token_sequence_t seq;
    std::uint64_t num;
    explicit operator bool() const noexcept { return static_cast<bool>(seq); }
};

match_t match_cardinal_number(token_sequence_t seq) noexcept;

#endif // INCLUDE_GUARD__GRAMMAR_H__GUID_2f67ead557e14b0abbcb5be53288968c