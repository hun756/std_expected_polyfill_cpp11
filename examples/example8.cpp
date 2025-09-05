#include <expected/expected.hpp>

#include <iostream>
#include <string>
#include <utility>
#include <vector>


int main()
{
    std::cout << "Example 9: in_place, initializer_list and comparisons\n" << std::endl;

    std_::expected<std::pair<int, int>, std::string> p(std_::detail::in_place, 3, 4);
    if (p)
    {
        std::cout << "Pair in-place: (" << p->first << ", " << p->second << ")" << std::endl;
    }

    std_::expected<std::vector<int>, std::string> v(std_::detail::in_place, {10, 20, 30});
    if (v)
    {
        std::cout << "Vector in-place:";
        for (int x : *v)
            std::cout << ' ' << x;
        std::cout << std::endl;
    }

    std_::unexpected<std::vector<int>> ue(std_::detail::in_place, {7, 8, 9});
    std::cout << "unexpected holds vector of size: " << ue.error().size() << std::endl;

    std_::expected<int, std::string> e = 42;
    if (e == 42)
    {
        std::cout << "expected equals raw value 42" << std::endl;
    }

    std_::expected<int, std::string> e_err = std_::unexpected<std::string>("oops");
    std_::unexpected<std::string> ue2("oops");
    if (e_err == ue2)
    {
        std::cout << "expected error equals unexpected value\n";
    }

    return 0;
}
