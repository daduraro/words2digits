#include "core/digitize.h"

#include <iostream>

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    return corelib::run(argc, argv, std::cin, std::cout, std::cerr);
}