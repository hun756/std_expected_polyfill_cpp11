#include <expected/expected.hpp>

#include <iostream>
#include <string>


struct FileError
{
    std::string message;
    int error_code;

    FileError(const std::string& msg, int code) : message(msg), error_code(code) {}
};

std_::expected<std::string, FileError> read_file(const std::string& filename)
{
    if (filename.empty())
    {
        return std_::unexpected<FileError>(FileError("Filename cannot be empty", 1));
    }
    if (filename == "nonexistent.txt")
    {
        return std_::unexpected<FileError>(FileError("File not found", 2));
    }

    return "File content: " + filename;
}

std_::expected<int, std::string> parse_number(const std::string& str)
{
    try
    {
        return std::stoi(str);
    }
    catch (const std::exception&)
    {
        return std_::unexpected<std::string>("Invalid number format");
    }
}

int main()
{
    std::cout << "Example 2: Error Handling with Custom Types\n" << std::endl;

    auto file_result = read_file("test.txt");
    if (file_result)
    {
        std::cout << "File content: " << *file_result << std::endl;
    }
    else
    {
        std::cout << "File error: " << file_result.error().message
                  << " (code: " << file_result.error().error_code << ")" << std::endl;
    }

    auto error_result = read_file("");
    if (!error_result)
    {
        std::cout << "Error: " << error_result.error().message << std::endl;
    }

    auto num_result = parse_number("42");
    if (num_result)
    {
        std::cout << "Parsed number: " << *num_result << std::endl;
    }

    auto invalid_result = parse_number("abc");
    if (!invalid_result)
    {
        std::cout << "Parse error: " << invalid_result.error() << std::endl;
    }

    std_::unexpected<std::string> err1("Error 1");
    std_::unexpected<std::string> err2("Error 1");
    std_::unexpected<std::string> err3("Error 2");

    std::cout << "err1 == err2: " << (err1 == err2) << std::endl;
    std::cout << "err1 == err3: " << (err1 == err3) << std::endl;

    return 0;
}
