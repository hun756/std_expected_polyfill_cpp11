#include <expected/expected.hpp>

#include <iostream>
#include <string>
#include <vector>


int main()
{
    std::cout << "Example 8: transform_error, error_or, bad_expected_access, swap, "
                 "make_unexpected, emplace\n"
              << std::endl;

    auto u_int = std_::make_unexpected(42);
    std::cout << "make_unexpected deduced error type: " << u_int.error() << std::endl;

    std_::expected<int, std::string> ok = 10;
    std_::expected<int, std::string> err = std_::unexpected<std::string>("boom");

    std::cout << "ok.error_or(\"fallback\"): " << ok.error_or(std::string("fallback")) << std::endl;
    std::cout << "err.error_or(\"fallback\"): " << err.error_or(std::string("fallback"))
              << std::endl;

    auto ev = err.transform_error(
        [](const std::string& e)
        {
            return std::string("mapped: ") + e;
        });

    if (!ev)
    {
        std::cout << "transform_error produced error (as expected): " << ev.error() << std::endl;
    }

    std_::expected<int, std::string> a = 1;
    std_::expected<int, std::string> b = std_::unexpected<std::string>("err B");

    std::cout << "Before swap: a.has_value=" << a.has_value() << ", b.has_value=" << b.has_value()
              << std::endl;
    swap(a, b);
    std::cout << "After swap: a.has_value=" << a.has_value() << ", b.has_value=" << b.has_value()
              << std::endl;

    std_::unexpected<std::string> ue1("u1");
    std_::unexpected<std::string> ue2("u2");
    std::cout << "Before unexpected swap: ue1==ue2? " << (ue1 == ue2) << std::endl;
    swap(ue1, ue2);
    std::cout << "After unexpected swap: ue1.error()=" << ue1.error() << std::endl;

    std_::expected<std::vector<int>, std::string> vec_exp;
    vec_exp.emplace({1, 2, 3, 4});
    if (vec_exp)
    {
        std::cout << "Emplaced vector:";
        for (int v : *vec_exp)
            std::cout << ' ' << v;
        std::cout << std::endl;
    }

    std_::expected<int, std::string> will_throw = std_::unexpected<std::string>("fatal");
    try
    {
        (void)will_throw.value();
    }
    catch (const std_::bad_expected_access<std::string>& ex)
    {
        std::cout << "Caught bad_expected_access with error: " << ex.error() << std::endl;
    }
    catch (...)
    {
        std::cout << "Caught unknown exception from value()" << std::endl;
    }

    return 0;
}
