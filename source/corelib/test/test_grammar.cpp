#include "unittest.h"

#include "core/token_stream.h"
#include "core/grammar.h"

#include <string>
#include <cstdint>
#include <utility>

using namespace corelib;

using test_arg = std::pair<std::string, std::uint64_t>;

struct test_grammar : ::testing::TestWithParam<test_arg> {};

TEST_P(test_grammar, conformance)
{
    auto param = GetParam();
    std::stringstream ss{ param.first };
    token_stream_t stream{ss};

    auto match = match_cardinal_number(stream.new_sequence());
    ASSERT_TRUE(match);
    ASSERT_EQ(match->num, param.second);

    std::stringstream out;
    stream.commit(match->seq, out);
    ASSERT_TRUE(stream.empty());
}

struct test_fail_grammar : ::testing::TestWithParam<std::string> {};
TEST_P(test_fail_grammar, conformance)
{
    auto param = GetParam();
    std::stringstream ss{ param };
    token_stream_t stream{ss};

    auto match = match_cardinal_number(stream.new_sequence());
    ASSERT_FALSE(match);
}


INSTANTIATE_TEST_SUITE_P(, test_grammar, ::testing::Values(
test_arg{"zero", 0},
test_arg{"one", 1},
test_arg{"eleven", 11},
test_arg{"fifty", 50},
test_arg{"one hundred", 100},
test_arg{"one thousand", 1000},
test_arg{"three hundred and forty-one", 341},
test_arg{"one thousand four hundred and eighty-seven", 1487},
test_arg{"eight thousand six hundred and sixty-eight", 8668},
test_arg{"three hundred and seventy-one thousand two hundred and seventeen", 371217},
test_arg{"two million six hundred and sixty-two thousand five hundred and two", 2662502},
test_arg{"a hundred and one", 101},
test_arg{"a hundred and fifty-nine", 159},
test_arg{"a thousand three hundred and eighty-five", 1385},
test_arg{"a hundred thousand", 100000},
test_arg{"a million three hundred and ninety-two", 1000392},
test_arg{"a MiLlioN    three \n hundred and  ninety-two", 1000392},
test_arg{"a hundred million", 100000000}
));

INSTANTIATE_TEST_SUITE_P(, test_fail_grammar, ::testing::Values(
"thousand",
"hundred",
"fourth",
"",
"asfr",
"for-ty",
"tw,o"
));
