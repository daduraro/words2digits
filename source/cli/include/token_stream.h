#ifndef INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b
#define INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <iomanip>
#include <locale>
#include <algorithm>
#include <iterator>

class token_sequence_t;

class token_stream_t {
private:
    enum token_type_e {
        space,
        alpha,
        other
    };

public:
    token_stream_t(std::istream& is) noexcept : is_(&is) {
        (*is_) >> std::noskipws;
    }

    explicit operator bool() const noexcept {
        return !tokens_.empty() || is_->good();
    }

    bool ignore(std::size_t n) noexcept {
        n = std::min<std::size_t>(tokens_.size(), n+1);
        bool newline = false;
        for (auto i=0u; i < n; ++i) {
            if (token_type_[i] == space) {
                if (std::find(tokens_[i].begin(), tokens_[i].end(), '\n') != tokens_[i].end()) {
                    newline = true;
                    break;
                }
            }
        }

        tokens_.erase(tokens_.begin(), std::next(tokens_.begin(), n));
        normalized_tokens_.erase(normalized_tokens_.begin(), std::next(normalized_tokens_.begin(), n));
        token_type_.erase(token_type_.begin(), std::next(token_type_.begin(), n));

        return newline;
    }

    void write_token(std::ostream& os) noexcept {
        assert(!tokens_.empty());

        // dump all tokens that are not alpha
        std::size_t n = 1;
        for (; n < tokens_.size(); ++n)
        {
            if (token_type_[n] == alpha) break;
        }

        for (std::size_t i = 0; i < n; ++i) os.write(tokens_[i].c_str(), tokens_[i].size());

        tokens_.erase(tokens_.begin(), std::next(tokens_.begin(), n));
        normalized_tokens_.erase(normalized_tokens_.begin(), std::next(normalized_tokens_.begin(), n));
        token_type_.erase(token_type_.begin(), std::next(token_type_.begin(), n));
    }

    const std::string& token(std::size_t id) noexcept {
        if (id >= tokens_.size()) grow(id);
        if (id < tokens_.size()) {
            return token_type_[id] == alpha ? normalized_tokens_[id] : tokens_[id];
        }
        return empty_;
    }

    std::size_t next_nonspace_token_id(std::size_t id) noexcept {
        ++id;
        for (; id < tokens_.size(); ++id) {
            if (token_type_[id] != space) return id;
        }

        while (grow()) {
            assert(!tokens_.empty());
            if (token_type_.back() != space) {
                return tokens_.size() - 1;
            }
        }

        // sentinel id
        return tokens_.size();
    }

private:
    static token_type_e classify_char(char c, const std::locale& loc) noexcept {
        if (std::isspace(c, loc)) return space;
        if (std::isalpha(c, loc)) return alpha;
        return other;
    }

    bool grow(std::size_t id) noexcept {
        for (std::size_t i = tokens_.size(); i <= id; ++i) {
            if (!grow()) return false;
        }
        return true;
    }

    bool grow() noexcept {
        char c;
        if (!is_->get(c)) return false;
        auto loc = is_->getloc();
        auto t = classify_char(c, loc);

        tokens_.emplace_back();
        normalized_tokens_.emplace_back();
        token_type_.push_back(t);

        auto& token = tokens_.back();
        token.push_back(c);

        // insert next characters until we find one that does not correspond
        while (is_->get(c)) {
            if (t != classify_char(c, loc)) {
                is_->unget();
                break;
            }
            token.push_back(c);
        }

        if (t == alpha) {
            auto& normalized_token = normalized_tokens_.back();
            std::transform(token.begin(), token.end(), std::back_inserter(normalized_token), [&loc](char in){ return std::tolower(in, loc); });
        }

        return true;
    }

    std::istream* is_;
    std::vector<std::string> tokens_;
    std::vector<std::string> normalized_tokens_;
    std::vector<token_type_e> token_type_;
    std::string empty_ = "";
};

class token_sequence_t {
public:
    // TODO once optional is implemented, remove default constructor and operator bool
    token_sequence_t() noexcept : stream_(nullptr), token_id_{} {}
    explicit operator bool() const noexcept {
        return stream_ != nullptr;
    }

    token_sequence_t(token_stream_t& stream) noexcept : stream_(&stream), token_id_(0)
    {
    }

    const std::string& curr() noexcept {
        assert(stream_ != nullptr);
        return stream_->token(token_id_);
    }

    void next_token() noexcept {
        token_id_ = stream_->next_nonspace_token_id(token_id_);
    }

    std::size_t token_id() noexcept {
        return token_id_;
    }

private:
    token_stream_t* stream_;
    std::size_t token_id_;
};


#endif // INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b