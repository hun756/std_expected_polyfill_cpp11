#include <expected/expected.hpp>

#include <iostream>
#include <memory>
#include <string>


struct ComplexError
{
    std::string message;
    int code;
    std::string details;

    ComplexError(const std::string& msg, int c, const std::string& det = "")
        : message(msg), code(c), details(det)
    {
    }
};

class DatabaseConnection
{
public:
    DatabaseConnection(const std::string& conn_str) : connection_string(conn_str)
    {
        std::cout << "DatabaseConnection created: " << conn_str << std::endl;
    }

    ~DatabaseConnection()
    {
        std::cout << "DatabaseConnection destroyed: " << connection_string << std::endl;
    }

    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    DatabaseConnection(DatabaseConnection&& other) noexcept
        : connection_string(std::move(other.connection_string))
    {
        std::cout << "DatabaseConnection moved" << std::endl;
    }

    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept
    {
        if (this != &other)
        {
            connection_string = std::move(other.connection_string);
            std::cout << "DatabaseConnection move assigned" << std::endl;
        }
        return *this;
    }

    std::string get_info() const
    {
        return "Connected to: " + connection_string;
    }

private:
    std::string connection_string;
};

std_::expected<DatabaseConnection, ComplexError> create_connection(const std::string& conn_str)
{
    if (conn_str.empty())
    {
        return std_::unexpected<ComplexError>(ComplexError("Empty connection string", 1));
    }
    if (conn_str.find("://") == std::string::npos)
    {
        return std_::unexpected<ComplexError>(
            ComplexError("Invalid format", 2, "Missing protocol"));
    }

    return DatabaseConnection(conn_str);
}

std_::expected<std::unique_ptr<int>, std::string> create_unique_ptr(int value)
{
    if (value < 0)
    {
        return std_::unexpected<std::string>("Negative value not allowed");
    }
    return std::make_unique<int>(value);
}

void demonstrate_conversions()
{
    std::cout << "\n=== Conversion Examples ===" << std::endl;

    std_::expected<int, std::string> int_exp = 42;
    std::cout << "Implicit conversion: " << *int_exp << std::endl;

    std_::expected<int, std::string> error_exp = std_::unexpected<std::string>("Error");
    if (!error_exp)
    {
        std::cout << "Error: " << error_exp.error() << std::endl;
    }
}

int main()
{
    std::cout << "Example 5: Advanced Features\n" << std::endl;

    auto conn_result = create_connection("postgresql://localhost/mydb");
    if (conn_result)
    {
        std::cout << "Connection info: " << conn_result->get_info() << std::endl;
    }
    else
    {
        std::cout << "Connection failed: " << conn_result.error().message << std::endl;
    }

    auto ptr_result = create_unique_ptr(100);
    if (ptr_result)
    {
        std::cout << "Created unique_ptr with value: " << **ptr_result << std::endl;
    }

    auto ptr_error = create_unique_ptr(-5);
    if (!ptr_error)
    {
        std::cout << "unique_ptr error: " << ptr_error.error() << std::endl;
    }

    std_::expected<int, std::string> default_exp;
    std::cout << "Default value: " << *default_exp << std::endl;

    std_::expected<int, std::string> value_exp = 123;
    std_::expected<int, std::string> error_exp =
        std_::unexpected<std::string>("Something went wrong");

    std::cout << "Value exp has value: " << value_exp.has_value() << std::endl;
    std::cout << "Error exp has value: " << error_exp.has_value() << std::endl;

    demonstrate_conversions();

    std_::expected<int, std::string> exp_a = 42;
    std_::expected<int, std::string> exp_b = 42;
    std_::expected<int, std::string> exp_c = 24;

    std::cout << "exp_a == exp_b: " << (exp_a == exp_b) << std::endl;
    std::cout << "exp_a == exp_c: " << (exp_a == exp_c) << std::endl;

    return 0;
}
