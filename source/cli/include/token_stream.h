#ifndef INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b
#define INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b

#include <iosfwd>
#include <cassert>
#include <vector>
#include <string>
#include <iterator>

/**
 * @brief The type of a token.
 */
enum class token_class_e {
    space,      //!< Token is formed by whitespace characters.
    alpha,      //!< Token is formed by letters.
    other,      //!< Token is punctuation, control characters, etc.
    replaced,   //!< Token was replaced by token_stream_t::replace().
    end         //!< Sentinel token
};

using token_id_t = std::size_t;

class token_sequence_t;

/**
 * @brief This class encapsulates the abstract notion of a token stream from an istream.
 *
 * Given a stream of characters, they are subdivided into chunks called tokens where
 * each character belong to the same category (see token_class_e).
 * To better visualize the notion of tokens, the text
 * "Today is not 25th of December, 99.8% guaranteed.\n" would be divided into the
 * following token stream:
 *
 \verbatim
 ID     0     1 2  3 4   5 6  7  8 9 10 11      12 13 14  15 16        17 18 19
       +-----+-+--+-+---+-+--+--+-+--+-+--------+-+-+-----+-+----------+-+--+-----+
 Text  |Today| |is| |not| |25|th| |of| |December|,| |99.7%| |guaranteed|.|\n|<end>|
       +-----+-+--+-+---+-+--+--+-+--+-+--------+-+-+-----+-+----------+-+--+-----+
 Class |a    |s|a |s|a  |s|o |a |s|a |s|a       |o|s|o    |s|a         |o|s |e    |
       +-----+-+--+-+---+-+--+--+-+--+-+--------+-+-+-----+-+----------+-+--+-----+
 \endverbatim
 *
 *
 * where each | separates a token, and the categories are alpha (a), space (s), other (o)
 * and end of stream (e).
 *
 * The token stream is lazily constructed from an istream and can be accessed
 * by the token_sequence_t helper class.
 * Every token of the token sequence is either active, committed or unfetched.
 * The active tokens form a *contiguous* window in which any previous token is
 * committed, and any token still not extracted from the associated istream is
 * unfetched.
 * Only active tokens can be committed, which will then become no longer active.
 *
 * See also class token_sequence_t.
 *
 * @invariant start_ indicates the first `active` token, and there will always be
 *            at least on active token (which might be the end sentinel token).
 */
class token_stream_t {
    friend class token_sequence_t;
public:
    /**
     * Constructs a token_stream_t from an istream.
     *
     * The lifetime of an object of token_stream_t must be
     * contained within the lifetime of its referred istream.
     * Modifying the istream once a token_stream_t is constructed
     * will produce uncontrolled side-effects on the token stream.
     */
    token_stream_t(std::istream& is) noexcept;

    /**
     * Check whether the token stream is empty, i.e. all tokens
     * up to the end token have been committed.
     *
     * @returns true if token stream is empty, false otherwise.
     */
    bool empty() const noexcept;

    /**
     * Check if the token stream has non-committed tokens.
     *
     * @returns true if token stream is not empty, false otherwise.
     */
    explicit operator bool() const noexcept;

    /**
     * Replaces a sequence of uncomitted tokens by a new value.
     *
     * After replacing a sequence of uncomitted tokens, they cannot be
     * accessed any longer and can only be committed.
     * Tokens IDs will not be affected, thus any non-overlapping sequence
     * will remain unaffected by this operation.
     *
     * @pre No token in sequence `seq` have been comitted.
     * @param seq Sequence of uncomitted tokens to be replaced.
     * @param value Value in which the tokens will be replaced to.
     * @param preserve_newline If true, if any space token contained a newline character,
     *        a newline character will be inserted at the *beginning* of value.
     */
    void replace(token_sequence_t seq, std::string value, bool preserve_newline = true) noexcept;

    /**
     * Commits all the tokens with ID equal or less than the current
     * token of `seq`.
     *
     * Any committed token cannot be accessed or replaced.
     * All committed tokens up to `id` that were not committed before
     * will be written to `os`.
     *
     * @note Committing a sequence whose current token is end token is safe
     *       and will have no effect if no uncomitted tokens exist.
     *
     * @param seq The sequence whose current token ID will be committed (inclusive).
     * @param os All uncommitted tokens up to `id` will be written to `os`.
     */
    void commit(token_sequence_t seq, std::ostream& os) noexcept;

    /**
     * Create a new token sequence.
     *
     * @param filter The token category of the single token of the new token sequence.
     *               It must be either alpha, other, space or end.
     * @returns A token sequence with a single token, which is the first non-committed
     *      token of category `filter`, or the end token if none other exists.
     */
    token_sequence_t new_sequence(token_class_e filter = token_class_e::alpha) noexcept;

private:
    /// Access to the normalized text of the active token `id`.
    std::string& token(token_id_t id) noexcept;

    /// Const access to the normalized text of the active token `id`.
    const std::string& token(token_id_t id) const noexcept;

    /// Access to the actual text of the active token `id`.
    std::string& token_raw(token_id_t id) noexcept;

    /// Const access to the actual text of the active token `id`.
    const std::string& token_raw(token_id_t id) const noexcept;

    /// Access to the category of active token `id`.
    token_class_e& token_class(token_id_t id) noexcept;

    /// Const access to the category of active token `id`.
    const token_class_e& token_class(token_id_t id) const noexcept;

    /// Checks whether a token is `active`.
    bool token_in_window(token_id_t) const noexcept;

    /// Obtains a new token from the associated stream and makes it active.
    void get_token() noexcept;

    /// Obtains
    token_id_t next_token(token_id_t id, bool skip_other, bool skip_ws) noexcept;

    std::istream* is_;  //!< Associated text stream.
    token_id_t first_;  //!< First active token ID.
    std::vector<std::string> tokens_;            //!< Raw text of the active tokens.
    std::vector<std::string> normalized_tokens_; //!< Normalized text of the active tokens.
    std::vector<token_class_e> token_type_;      //!< Category of the active tokens.
};

/**
 * @brief A token sequence represent a contiguous sequence of tokens from a token_stream_t.
 *
 * A token sequence is represented by an initial starting token ID, and the last token ID
 * which also correspond to the current token ID.
 * A token sequence can only grow, and thus the current token ID can only refer
 * to posterior tokens from the current token when the sequence was constructed.
 * Moreover, only the current token can be accessed, making it a class similar
 * to a forward iterator.
 * Finally, a token sequence is only meaningful when it has an associated token sequence
 * and its semantics are those of a view (e.g. string_view, span, ...).
 * A token sequence will get invalidated if any of its referring tokens become committed and,
 * in particular, the current sequence cannot be accessed if it has been committed.
 *
 * @note An invalid token sequence can be constructed, but accessing to any of its member
 *       function, except from operator bool() and the copy/assignment constructors,
 *       is a violation of their preconditions.
 */
class token_sequence_t {
    friend class token_stream_t;
public:
    // TODO once optional is implemented, remove default constructor and operator bool

    /// Constructs an invalid token sequence.
    token_sequence_t() noexcept : stream_(nullptr), start_(), curr_() {};

    /// Checks if the token sequence is valid.
    explicit operator bool() const noexcept {
        return stream_ != nullptr;
    }

    // default copy/move constructors/assignment as token_sequence_t
    // has value type semantics (it is a lightweight view type)
    token_sequence_t(const token_sequence_t& other) = default;
    token_sequence_t(token_sequence_t&& other) = default;
    token_sequence_t& operator=(const token_sequence_t& other) = default;
    token_sequence_t& operator=(token_sequence_t&& other) = default;

    /// Access to the normalized text of the current token.
    const std::string& curr() const noexcept {
        assert(*this);
        return stream_->token(curr_);
    }

    /**
     * @brief Grow sequence and updates current token.
     *
     * The next current token will be the first token of the stream
     * that is either alpha, other (if skip_other is false),
     * space (if skip_ws is false) or the end token.
     * All tokens within current token and next one will be added
     * to the sequence.
     *
     * @pre Current token of sequence is valid (neither committed nor unfetched).
     * @param skip_other If true, the next current token can be of class other.
     * @param skip_ws If true, the next current token can be of class space.
     */
    void next_token(bool skip_other = false, bool skip_ws = true) noexcept {
        curr_ = stream_->next_token(curr_, skip_other, skip_ws);
    }

 private:
    /**
     * @brief Internal private class that implements iterating the IDs of
     *        the tokens inside the token sequence.
     */
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

    /// Private constructor of a token sequence.
    token_sequence_t(token_stream_t& stream, token_id_t start, token_id_t curr) noexcept : stream_(&stream), start_(start), curr_(curr) {};

    /// Returns the first token ID in the sequence.
    token_id_t first_id() const noexcept {
        return start_;
    }

    /// Returns the last and current token ID in the sequence.
    token_id_t curr_id() const noexcept {
        return curr_;
    }

    /// Returns an iterator to the first token ID of the sequence.
    token_id_iterator_t begin() const noexcept {
        return { start_ };
    }

    /// Returns an iterator to the last (non-inclusive) token ID of the sequence.
    token_id_iterator_t end() const noexcept {
        return { curr_ + 1 };
    }

    token_stream_t* stream_;    //!< The associated token stream.
    token_id_t start_;          //!< The ID of the first token of the sequence.
    token_id_t curr_;           //!< The ID of the current (and last) token of the sequence.
};


#endif // INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b