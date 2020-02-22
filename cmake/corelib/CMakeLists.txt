set(CORELIB_DIR ${SOURCE_DIR}/corelib)
set(CORELIB_SOURCE_DIR ${CORELIB_DIR}/src)
set(CORELIB_INCLUDE_DIR ${CORELIB_DIR}/include)
set(CORELIB_TEST_DIR ${CORELIB_DIR}/test)

set(CORELIB_HEADERS
    ${CORELIB_INCLUDE_DIR}/digitize.h
    ${CORELIB_INCLUDE_DIR}/grammar.h
    ${CORELIB_INCLUDE_DIR}/token_stream.h
)

set(CORELIB_SOURCES
    ${CORELIB_SOURCE_DIR}/digitize.cpp
    ${CORELIB_SOURCE_DIR}/grammar.cpp
    ${CORELIB_SOURCE_DIR}/token_stream.cpp
)

# tests
package_add_test(${CORELIB_TEST_DIR}/test_token_stream.cpp)
package_add_test(${CORELIB_TEST_DIR}/test_grammar.cpp)

# doc
package_add_doc(${CORELIB_DIR})

add_library(corelib STATIC ${CORELIB_SOURCES})

set_target_properties(corelib PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED 1
)

target_include_directories(corelib
PUBLIC
    $<BUILD_INTERFACE:${CORELIB_INCLUDE_DIR}>
)

target_link_libraries(corelib PUBLIC absl::base)

# add more C++ conformance in MSVC builds
if (MSVC)
    target_compile_options(corelib
    PUBLIC
        "/permissive-"
        "/Zc:rvalueCast"
        "/Zc:wchar_t"
        "/Zc:inline"
        "/Zc:forScope"
        "/Zc:externConstexpr"
        "/Zc:__cplusplus"
    )
endif()
