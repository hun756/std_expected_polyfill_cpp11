#ifndef LIB_STD_EXPECTED_POLYFILL_CPP11_HPP_ztk3ue
#define LIB_STD_EXPECTED_POLYFILL_CPP11_HPP_ztk3ue

#include <exception>
#include <type_traits>
#include <utility>

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

template <bool TriviallyDestructible>
struct expected_destructor_base
{
    bool has_val;
    constexpr expected_destructor_base() : has_val(true) {}
    constexpr expected_destructor_base(bool b) : has_val(b) {}
};

template <>
struct expected_destructor_base<false>
{
    bool has_val;
    constexpr expected_destructor_base() : has_val(true) {}
    constexpr expected_destructor_base(bool b) : has_val(b) {}

    ~expected_destructor_base() {}
};

template <class T,
          class E,
          bool = is_trivially_destructible<T>::value && is_trivially_destructible<E>::value>
struct expected_storage_base : expected_destructor_base<false>
{
    using base = expected_destructor_base<false>;
    union
    {
        T val;
        E err;
    };

    template <class... Args>
    constexpr expected_storage_base(in_place_t, Args&&... args)
        : base(true), val(forward<Args>(args)...)
    {
    }

    template <class... Args>
    constexpr expected_storage_base(in_place_type_t<unexpected<E>>, Args&&... args)
        : base(false), err(forward<Args>(args)...)
    {
    }

    ~expected_storage_base()
    {
        if (this->has_val)
        {
            val.~T();
        }
        else
        {
            err.~E();
        }
    }
};

template <class T, class E>
struct expected_storage_base<T, E, true> : expected_destructor_base<true>
{
    using base = expected_destructor_base<true>;
    union
    {
        T val;
        E err;
    };

    template <class... Args>
    constexpr expected_storage_base(in_place_t, Args&&... args)
        : base(true), val(forward<Args>(args)...)
    {
    }

    template <class... Args>
    constexpr expected_storage_base(in_place_type_t<unexpected<E>>, Args&&... args)
        : base(false), err(forward<Args>(args)...)
    {
    }
};

template <class E, bool = is_trivially_destructible<E>::value>
struct expected_void_storage_base : expected_destructor_base<false>
{
    using base = expected_destructor_base<false>;
    union
    {
        char dummy;
        E err;
    };

    constexpr expected_void_storage_base() : base(true), dummy() {}

    template <class... Args>
    constexpr expected_void_storage_base(in_place_type_t<unexpected<E>>, Args&&... args)
        : base(false), err(forward<Args>(args)...)
    {
    }

    ~expected_void_storage_base()
    {
        if (!this->has_val)
        {
            err.~E();
        }
    }
};

template <class E>
struct expected_void_storage_base<E, true> : expected_destructor_base<true>
{
    using base = expected_destructor_base<true>;
    union
    {
        char dummy;
        E err;
    };

    constexpr expected_void_storage_base() : base(true), dummy() {}

    template <class... Args>
    constexpr expected_void_storage_base(in_place_type_t<unexpected<E>>, Args&&... args)
        : base(false), err(forward<Args>(args)...)
    {
    }
};

template <class T>
struct is_nothrow_swappable
{
    template <class U>
    static auto test(int) -> decltype(swap(declval<U&>(), declval<U&>()), true_type{});
    template <class>
    static false_type test(...);

    static constexpr bool value = noexcept(swap(declval<T&>(), declval<T&>()));
};

}  // namespace detail

template <class E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public exception
{
public:
    bad_expected_access() = default;

    const char* what() const noexcept override
    {
        return "bad expected access";
    }
};

template <class E>
class bad_expected_access : public bad_expected_access<void>
{
public:
    explicit bad_expected_access(E e) : err(move(e)) {}

    const E& error() const& noexcept
    {
        return err;
    }

    E& error() & noexcept
    {
        return err;
    }

    const E&& error() const&& noexcept
    {
        return move(err);
    }

    E&& error() && noexcept
    {
        return move(err);
    }

private:
    E err;
};

template <class E>
class unexpected
{
public:
    static_assert(!is_same<E, void>::value, "E must not be void");
    static_assert(!is_reference<E>::value, "E must not be a reference");
    static_assert(!detail::is_unexpected<E>::value, "E must not be unexpected (no nesting)");

    template <class Err = E>
    constexpr explicit unexpected(Err&& e) noexcept(is_nothrow_constructible<E, Err&&>::value)
        : val(forward<Err>(e))
    {
    }

    template <class... Args>
    constexpr explicit unexpected(detail::in_place_t, Args&&... args) noexcept(
        is_nothrow_constructible<E, Args...>::value)
        : val(forward<Args>(args)...)
    {
    }

    constexpr const E& error() const& noexcept
    {
        return val;
    }

    constexpr E& error() & noexcept
    {
        return val;
    }

    constexpr const E&& error() const&& noexcept
    {
        return move(val);
    }

    constexpr E&& error() && noexcept
    {
        return move(val);
    }

    constexpr void swap(unexpected& other) noexcept(detail::is_nothrow_swappable<E>::value)
    {
        using std::swap;
        swap(val, other.val);
    }

private:
    E val;
};


template <class E1, class E2>
constexpr bool operator==(const unexpected<E1>& lhs, const unexpected<E2>& rhs)
{
    return lhs.error() == rhs.error();
}

template <class E1, class E2>
constexpr bool operator!=(const unexpected<E1>& lhs, const unexpected<E2>& rhs)
{
    return !(lhs == rhs);
}

template <class E>
constexpr void swap(unexpected<E>& lhs, unexpected<E>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

template <class E>
constexpr unexpected<typename decay<E>::type> make_unexpected(E&& e)
{
    return unexpected<typename decay<E>::type>(forward<E>(e));
}

}  // namespace std

#endif  // End of include guard: LIB_STD_EXPECTED_POLYFILL_CPP11_HPP_ztk3ue
