set(CLI_DIR ${SOURCE_DIR}/cli)
set(CLI_SOURCE_DIR ${CLI_DIR}/src)
set(CLI_INCLUDE_DIR ${CLI_DIR}/include)
set(CLI_TEST_DIR ${CLI_DIR}/test)

set(CLI_HEADERS
    ${CLI_INCLUDE_DIR}/args.h
    ${CLI_INCLUDE_DIR}/run.h
)

set(CLI_SOURCES
    ${CLI_SOURCE_DIR}/args.cpp
    ${CLI_SOURCE_DIR}/run.cpp
)

add_library(cli STATIC ${CLI_SOURCES})

set_target_properties(cli PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED 1
)

target_include_directories(cli
PUBLIC
    $<BUILD_INTERFACE:${CLI_INCLUDE_DIR}>
)

target_link_libraries(cli PUBLIC corelib absl::variant absl::strings)

package_add_test(${CLI_TEST_DIR}/test_run.cpp)
package_add_doc(${CLI_DIR})

# the final executable
add_executable(words2digits ${CLI_SOURCE_DIR}/main.cpp)
target_link_libraries(words2digits PRIVATE cli)
set_target_properties(words2digits PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED 1
)
