#include <expected/expected.hpp>
#include <gtest/gtest.h>

#include <string>
#include <vector>


struct ParseError
{
    std::string message;
    int line;

    ParseError(const std::string& msg, int l = 0) : message(msg), line(l) {}
    ParseError(const ParseError& other) = default;
    ParseError(ParseError&& other) = default;
    ParseError& operator=(const ParseError& other) = default;
    ParseError& operator=(ParseError&& other) = default;

    bool operator==(const ParseError& other) const
    {
        return message == other.message && line == other.line;
    }
};

class ExpectedMonadicTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ExpectedMonadicTest, ValueOr_SuccessReturnsValue)
{
    auto result = std_::expected<int, ParseError>(42).value_or(99);

    EXPECT_EQ(result, 42);
}

TEST_F(ExpectedMonadicTest, ValueOr_ErrorReturnsFallback)
{
    std_::expected<int, ParseError> error_exp(std_::unexpected<ParseError>(ParseError{"Invalid"}));
    auto result = error_exp.value_or(99);

    EXPECT_EQ(result, 99);
}

TEST_F(ExpectedMonadicTest, ValueOr_TypeConversion)
{
    auto result = std_::expected<int, ParseError>(42).value_or(3.14);

    static_assert(std::is_same<decltype(result), int>::value,
                  "value_or should preserve expected value type");

    EXPECT_EQ(result, 42);
}

TEST_F(ExpectedMonadicTest, ValueOr_ComplexTypes)
{
    std::vector<int> default_vec{1, 2, 3};
    std::vector<int> success_vec{4, 5, 6};

    std_::expected<std::vector<int>, ParseError> success_exp(success_vec);
    auto result = success_exp.value_or(default_vec);

    EXPECT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 4);
}

TEST_F(ExpectedMonadicTest, ErrorOr_SuccessReturnsFallback)
{
    auto fallback_error = ParseError{"Default error"};
    auto result = std_::expected<int, ParseError>(42).error_or(fallback_error);

    EXPECT_EQ(result.message, "Default error");
}

TEST_F(ExpectedMonadicTest, ErrorOr_ErrorReturnsActualError)
{
    auto fallback_error = ParseError{"Default error"};
    std_::expected<int, ParseError> error_exp(
        std_::unexpected<ParseError>(ParseError{"Actual error"}));
    auto result = error_exp.error_or(fallback_error);

    EXPECT_EQ(result.message, "Actual error");
}

TEST_F(ExpectedMonadicTest, Comparison_ExpectedWithExpected)
{
    std_::expected<int, ParseError> success1(42);
    std_::expected<int, ParseError> success2(42);
    std_::expected<int, ParseError> success3(99);

    std_::expected<int, ParseError> error1(std_::unexpected<ParseError>(ParseError{"error1"}));
    std_::expected<int, ParseError> error2(std_::unexpected<ParseError>(ParseError{"error1"}));
    std_::expected<int, ParseError> error3(std_::unexpected<ParseError>(ParseError{"error2"}));

    EXPECT_TRUE(success1 == success2);
    EXPECT_FALSE(success1 == success3);
    EXPECT_FALSE(success1 != success2);
    EXPECT_TRUE(success1 != success3);

    EXPECT_TRUE(error1 == error2);
    EXPECT_FALSE(error1 == error3);
    EXPECT_FALSE(error1 != error2);
    EXPECT_TRUE(error1 != error3);

    EXPECT_FALSE(success1 == error1);
    EXPECT_TRUE(success1 != error1);
}

TEST_F(ExpectedMonadicTest, Comparison_ExpectedWithValue)
{
    std_::expected<int, ParseError> success(42);
    std_::expected<int, ParseError> error(std_::unexpected<ParseError>(ParseError{"error"}));

    EXPECT_TRUE(success == 42);
    EXPECT_FALSE(success == 99);
    EXPECT_FALSE(success != 42);
    EXPECT_TRUE(success != 99);

    EXPECT_FALSE(error == 42);
    EXPECT_TRUE(error != 42);
}

TEST_F(ExpectedMonadicTest, Comparison_ExpectedWithUnexpected)
{
    std_::expected<int, ParseError> success(42);
    std_::expected<int, ParseError> error(std_::unexpected<ParseError>(ParseError{"test error"}));

    std_::unexpected<ParseError> unexp1(ParseError{"test error"});
    std_::unexpected<ParseError> unexp2(ParseError{"other error"});

    EXPECT_FALSE(success == unexp1);
    EXPECT_TRUE(success != unexp1);

    EXPECT_TRUE(error == unexp1);
    EXPECT_FALSE(error == unexp2);
    EXPECT_FALSE(error != unexp1);
    EXPECT_TRUE(error != unexp2);
}

TEST_F(ExpectedMonadicTest, Assignment_ValueToSuccess)
{
    std_::expected<int, ParseError> exp(42);
    exp = 99;

    EXPECT_TRUE(exp.has_value());
    EXPECT_EQ(*exp, 99);
}

TEST_F(ExpectedMonadicTest, Assignment_ValueToError)
{
    std_::expected<int, ParseError> exp(std_::unexpected<ParseError>(ParseError{"error"}));
    exp = 42;

    EXPECT_TRUE(exp.has_value());
    EXPECT_EQ(*exp, 42);
}

TEST_F(ExpectedMonadicTest, Assignment_UnexpectedToSuccess)
{
    std_::expected<int, ParseError> exp(42);
    exp = std_::unexpected<ParseError>(ParseError{"new error"});

    EXPECT_FALSE(exp.has_value());
    EXPECT_EQ(exp.error().message, "new error");
}

TEST_F(ExpectedMonadicTest, Assignment_UnexpectedToError)
{
    std_::expected<int, ParseError> exp(std_::unexpected<ParseError>(ParseError{"old error"}));
    exp = std_::unexpected<ParseError>(ParseError{"new error"});

    EXPECT_FALSE(exp.has_value());
    EXPECT_EQ(exp.error().message, "new error");
}

TEST_F(ExpectedMonadicTest, Assignment_MoveSemantics)
{
    std_::expected<std::string, ParseError> exp("initial");

    std::string new_value = "moved value";
    exp = std::move(new_value);

    EXPECT_TRUE(exp.has_value());
    EXPECT_EQ(*exp, "moved value");
}

TEST_F(ExpectedMonadicTest, Access_PointerOperator)
{
    std::string test_string = "Hello World";
    std_::expected<std::string, ParseError> exp(test_string);

    EXPECT_EQ(exp->size(), 11u);
    EXPECT_EQ(exp->substr(0, 5), "Hello");
}

TEST_F(ExpectedMonadicTest, Access_DereferenceOperator)
{
    std_::expected<int, ParseError> exp(42);

    EXPECT_EQ(*exp, 42);

    *exp = 99;
    EXPECT_EQ(*exp, 99);
}

TEST_F(ExpectedMonadicTest, Access_ValueMethod)
{
    std_::expected<int, ParseError> success_exp(42);
    EXPECT_EQ(success_exp.value(), 42);

    std_::expected<int, ParseError> error_exp(std_::unexpected<ParseError>(ParseError{"error"}));
    EXPECT_THROW(error_exp.value(), std_::bad_expected_access<ParseError>);
}

TEST_F(ExpectedMonadicTest, Access_ErrorMethod)
{
    std_::expected<int, ParseError> error_exp(
        std_::unexpected<ParseError>(ParseError{"test error", 42}));

    EXPECT_EQ(error_exp.error().message, "test error");
    EXPECT_EQ(error_exp.error().line, 42);
}

TEST_F(ExpectedMonadicTest, Access_BoolConversion)
{
    std_::expected<int, ParseError> success_exp(42);
    std_::expected<int, ParseError> error_exp(std_::unexpected<ParseError>(ParseError{"error"}));

    EXPECT_TRUE(success_exp);
    EXPECT_FALSE(error_exp);

    EXPECT_TRUE(success_exp.has_value());
    EXPECT_FALSE(error_exp.has_value());
}

TEST_F(ExpectedMonadicTest, Swap_BothSuccess)
{
    std_::expected<int, ParseError> exp1(42);
    std_::expected<int, ParseError> exp2(99);

    exp1.swap(exp2);

    EXPECT_TRUE(exp1.has_value());
    EXPECT_TRUE(exp2.has_value());
    EXPECT_EQ(*exp1, 99);
    EXPECT_EQ(*exp2, 42);
}

TEST_F(ExpectedMonadicTest, Swap_BothError)
{
    std_::expected<int, ParseError> exp1(std_::unexpected<ParseError>(ParseError{"error1"}));
    std_::expected<int, ParseError> exp2(std_::unexpected<ParseError>(ParseError{"error2"}));

    exp1.swap(exp2);

    EXPECT_FALSE(exp1.has_value());
    EXPECT_FALSE(exp2.has_value());
    EXPECT_EQ(exp1.error().message, "error2");
    EXPECT_EQ(exp2.error().message, "error1");
}

TEST_F(ExpectedMonadicTest, Swap_SuccessAndError)
{
    std_::expected<int, ParseError> success_exp(42);
    std_::expected<int, ParseError> error_exp(std_::unexpected<ParseError>(ParseError{"error"}));

    success_exp.swap(error_exp);

    EXPECT_FALSE(success_exp.has_value());
    EXPECT_TRUE(error_exp.has_value());
    EXPECT_EQ(success_exp.error().message, "error");
    EXPECT_EQ(*error_exp, 42);
}

TEST_F(ExpectedMonadicTest, EdgeCase_ExceptionInAssignment)
{
    struct ThrowingType
    {
        int value;
        ThrowingType(int v) : value(v) {}
        ThrowingType& operator=(int v)
        {
            if (v == 666)
                throw std::runtime_error("Assignment failed");
            value = v;
            return *this;
        }
        bool operator==(const ThrowingType& other) const
        {
            return value == other.value;
        }
    };

    std_::expected<ThrowingType, ParseError> exp(ThrowingType{42});

    exp = ThrowingType{99};
    EXPECT_TRUE(exp.has_value());
    EXPECT_EQ(exp->value, 99);
}

TEST_F(ExpectedMonadicTest, EdgeCase_ConstExpected)
{
    const std_::expected<int, ParseError> const_success(42);
    const std_::expected<int, ParseError> const_error(
        std_::unexpected<ParseError>(ParseError{"error"}));

    EXPECT_EQ(const_success.value_or(99), 42);
    EXPECT_EQ(const_error.value_or(99), 99);

    EXPECT_EQ(const_success.error_or(ParseError{"default"}).message, "default");
    EXPECT_EQ(const_error.error_or(ParseError{"default"}).message, "error");
}

TEST_F(ExpectedMonadicTest, RealWorld_ConfigurationParsing)
{
    struct Config
    {
        std::string database_url;
        int port;
        bool debug_mode;

        Config(const std::string& url, int p, bool debug)
            : database_url(url), port(p), debug_mode(debug)
        {
        }
    };

    enum class ConfigError
    {
        MissingFile,
        InvalidFormat,
        InvalidPort
    };

    auto parse_config = [](const std::string& content) -> std_::expected<Config, ConfigError>
    {
        if (content.empty())
        {
            return std_::unexpected<ConfigError>(ConfigError::MissingFile);
        }
        if (content == "invalid")
        {
            return std_::unexpected<ConfigError>(ConfigError::InvalidFormat);
        }
        return Config{"postgresql://localhost", 5'432, true};
    };

    auto success_result = parse_config("valid config content");
    EXPECT_TRUE(success_result.has_value());
    EXPECT_EQ(success_result->database_url, "postgresql://localhost");
    EXPECT_EQ(success_result->port, 5'432);

    auto missing_file = parse_config("");
    EXPECT_FALSE(missing_file.has_value());
    EXPECT_EQ(missing_file.error(), ConfigError::MissingFile);

    auto invalid_format = parse_config("invalid");
    EXPECT_FALSE(invalid_format.has_value());
    EXPECT_EQ(invalid_format.error(), ConfigError::InvalidFormat);
}

TEST_F(ExpectedMonadicTest, RealWorld_ChainedOperations)
{
    auto validate_positive = [](int x) -> std_::expected<int, std::string>
    {
        if (x > 0)
            return x;
        return std_::unexpected<std::string>("Number must be positive");
    };

    auto double_value = [](int x) -> std_::expected<int, std::string>
    {
        return x * 2;
    };

    auto to_string = [](int x) -> std_::expected<std::string, std::string>
    {
        return std::to_string(x);
    };

    std_::expected<int, std::string> input1(5);
    auto result1 = validate_positive(*input1);
    if (result1.has_value())
    {
        auto doubled = double_value(*result1);
        if (doubled.has_value())
        {
            auto str_result = to_string(*doubled);
            EXPECT_TRUE(str_result.has_value());
            EXPECT_EQ(*str_result, "10");
        }
    }

    auto invalid_result = validate_positive(-5);
    EXPECT_FALSE(invalid_result.has_value());
    EXPECT_EQ(invalid_result.error(), "Number must be positive");
}
