#include <expected/expected.hpp>

#include <iostream>
#include <string>


std_::expected<int, std::string> divide(int a, int b)
{
    if (b == 0)
    {
        return std_::unexpected<std::string>("Division by zero");
    }
    return a / b;
}

int main()
{
    std::cout << "Example 1: Basic Usage of std::expected\n" << std::endl;

    auto result1 = divide(10, 2);
    if (result1.has_value())
    {
        std::cout << "Result: " << result1.value() << std::endl;
    }
    else
    {
        std::cout << "Error: " << result1.error() << std::endl;
    }

    auto result2 = divide(10, 0);
    if (result2.has_value())
    {
        std::cout << "Result: " << result2.value() << std::endl;
    }
    else
    {
        std::cout << "Error: " << result2.error() << std::endl;
    }

    if (result1)
    {
        std::cout << "Using operator*: " << *result1 << std::endl;
    }

    return 0;
}
