#ifndef LIB_STD_EXPECTED_POLYFILL_CPP11_HPP_ztk3ue
#define LIB_STD_EXPECTED_POLYFILL_CPP11_HPP_ztk3ue

#include <type_traits>

namespace std
{

template <class E>
class unexpected;

template <class T, class E>
class expected;

namespace detail
{

template <class T>
struct is_unexpected_impl : false_type
{
};

template <class E>
struct is_unexpected_impl<unexpected<E>> : true_type
{
};

template <class T>
struct is_unexpected : is_unexpected_impl<typename decay<T>::type>
{
};

template <class T>
struct is_expected_impl : false_type
{
};

template <class T, class E>
struct is_expected_impl<expected<T, E>> : true_type
{
};

template <class T>
struct is_expected : is_expected_impl<typename decay<T>::type>
{
};

template <class T>
struct remove_cvref
{
    typedef typename remove_cv<typename remove_reference<T>::type>::type type;
};

struct in_place_t
{
    explicit in_place_t() = default;
};
constexpr in_place_t in_place{};

template <class T>
struct in_place_type_t
{
    explicit in_place_type_t() = default;
};


}  // namespace detail

}  // namespace std

#endif  // End of include guard: LIB_STD_EXPECTED_POLYFILL_CPP11_HPP_ztk3ue
