#include <expected/expected.hpp>
#include <gtest/gtest.h>

class UnexpectedTest : public ::testing::Test
{
protected:
    std_::unexpected<int> u_int{10};
    std_::unexpected<std::string> u_str{"error message"};
};

TEST_F(UnexpectedTest, ConstructionAndAccess)
{
    EXPECT_EQ(u_int.error(), 10);
    EXPECT_EQ(u_str.error(), "error message");

    const std_::unexpected<int> cu_int{20};
    EXPECT_EQ(cu_int.error(), 20);
    u_int.error() = 15;
    EXPECT_EQ(u_int.error(), 15);

    std::string s = std::move(u_str).error();
    EXPECT_EQ(s, "error message");

    std_::unexpected<std::string> u_inplace(std_::detail::in_place, 5, 'c');
    EXPECT_EQ(u_inplace.error(), "ccccc");
}

TEST_F(UnexpectedTest, Comparison)
{
    std_::unexpected<int> u1{10};
    std_::unexpected<int> u2{20};
    std_::unexpected<long> u3{10L};

    EXPECT_TRUE(u_int == u1);
    EXPECT_FALSE(u_int == u2);
    EXPECT_TRUE(u_int != u2);
    EXPECT_TRUE(u_int == u3);
}

TEST_F(UnexpectedTest, Swap)
{
    std_::unexpected<int> u1{10};
    std_::unexpected<int> u2{20};

    swap(u1, u2);

    EXPECT_EQ(u1.error(), 20);
    EXPECT_EQ(u2.error(), 10);
}

TEST_F(UnexpectedTest, MakeUnexpected)
{
    auto ue = std_::make_unexpected(42);

    using ExpectedType = std_::unexpected<int>;
    static_assert(std::is_same<decltype(ue), ExpectedType>::value,
                  "Type deduction failed for make_unexpected");
    EXPECT_EQ(ue.error(), 42);

    auto ue_str = std_::make_unexpected("test");

    using ExpectedStrType = std_::unexpected<const char*>;
    static_assert(std::is_same<decltype(ue_str), ExpectedStrType>::value,
                  "Type deduction failed for string literal");
    EXPECT_STREQ(ue_str.error(), "test");
}

TEST(BadExpectedAccessTest, StoresError)
{
    try
    {
        throw std_::bad_expected_access<std::string>("custom error");
    }
    catch (const std_::bad_expected_access<std::string>& e)
    {
        EXPECT_EQ(e.error(), "custom error");
        EXPECT_STREQ(e.what(), "bad expected access");
    }
}
