#include "token_stream.h"

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <iomanip>
#include <locale>
#include <algorithm>
#include <iterator>

namespace {
    token_class_e classify_char(char c, const std::locale& loc) noexcept {
        if (std::isspace(c, loc)) return token_class_e::space;
        if (std::isalpha(c, loc)) return token_class_e::alpha;
        return token_class_e::other;
    }
}

token_stream_t::token_stream_t(std::istream& is) noexcept : is_(&is), first_(0) {
    (*is_) >> std::noskipws;
    get_token();
}

bool token_stream_t::empty() const noexcept {
    return token_type_.front() == token_class_e::end;
}

token_stream_t::operator bool() const noexcept {
    return !empty();
}

void token_stream_t::replace(token_sequence_t seq, std::string value, bool preserve_newline) noexcept {
    // TODO probably replace should be replace_and_commit, as after a replace the only valid option
    //      is to commit the changes

    // find whether we should place a newline at the beginning of value
    bool newline = false;
    if (preserve_newline) {
        for (auto id : seq) {
            if (token_class(id) == token_class_e::space) {
                if (std::find(tokens_[id-first_].begin(), tokens_[id-first_].end(), '\n') != tokens_[id-first_].end()) {
                    newline = true;
                    break;
                }
            }
        }
    }
    if (newline) {
        value.insert(value.begin(), '\n');
    }

    // mark all token sequence as replaced, and clear their contents
    for (auto id : seq) {
        token_class(id) = token_class_e::replaced;
        token(id).clear();
        token_raw(id).clear();
    }

    // change first token to value
    token_raw(seq.first_id()) = std::move(value);
}

void token_stream_t::commit(token_sequence_t seq, std::ostream& os) noexcept {
    // check if already committed
    //assert(id >= first_);
    auto id = seq.curr_id();
    if (id < first_) return;

    assert(token_in_window(id));

    // id is inclusive, make it exclusive (sum one) unless it is the end token
    if (token_class(id) != token_class_e::end) {
        ++id;
    }

    // commit all tokens up to id
    for (auto i = first_; i < id; ++i) {
        const auto& token = token_raw(i);
        os.write(token.c_str(), token.size());
    }
    tokens_.erase(tokens_.begin(), std::next(tokens_.begin(), id - first_));
    normalized_tokens_.erase(normalized_tokens_.begin(), std::next(normalized_tokens_.begin(), id - first_));
    token_type_.erase(token_type_.begin(), std::next(token_type_.begin(), id - first_));

    // check whether we should obtain next token
    if (!token_in_window(id)) get_token();

    // next non-commited token is id
    first_ = id;
    assert(token_in_window(first_)); // check class invariant is preserved
}

std::string& token_stream_t::token(std::size_t id) noexcept {
    assert(token_in_window(id));
    return normalized_tokens_[id - first_];
}

const std::string& token_stream_t::token(std::size_t id) const noexcept {
    assert(token_in_window(id));
    return normalized_tokens_[id - first_];
}

std::string& token_stream_t::token_raw(std::size_t id) noexcept {
    assert(token_in_window(id));
    return tokens_[id - first_];
}

const std::string& token_stream_t::token_raw(std::size_t id) const noexcept {
    assert(token_in_window(id));
    return tokens_[id - first_];
}

token_class_e& token_stream_t::token_class(std::size_t id) noexcept {
    assert(token_in_window(id));
    return token_type_[id - first_];
}

const token_class_e& token_stream_t::token_class(std::size_t id) const noexcept {
    assert(id >= first_ && id + first_ < token_type_.size());
    return token_type_[id - first_];
}

token_sequence_t token_stream_t::new_sequence(token_class_e filter) noexcept {
    assert(filter != token_class_e::replaced);
    auto id = first_;
    token_class_e c = token_class(id);
    while (c != token_class_e::end && c != filter) {
        ++id;
        if (!token_in_window(id)) get_token();
        c = token_class(id);
    }

    return { *this, id, id };
}

bool token_stream_t::token_in_window(token_id_t id) const noexcept {
    assert(tokens_.size() == normalized_tokens_.size() && tokens_.size() == token_type_.size());
    return id >= first_ && id - first_ < tokens_.size();
}

void token_stream_t::get_token() noexcept {
    char c;

    // check if already at the end of the token stream
    if (!token_type_.empty() && token_type_.back() == token_class_e::end) {
        return;
    }

    // if we cannot obtain any new character, insert end token
    if (!is_->get(c)) {
        tokens_.emplace_back();
        normalized_tokens_.emplace_back();
        token_type_.push_back(token_class_e::end);
        return;
    }

    // classify the type of token from the type of the character following the input stream locale
    auto loc = is_->getloc();
    auto t = classify_char(c, loc);

    tokens_.emplace_back();
    normalized_tokens_.emplace_back();
    token_type_.push_back(t);

    auto& token = tokens_.back();
    token.push_back(c);

    // complete the token by inserting characters until one is of another class
    while (is_->get(c)) {
        if (t != classify_char(c, loc)) {
            is_->unget();
            break;
        }
        token.push_back(c);
    }

    // normalize token, which means convert to lowercase for alpha tokens, and leave as is for the rest
    if (t == token_class_e::alpha) {
        auto& normalized_token = normalized_tokens_.back();
        std::transform(token.begin(), token.end(), std::back_inserter(normalized_token), [&loc](char in){ return std::tolower(in, loc); });
    }
    else {
        normalized_tokens_.back() = tokens_.back();
    }
}


token_id_t token_stream_t::next_token(token_id_t id, bool skip_other, bool skip_ws) noexcept
{
    assert(token_in_window(id));
    if (token_class(id) == token_class_e::end) return id;
    while (true) {
        ++id;
        if (!token_in_window(id)) get_token();
        assert(token_in_window(id));

        auto c = token_class(id);
        if (c == token_class_e::other && skip_other) continue;
        if (c == token_class_e::space && skip_ws) continue;
        break;
    }
    return id;
}

