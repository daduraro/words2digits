#ifndef INCLUDE_GUARD__ARGS_H__GUID_61e8c7f5f6f142508859d5a7a7bacdb4
#define INCLUDE_GUARD__ARGS_H__GUID_61e8c7f5f6f142508859d5a7a7bacdb4

#include "absl/types/optional.h"
#include "absl/types/variant.h"

#include <iosfwd>
#include <string>

/// Parsed arguments.
struct args_t {
    bool overwrite;                         //!< Whether outfile can be overwritten.
    absl::optional<std::string> infile;     //!< Path to input file.
    absl::optional<std::string> outfile;    //!< Path to output file
};

/**
 * @brief Parses the command line arguments.
 *
 * @param argc Number of command line arguments.
 * @param argv Command line arguments.
 * @param os Output stream where messages will be reported when no error occurred.
 * @param err Output stream where messages wil lbe reported on errors.
 *
 * @returns Integer with the status code EXIT_SUCCESS or EXIT_FAILURE if no further
 *  process have to be done, or the parsed args_t.
 */
absl::variant<int, args_t> parse_args(int argc, char const* const* argv, std::ostream& os, std::ostream& err) noexcept;

#endif // INCLUDE_GUARD__ARGS_H__GUID_61e8c7f5f6f142508859d5a7a7bacdb4