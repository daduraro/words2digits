#include "args.h"
#include "core/digitize.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <exception>
#include <utility>
#include <algorithm>
#include <cassert>
#include <cstdlib>

int run(int argc, char const* const* argv, std::istream& in, std::ostream& out, std::ostream& err) noexcept
{
    // parse arguments
    auto args_variant = parse_args(argc, argv, out, err);
    if (absl::holds_alternative<int>(args_variant))
        return absl::get<int>(args_variant);

    auto& args = absl::get<args_t>(args_variant);

    // open files if appropiate
    std::ifstream ifobj;
    std::ofstream ofobj;

    if (args.infile) {
        ifobj.open(*args.infile);
        if (!ifobj.good()) {
            err << "error: could not access '" << *args.infile << "'" << std::endl;
            return EXIT_FAILURE;
        }
    }

    if (args.outfile) {
        // TODO unfortunately, there is a filesystem race condition here, however, until
        //      C++17 the C11 'x' flag of fopen was not standardized, thus there is
        //      no reliable way to avoid it using only the standard library.
        //      See https://en.cppreference.com/w/cpp/io/c/fopen.
        //      In case this was critical, system-dependent APIs should be used.
        std::ifstream test{ *args.outfile, std::ios::binary };
        if (test.good() && !args.overwrite) {
            err << "error: file '" << *args.outfile << "' already exists, use --force to overwrite it" << std::endl;
            return EXIT_FAILURE;
        }
        ofobj.open(*args.outfile);
        if (!ofobj.good()) {
            err << "error: could not access '" << *args.outfile << "'" << std::endl;
            return EXIT_FAILURE;
        }
    }

    // dispatch appropriately
    if (ifobj.is_open() && ofobj.is_open()) {
        core::convert(ifobj, ofobj);
    }
    else if (ifobj.is_open()) {
        core::convert(ifobj, out);
    }
    else {
        assert(!ofobj.is_open());
        core::convert(in, out);
    }

    return EXIT_SUCCESS;
}
