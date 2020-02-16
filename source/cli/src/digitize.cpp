#include "digitize.h"

#include "grammar.h"
#include "token_stream.h"

#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <exception>
#include <utility>
#include <algorithm>

namespace {

    /// Prints usage/help message.
    void usage(std::string name, std::ostream& os) noexcept {
        auto pos = std::find_if(name.rbegin(), name.rend(), [](char c){ return c == '/' || c == '\\'; });
        name = name.substr(std::distance(pos, name.rend()));
        os << "Usage:\n"
           << "  " << name << " [<input-file> [[--force|-f] <output-file>]]\n"
           << "  " << name << " [--help | -h]\n"
              "\n"
              "Replaces every textual number to their corresponding digits.\n\n"
              "If no <input-file> is supplied, reads from stdin. If no <output-file>\n"
              "is supplied, writes to stdout. It will not replace the contents of\n"
              "<output-file> unless '--force' or '-f' is supplied.\n"
              "Use end of command options argument '--' (double-dash) to specify\n"
              "<input-file> and <output-file> paths that start with '-' (dash)." << std::endl;
    }

    /// Parsed arguments.
    struct args_t {
        bool help;
        bool force;
        // TODO change infile / outfile to optional when implementing it
        std::pair<bool, std::string> infile;
        std::pair<bool, std::string> outfile;
    };

    // TODO change return type to std::optional<args_t> when implementing optional
    /// Parse command arguments to args_t.
    std::pair<bool, args_t> parse_args(int argc, char** argv) noexcept
    {
        std::vector<std::string> args(std::next(argv), std::next(argv, argc));

        args_t parsed_args;
        auto& help = parsed_args.help;
        auto& force = parsed_args.force;
        auto& infile = parsed_args.infile;
        auto& outfile = parsed_args.outfile;

        help = false;
        force = false;
        infile.first = false;
        outfile.first = false;

        bool dd = false;

        for (auto& arg : args) {
            if (arg[0] != '-' || dd) {
                if (outfile.first) {
                    std::cerr << "ERROR: too many arguments provided" << std::endl;
                    return {false, {}};
                }
                if (infile.first) outfile = { true, std::move(arg) };
                else infile = { true, std::move(arg) };
                continue;
            }

            if (arg == "--help" || arg == "-h") {
                help = true;
                continue;
            }

            if (arg == "--force" || arg == "-f") {
                force = true;
                continue;
            }

            if (arg == "--") {
                dd = true;
                continue;
            }

            std::cerr << "ERROR: unrecognized command option '" << arg << "'" << std::endl;
            return { false, {} };
        }

        return { true, std::move(parsed_args) };
    }
}

void convert(std::istream& is, std::ostream& os) noexcept
{
    std::locale loc("en_US.UTF-8");
    is.imbue(loc);
    token_stream_t stream(is);

    while (stream) {
        token_sequence_t seq = stream.new_sequence();
        auto m = match_cardinal_number(seq);
        if (m) {
            stream.replace(m.seq, std::to_string(m.num));
            stream.commit(m.seq, os);
        }
        else {
            stream.commit(seq, os);
        }
    }
}

int run(int argc, char** argv) noexcept
{
    // parse arguments
    auto args = parse_args(argc, argv);
    if (!args.first) return 1;

    // destructure binding
    auto& parsed_args = args.second;
    auto& help = parsed_args.help;
    auto& force = parsed_args.force;
    auto& infile = parsed_args.infile;
    auto& outfile = parsed_args.outfile;

    // show help message if requested
    if (help) {
        usage(argv[0], std::cout);
        return 0;
    }

    // open files if appropiate
    std::ifstream ifobj;
    std::ofstream ofobj;

    if (infile.first) {
        ifobj.open(infile.second, std::ios::binary);
        if (!ifobj.good()) {
            std::cerr << "ERROR could not access '" << infile.second << "'" << std::endl;
            return 1;
        }
    }

    if (outfile.first) {
        // TODO unfortunately, there is a filesystem race condition here, however, until
        //      C++17 the C11 'x' flag of fopen was not standardized, thus there is
        //      no reliable way to avoid it using only the standard library.
        //      See https://en.cppreference.com/w/cpp/io/c/fopen.
        //      In case this was critical, system-dependent APIs should be used.
        std::ifstream test{ outfile.second, std::ios::binary };
        if (test.good() && !force) {
            std::cerr << "ERROR file '" << outfile.second << "' already exists, use --force to overwrite it" << std::endl;
            return 1;
        }
        ofobj.open(outfile.second, std::ios::binary);
        if (!ofobj.good()) {
            std::cerr << "ERROR could not access '" << outfile.second << "'" << std::endl;
            return 1;
        }
    }

    // dispatch appropriately
    if (ifobj.is_open() && ofobj.is_open()) {
        convert(ifobj, ofobj);
    }
    else if (ifobj.is_open()) {
        convert(ifobj, std::cout);
    }
    else {
        std::ios::sync_with_stdio(false);
        assert(!ofobj.is_open());
        convert(std::cin, std::cout);
    }

    return 0;
}

