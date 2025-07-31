#include <expected/expected.hpp>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>


struct NonCopyable
{
    int value;
    explicit NonCopyable(int v) : value(v) {}
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&& other) noexcept : value(other.value)
    {
        other.value = -1;
    }
    NonCopyable& operator=(NonCopyable&& other) noexcept
    {
        if (this != &other)
        {
            value = other.value;
            other.value = -1;
        }
        return *this;
    }
    bool operator==(const NonCopyable& other) const
    {
        return value == other.value;
    }
};

struct ThrowingConstructor
{
    int value;
    ThrowingConstructor(int v) : value(v)
    {
        if (v == 666)
            throw std::runtime_error("Constructor throws");
    }
    bool operator==(const ThrowingConstructor& other) const
    {
        return value == other.value;
    }
};

class UnexpectedTest : public ::testing::Test
{
protected:
    std_::unexpected<int> u_int{10};
    std_::unexpected<std::string> u_str{"error message"};

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(UnexpectedTest, DefaultConstruction_Disabled)
{
    static_assert(!std::is_default_constructible<std_::unexpected<int>>::value,
                  "unexpected should not be default constructible");
    static_assert(!std::is_default_constructible<std_::unexpected<std::string>>::value,
                  "unexpected should not be default constructible");
}

TEST_F(UnexpectedTest, DirectConstruction)
{
    std_::unexpected<int> u1(42);
    EXPECT_EQ(u1.error(), 42);

    std_::unexpected<std::string> u2("test error");
    EXPECT_EQ(u2.error(), "test error");

    std_::unexpected<std::vector<int>> u3(std::vector<int>{1, 2, 3});
    EXPECT_EQ(u3.error().size(), 3u);
    EXPECT_EQ(u3.error()[0], 1);
}

TEST_F(UnexpectedTest, InPlaceConstruction)
{
    std_::unexpected<std::string> u_inplace(std_::detail::in_place, 5u, 'c');
    EXPECT_EQ(u_inplace.error(), "ccccc");

    std_::unexpected<std::vector<int>> u_vec(std_::detail::in_place, 3u, 42);
    EXPECT_EQ(u_vec.error().size(), 3u);
    EXPECT_EQ(u_vec.error()[0], 42);
    EXPECT_EQ(u_vec.error()[1], 42);
    EXPECT_EQ(u_vec.error()[2], 42);
}

TEST_F(UnexpectedTest, CopyConstruction)
{
    std_::unexpected<int> original(42);
    std_::unexpected<int> copy(original);

    EXPECT_EQ(copy.error(), 42);
    EXPECT_EQ(original.error(), 42);

    std_::unexpected<std::string> str_original("test");
    std_::unexpected<std::string> str_copy(str_original);
    EXPECT_EQ(str_copy.error(), "test");
    EXPECT_EQ(str_original.error(), "test");
}

TEST_F(UnexpectedTest, MoveConstruction)
{
    std_::unexpected<std::string> original("movable string");
    std_::unexpected<std::string> moved(std::move(original));

    EXPECT_EQ(moved.error(), "movable string");

    auto ptr = std::make_unique<int>(42);
    int* raw_ptr = ptr.get();
    std_::unexpected<std::unique_ptr<int>> u_ptr(std::move(ptr));
    EXPECT_EQ(u_ptr.error().get(), raw_ptr);
    EXPECT_EQ(*u_ptr.error(), 42);
    EXPECT_EQ(ptr.get(), nullptr);
}

TEST_F(UnexpectedTest, MoveOnlyTypes)
{
    std_::unexpected<NonCopyable> u_move_only(NonCopyable{123});
    EXPECT_EQ(u_move_only.error().value, 123);

    std_::unexpected<NonCopyable> u_moved(std::move(u_move_only));
    EXPECT_EQ(u_moved.error().value, 123);
    EXPECT_EQ(u_move_only.error().value, -1);
}

TEST_F(UnexpectedTest, ErrorAccess_AllOverloads)
{
    const std_::unexpected<int> const_u(42);
    const int& const_ref = const_u.error();
    EXPECT_EQ(const_ref, 42);

    std_::unexpected<int> mutable_u(42);
    int& mutable_ref = mutable_u.error();
    EXPECT_EQ(mutable_ref, 42);

    mutable_ref = 99;
    EXPECT_EQ(mutable_u.error(), 99);

    const int&& const_rvalue_ref = std::move(const_u).error();
    EXPECT_EQ(const_rvalue_ref, 42);

    int&& rvalue_ref = std::move(mutable_u).error();
    EXPECT_EQ(rvalue_ref, 99);
}

TEST_F(UnexpectedTest, ErrorAccess_StringMoveSemantics)
{
    std_::unexpected<std::string> u_str("movable string");

    std::string moved_str = std::move(u_str).error();
    EXPECT_EQ(moved_str, "movable string");
}

TEST_F(UnexpectedTest, EqualityComparison_SameTypes)
{
    std_::unexpected<int> u1(42);
    std_::unexpected<int> u2(42);
    std_::unexpected<int> u3(99);

    EXPECT_TRUE(u1 == u2);
    EXPECT_FALSE(u1 == u3);
    EXPECT_TRUE(u1 != u3);
    EXPECT_FALSE(u1 != u2);
}

TEST_F(UnexpectedTest, EqualityComparison_DifferentTypes)
{
    std_::unexpected<int> u_int(42);
    std_::unexpected<long> u_long(42L);
    std_::unexpected<double> u_double(42.0);

    EXPECT_TRUE(u_int == u_long);
    EXPECT_TRUE(u_int == u_double);
    EXPECT_FALSE(u_int != u_long);
    EXPECT_FALSE(u_int != u_double);

    std_::unexpected<long> u_long_diff(99L);
    EXPECT_FALSE(u_int == u_long_diff);
    EXPECT_TRUE(u_int != u_long_diff);
}

TEST_F(UnexpectedTest, EqualityComparison_StringTypes)
{
    std_::unexpected<std::string> u_str("test");
    std_::unexpected<const char*> u_cstr("test");

    EXPECT_TRUE(u_str == u_cstr);
    EXPECT_FALSE(u_str != u_cstr);

    std_::unexpected<std::string> u_str_diff("different");
    EXPECT_FALSE(u_str == u_str_diff);
    EXPECT_TRUE(u_str != u_str_diff);
}

TEST_F(UnexpectedTest, EqualityComparison_ComplexTypes)
{
    std::vector<int> vec1{1, 2, 3};
    std::vector<int> vec2{1, 2, 3};
    std::vector<int> vec3{4, 5, 6};

    std_::unexpected<std::vector<int>> u_vec1(vec1);
    std_::unexpected<std::vector<int>> u_vec2(vec2);
    std_::unexpected<std::vector<int>> u_vec3(vec3);

    EXPECT_TRUE(u_vec1 == u_vec2);
    EXPECT_FALSE(u_vec1 == u_vec3);
    EXPECT_FALSE(u_vec1 != u_vec2);
    EXPECT_TRUE(u_vec1 != u_vec3);
}

TEST_F(UnexpectedTest, MemberSwap)
{
    std_::unexpected<int> u1(10);
    std_::unexpected<int> u2(20);

    u1.swap(u2);

    EXPECT_EQ(u1.error(), 20);
    EXPECT_EQ(u2.error(), 10);
}

TEST_F(UnexpectedTest, FreeSwapFunction)
{
    std_::unexpected<int> u1(100);
    std_::unexpected<int> u2(200);

    swap(u1, u2);

    EXPECT_EQ(u1.error(), 200);
    EXPECT_EQ(u2.error(), 100);
}

TEST_F(UnexpectedTest, SwapWithComplexTypes)
{
    std::string str1 = "first string";
    std::string str2 = "second string";

    std_::unexpected<std::string> u1(str1);
    std_::unexpected<std::string> u2(str2);

    u1.swap(u2);

    EXPECT_EQ(u1.error(), "second string");
    EXPECT_EQ(u2.error(), "first string");
}

TEST_F(UnexpectedTest, SwapNoexceptSpecification)
{
    static_assert(noexcept(std::declval<std_::unexpected<int>>().swap(
                      std::declval<std_::unexpected<int>&>())),
                  "int swap should be noexcept");

    static_assert(noexcept(std::declval<std_::unexpected<std::string>>().swap(
                      std::declval<std_::unexpected<std::string>&>())),
                  "string swap should be noexcept");
}

TEST_F(UnexpectedTest, MakeUnexpected_BasicTypes)
{
    auto u_int = std_::make_unexpected(42);
    static_assert(std::is_same<decltype(u_int), std_::unexpected<int>>::value,
                  "make_unexpected should deduce int");
    EXPECT_EQ(u_int.error(), 42);

    auto u_double = std_::make_unexpected(3.14);
    static_assert(std::is_same<decltype(u_double), std_::unexpected<double>>::value,
                  "make_unexpected should deduce double");
    EXPECT_EQ(u_double.error(), 3.14);

    auto u_bool = std_::make_unexpected(true);
    static_assert(std::is_same<decltype(u_bool), std_::unexpected<bool>>::value,
                  "make_unexpected should deduce bool");
    EXPECT_EQ(u_bool.error(), true);
}

TEST_F(UnexpectedTest, MakeUnexpected_StringTypes)
{
    auto u_cstr = std_::make_unexpected("C string");
    static_assert(std::is_same<decltype(u_cstr), std_::unexpected<const char*>>::value,
                  "make_unexpected should deduce const char*");
    EXPECT_STREQ(u_cstr.error(), "C string");

    std::string cpp_str = "C++ string";
    auto u_str = std_::make_unexpected(cpp_str);
    static_assert(std::is_same<decltype(u_str), std_::unexpected<std::string>>::value,
                  "make_unexpected should deduce std::string");
    EXPECT_EQ(u_str.error(), "C++ string");

    auto u_str_move = std_::make_unexpected(std::move(cpp_str));
    static_assert(std::is_same<decltype(u_str_move), std_::unexpected<std::string>>::value,
                  "make_unexpected should deduce std::string from move");
    EXPECT_EQ(u_str_move.error(), "C++ string");
}

TEST_F(UnexpectedTest, MakeUnexpected_ComplexTypes)
{
    std::vector<int> vec{1, 2, 3, 4, 5};
    auto u_vec = std_::make_unexpected(std::move(vec));
    static_assert(std::is_same<decltype(u_vec), std_::unexpected<std::vector<int>>>::value,
                  "make_unexpected should deduce std::vector<int>");
    EXPECT_EQ(u_vec.error().size(), 5u);
    EXPECT_EQ(u_vec.error()[0], 1);
    EXPECT_EQ(u_vec.error()[4], 5);
}

TEST_F(UnexpectedTest, MakeUnexpected_MoveOnlyTypes)
{
    auto ptr = std::make_unique<int>(42);
    auto u_ptr = std_::make_unexpected(std::move(ptr));
    static_assert(std::is_same<decltype(u_ptr), std_::unexpected<std::unique_ptr<int>>>::value,
                  "make_unexpected should deduce std::unique_ptr<int>");
    EXPECT_EQ(*u_ptr.error(), 42);
    EXPECT_EQ(ptr.get(), nullptr);
}

TEST(BadExpectedAccessTest, VoidSpecialization)
{
    std_::bad_expected_access<void> base_exception;
    EXPECT_STREQ(base_exception.what(), "bad expected access");

    std::exception& base_ref = base_exception;
    EXPECT_STREQ(base_ref.what(), "bad expected access");
}

TEST(BadExpectedAccessTest, TypedSpecialization_BasicTypes)
{
    std_::bad_expected_access<int> int_exception(42);
    EXPECT_EQ(int_exception.error(), 42);
    EXPECT_STREQ(int_exception.what(), "bad expected access");

    std_::bad_expected_access<std::string> str_exception("custom error");
    EXPECT_EQ(str_exception.error(), "custom error");
    EXPECT_STREQ(str_exception.what(), "bad expected access");

    std_::bad_expected_access<double> double_exception(3.14159);
    EXPECT_EQ(double_exception.error(), 3.14159);
}

TEST(BadExpectedAccessTest, ErrorAccess_AllOverloads)
{
    std_::bad_expected_access<std::string> exception("test error");

    const auto& const_exception = exception;
    const std::string& const_error = const_exception.error();
    EXPECT_EQ(const_error, "test error");

    std::string& mutable_error = exception.error();
    EXPECT_EQ(mutable_error, "test error");
    mutable_error = "modified error";
    EXPECT_EQ(exception.error(), "modified error");

    const std::string&& const_rvalue_error = std::move(const_exception).error();
    EXPECT_EQ(const_rvalue_error, "modified error");

    std::string moved_error = std::move(exception).error();
    EXPECT_EQ(moved_error, "modified error");
}

TEST(BadExpectedAccessTest, ExceptionInheritance)
{
    std_::bad_expected_access<int> typed_exception(42);

    std_::bad_expected_access<void>& base_ref = typed_exception;
    EXPECT_STREQ(base_ref.what(), "bad expected access");

    std::exception& std_exception_ref = typed_exception;
    EXPECT_STREQ(std_exception_ref.what(), "bad expected access");
}

TEST(BadExpectedAccessTest, ThrowAndCatch)
{
    try
    {
        throw std_::bad_expected_access<std::string>("test error message");
    }
    catch (const std_::bad_expected_access<std::string>& e)
    {
        EXPECT_EQ(e.error(), "test error message");
        EXPECT_STREQ(e.what(), "bad expected access");
    }
    catch (...)
    {
        FAIL() << "Should have caught bad_expected_access<std::string>";
    }
}

TEST(BadExpectedAccessTest, PolymorphicCatch)
{
    try
    {
        throw std_::bad_expected_access<int>(404);
    }
    catch (const std_::bad_expected_access<void>& e)
    {
        EXPECT_STREQ(e.what(), "bad expected access");
    }
    catch (...)
    {
        FAIL() << "Should have caught bad_expected_access<void>";
    }

    try
    {
        throw std_::bad_expected_access<std::string>("not found");
    }
    catch (const std::exception& e)
    {
        EXPECT_STREQ(e.what(), "bad expected access");
    }
    catch (...)
    {
        FAIL() << "Should have caught std::exception";
    }
}

TEST(UnexpectedConstraintsTest, StaticAsserts) {}

TEST(UnexpectedConstraintsTest, MoveSemantics)
{
    std::string str = "test string";
    std_::unexpected<std::string> u1(str);
    std_::unexpected<std::string> u2(std::move(str));

    EXPECT_EQ(u1.error(), "test string");
    EXPECT_EQ(u2.error(), "test string");
}

TEST(UnexpectedConstraintsTest, ExceptionSafety)
{
    EXPECT_THROW(std_::unexpected<ThrowingConstructor>(666), std::runtime_error);

    EXPECT_NO_THROW(std_::unexpected<ThrowingConstructor>(42));
    std_::unexpected<ThrowingConstructor> u(42);
    EXPECT_EQ(u.error().value, 42);
}

TEST(UnexpectedPerformanceTest, NoexceptSpecifications)
{
    static_assert(noexcept(std_::unexpected<int>(42)),
                  "unexpected<int> constructor should be noexcept");

    static_assert(noexcept(std_::unexpected<std::string>(std::declval<std::string>())),
                  "unexpected<string> move constructor should be noexcept");

    static_assert(noexcept(std::declval<std_::unexpected<int>>().error()),
                  "error() should be noexcept");
}

TEST(UnexpectedPerformanceTest, TriviallyCopyable)
{
    static_assert(std::is_copy_constructible<std_::unexpected<int>>::value,
                  "unexpected<int> should be copy constructible");
    static_assert(std::is_move_constructible<std_::unexpected<int>>::value,
                  "unexpected<int> should be move constructible");

    static_assert(std::is_copy_assignable<std_::unexpected<int>>::value,
                  "unexpected<int> should be copy assignable");
    static_assert(std::is_move_assignable<std_::unexpected<int>>::value,
                  "unexpected<int> should be move assignable");

    static_assert(!std::is_trivially_copyable<std_::unexpected<std::string>>::value,
                  "unexpected<string> should not be trivially copyable");
}

TEST(UnexpectedRealWorldTest, ErrorCodes)
{
    enum class ErrorCode
    {
        None = 0,
        FileNotFound = 404,
        AccessDenied = 403,
        InternalError = 500
    };

    auto file_error = std_::make_unexpected(ErrorCode::FileNotFound);
    auto access_error = std_::make_unexpected(ErrorCode::AccessDenied);

    EXPECT_EQ(file_error.error(), ErrorCode::FileNotFound);
    EXPECT_EQ(access_error.error(), ErrorCode::AccessDenied);
    EXPECT_TRUE(file_error != access_error);
}

TEST(UnexpectedRealWorldTest, CustomErrorTypes)
{
    struct ParseError
    {
        std::string message;
        int line;
        int column;

        ParseError(const std::string& msg, int l, int c) : message(msg), line(l), column(c) {}

        bool operator==(const ParseError& other) const
        {
            return message == other.message && line == other.line && column == other.column;
        }
    };

    auto parse_error = std_::make_unexpected(ParseError{"Syntax error", 42, 15});
    EXPECT_EQ(parse_error.error().message, "Syntax error");
    EXPECT_EQ(parse_error.error().line, 42);
    EXPECT_EQ(parse_error.error().column, 15);

    auto copied_error = parse_error;
    EXPECT_TRUE(copied_error == parse_error);

    auto moved_error = std::move(parse_error);
    EXPECT_EQ(moved_error.error().message, "Syntax error");
}
