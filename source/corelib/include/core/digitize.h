#ifndef INCLUDE_GUARD__DIGITIZE_H__GUID_2f2d7b62d36544a3bd505f8f5d8a53e2
#define INCLUDE_GUARD__DIGITIZE_H__GUID_2f2d7b62d36544a3bd505f8f5d8a53e2

#include <iosfwd>

namespace core {

    /**
     * @brief Replace each occurrance of a textual number in `is` to digits and output
     *        the modified text to `os`.
     *
     * @param is Input stream that will be consumed.
     * @param os Output stream where resulting text will be written to.
     */
    void convert(std::istream& is, std::ostream& os) noexcept;

}

#endif // INCLUDE_GUARD__DIGITIZE_H__GUID_2f2d7b62d36544a3bd505f8f5d8a53e2