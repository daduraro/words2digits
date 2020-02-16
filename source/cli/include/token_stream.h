#ifndef INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b
#define INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b

#include <iosfwd>
#include <cassert>
#include <vector>
#include <string>
#include <iterator>

enum class token_class_e {
    space,
    alpha,
    other,
    replaced,
    end
};

using token_id_t = std::size_t;

class token_sequence_t;


class token_stream_t {
    friend class token_sequence_t;
public:
    token_stream_t(std::istream& is) noexcept;

    bool empty() const noexcept;

    explicit operator bool() const noexcept;

    void replace(token_sequence_t seq, std::string value, bool preserve_newline = true) noexcept;

    void commit(token_id_t id, std::ostream& os) noexcept;

    token_sequence_t new_sequence(token_class_e filter = token_class_e::alpha) noexcept;

private:
    // accessors
    std::string& token(token_id_t id) noexcept;
    const std::string& token(token_id_t id) const noexcept;

    std::string& token_raw(token_id_t id) noexcept;
    const std::string& token_raw(token_id_t id) const noexcept;

    token_class_e& token_class(token_id_t id) noexcept;
    const token_class_e& token_class(token_id_t id) const noexcept;

    bool token_in_window(token_id_t) const noexcept;
    void get_token() noexcept;

    token_id_t next_token(token_id_t id, bool skip_other, bool skip_ws) noexcept;

    std::istream* is_;
    token_id_t first_;
    std::vector<std::string> tokens_;
    std::vector<std::string> normalized_tokens_;
    std::vector<token_class_e> token_type_;
};

class token_id_iterator_t {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = token_id_t;
    using pointer = token_id_t;
    using reference = token_id_t;
    using iterator_category = std::forward_iterator_tag;

    token_id_iterator_t(token_id_t idx) noexcept : idx_(idx) {}
    token_id_t operator*() noexcept { return idx_; }
    token_id_t operator->() noexcept { return idx_; }
    token_id_iterator_t& operator++() noexcept { ++idx_; return *this; }
    token_id_iterator_t operator++(int) noexcept { ++idx_; return { idx_ - 1 }; }
    friend bool operator==(token_id_iterator_t lhs, token_id_iterator_t rhs) noexcept { return lhs.idx_ == rhs.idx_; }
    friend bool operator!=(token_id_iterator_t lhs, token_id_iterator_t rhs) noexcept { return !(lhs == rhs); }
    friend bool operator<(token_id_iterator_t lhs, token_id_iterator_t rhs) noexcept { return lhs.idx_ < rhs.idx_; }

private:
    token_id_t idx_;
};

class token_sequence_t {
    friend class token_stream_t;
public:
    // TODO once optional is implemented, remove default constructor and operator bool
    token_sequence_t() noexcept : stream_(nullptr), start_(), curr_() {};
    explicit operator bool() const noexcept {
        return stream_ != nullptr;
    }

    // default copy/move constructors/assignment as token_sequence_t
    // has value type semantics (it is a lightweight view type)
    token_sequence_t(const token_sequence_t& other) = default;
    token_sequence_t(token_sequence_t&& other) = default;
    token_sequence_t& operator=(const token_sequence_t& other) = default;
    token_sequence_t& operator=(token_sequence_t&& other) = default;

    const std::string& curr() const noexcept {
        assert(*this);
        return stream_->token(curr_);
    }

    void next_token(bool skip_other = false, bool skip_ws = true) noexcept {
        curr_ = stream_->next_token(curr_, skip_other, skip_ws);
    }

    token_id_t first_id() const noexcept {
        return start_;
    }

    token_id_t curr_id() const noexcept {
        return curr_;
    }

    token_id_iterator_t begin() const noexcept {
        return { start_ };
    }

    token_id_iterator_t end() const noexcept {
        return { curr_ + 1 };
    }

private:
    token_sequence_t(token_stream_t& stream, token_id_t start, token_id_t curr) noexcept : stream_(&stream), start_(start), curr_(curr) {};

    token_stream_t* stream_;
    token_id_t start_;
    token_id_t curr_;
};


#endif // INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b