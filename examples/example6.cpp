#include <expected/expected.hpp>

#include <iostream>
#include <string>
#include <vector>


std_::unexpected<std::string> make_error(const std::string& msg)
{
    return std_::unexpected<std::string>(msg);
}

std_::expected<int, std::string> safe_operation(bool should_fail = false)
{
    if (should_fail)
    {
        return make_error("Operation failed");
    }
    return 42;
}

class Resource
{
public:
    Resource()
    {
        std::cout << "Resource acquired" << std::endl;
    }
    ~Resource()
    {
        std::cout << "Resource released" << std::endl;
    }

    void use() const
    {
        std::cout << "Resource used" << std::endl;
    }
};

std_::expected<void, std::string> use_resource_safely(bool should_fail = false)
{
    Resource res;

    if (should_fail)
    {
        return make_error("Failed to use resource");
    }

    res.use();
    return {};
}

std_::expected<std::vector<int>, std::string> process_data(const std::vector<int>& data)
{
    if (data.empty())
    {
        return make_error("Data cannot be empty");
    }

    std::vector<int> result;
    for (int val : data)
    {
        if (val < 0)
        {
            return make_error("Negative values not allowed");
        }
        result.push_back(val * 2);
    }

    return result;
}

std_::expected<std::string, std::string> format_result(const std::vector<int>& data)
{
    std::string result = "Processed: ";
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (i > 0)
            result += ", ";
        result += std::to_string(data[i]);
    }
    return result;
}

std_::expected<std::string, std::string> process_and_format(const std::vector<int>& input)
{
    return process_data(input).and_then(format_result);
}

std_::expected<int, std::string> recover_from_error(const std::string& error_msg)
{
    if (error_msg == "Network timeout")
    {
        return 0;
    }
    else if (error_msg == "Invalid input")
    {
        return make_error("Unrecoverable error");
    }
    return make_error("Unknown error: " + error_msg);
}

void demonstrate_const_correctness()
{
    const std_::expected<int, std::string> const_exp = 42;
    const std_::expected<int, std::string> const_error = make_error("const error");

    std::cout << "const_exp.has_value(): " << const_exp.has_value() << std::endl;
    if (const_exp)
    {
        std::cout << "const_exp.value(): " << const_exp.value() << std::endl;
        std::cout << "const_exp.operator*(): " << *const_exp << std::endl;
    }

    if (!const_error)
    {
        std::cout << "const_error.error(): " << const_error.error() << std::endl;
    }
}

int main()
{
    std::cout << "Example 7: Utility Functions and Advanced Patterns\n" << std::endl;

    auto unexp = std_::make_unexpected<std::string>("Custom error");
    std_::expected<int, std::string> exp_from_unexp = unexp;
    std::cout << "make_unexpected: " << exp_from_unexp.error() << std::endl;

    std::cout << "\n=== Exception Safety ===" << std::endl;
    auto resource_result = use_resource_safely(false);
    if (resource_result)
    {
        std::cout << "Resource used successfully" << std::endl;
    }

    auto failed_resource = use_resource_safely(true);
    if (!failed_resource)
    {
        std::cout << "Resource usage failed: " << failed_resource.error() << std::endl;
    }

    std::cout << "\n=== Pipeline Pattern ===" << std::endl;
    std::vector<int> test_data = {1, 2, 3, 4, 5};
    auto pipeline_result = process_and_format(test_data);
    if (pipeline_result)
    {
        std::cout << "Pipeline result: " << *pipeline_result << std::endl;
    }

    auto initial_result = safe_operation(true);
    if (!initial_result)
    {
        std::cout << "Initial error: " << initial_result.error() << std::endl;

        auto recovery_result = safe_operation(false);
        if (recovery_result)
        {
            std::cout << "Recovered value: " << *recovery_result << std::endl;
        }
    }

    std::vector<int> bad_data = {1, -2, 3};
    auto failed_pipeline = process_and_format(bad_data);
    if (!failed_pipeline)
    {
        std::cout << "Pipeline failed: " << failed_pipeline.error() << std::endl;
    }

    std::cout << "\n=== Const Correctness ===" << std::endl;
    demonstrate_const_correctness();

    auto first_attempt = safe_operation(true);
    if (!first_attempt)
    {
        std::cout << "First attempt failed: " << first_attempt.error() << std::endl;
        auto second_attempt = safe_operation(false);
        if (second_attempt)
        {
            auto final_result = *second_attempt * 2;
            std::cout << "Final result after recovery: " << final_result << std::endl;
        }
    }

    return 0;
}
