#ifndef INCLUDE_GUARD__RUN_H__GUID_638a59f61a334447a26f01a95dfc60f0
#define INCLUDE_GUARD__RUN_H__GUID_638a59f61a334447a26f01a95dfc60f0

#include <iosfwd>

/**
 * @brief Executes words2digits main body.
 *
 * @param argc Number of command line arguments.
 * @param argv Command line arguments.
 * @param in Input stream where tokens will be read, unless
 *  an input path is specified by the command line arguments.
 * @param out Stream where messages will be printed in normal execution.
 * @param err Stream where messages will be printed when errors occur.
 * @returns EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int run(int argc, char const* const* argv, std::istream& in, std::ostream& out, std::ostream& err) noexcept;

#endif // INCLUDE_GUARD__RUN_H__GUID_638a59f61a334447a26f01a95dfc60f0