#include "run.h"

#include <iostream>

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    return run(argc, argv, std::cin, std::cout, std::cerr);
}