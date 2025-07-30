#include <expected/expected.hpp>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>


struct MyError
{
    int code;
    std::string message;

    MyError() : code(0), message("default") {}
    MyError(int c) : code(c), message("error " + std::to_string(c)) {}
    MyError(int c, const std::string& msg) : code(c), message(msg) {}

    bool operator==(const MyError& other) const
    {
        return code == other.code && message == other.message;
    }

    bool operator!=(const MyError& other) const
    {
        return !(*this == other);
    }
};

struct NonCopyable
{
    int value;
    NonCopyable(int v) : value(v) {}
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;

    bool operator==(const NonCopyable& other) const
    {
        return value == other.value;
    }
};

class ExpectedConstructorTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ExpectedConstructorTest, DefaultConstructor_ValidValue)
{
    std_::expected<int, MyError> e;

    EXPECT_TRUE(e.has_value());
    EXPECT_TRUE(static_cast<bool>(e));
    EXPECT_EQ(*e, 0);
    EXPECT_EQ(e.value(), 0);
}

TEST_F(ExpectedConstructorTest, DefaultConstructor_CustomType)
{
    std_::expected<std::string, MyError> e;

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, "");
    EXPECT_EQ(e.value(), "");
}

TEST_F(ExpectedConstructorTest, DefaultConstructor_NonTrivialType)
{
    std_::expected<std::vector<int>, MyError> e;

    EXPECT_TRUE(e.has_value());
    EXPECT_TRUE(e->empty());
    EXPECT_EQ(e->size(), 0u);
}

TEST_F(ExpectedConstructorTest, ValueConstructor_ImplicitConversion)
{
    std_::expected<int, MyError> e(42);

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 42);
}

TEST_F(ExpectedConstructorTest, ValueConstructor_StringFromCString)
{
    std_::expected<std::string, MyError> e("Hello");

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, "Hello");
}

TEST_F(ExpectedConstructorTest, ValueConstructor_MoveSemantics)
{
    std::string original = "Move me";
    std_::expected<std::string, MyError> e(std::move(original));

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, "Move me");
}

TEST_F(ExpectedConstructorTest, ValueConstructor_NonCopyableType)
{
    std_::expected<NonCopyable, MyError> e(NonCopyable{99});

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(e->value, 99);
}

TEST_F(ExpectedConstructorTest, ValueConstructor_ExplicitConversion)
{
    std_::expected<std::vector<int>, MyError> e(std::vector<int>{1, 2, 3});

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(e->size(), 3u);
    EXPECT_EQ((*e)[0], 1);
    EXPECT_EQ((*e)[1], 2);
    EXPECT_EQ((*e)[2], 3);
}

TEST_F(ExpectedConstructorTest, UnexpectedConstructor_ImplicitConversion)
{
    std_::unexpected<MyError> unexp(MyError{404, "Not Found"});
    std_::expected<int, MyError> e(unexp);

    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error().code, 404);
    EXPECT_EQ(e.error().message, "Not Found");
}

TEST_F(ExpectedConstructorTest, UnexpectedConstructor_ExplicitConversion)
{
    std_::unexpected<MyError> unexp(MyError{500, "Internal Error"});
    std_::expected<std::string, MyError> e(unexp);

    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error().code, 500);
    EXPECT_EQ(e.error().message, "Internal Error");
}

TEST_F(ExpectedConstructorTest, UnexpectedConstructor_MoveSemantics)
{
    std_::unexpected<MyError> unexp(MyError{401, "Unauthorized"});
    std_::expected<int, MyError> e(std::move(unexp));

    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error().code, 401);
    EXPECT_EQ(e.error().message, "Unauthorized");
}

TEST_F(ExpectedConstructorTest, InPlaceConstructor_BasicTypes)
{
    std_::expected<int, MyError> e(std_::detail::in_place, 42);

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 42);
}

TEST_F(ExpectedConstructorTest, InPlaceConstructor_StringWithMultipleArgs)
{
    std_::expected<std::string, MyError> e(std_::detail::in_place, 5u, 'A');

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, "AAAAA");
}

TEST_F(ExpectedConstructorTest, InPlaceConstructor_VectorWithInitializerList)
{
    std_::expected<std::vector<int>, MyError> e(std_::detail::in_place,
                                               std::initializer_list<int>{1, 2, 3, 4, 5});

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(e->size(), 5u);
    EXPECT_EQ((*e)[0], 1);
    EXPECT_EQ((*e)[4], 5);
}

TEST_F(ExpectedConstructorTest, InPlaceConstructor_CustomTypeWithMultipleArgs)
{
    std_::expected<MyError, int> e(std_::detail::in_place, 200, "Success");

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(e->code, 200);
    EXPECT_EQ(e->message, "Success");
}

TEST_F(ExpectedConstructorTest, InPlaceTypeConstructor_UnexpectedWithSingleArg)
{
    using unexpected_type = std_::unexpected<MyError>;
    std_::expected<int, MyError> e(std_::detail::in_place_type_t<unexpected_type>{}, 404);

    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error().code, 404);
}

TEST_F(ExpectedConstructorTest, InPlaceTypeConstructor_UnexpectedWithMultipleArgs)
{
    using unexpected_type = std_::unexpected<MyError>;
    std_::expected<std::string, MyError> e(std_::detail::in_place_type_t<unexpected_type>{},
                                          500,
                                          "Internal Server Error");

    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error().code, 500);
    EXPECT_EQ(e.error().message, "Internal Server Error");
}

TEST_F(ExpectedConstructorTest, InPlaceTypeConstructor_UnexpectedWithInitializerList)
{
    using unexpected_type = std_::unexpected<std::vector<int>>;
    std_::expected<std::string, std::vector<int>> e(std_::detail::in_place_type_t<unexpected_type>{},
                                                   std::initializer_list<int>{1, 2, 3});

    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error().size(), 3u);
    EXPECT_EQ(e.error()[0], 1);
    EXPECT_EQ(e.error()[2], 3);
}

TEST_F(ExpectedConstructorTest, EdgeCase_ConstRefValue)
{
    const int value = 42;
    std_::expected<int, MyError> e(value);

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 42);
}

TEST_F(ExpectedConstructorTest, EdgeCase_RValueRefValue)
{
    std_::expected<std::string, MyError> e(std::string("temporary"));

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, "temporary");
}

TEST_F(ExpectedConstructorTest, EdgeCase_EmptyString)
{
    std_::expected<std::string, MyError> e("");

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, "");
    EXPECT_TRUE(e->empty());
}

TEST_F(ExpectedConstructorTest, EdgeCase_ZeroValue)
{
    std_::expected<int, MyError> e(0);

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 0);
}

TEST_F(ExpectedConstructorTest, EdgeCase_NegativeValue)
{
    std_::expected<int, MyError> e(-42);

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, -42);
}

TEST_F(ExpectedConstructorTest, Performance_InPlaceConstruction)
{
    std_::expected<std::vector<int>, MyError> e(std_::detail::in_place, 1'000u, 42);

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(e->size(), 1'000u);
    EXPECT_EQ((*e)[0], 42);
    EXPECT_EQ((*e)[999], 42);
}

TEST_F(ExpectedConstructorTest, Performance_MoveOptimization)
{
    std::vector<int> large_vector(1'000, 42);
    std_::expected<std::vector<int>, MyError> e(std::move(large_vector));

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(e->size(), 1'000u);
    EXPECT_EQ((*e)[0], 42);
}

TEST_F(ExpectedConstructorTest, TypeTraits_ConstructibilityConstraints)
{
    static_assert(std::is_constructible<std_::expected<int, MyError>, int>::value,
                  "Should be constructible from int");

    static_assert(std::is_constructible<std_::expected<std::string, MyError>, const char*>::value,
                  "Should be constructible from const char*");
}

TEST_F(ExpectedConstructorTest, TypeTraits_ConversionConstraints)
{
    static_assert(std::is_convertible<int, std_::expected<int, MyError>>::value,
                  "Should be implicitly convertible from int");

    static_assert(std::is_convertible<const char*, std_::expected<std::string, MyError>>::value,
                  "Should be implicitly convertible from const char*");
}

TEST_F(ExpectedConstructorTest, Advanced_PolymorphicTypes)
{
    struct Base
    {
        virtual ~Base() = default;
        int value = 42;
    };
    struct Derived : Base
    {
        int extra = 99;
    };

    std_::expected<std::unique_ptr<Base>, MyError> e(std::make_unique<Derived>());

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ((*e)->value, 42);
}

TEST_F(ExpectedConstructorTest, Advanced_EmplaceVsDirectConstruction)
{
    struct ExpensiveToCopy
    {
        std::vector<int> data;
        ExpensiveToCopy() : data(1'000, 42) {}
        ExpensiveToCopy(size_t n, int val) : data(n, val) {}
    };

    std_::expected<ExpensiveToCopy, MyError> e1(std_::detail::in_place, 500u, 99);
    EXPECT_TRUE(e1.has_value());
    EXPECT_EQ(e1->data.size(), 500u);
    EXPECT_EQ(e1->data[0], 99);

    std_::expected<ExpensiveToCopy, MyError> e2(ExpensiveToCopy{200u, 88});
    EXPECT_TRUE(e2.has_value());
    EXPECT_EQ(e2->data.size(), 200u);
    EXPECT_EQ(e2->data[0], 88);
}

TEST_F(ExpectedConstructorTest, Constraints_PreventAmbiguousConstruction)
{
    std_::expected<int, int> e1(42);
    EXPECT_TRUE(e1.has_value());
    EXPECT_EQ(*e1, 42);

    std_::expected<int, int> e2(std_::unexpected<int>(42));
    EXPECT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error(), 42);
}

TEST_F(ExpectedConstructorTest, Constraints_InPlaceDisambiguation)
{
    struct Convertible
    {
        int value;
        Convertible(int v) : value(v) {}
        operator int() const
        {
            return value;
        }
    };

    std_::expected<Convertible, MyError> e1(std_::detail::in_place, 42);
    EXPECT_TRUE(e1.has_value());
    EXPECT_EQ(e1->value, 42);

    std_::expected<int, MyError> e2(Convertible{42});
    EXPECT_TRUE(e2.has_value());
    EXPECT_EQ(*e2, 42);
}

TEST_F(ExpectedConstructorTest, RealWorld_FileOperations)
{
    enum class FileError
    {
        NotFound,
        PermissionDenied,
        IOError
    };

    auto open_file = [](const std::string& path) -> std_::expected<std::string, FileError>
    {
        if (path.empty())
        {
            return std_::unexpected<FileError>(FileError::NotFound);
        }
        return std::string("File content from ") + path;
    };

    auto success = open_file("valid_path.txt");
    EXPECT_TRUE(success.has_value());
    EXPECT_EQ(*success, "File content from valid_path.txt");

    auto failure = open_file("");
    EXPECT_FALSE(failure.has_value());
    EXPECT_EQ(failure.error(), FileError::NotFound);
}

TEST_F(ExpectedConstructorTest, RealWorld_NetworkOperations)
{
    struct NetworkResponse
    {
        int status_code;
        std::string body;

        NetworkResponse(int code, const std::string& data) : status_code(code), body(data) {}
    };

    struct NetworkError
    {
        int error_code;
        std::string message;

        NetworkError(int code, const std::string& msg) : error_code(code), message(msg) {}
    };

    std_::expected<NetworkResponse, NetworkError> success(std_::detail::in_place, 200, "Success");

    EXPECT_TRUE(success.has_value());
    EXPECT_EQ(success->status_code, 200);
    EXPECT_EQ(success->body, "Success");

    std_::expected<NetworkResponse, NetworkError> error(
        std_::unexpected<NetworkError>(NetworkError{404, "Not Found"}));

    EXPECT_FALSE(error.has_value());
    EXPECT_EQ(error.error().error_code, 404);
    EXPECT_EQ(error.error().message, "Not Found");
}

TEST_F(ExpectedConstructorTest, RealWorld_ParsingOperations)
{
    auto parse_int = [](const std::string& str) -> std_::expected<int, std::string>
    {
        if (str.empty())
        {
            return std_::unexpected<std::string>("Empty string");
        }
        if (str == "42")
        {
            return 42;
        }
        return std_::unexpected<std::string>("Invalid number: " + str);
    };

    auto success = parse_int("42");
    EXPECT_TRUE(success.has_value());
    EXPECT_EQ(*success, 42);

    auto failure = parse_int("invalid");
    EXPECT_FALSE(failure.has_value());
    EXPECT_EQ(failure.error(), "Invalid number: invalid");

    auto empty_failure = parse_int("");
    EXPECT_FALSE(empty_failure.has_value());
    EXPECT_EQ(empty_failure.error(), "Empty string");
}

TEST_F(ExpectedConstructorTest, Memory_SizeAndAlignment)
{
    struct SmallType
    {
        char data[4];
    };
    struct LargeType
    {
        char data[1'024];
    };

    constexpr size_t small_expected_size = sizeof(std_::expected<SmallType, MyError>);
    constexpr size_t large_expected_size = sizeof(std_::expected<LargeType, MyError>);

    EXPECT_LT(small_expected_size, large_expected_size);
    EXPECT_GE(small_expected_size, sizeof(SmallType));
    EXPECT_GE(large_expected_size, sizeof(LargeType));

    EXPECT_EQ(alignof(std_::expected<SmallType, MyError>),
              std::max(alignof(SmallType), alignof(MyError)));
}

TEST_F(ExpectedConstructorTest, CopyConstructor_ValidValue_WhenAvailable)
{
    std_::expected<int, MyError> original(42);

    auto copy_if_possible = [](const std_::expected<int, MyError>& e)
    {
        return std_::expected<int, MyError>(
            e.has_value() ? std_::expected<int, MyError>(*e)
                          : std_::expected<int, MyError>(std_::unexpected<MyError>(e.error())));
    };

    auto copy = copy_if_possible(original);

    EXPECT_TRUE(copy.has_value());
    EXPECT_EQ(*copy, 42);
    EXPECT_EQ(*original, 42);
}

TEST_F(ExpectedConstructorTest, CopyConstructor_ErrorValue_WhenAvailable)
{
    std_::expected<int, MyError> original(std_::unexpected<MyError>(MyError{404, "Not Found"}));

    auto copy_if_possible = [](const std_::expected<int, MyError>& e)
    {
        return std_::expected<int, MyError>(
            e.has_value() ? std_::expected<int, MyError>(*e)
                          : std_::expected<int, MyError>(std_::unexpected<MyError>(e.error())));
    };

    auto copy = copy_if_possible(original);

    EXPECT_FALSE(copy.has_value());
    EXPECT_EQ(copy.error().code, 404);
    EXPECT_EQ(copy.error().message, "Not Found");
}

TEST_F(ExpectedConstructorTest, MoveConstructor_ValidValue_WhenAvailable)
{
    std_::expected<std::string, MyError> original("Hello World");

    auto move_if_possible = [](std_::expected<std::string, MyError>&& e)
    {
        return std_::expected<std::string, MyError>(
            e.has_value() ? std_::expected<std::string, MyError>(std::move(*e))
                          : std_::expected<std::string, MyError>(
                                std_::unexpected<MyError>(std::move(e.error()))));
    };

    auto moved = move_if_possible(std::move(original));

    EXPECT_TRUE(moved.has_value());
    EXPECT_EQ(*moved, "Hello World");
}

TEST_F(ExpectedConstructorTest, ExceptionSafety_NoThrowConstructors)
{
    static_assert(std::is_nothrow_default_constructible<std_::expected<int, MyError>>::value,
                  "Default constructor should be noexcept for int");
}
