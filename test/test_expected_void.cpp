#include <expected/expected.hpp>
#include <gtest/gtest.h>

#include <memory>
#include <string>


struct TestError
{
    std::string message;
    int code;

    TestError(const std::string& msg = "", int c = 0) : message(msg), code(c) {}

    bool operator==(const TestError& other) const
    {
        return message == other.message && code == other.code;
    }

    bool operator!=(const TestError& other) const
    {
        return !(*this == other);
    }
};

struct NetworkError
{
    enum class Type
    {
        Timeout,
        ConnectionRefused,
        DNSFailure
    };

    Type type;
    std::string host;
    int port;

    NetworkError(Type t, const std::string& h, int p = 80) : type(t), host(h), port(p) {}

    bool operator==(const NetworkError& other) const
    {
        return type == other.type && host == other.host && port == other.port;
    }
};

struct MoveOnlyError
{
    std::unique_ptr<std::string> message;
    int error_id;

    MoveOnlyError(MoveOnlyError&& other) noexcept
        : message(std::move(other.message)), error_id(other.error_id)
    {
    }

    MoveOnlyError& operator=(MoveOnlyError&& other) noexcept
    {
        if (this != &other)
        {
            message = std::move(other.message);
            error_id = other.error_id;
        }
        return *this;
    }

    explicit MoveOnlyError(const std::string& msg, int id = 0)
        : message(std::make_unique<std::string>(msg)), error_id(id)
    {
    }

    MoveOnlyError(const MoveOnlyError&) = delete;
    MoveOnlyError& operator=(const MoveOnlyError&) = delete;

    bool operator==(const MoveOnlyError& other) const
    {
        return message && other.message && *message == *other.message && error_id == other.error_id;
    }
};

class ExpectedVoidTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}

    TestError makeBasicError(const std::string& msg, int code) const
    {
        return TestError{msg, code};
    }

    NetworkError makeNetworkError(NetworkError::Type type,
                                  const std::string& host,
                                  int port = 80) const
    {
        return NetworkError{type, host, port};
    }
};

TEST_F(ExpectedVoidTest, DefaultConstruction_AllErrorTypes)
{
    std_::expected<void, TestError> basic_exp;
    std_::expected<void, NetworkError> network_exp;
    std_::expected<void, int> int_exp;

    EXPECT_TRUE(basic_exp.has_value());
    EXPECT_TRUE(basic_exp);
    EXPECT_TRUE(network_exp.has_value());
    EXPECT_TRUE(network_exp);
    EXPECT_TRUE(int_exp.has_value());
    EXPECT_TRUE(int_exp);
}

TEST_F(ExpectedVoidTest, InPlaceConstruction_ExplicitSuccess)
{
    std_::expected<void, TestError> exp(std_::detail::in_place);

    EXPECT_TRUE(exp.has_value());
    EXPECT_TRUE(exp);
    EXPECT_NO_THROW(exp.value());
}

TEST_F(ExpectedVoidTest, UnexpectedConstruction_BasicError)
{
    auto error = makeBasicError("Not Found", 404);
    std_::expected<void, TestError> exp{std_::unexpected<TestError>(error)};

    EXPECT_FALSE(exp.has_value());
    EXPECT_FALSE(exp);
    EXPECT_EQ(exp.error(), error);
    EXPECT_EQ(exp.error().message, "Not Found");
    EXPECT_EQ(exp.error().code, 404);
}

TEST_F(ExpectedVoidTest, UnexpectedConstruction_NetworkError)
{
    auto error = makeNetworkError(NetworkError::Type::Timeout, "example.com", 80);
    std_::expected<void, NetworkError> exp{std_::unexpected<NetworkError>(error)};

    EXPECT_FALSE(exp.has_value());
    EXPECT_FALSE(exp);
    EXPECT_EQ(exp.error(), error);
    EXPECT_EQ(exp.error().type, NetworkError::Type::Timeout);
    EXPECT_EQ(exp.error().host, "example.com");
    EXPECT_EQ(exp.error().port, 80);
}

TEST_F(ExpectedVoidTest, UnexpectedConstruction_MoveOnlyError)
{
    auto move_error = MoveOnlyError{"Critical error", 999};
    std_::expected<void, MoveOnlyError> exp{std_::unexpected<MoveOnlyError>(std::move(move_error))};

    EXPECT_FALSE(exp.has_value());
    EXPECT_FALSE(exp);
    EXPECT_EQ(exp.error().error_id, 999);
    EXPECT_EQ(*exp.error().message, "Critical error");
}

TEST_F(ExpectedVoidTest, CopyConstruction_SuccessToSuccess)
{
    std_::expected<void, TestError> original;
    std_::expected<void, TestError> copy(original);

    EXPECT_TRUE(original.has_value());
    EXPECT_TRUE(copy.has_value());
    EXPECT_NO_THROW(original.value());
    EXPECT_NO_THROW(copy.value());
}

TEST_F(ExpectedVoidTest, CopyConstruction_ErrorToError_BasicError)
{
    auto error = makeBasicError("Server Error", 500);
    std_::expected<void, TestError> original{std_::unexpected<TestError>(error)};
    std_::expected<void, TestError> copy(original);

    EXPECT_FALSE(original.has_value());
    EXPECT_FALSE(copy.has_value());
    EXPECT_EQ(copy.error(), error);
    EXPECT_EQ(copy.error().message, "Server Error");
    EXPECT_EQ(copy.error().code, 500);
    EXPECT_EQ(original.error(), error);
}

TEST_F(ExpectedVoidTest, CopyConstruction_ErrorToError_NetworkError)
{
    auto error = makeNetworkError(NetworkError::Type::ConnectionRefused, "localhost", 8'080);
    std_::expected<void, NetworkError> original{std_::unexpected<NetworkError>(error)};
    std_::expected<void, NetworkError> copy(original);

    EXPECT_FALSE(original.has_value());
    EXPECT_FALSE(copy.has_value());
    EXPECT_EQ(copy.error(), error);
    EXPECT_EQ(copy.error().type, NetworkError::Type::ConnectionRefused);
    EXPECT_EQ(copy.error().host, "localhost");
    EXPECT_EQ(copy.error().port, 8'080);
}

TEST_F(ExpectedVoidTest, MoveConstruction_SuccessToSuccess)
{
    std_::expected<void, TestError> original;
    std_::expected<void, TestError> moved(std::move(original));

    EXPECT_TRUE(moved.has_value());
    EXPECT_NO_THROW(moved.value());
}

TEST_F(ExpectedVoidTest, MoveConstruction_ErrorToError_MoveOnlyError)
{
    auto error = MoveOnlyError{"Move-only error message", 777};
    std_::expected<void, MoveOnlyError> original{std_::unexpected<MoveOnlyError>(std::move(error))};
    std_::expected<void, MoveOnlyError> moved(std::move(original));

    EXPECT_FALSE(moved.has_value());
    EXPECT_EQ(moved.error().error_id, 777);
    EXPECT_EQ(*moved.error().message, "Move-only error message");
}

TEST_F(ExpectedVoidTest, CopyAssignment_AllCombinations)
{
    auto error404 = makeBasicError("Not Found", 404);
    auto error500 = makeBasicError("Internal Server Error", 500);

    std_::expected<void, TestError> exp1;
    std_::expected<void, TestError> exp2;
    exp1 = exp2;
    EXPECT_TRUE(exp1.has_value());
    EXPECT_TRUE(exp2.has_value());

    std_::expected<void, TestError> success_exp;
    std_::expected<void, TestError> error_exp{std_::unexpected<TestError>(error404)};
    success_exp = error_exp;
    EXPECT_FALSE(success_exp.has_value());
    EXPECT_EQ(success_exp.error(), error404);
    EXPECT_FALSE(error_exp.has_value());

    std_::expected<void, TestError> error_exp2{std_::unexpected<TestError>(error500)};
    std_::expected<void, TestError> success_exp2;
    error_exp2 = success_exp2;
    EXPECT_TRUE(error_exp2.has_value());
    EXPECT_NO_THROW(error_exp2.value());

    std_::expected<void, TestError> error1{std_::unexpected<TestError>(error404)};
    std_::expected<void, TestError> error2{std_::unexpected<TestError>(error500)};
    error1 = error2;
    EXPECT_FALSE(error1.has_value());
    EXPECT_EQ(error1.error(), error500);
}

TEST_F(ExpectedVoidTest, UnexpectedAssignment_ToSuccessAndError)
{
    auto error = makeBasicError("New Error", 503);

    std_::expected<void, TestError> success_exp;
    success_exp = std_::unexpected<TestError>(error);
    EXPECT_FALSE(success_exp.has_value());
    EXPECT_EQ(success_exp.error(), error);

    auto old_error = makeBasicError("Old Error", 400);
    std_::expected<void, TestError> error_exp{std_::unexpected<TestError>(old_error)};
    error_exp = std_::unexpected<TestError>(error);
    EXPECT_FALSE(error_exp.has_value());
    EXPECT_EQ(error_exp.error(), error);
    EXPECT_NE(error_exp.error(), old_error);
}

TEST_F(ExpectedVoidTest, MoveAssignment_MoveOnlyError)
{
    std_::expected<void, MoveOnlyError> exp;
    auto error = MoveOnlyError{"Moved error", 666};

    exp = std_::unexpected<MoveOnlyError>(std::move(error));

    EXPECT_FALSE(exp.has_value());
    EXPECT_EQ(exp.error().error_id, 666);
    EXPECT_EQ(*exp.error().message, "Moved error");
}

TEST_F(ExpectedVoidTest, ValueAccess_SuccessState_AllOverloads)
{
    std_::expected<void, TestError> exp;
    const std_::expected<void, TestError> const_exp;

    EXPECT_NO_THROW(exp.value());

    EXPECT_NO_THROW(const_exp.value());

    EXPECT_NO_THROW(std::move(exp).value());

    EXPECT_NO_THROW(std::move(const_exp).value());
}

TEST_F(ExpectedVoidTest, ValueAccess_ErrorState_ExceptionHandling)
{
    auto error = makeBasicError("Access Error", 403);
    std_::expected<void, TestError> exp{std_::unexpected<TestError>(error)};

    EXPECT_THROW(exp.value(), std_::bad_expected_access<TestError>);

    try
    {
        exp.value();
        FAIL() << "Expected std_::bad_expected_access exception was not thrown";
    }
    catch (const std_::bad_expected_access<TestError>& e)
    {
        EXPECT_EQ(e.error(), error);
        EXPECT_EQ(e.error().message, "Access Error");
        EXPECT_EQ(e.error().code, 403);
    }
    catch (...)
    {
        FAIL() << "Wrong exception type thrown";
    }
}

TEST_F(ExpectedVoidTest, ErrorAccess_AllOverloads_Comprehensive)
{
    auto error = makeBasicError("Test Error", 500);
    std_::expected<void, TestError> exp{std_::unexpected<TestError>(error)};

    TestError& mutable_error = exp.error();
    EXPECT_EQ(mutable_error, error);

    mutable_error.code = 503;
    EXPECT_EQ(exp.error().code, 503);

    const auto& const_exp = exp;
    const TestError& const_error = const_exp.error();
    EXPECT_EQ(const_error.code, 503);
    EXPECT_EQ(const_error.message, "Test Error");

    auto moved_exp = std::move(exp);
    TestError&& moved_error = std::move(moved_exp).error();
    EXPECT_EQ(moved_error.code, 503);
}

TEST_F(ExpectedVoidTest, Emplace_FromErrorToSuccess)
{
    auto error = makeBasicError("Initial Error", 404);
    std_::expected<void, TestError> exp{std_::unexpected<TestError>(error)};

    EXPECT_FALSE(exp.has_value());

    exp.emplace();

    EXPECT_TRUE(exp.has_value());
    EXPECT_NO_THROW(exp.value());
}

TEST_F(ExpectedVoidTest, Emplace_FromSuccessToSuccess)
{
    std_::expected<void, TestError> exp;

    EXPECT_TRUE(exp.has_value());

    exp.emplace();

    EXPECT_TRUE(exp.has_value());
    EXPECT_NO_THROW(exp.value());
}

TEST_F(ExpectedVoidTest, Emplace_MultipleOperations)
{
    auto error = makeBasicError("Error", 500);
    std_::expected<void, TestError> exp{std_::unexpected<TestError>(error)};

    exp.emplace();
    EXPECT_TRUE(exp.has_value());

    exp.emplace();
    EXPECT_TRUE(exp.has_value());

    auto new_error = makeBasicError("New Error", 404);
    exp = std_::unexpected<TestError>(new_error);
    EXPECT_FALSE(exp.has_value());

    exp.emplace();
    EXPECT_TRUE(exp.has_value());
}

TEST_F(ExpectedVoidTest, ErrorOr_SuccessState_ReturnsDefault)
{
    std_::expected<void, TestError> exp;
    auto default_error = makeBasicError("Default", 0);

    auto result = exp.error_or(default_error);

    EXPECT_EQ(result, default_error);
    EXPECT_EQ(result.message, "Default");
    EXPECT_EQ(result.code, 0);
}

TEST_F(ExpectedVoidTest, ErrorOr_ErrorState_ReturnsActualError)
{
    auto actual_error = makeBasicError("Actual Error", 404);
    std_::expected<void, TestError> exp{std_::unexpected<TestError>(actual_error)};
    auto default_error = makeBasicError("Default", 0);

    auto result = exp.error_or(default_error);

    EXPECT_EQ(result, actual_error);
    EXPECT_EQ(result.message, "Actual Error");
    EXPECT_EQ(result.code, 404);
}

TEST_F(ExpectedVoidTest, Comparison_ComprehensiveTesting)
{
    auto error1 = makeBasicError("Error 1", 400);
    auto error2 = makeBasicError("Error 1", 400);
    auto error3 = makeBasicError("Error 2", 500);

    std_::expected<void, TestError> success1;
    std_::expected<void, TestError> success2;
    std_::expected<void, TestError> error_exp1{std_::unexpected<TestError>(error1)};
    std_::expected<void, TestError> error_exp2{std_::unexpected<TestError>(error2)};
    std_::expected<void, TestError> error_exp3{std_::unexpected<TestError>(error3)};

    EXPECT_TRUE(success1 == success2);
    EXPECT_FALSE(success1 != success2);

    EXPECT_TRUE(error_exp1 == error_exp2);
    EXPECT_FALSE(error_exp1 != error_exp2);

    EXPECT_FALSE(error_exp1 == error_exp3);
    EXPECT_TRUE(error_exp1 != error_exp3);

    EXPECT_FALSE(success1 == error_exp1);
    EXPECT_TRUE(success1 != error_exp1);
    EXPECT_FALSE(error_exp1 == success1);
    EXPECT_TRUE(error_exp1 != success1);
}

TEST_F(ExpectedVoidTest, Comparison_WithUnexpected)
{
    auto error = makeBasicError("Test Error", 404);
    auto different_error = makeBasicError("Different Error", 500);

    std_::expected<void, TestError> success;
    std_::expected<void, TestError> error_exp{std_::unexpected<TestError>(error)};

    std_::unexpected<TestError> unexp_same{error};
    std_::unexpected<TestError> unexp_different{different_error};

    EXPECT_FALSE(success == unexp_same);
    EXPECT_TRUE(success != unexp_same);

    EXPECT_TRUE(error_exp == unexp_same);
    EXPECT_FALSE(error_exp != unexp_same);

    EXPECT_FALSE(error_exp == unexp_different);
    EXPECT_TRUE(error_exp != unexp_different);
}

TEST_F(ExpectedVoidTest, AndThen_SuccessChaining_MultipleOperations)
{
    int call_count = 0;

    auto operation1 = [&call_count]() -> std_::expected<void, TestError>
    {
        call_count++;
        return {};
    };

    auto operation2 = [&call_count]() -> std_::expected<void, TestError>
    {
        call_count++;
        return {};
    };

    auto operation3 = [&call_count]() -> std_::expected<void, TestError>
    {
        call_count++;
        return {};
    };

    std_::expected<void, TestError> exp;
    auto result = exp.and_then(operation1).and_then(operation2).and_then(operation3);

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(call_count, 3);
}

TEST_F(ExpectedVoidTest, AndThen_ErrorPropagation_EarlyTermination)
{
    bool operation2_called = false;
    bool operation3_called = false;

    auto operation1 = []() -> std_::expected<void, TestError>
    {
        return std_::unexpected<TestError>(TestError{"Operation 1 failed", 501});
    };

    auto operation2 = [&operation2_called]() -> std_::expected<void, TestError>
    {
        operation2_called = true;
        return {};
    };

    auto operation3 = [&operation3_called]() -> std_::expected<void, TestError>
    {
        operation3_called = true;
        return {};
    };

    std_::expected<void, TestError> exp;
    auto result = exp.and_then(operation1).and_then(operation2).and_then(operation3);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message, "Operation 1 failed");
    EXPECT_EQ(result.error().code, 501);
    EXPECT_FALSE(operation2_called);
    EXPECT_FALSE(operation3_called);
}

TEST_F(ExpectedVoidTest, Transform_VoidToValue_WithSideEffects)
{
    int counter = 0;
    auto void_to_int = [&counter]() -> int
    {
        counter += 10;
        return 42;
    };

    std_::expected<void, TestError> exp;
    auto result = exp.transform(void_to_int);

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(counter, 10);

    bool function_called = false;
    auto tracking_function = [&function_called]() -> int
    {
        function_called = true;
        return 99;
    };

    auto result2 = exp.transform(tracking_function);
    EXPECT_TRUE(function_called);
    EXPECT_TRUE(result2.has_value());
}

TEST_F(ExpectedVoidTest, Transform_VoidToVoid_SideEffects)
{
    int counter = 0;
    auto void_operation = [&counter]()
    {
        counter += 10;
    };

    std_::expected<void, TestError> exp;
    auto result = exp.transform(void_operation);

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(counter, 10);

    auto result2 = result.transform(
        [&counter]()
        {
            counter *= 2;
        });
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(counter, 20);
}

TEST_F(ExpectedVoidTest, Transform_ErrorState_NoExecution)
{
    bool function_called = false;
    auto function = [&function_called]() -> int
    {
        function_called = true;
        return 42;
    };

    auto error = makeBasicError("Transform Error", 500);
    std_::expected<void, TestError> exp{std_::unexpected<TestError>(error)};
    auto result = exp.transform(function);

    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(function_called);
    EXPECT_EQ(result.error(), error);
}

TEST_F(ExpectedVoidTest, RealWorld_ValidationChain_FileOperations)
{
    auto validate_path = [](const std::string& path) -> std_::expected<void, TestError>
    {
        if (path.empty())
        {
            return std_::unexpected<TestError>(TestError{"Path cannot be empty", 400});
        }
        if (path.find("..") != std::string::npos)
        {
            return std_::unexpected<TestError>(TestError{"Path traversal not allowed", 403});
        }
        return {};
    };

    auto create_directory = [](const std::string& path) -> std_::expected<void, TestError>
    {
        if (path == "/readonly")
        {
            return std_::unexpected<TestError>(TestError{"Permission denied", 403});
        }
        return {};
    };

    auto write_file = []() -> std_::expected<void, TestError>
    {
        return {};
    };

    auto set_permissions = []() -> std_::expected<void, TestError>
    {
        return {};
    };

    auto success_result = validate_path("/tmp/myfile.txt")
                              .and_then(
                                  [&]()
                                  {
                                      return create_directory("/tmp");
                                  })
                              .and_then(
                                  [&]()
                                  {
                                      return write_file();
                                  })
                              .and_then(
                                  [&]()
                                  {
                                      return set_permissions();
                                  });

    EXPECT_TRUE(success_result.has_value());

    auto failure_result1 = validate_path("")
                               .and_then(
                                   [&]()
                                   {
                                       return create_directory("/tmp");
                                   })
                               .and_then(
                                   [&]()
                                   {
                                       return write_file();
                                   })
                               .and_then(
                                   [&]()
                                   {
                                       return set_permissions();
                                   });

    EXPECT_FALSE(failure_result1.has_value());
    EXPECT_EQ(failure_result1.error().message, "Path cannot be empty");
    EXPECT_EQ(failure_result1.error().code, 400);

    auto failure_result2 = validate_path("/readonly/file.txt")
                               .and_then(
                                   [&]()
                                   {
                                       return create_directory("/readonly");
                                   })
                               .and_then(
                                   [&]()
                                   {
                                       return write_file();
                                   })
                               .and_then(
                                   [&]()
                                   {
                                       return set_permissions();
                                   });

    EXPECT_FALSE(failure_result2.has_value());
    EXPECT_EQ(failure_result2.error().message, "Permission denied");
    EXPECT_EQ(failure_result2.error().code, 403);
}

TEST_F(ExpectedVoidTest, RealWorld_NetworkOperationChain)
{
    auto connect_to_server = [](const std::string& host,
                                int port) -> std_::expected<void, NetworkError>
    {
        if (host == "unreachable.com")
        {
            return std_::unexpected<NetworkError>(
                NetworkError{NetworkError::Type::Timeout, host, port});
        }
        if (port == 9'999)
        {
            return std_::unexpected<NetworkError>(
                NetworkError{NetworkError::Type::ConnectionRefused, host, port});
        }
        return {};
    };

    auto authenticate = []() -> std_::expected<void, NetworkError>
    {
        return {};
    };

    auto send_data = []() -> std_::expected<void, NetworkError>
    {
        return {};
    };

    auto disconnect = []() -> std_::expected<void, NetworkError>
    {
        return {};
    };

    auto success_result = connect_to_server("api.example.com", 443)
                              .and_then(
                                  [&]()
                                  {
                                      return authenticate();
                                  })
                              .and_then(
                                  [&]()
                                  {
                                      return send_data();
                                  })
                              .and_then(
                                  [&]()
                                  {
                                      return disconnect();
                                  });

    EXPECT_TRUE(success_result.has_value());

    auto timeout_result = connect_to_server("unreachable.com", 80)
                              .and_then(
                                  [&]()
                                  {
                                      return authenticate();
                                  })
                              .and_then(
                                  [&]()
                                  {
                                      return send_data();
                                  })
                              .and_then(
                                  [&]()
                                  {
                                      return disconnect();
                                  });

    EXPECT_FALSE(timeout_result.has_value());
    EXPECT_EQ(timeout_result.error().type, NetworkError::Type::Timeout);
    EXPECT_EQ(timeout_result.error().host, "unreachable.com");

    auto refused_result = connect_to_server("localhost", 9'999)
                              .and_then(
                                  [&]()
                                  {
                                      return authenticate();
                                  })
                              .and_then(
                                  [&]()
                                  {
                                      return send_data();
                                  })
                              .and_then(
                                  [&]()
                                  {
                                      return disconnect();
                                  });

    EXPECT_FALSE(refused_result.has_value());
    EXPECT_EQ(refused_result.error().type, NetworkError::Type::ConnectionRefused);
    EXPECT_EQ(refused_result.error().port, 9'999);
}
