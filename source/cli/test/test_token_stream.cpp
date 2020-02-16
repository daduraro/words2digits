#include "unittest.h"

#include "token_stream.h"

#include <string>
#include <sstream>

struct test_token_stream : ::testing::Test {};

TEST(test_token_stream, conformance) {
    std::stringstream ss(u8"abc \n 32%-_");

    token_stream_t stream{ ss };
    auto seq = stream.new_sequence();
    ASSERT_EQ(seq.curr(), u8"abc");

    seq.next_token(false, false);
    ASSERT_EQ(seq.curr(), u8" \n ");

    seq.next_token(false, false);
    ASSERT_EQ(seq.curr(), u8"32%-_");

    // end token
    seq.next_token(false, false);
    ASSERT_EQ(seq.curr(), u8"");

    // still end token
    seq.next_token(false, false);
    ASSERT_EQ(seq.curr(), u8"");

    // commit some tokens
    std::stringstream out;
    stream.commit(stream.new_sequence(), out);
    ASSERT_EQ(out.str(), u8"abc");

    stream.commit(stream.new_sequence(), out); // will try to find alpha token, but will fail and reach the end
    ASSERT_EQ(out.str(), u8"abc \n 32%-_");

    // stream has ended
    ASSERT_FALSE(static_cast<bool>(stream));
}

TEST(test_token_stream, new_sequence) {
    std::stringstream ss(u8"abc \n 32%-_");

    token_stream_t stream{ ss };

    ASSERT_EQ(stream.new_sequence().curr(), u8"abc");
    ASSERT_EQ(stream.new_sequence(token_class_e::alpha).curr(), u8"abc");
    ASSERT_EQ(stream.new_sequence(token_class_e::space).curr(), u8" \n ");
    ASSERT_EQ(stream.new_sequence(token_class_e::other).curr(), u8"32%-_");
    ASSERT_EQ(stream.new_sequence(token_class_e::end).curr(), u8"");
}

TEST(test_token_stream, commit) {
    std::stringstream ss(u8"abc \n 32%-_");

    token_stream_t stream{ ss };
    auto seq = stream.new_sequence();

    std::stringstream out;

    stream.commit(seq, out);
    stream.commit(seq, out);

    ASSERT_EQ(out.str(), u8"abc");

    stream.commit(stream.new_sequence(token_class_e::space), out);
    ASSERT_EQ(out.str(), u8"abc \n ");

    stream.commit(stream.new_sequence(token_class_e::end), out);
    ASSERT_EQ(out.str(), u8"abc \n 32%-_");
}

TEST(test_token_stream, replace) {
    std::stringstream ss(u8"abc \n 32%-_");

    token_stream_t stream{ ss };
    auto seq = stream.new_sequence();

    std::stringstream out;
    stream.replace(seq, "xyz", false);
    stream.commit(seq, out);
    ASSERT_EQ(out.str(), u8"xyz");

    seq = stream.new_sequence(token_class_e::space);
    seq.next_token(false, false);
    stream.replace(seq, "second_part");
    stream.commit(seq, out);
    ASSERT_EQ(out.str(), u8"xyz\nsecond_part");

    seq = stream.new_sequence(token_class_e::end);
    stream.replace(seq, "_fin");
    stream.commit(seq, out);
    ASSERT_EQ(out.str(), u8"xyz\nsecond_part_fin");
}