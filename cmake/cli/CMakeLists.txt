set(CLI_DIR ${SOURCE_DIR}/cli)
set(CLI_SOURCE_DIR ${CLI_DIR}/src)
set(CLI_INCLUDE_DIR ${CLI_DIR}/include)
set(CLI_TEST_DIR ${CLI_DIR}/test)

set(CLI_HEADERS
)

set(CLI_SOURCES
    ${CLI_SOURCE_DIR}/main.cpp
    ${CLI_SOURCE_DIR}/grammar.cpp
    ${CLI_SOURCE_DIR}/token_stream.cpp
)

set(CLI_NAME words2digits)
add_executable(${CLI_NAME} ${CLI_SOURCES})

set_target_properties(${CLI_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED 1
)

target_include_directories(${CLI_NAME}
PRIVATE
    $<BUILD_INTERFACE:${CLI_INCLUDE_DIR}>
)

#target_link_libraries(${CLI_NAME} PRIVATE corelib)

if (MSVC)
    target_compile_options(${CLI_NAME}
    PRIVATE
        "/permissive-"
        "/Zc:rvalueCast"
        "/Zc:wchar_t"
        "/Zc:inline"
        "/Zc:forScope"
        "/Zc:externConstexpr"
        "/Zc:__cplusplus"
    )
endif()