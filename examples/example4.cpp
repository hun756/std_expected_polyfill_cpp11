#include <expected/expected.hpp>

#include <iostream>
#include <string>


std_::expected<void, std::string> connect_to_database(const std::string& connection_string)
{
    if (connection_string.empty())
    {
        return std_::unexpected<std::string>("Connection string cannot be empty");
    }
    if (connection_string == "invalid")
    {
        return std_::unexpected<std::string>("Invalid connection string format");
    }

    std::cout << "Connected to database: " << connection_string << std::endl;
    return {};
}

std_::expected<void, std::string> execute_query(const std::string& query)
{
    if (query.empty())
    {
        return std_::unexpected<std::string>("Query cannot be empty");
    }
    if (query.find("DROP") != std::string::npos)
    {
        return std_::unexpected<std::string>("DROP statements are not allowed");
    }

    std::cout << "Executed query: " << query << std::endl;
    return {};
}

std_::expected<int, std::string> get_user_count()
{
    return 42;
}

int main()
{
    std::cout << "Example 4: expected<void, E> Specialization\n" << std::endl;

    auto connect_result = connect_to_database("postgresql://localhost/mydb");
    if (connect_result)
    {
        std::cout << "Connection successful!" << std::endl;
    }
    else
    {
        std::cout << "Connection failed: " << connect_result.error() << std::endl;
    }

    auto failed_connect = connect_to_database("");
    if (!failed_connect)
    {
        std::cout << "Expected error: " << failed_connect.error() << std::endl;
    }

    auto workflow = connect_to_database("postgresql://localhost/mydb")
                        .and_then(
                            []()
                            {
                                return execute_query("SELECT * FROM users");
                            })
                        .and_then(
                            []()
                            {
                                return execute_query("UPDATE users SET last_login = NOW()");
                            });

    if (workflow)
    {
        std::cout << "Workflow completed successfully!" << std::endl;
    }
    else
    {
        std::cout << "Workflow failed: " << workflow.error() << std::endl;
    }

    auto mixed = get_user_count().and_then(
        [](int count) -> std_::expected<void, std::string>
        {
            std::cout << "User count: " << count << std::endl;
            if (count > 100)
            {
                return std_::unexpected<std::string>("Too many users");
            }
            return {};
        });

    if (mixed)
    {
        std::cout << "Mixed operation successful!" << std::endl;
    }
    else
    {
        std::cout << "Mixed operation failed: " << mixed.error() << std::endl;
    }

    std_::expected<void, std::string> void_success = {};
    std_::expected<void, std::string> void_error = std_::unexpected<std::string>("Some error");

    std::cout << "void_success has_value: " << void_success.has_value() << std::endl;
    std::cout << "void_error has_value: " << void_error.has_value() << std::endl;

    return 0;
}
