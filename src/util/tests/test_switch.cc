#include <gtest/gtest.h>

#include <util/switch.hh>

TEST(switcher, switcher_string_executes_correct_lambda)
{
    int count = 0;

    util::switcher::string("bravo", {
        {"alpha",   [&count]() { count = 1; }},
        {"bravo",   [&count]() { count = 2; }},
        {"charlie", [&count]() { count = 3; }}
    });

    EXPECT_EQ(count, 2);
}

TEST(switcher, switcher_string_executes_default)
{
    int count = 0;

    util::switcher::string("none", {
        {"alpha",   [&count]() { count = 1; }},
        {"bravo",   [&count]() { count = 2; }},
        {"charlie", [&count]() { count = 3; }}
    }, [&count]() { count = -1; }
    );

    EXPECT_EQ(count, -1);
}