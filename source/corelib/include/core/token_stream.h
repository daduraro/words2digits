#ifndef INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b
#define INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b

#include <iosfwd>
#include <vector>
#include <string>
#include <iterator>
#include <cassert>

namespace core {

    /**
     * @brief The type of a token.
     */
    enum class token_category_e {
        space,      //!< Token is formed by whitespace characters.
        alpha,      //!< Token is formed by letters.
        other,      //!< Token is punctuation, control characters, etc.
        end         //!< Sentinel token
    };

    class token_view_t;
    class forward_token_iterator_t;
    class input_token_iterator_t;

    struct eof_token_t {};

    /**
     * @brief This class encapsulates the abstract notion of a token stream from an istream.
     *
     * Given a stream of characters, they are subdivided into chunks called tokens where
     * each character belong to the same category (see token_category_e).
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
    * where each | separates a token, and the categories are alpha (a), space (s), other (o)
    * and end of stream (e).
    *
    * The token stream is lazily constructed from an istream and can be accessed
    * by the forward_token_iterator_t and input_token_iterator_t helpers.
    *
    * @note In order to support forward_token_iterator_t, this class stores the tokens
    *   in a transient storage. Once a token is consumed by incrementing a input_token_iterator_t,
    *   it can no longer be accessed by any iterator.
    */
    class token_stream_t {
        friend class token_view_t;
        friend class forward_token_iterator_t;
        friend class input_token_iterator_t;
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
         * @brief Returns an input iterator to the first valid token of the stream.
         */
        input_token_iterator_t begin() noexcept;

        /**
         * @brief Returns a sentinal iterator to the end of the stream.
         */
        eof_token_t end() const noexcept;

    private:
        /// Access to the normalized text of the stored token `id`.
        std::string& token(std::size_t id) noexcept;

        /// Const access to the normalized text of the stored token `id`.
        const std::string& token(std::size_t id) const noexcept;

        /// Access to the actual text of the stored token `id`.
        std::string& token_raw(std::size_t id) noexcept;

        /// Const access to the actual text of the stored token `id`.
        const std::string& token_raw(std::size_t id) const noexcept;

        /// Access to the category of stored token `id`.
        token_category_e& token_category(std::size_t id) noexcept;

        /// Const access to the category of stored token `id`.
        const token_category_e& token_category(std::size_t id) const noexcept;

        /// Checks whether a token is being stored.
        bool token_in_window(std::size_t) const noexcept;

        /// Returns last token ID that is stored.
        std::size_t last() noexcept;

        /// Consumes a new token from the associated stream and stores it.
        void get_token() noexcept;

        /// Consumes tokens from the associated stream until `id` token has been stored or EOF is reached.
        std::size_t get_token(std::size_t id) noexcept;

        /// Consumes removing all tokens from storage up to `id`.
        std::size_t get_remove_token(std::size_t id) noexcept;

        std::istream* is_;                              //!< Associated text stream.
        std::size_t first_;                             //!< First active token ID.
        std::vector<std::string> tokens_;               //!< Raw text of the active tokens.
        std::vector<std::string> normalized_tokens_;    //!< Normalized text of the active tokens.
        std::vector<token_category_e> token_type_;      //!< Category of the active tokens.
    };

    class token_view_t {
        friend class forward_token_iterator_t;
        friend class input_token_iterator_t;
    public:
        /// The token category.
        token_category_e category() const noexcept { return stream_->token_category(id_); };
        /// True if the token is end of tokens.
        bool is_end() const noexcept { return category() == token_category_e::end; }
        /// True if the token category is space.
        bool is_space() const noexcept { return category() == token_category_e::space; }
        /// True if the token category is alpha.
        bool is_alpha() const noexcept { return category() == token_category_e::alpha; }
        /// True if the token category is other.
        bool is_other() const noexcept { return category() == token_category_e::other; }
        /// The normalized textual representation of the token.
        const std::string& str() const noexcept { return stream_->token(id_); };
        /// The original textual representation of the token.
        const std::string& raw_str() const noexcept { return stream_->token_raw(id_); };
        /// The sequential ID of the token.
        std::size_t id() const noexcept { return id_; }

    private:
        token_view_t(token_stream_t& stream, std::size_t id) noexcept : stream_(&stream), id_(id) { assert(stream.token_in_window(id)); }

        token_stream_t* stream_;
        std::size_t id_;
    };

    /**
     * @brief Forward token iterator.
     *
     * This is (almost) a forward iterator (lacks homogeneous type comparison) that represent
     * a given token of a token_stream_t. Incrementing this iterator may trigger that its
     * associated token_stream_t consumes characters of the istream in order to build new
     * tokens.
     */
    class forward_token_iterator_t : private token_view_t {
        friend class input_token_iterator_t;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const token_view_t;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        /// Dereference iterator.
        reference operator*() const noexcept { return static_cast<reference>(*this); }

        /// Dereference iterator.
        pointer operator->() const noexcept { return static_cast<pointer>(this); }

        /**
         * @brief Pre-increment iterator.
         *
         * @note This may trigger that token_stream_t consumes from its istream.
         */
        forward_token_iterator_t& operator++() noexcept { id_ = stream_->get_token(id_+1); return *this; }

        /**
         * @brief Post-increment iterator.
         *
         * @note This may trigger that token_stream_t consumes from its istream.
         */
        forward_token_iterator_t operator++(int) noexcept { forward_token_iterator_t retval = *this; ++(*this); return retval;}

        /**
         * @brief Increment iterator by a certain amount.
         *
         * @param incr The amount in which the iterator is incremented.
         * @note This may trigger that token_stream_t consumes from its istream.
         */
        forward_token_iterator_t& operator+=(std::size_t incr) noexcept { id_ = stream_->get_token(id_+incr); return *this; }

        /**
         * @brief Returns a copy of an iterator after advancing a certain amount.
         *
         * @param incr The amount in which the iterator is incremented.
         * @note This may trigger that token_stream_t consumes from its istream.
         */
        friend forward_token_iterator_t operator+(forward_token_iterator_t it, std::size_t incr) noexcept { it += incr; return it; }

        /**
         * @brief Returns a copy of an iterator after advancing a certain amount.
         *
         * @param incr The amount in which the iterator is incremented.
         * @note This may trigger that token_stream_t consumes from its istream.
         */
        friend forward_token_iterator_t operator+(std::size_t incr, forward_token_iterator_t it) noexcept { it += incr; return it; }

        /// Heterogeneous comparison, returns whether self represents end of tokens.
        friend bool operator==(const forward_token_iterator_t& self, const eof_token_t&) noexcept { return self.is_end(); }

        /// Heterogeneous comparison, returns whether self represents end of tokens.
        friend bool operator==(const eof_token_t&, const forward_token_iterator_t& self) noexcept { return self.is_end(); }

        /// Heterogeneous comparison, returns whether self does not represent end of tokens.
        friend bool operator!=(const forward_token_iterator_t& self, const eof_token_t&) noexcept { return !self.is_end(); }

        /// Heterogeneous comparison, returns whether self does not represent end of tokens.
        friend bool operator!=(const eof_token_t&, const forward_token_iterator_t& self) noexcept { return !self.is_end(); }

    private:
        forward_token_iterator_t(token_stream_t& stream, std::size_t id) noexcept : token_view_t(stream, id) {};
    };


    /**
     * @brief Input token iterator.
     *
     * This is (almost) an input iterator (lacks homogeneous type comparison) that represent
     * a given token of a token_stream_t. Incrementing this iterator may trigger that its
     * associated token_stream_t consumes characters of the istream in order to build new
     * tokens. Moreover, when incrementing this iterator, all the previous tokens cannot
     * be accessed anymore.
     */
    class input_token_iterator_t : private forward_token_iterator_t {
        friend class token_stream_t;
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = const token_view_t;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        /// Obtains a forward iterator from current iterator.
        forward_token_iterator_t look_ahead() const noexcept { return static_cast<forward_token_iterator_t>(*this); }

        /// Dereference iterator.
        reference operator*() const noexcept { return forward_token_iterator_t::operator*(); }

        /// Dereference iterator.
        pointer operator->() const noexcept { return static_cast<pointer>(this); }

        /**
         * @brief Pre-increment iterator.
         *
         * This may trigger that token_stream_t consumes from its istream.
         * All token_view_t and iterators to a previous token get invalidated.
         */
        input_token_iterator_t& operator++() noexcept { id_ = stream_->get_remove_token(id_+1); return *this; }

        /**
         * @brief Post-increment iterator.
         *
         * @note This may trigger that token_stream_t consumes from its istream.
         * @note All token_view_t and iterators to a previous token get invalidated.
         * @warning The returned iterator is invalid.
         */
        input_token_iterator_t operator++(int) noexcept { input_token_iterator_t retval = *this; ++(*this); return retval;}

        /**
         * @brief Increment iterator by a certain amount.
         *
         * @param incr The amount in which the iterator is incremented.
         * @note This may trigger that token_stream_t consumes from its istream.
         * @note All token_view_t and iterators to a previous token get invalidated.
         */
        input_token_iterator_t& operator+=(std::size_t incr) noexcept { id_ = stream_->get_remove_token(id_+incr); return *this; }

        /// Heterogeneous comparison, returns whether self represents end of tokens.
        friend bool operator==(const input_token_iterator_t& self, const eof_token_t&) noexcept { return self->is_end(); }

        /// Heterogeneous comparison, returns whether self represents end of tokens.
        friend bool operator==(const eof_token_t&, const input_token_iterator_t& self) noexcept { return self->is_end(); }

        /// Heterogeneous comparison, returns whether self represents end of tokens.
        friend bool operator!=(const input_token_iterator_t& self, const eof_token_t&) noexcept { return !self->is_end(); }

        /// Heterogeneous comparison, returns whether self represents end of tokens.
        friend bool operator!=(const eof_token_t&, const input_token_iterator_t& self) noexcept { return !self->is_end(); }

    private:
        input_token_iterator_t(token_stream_t& stream, std::size_t id) noexcept : forward_token_iterator_t(stream, id) {};
    };

}

#endif // INCLUDE_GUARD__TOKEN_STREAM_H__GUID_58400c2d0a5b481c8ecbf531a1ab968b