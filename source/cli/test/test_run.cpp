#include "unittest.h"

#include "digitize.h"

#include <sstream>
#include <fstream>
#include <cstdio>
#include <array>

struct test_run : ::testing::Test {};

TEST(test_run, conformance)
{
    auto fname = "test_bsbDRKWsvX";
    auto fname_out = "test_bsbDRKWsvX.out";
    std::remove(fname);
    std::remove(fname_out);

    // the test assumes this file does not exist
    ASSERT_FALSE(std::ifstream{fname}.is_open());
    ASSERT_FALSE(std::ifstream{fname_out}.is_open());

    std::stringstream in;

    // ask for help
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 4>{ "exe", "--help", fname, "asdfr"};
        auto code = run((int)arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 0);
        ASSERT_TRUE(err.str().empty());
        ASSERT_FALSE(out.str().empty());
    }

    // check certain flags are recognized
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 4>{ "exe", "--help", "--force", "--"};
        auto code = run((int)arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 0);
        ASSERT_TRUE(err.str().empty());
        ASSERT_FALSE(out.str().empty());
    }

    // check certain flags are recognized
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 4>{ "exe", "-h", "-f", "--"};
        auto code = run((int)arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 0);
        ASSERT_TRUE(err.str().empty());
        ASSERT_FALSE(out.str().empty());
    }

    // check -- flag
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 4>{ "exe", "-h", "--", "-e"};
        auto code = run((int)arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 0);
        ASSERT_TRUE(err.str().empty());
        ASSERT_FALSE(out.str().empty());
    }

    // trigger too many arguments
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 5>{ "exe", "arg0", "arg1", "arg2", "arg3"};
        auto code = run((int) arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 1);
        ASSERT_FALSE(err.str().empty());
        ASSERT_TRUE(out.str().empty());
    }

    // trigger unrecognized flag
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 4>{ "exe", "-e", "arg1", "arg2" };
        auto code = run((int) arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 1);
        ASSERT_FALSE(err.str().empty());
        ASSERT_TRUE(out.str().empty());
    }

    // trigger file not exists
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 2>{ "exe", fname };
        auto code = run((int) arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 1);
        ASSERT_FALSE(err.str().empty());
        ASSERT_TRUE(out.str().empty());
    }

    // create file
    std::ofstream fobj{ fname };
    ASSERT_TRUE(fobj.is_open());

    // read from empty file
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 2>{ "exe", fname };
        auto code = run((int) arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 0);
        ASSERT_TRUE(err.str().empty());
        ASSERT_EQ(out.str(), "");
    }

    fobj << "random token forty-two";
    fobj.close();

    // read from file
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 2>{ "exe", fname };
        auto code = run((int) arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 0);
        ASSERT_TRUE(err.str().empty());
        ASSERT_EQ(out.str(), "random token 42");
    }

    // fail to overwrite file
    std::ofstream fobj_out{ fname_out };
    ASSERT_TRUE(fobj_out.is_open());
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 3>{ "exe", fname, fname_out };
        auto code = run((int) arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 1);
        ASSERT_FALSE(err.str().empty());
        ASSERT_TRUE(out.str().empty());
    }
    fobj_out.close();

    // overwrite file
    {
        std::stringstream out, err;
        auto arr = std::array<const char*, 4>{ "exe", fname, fname_out, "--force" };
        auto code = run((int) arr.size(), arr.data(), in, out, err);
        ASSERT_EQ(code, 0);
        ASSERT_TRUE(err.str().empty());
        ASSERT_TRUE(out.str().empty());

        std::stringstream file_contents;
        std::ifstream check_out{ fname_out };
        ASSERT_TRUE(check_out.is_open());
        file_contents << check_out.rdbuf();
        check_out.close();
        ASSERT_EQ(file_contents.str(), "random token 42");
    }

    // check in was never modified
    ASSERT_TRUE(in.str().empty());

    // clean-up
    std::remove(fname);
    std::remove(fname_out);
}