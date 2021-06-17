#include <gtest/gtest.h>

#include <util/json.hh>

const json TEST_JSON = R"(
{
    "name": "Test JSON",
    "required": true,
    "life": 40,
    "phi": 1.618
}
)"_json;

TEST(util, validates_returns_correct_values_on_success)
{
    std::string name, option;
    bool required;
    int life;
    double phi;

    bool success = util::json::validate("", TEST_JSON,
        util::json::required { name, "name" },
        util::json::required { required, "required" },
        util::json::required { life, "life" },
        util::json::optionnal { phi, "phi", 3.1415 },
        util::json::optionnal { option, "option", "facultative" }
    );

    EXPECT_TRUE(success);
    EXPECT_EQ(name, "Test JSON");
    EXPECT_EQ(required, true);
    EXPECT_EQ(life, 40);
    EXPECT_EQ(phi, 1.618);
    EXPECT_EQ(option, "facultative");
}

TEST(util, validates_returns_false_on_failure)
{
    std::string name;
    int c;

    bool success = util::json::validate("", TEST_JSON,
        util::json::required(name, "name"),
        util::json::required(c, "c")
    );

    EXPECT_FALSE(success);
}
