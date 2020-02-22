#include "unittest.h"

#include "core/token_stream.h"

#include <string>
#include <sstream>

using namespace core;

struct test_token_stream : ::testing::Test {};

TEST(test_token_stream, conformance) {
    std::stringstream ss(u8"abc \n 32%-_");
    token_stream_t stream{ ss };

    ASSERT_TRUE(stream);

    auto end = stream.end();

    auto it = stream.begin();
    ASSERT_TRUE((it.look_ahead() + 3)->is_end());

    ASSERT_TRUE(it->is_alpha());

    ++it;
    ASSERT_TRUE(stream.begin()->is_space());

    ++it;
    ASSERT_TRUE(stream.begin()->is_other());
    ASSERT_EQ(it->id(), 2);

    ++it;
    ASSERT_TRUE(stream.empty());
    ASSERT_EQ(it->id(), 3);
    ASSERT_TRUE(it->is_end());

    ++it;
    ASSERT_TRUE(it->is_end());
    ASSERT_EQ(it->id(), 3);
}

