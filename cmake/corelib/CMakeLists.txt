set(CORELIB_DIR ${SOURCE_DIR}/corelib)
set(CORELIB_SOURCE_DIR ${CORELIB_DIR}/src)
set(CORELIB_INCLUDE_DIR ${CORELIB_DIR}/include)
set(CORELIB_TEST_DIR ${CORELIB_DIR}/test)

set(CORELIB_HEADERS
    ${CORELIB_INCLUDE_DIR}/digitize/token_stream.h
)

set(CORELIB_SOURCES
    ${CORELIB_SOURCE_DIR}/token_stream.cpp
)

add_library(corelib STATIC ${CORELIB_SOURCES})

set_target_properties(corelib PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED 1
)

target_include_directories(corelib
PUBLIC
    $<BUILD_INTERFACE:${CORELIB_INCLUDE_DIR}>
)

if (MSVC)
    target_compile_options(corelib
    INTERFACE
        "/permissive-"
        "/Zc:rvalueCast"
        "/Zc:wchar_t"
        "/Zc:inline"
        "/Zc:forScope"
        "/Zc:externConstexpr"
        "/Zc:__cplusplus"
    )
endif()