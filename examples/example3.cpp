#include <expected/expected.hpp>

#include <iostream>
#include <string>
#include <vector>


std_::expected<int, std::string> safe_divide(int a, int b)
{
    if (b == 0)
    {
        return std_::unexpected<std::string>("Division by zero");
    }
    return a / b;
}

std_::expected<std::string, std::string> int_to_string(int x)
{
    return std::to_string(x);
}

std_::expected<int, std::string> string_to_int(const std::string& str)
{
    try
    {
        return std::stoi(str);
    }
    catch (...)
    {
        return std_::unexpected<std::string>("Invalid number");
    }
}

std_::expected<std::vector<int>, std::string> create_vector(int size)
{
    if (size < 0)
    {
        return std_::unexpected<std::string>("Negative size not allowed");
    }
    return std::vector<int>(static_cast<size_t>(size), 0);
}

int main()
{
    std::cout << "Example 3: Monadic Operations\n" << std::endl;

    auto result1 = safe_divide(10, 2).and_then(int_to_string);
    if (result1)
    {
        std::cout << "and_then result: " << *result1 << std::endl;
    }

    auto chained = safe_divide(20, 4)
                       .and_then(
                           [](int x)
                           {
                               return safe_divide(x, 2);
                           })
                       .and_then(int_to_string);
    if (chained)
    {
        std::cout << "Chained and_then: " << *chained << std::endl;
    }

    auto error_chain = safe_divide(10, 0).and_then(int_to_string);
    if (!error_chain)
    {
        std::cout << "Error in chain: " << error_chain.error() << std::endl;
    }

    auto result2 = safe_divide(10, 0);
    if (!result2)
    {
        std::cout << "Handling error: " << result2.error() << std::endl;
        result2 = 42;
    }
    if (result2)
    {
        std::cout << "or_else result: " << *result2 << std::endl;
    }

    auto transformed = safe_divide(15, 3).transform(
        [](int x)
        {
            return x * 2;
        });
    if (transformed)
    {
        std::cout << "Transformed: " << *transformed << std::endl;
    }

    int default_val = safe_divide(10, 0).value_or(999);
    std::cout << "value_or with error: " << default_val << std::endl;

    int success_val = safe_divide(10, 2).value_or(999);
    std::cout << "value_or with success: " << success_val << std::endl;

    auto vec_result = create_vector(5).transform(
        [](std::vector<int> v)
        {
            for (size_t i = 0; i < v.size(); ++i)
            {
                v[i] = static_cast<int>(i) * 2;
            }
            return v;
        });

    if (vec_result)
    {
        std::cout << "Vector: ";
        for (int val : *vec_result)
        {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
