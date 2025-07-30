#include <expected/expected.hpp>
#include <gtest/gtest.h>

struct MyError
{
    int code;
    bool operator==(const MyError& other) const
    {
        return code == other.code;
    }
};

TEST(TypeTraits, IsUnexpected)
{
    EXPECT_FALSE(std_::detail::is_unexpected<int>::value);
    EXPECT_FALSE(std_::detail::is_unexpected<void>::value);
    EXPECT_FALSE(std_::detail::is_unexpected<MyError>::value);

    EXPECT_TRUE(std_::detail::is_unexpected<std_::unexpected<int>>::value);
    EXPECT_TRUE(std_::detail::is_unexpected<std_::unexpected<MyError>>::value);
    EXPECT_TRUE(std_::detail::is_unexpected<const std_::unexpected<int>>::value);
    EXPECT_TRUE(std_::detail::is_unexpected<std_::unexpected<int>&>::value);
    EXPECT_TRUE(std_::detail::is_unexpected<const std_::unexpected<int>&>::value);
}


TEST(TypeTraits_IsExpected, Basic)
{
    EXPECT_FALSE((std_::detail::is_expected<int>::value));
    EXPECT_FALSE((std_::detail::is_expected<std_::unexpected<int>>::value));

    EXPECT_TRUE((std_::detail::is_expected<std_::expected<int, MyError>>::value));
    EXPECT_TRUE((std_::detail::is_expected<std_::expected<void, MyError>>::value));
    EXPECT_TRUE((std_::detail::is_expected<const std_::expected<int, int>&>::value));
    EXPECT_TRUE((std_::detail::is_expected<std_::expected<int, int>&&>::value));
}

TEST(TypeTraitsTest, RemoveCvref)
{
    bool is_same1 = std::is_same<int, std_::detail::remove_cvref<int>::type>::value;
    EXPECT_TRUE(is_same1);

    bool is_same2 = std::is_same<int, std_::detail::remove_cvref<const int>::type>::value;
    EXPECT_TRUE(is_same2);

    bool is_same3 = std::is_same<int, std_::detail::remove_cvref<int&>::type>::value;
    EXPECT_TRUE(is_same3);

    bool is_same4 = std::is_same<int, std_::detail::remove_cvref<const int&>::type>::value;
    EXPECT_TRUE(is_same4);

    bool is_same5 = std::is_same<int, std_::detail::remove_cvref<int&&>::type>::value;
    EXPECT_TRUE(is_same5);

    using ExpectedType = std_::expected<double, MyError>;
    bool is_same_complex =
        std::is_same<ExpectedType, std_::detail::remove_cvref<const ExpectedType&>::type>::value;
    EXPECT_TRUE(is_same_complex);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
