# configure directories
get_filename_component(ROOT_DIR ".." ABSOLUTE)
set(SOURCE_DIR ${ROOT_DIR}/source)
set(EXTERN_DIR ${ROOT_DIR}/extern)

set(CMAKE_CXX_STANDARD 11)

# macro to add gtest
set(TEST_SOURCES "")
macro(package_add_test)
    set(TEST_SOURCES ${TEST_SOURCES} ${ARGN}) # this is just so that reading the variable works
    set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE) # this is the actual TEST_SOURCES, only if just 1 scope deeper (function/subdirectories scope)
endmacro()

# macro to add docs
set(DOC_SOURCES "")
macro(package_add_doc)
    set(DOC_SOURCES ${DOC_SOURCES} ${ARGN}) # this is just so that reading the variable works
    set(DOC_SOURCES ${DOC_SOURCES} PARENT_SCOPE) # this is the actual TEST_SOURCES, only if just 1 scope deeper (function/subdirectories scope)
endmacro()

option(W2D_TESTS "Build the tests" OFF)
option(W2D_COVERAGE "For GCC target, compile tests with gcov" OFF)
option(W2D_BUILD_DOC "Build the docs" ON)

if (CMAKE_COMPILER_IS_GNUCXX AND W2D_COVERAGE)
    # TODO assert that CMAKE_BUILD_TYPE is in Debug
    add_compile_options(-DNDEBUG -fprofile-arcs -ftest-coverage -fPIC)
    add_link_options(-lgcov --coverage)
endif()

add_subdirectory("${EXTERN_DIR}/abseil" "extern/abseil-cpp" EXCLUDE_FROM_ALL)
add_subdirectory(corelib)
add_subdirectory(cli)

# add unittest target
if (W2D_TESTS)
#    enable_testing()
   add_subdirectory(tests)
endif()

# add doxygen target
if (W2D_BUILD_DOC)
    find_package(Doxygen)
    set(DOXYGEN_EXCLUDE_PATTERNS */test/*)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    if (DOXYGEN_FOUND)
        doxygen_add_docs(doxygen ${ROOT_DIR}/README.md ${DOC_SOURCES})
    else()
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif()
endif()
