#include "core/token_stream.h"

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <iomanip>
#include <locale>
#include <algorithm>
#include <iterator>

namespace {
    using namespace core;
    token_category_e classify_char(char c, const std::locale& loc) noexcept {
        if (std::isspace(c, loc)) return token_category_e::space;
        if (std::isalpha(c, loc)) return token_category_e::alpha;
        return token_category_e::other;
    }
}

namespace core {

    token_stream_t::token_stream_t(std::istream& is) noexcept : is_(&is), first_(0) {
        (*is_) >> std::noskipws;
        get_token();
    }

    bool token_stream_t::empty() const noexcept {
        return token_type_.front() == token_category_e::end;
    }

    token_stream_t::operator bool() const noexcept {
        return !empty();
    }

    input_token_iterator_t token_stream_t::begin() noexcept {
        return { *this, first_ };
    }

    eof_token_t token_stream_t::end() const noexcept {
        return {};
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

    token_category_e& token_stream_t::token_category(std::size_t id) noexcept {
        assert(token_in_window(id));
        return token_type_[id - first_];
    }

    const token_category_e& token_stream_t::token_category(std::size_t id) const noexcept {
        assert(id >= first_ && id + first_ < token_type_.size());
        return token_type_[id - first_];
    }

    bool token_stream_t::token_in_window(std::size_t id) const noexcept {
        assert(tokens_.size() == normalized_tokens_.size() && tokens_.size() == token_type_.size());
        return id >= first_ && id - first_ < tokens_.size();
    }

    std::size_t token_stream_t::last() noexcept {
        return first_ + tokens_.size() - 1;
    }

    void token_stream_t::get_token() noexcept {
        char c;

        // check if already at the end of the token stream
        if (!token_type_.empty() && token_type_.back() == token_category_e::end) {
            return;
        }

        // if we cannot obtain any new character, insert end token
        if (!is_->get(c)) {
            tokens_.emplace_back();
            normalized_tokens_.emplace_back();
            token_type_.push_back(token_category_e::end);
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
        if (t == token_category_e::alpha) {
            auto& normalized_token = normalized_tokens_.back();
            std::transform(token.begin(), token.end(), std::back_inserter(normalized_token), [&loc](char in){ return std::tolower(in, loc); });
        }
        else {
            normalized_tokens_.back() = tokens_.back();
        }
    }


    std::size_t token_stream_t::get_token(std::size_t id) noexcept
    {
        assert(id >= first_);
        if (token_in_window(id)) return id;

        auto idx = last();
        while (token_category(idx) != token_category_e::end) {
            get_token();
            ++idx;
            if (idx == id) return id;
        }

        return idx;
    }

    std::size_t token_stream_t::get_remove_token(std::size_t id) noexcept
    {
        assert(id >= first_);

        // get_token until id is in the window
        auto idx = std::min(last(), id);
        while (idx != id && token_category(idx) != token_category_e::end) {
            get_token();
            ++idx;
        }

        // all tokens up-to idx (non-inclusive) must be removed
        tokens_.erase(tokens_.begin(), std::next(tokens_.begin(), idx - first_));
        normalized_tokens_.erase(normalized_tokens_.begin(), std::next(normalized_tokens_.begin(), idx - first_));
        token_type_.erase(token_type_.begin(), std::next(token_type_.begin(), idx - first_));

        first_ = idx;
        assert(token_in_window(first_));
        return idx;
    }
}