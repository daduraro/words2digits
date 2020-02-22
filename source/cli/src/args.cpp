#include "args.h"

#include "absl/strings/string_view.h"

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <iterator>

namespace {
    /// Prints usage message.
    void print_usage(absl::string_view name, std::ostream& os) noexcept {
        name.remove_prefix(std::distance(std::find_if(name.rbegin(), name.rend(), [](char c){ return c == '/' || c == '\\'; }), name.rend()));
        os <<
            "Usage:\n"
            "  " << name << " [<input-file> [[--force|-f] <output-file>]]\n"
            "  " << name << " [--help | -h]\n";
        os << std::flush;
    }

    /// Prints help message.
    void print_help(absl::string_view name, std::ostream& os) noexcept {
        print_usage(name, os);
        os <<
            "\n"
            "Description:\n"
            "  Replaces every textual number to their corresponding digits.\n\n"
            "  If no <input-file> is supplied, reads from stdin. If no <output-file>\n"
            "  is supplied, writes to stdout. It will not replace the contents of\n"
            "  <output-file> unless '--force' or '-f' is supplied.\n"
            "  Use end of command options argument '--' (double-dash) to specify\n"
            "  <input-file> and <output-file> paths that start with '-' (dash).\n";
        os << std::flush;
    }
}

absl::variant<int, args_t> parse_args(int argc, char const* const* argv, std::ostream& os, std::ostream& err) noexcept
{
    std::vector<absl::string_view> args(std::next(argv), std::next(argv, argc));

    args_t parsed_args;
    auto& overwrite = parsed_args.overwrite;
    auto& infile = parsed_args.infile;
    auto& outfile = parsed_args.outfile;

    bool help = false;
    overwrite = false;
    infile = absl::nullopt;
    outfile = absl::nullopt;

    bool end_optional = false;

    for (auto& arg : args) {
        if (arg[0] != '-' || end_optional) {
            if (outfile) {
                err << "syntax error: too many arguments provided\n";
                print_usage(args[0], err);
                return EXIT_FAILURE;
            }
            if (infile) outfile.emplace(arg);
            else        infile.emplace(arg);
            continue;
        }

        if (arg == "--help" || arg == "-h") {
            help = true;
            continue;
        }

        if (arg == "--force" || arg == "-f") {
            overwrite = true;
            continue;
        }

        if (arg == "--") {
            end_optional = true;
            continue;
        }

        err << "syntax error: unrecognized command option '" << arg << "'\n";
        print_usage(args[0], err);
        return EXIT_FAILURE;
    }

    if (help) {
        print_help(args[0], os);
        return EXIT_SUCCESS;
    }

    return parsed_args;
}