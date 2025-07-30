#include <expected/expected.hpp>
#include <gtest/gtest.h>

class UnexpectedTest : public ::testing::Test
{
protected:
    std::unexpected<int> u_int{10};
    std::unexpected<std::string> u_str{"error message"};
};

TEST_F(UnexpectedTest, ConstructionAndAccess)
{
    EXPECT_EQ(u_int.error(), 10);
    EXPECT_EQ(u_str.error(), "error message");

    const std::unexpected<int> cu_int{20};
    EXPECT_EQ(cu_int.error(), 20);
    u_int.error() = 15;
    EXPECT_EQ(u_int.error(), 15);

    std::string s = std::move(u_str).error();
    EXPECT_EQ(s, "error message");

    std::unexpected<std::string> u_inplace(std::detail::in_place, 5, 'c');
    EXPECT_EQ(u_inplace.error(), "ccccc");
}

TEST_F(UnexpectedTest, Comparison)
{
    std::unexpected<int> u1{10};
    std::unexpected<int> u2{20};
    std::unexpected<long> u3{10L};

    EXPECT_TRUE(u_int == u1);
    EXPECT_FALSE(u_int == u2);
    EXPECT_TRUE(u_int != u2);
    EXPECT_TRUE(u_int == u3);
}

TEST_F(UnexpectedTest, Swap)
{
    std::unexpected<int> u1{10};
    std::unexpected<int> u2{20};

    swap(u1, u2);

    EXPECT_EQ(u1.error(), 20);
    EXPECT_EQ(u2.error(), 10);
}

TEST_F(UnexpectedTest, MakeUnexpected)
{
    auto ue = std::make_unexpected(42);

    using ExpectedType = std::unexpected<int>;
    static_assert(std::is_same<decltype(ue), ExpectedType>::value,
                  "Type deduction failed for make_unexpected");
    EXPECT_EQ(ue.error(), 42);

    auto ue_str = std::make_unexpected("test");

    using ExpectedStrType = std::unexpected<const char*>;
    static_assert(std::is_same<decltype(ue_str), ExpectedStrType>::value,
                  "Type deduction failed for string literal");
    EXPECT_STREQ(ue_str.error(), "test");
}

TEST(BadExpectedAccessTest, StoresError)
{
    try
    {
        throw std::bad_expected_access<std::string>("custom error");
    }
    catch (const std::bad_expected_access<std::string>& e)
    {
        EXPECT_EQ(e.error(), "custom error");
        EXPECT_STREQ(e.what(), "bad expected access");
    }
}
