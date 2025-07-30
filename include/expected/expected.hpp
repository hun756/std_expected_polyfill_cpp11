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
        : base(true), val(std::forward<Args>(args)...)
    {
    }

    template <class... Args>
    constexpr expected_storage_base(in_place_type_t<unexpected<E>>, Args&&... args)
        : base(false), err(std::forward<Args>(args)...)
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
        : base(true), val(std::forward<Args>(args)...)
    {
    }

    template <class... Args>
    constexpr expected_storage_base(in_place_type_t<unexpected<E>>, Args&&... args)
        : base(false), err(std::forward<Args>(args)...)
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
        : base(false), err(std::forward<Args>(args)...)
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
        : base(false), err(std::forward<Args>(args)...)
    {
    }
};

template <class T, class E>
using expected_storage = expected_storage_base<T, E>;

template <class E>
using expected_void_storage = expected_void_storage_base<E>;


template <class T, class E>
struct expected_copy_base : expected_storage<T, E>
{
    using expected_storage<T, E>::expected_storage;

    expected_copy_base() = default;
    expected_copy_base(const expected_copy_base&) = default;
    expected_copy_base(expected_copy_base&&) = default;
    expected_copy_base& operator=(const expected_copy_base&) = default;
    expected_copy_base& operator=(expected_copy_base&&) = default;

    template <
        bool Dummy = true,
        class = typename enable_if<
            Dummy && (!is_copy_constructible<T>::value || !is_copy_constructible<E>::value)>::type>
    expected_copy_base(const expected_copy_base&) = delete;
};

template <class E>
struct expected_void_copy_base : expected_void_storage<E>
{
    using expected_void_storage<E>::expected_void_storage;

    expected_void_copy_base() = default;
    expected_void_copy_base(const expected_void_copy_base&) = default;
    expected_void_copy_base(expected_void_copy_base&&) = default;
    expected_void_copy_base& operator=(const expected_void_copy_base&) = default;
    expected_void_copy_base& operator=(expected_void_copy_base&&) = default;

    template <bool Dummy = true,
              class = typename enable_if<Dummy && !is_copy_constructible<E>::value>::type>
    expected_void_copy_base(const expected_void_copy_base&) = delete;
};

template <class T, class E>
struct expected_move_base : expected_copy_base<T, E>
{
    using expected_copy_base<T, E>::expected_copy_base;

    expected_move_base() = default;
    expected_move_base(const expected_move_base&) = default;
    expected_move_base(expected_move_base&&) = default;
    expected_move_base& operator=(const expected_move_base&) = default;
    expected_move_base& operator=(expected_move_base&&) = default;

    template <
        bool Dummy = true,
        class = typename enable_if<
            Dummy && (!is_move_constructible<T>::value || !is_move_constructible<E>::value)>::type>
    expected_move_base(expected_move_base&&) = delete;
};

template <class E>
struct expected_void_move_base : expected_void_copy_base<E>
{
    using expected_void_copy_base<E>::expected_void_copy_base;

    expected_void_move_base() = default;
    expected_void_move_base(const expected_void_move_base&) = default;
    expected_void_move_base(expected_void_move_base&&) = default;
    expected_void_move_base& operator=(const expected_void_move_base&) = default;
    expected_void_move_base& operator=(expected_void_move_base&&) = default;

    template <bool Dummy = true,
              class = typename enable_if<Dummy && !is_move_constructible<E>::value>::type>
    expected_void_move_base(expected_void_move_base&&) = delete;
};

template <class T, class E>
struct expected_copy_assign_base : expected_move_base<T, E>
{
    using expected_move_base<T, E>::expected_move_base;

    expected_copy_assign_base() = default;
    expected_copy_assign_base(const expected_copy_assign_base&) = default;
    expected_copy_assign_base(expected_copy_assign_base&&) = default;
    expected_copy_assign_base& operator=(const expected_copy_assign_base&) = default;
    expected_copy_assign_base& operator=(expected_copy_assign_base&&) = default;

    template <bool Dummy = true,
              class = typename enable_if<
                  Dummy
                  && (!is_copy_assignable<T>::value || !is_copy_constructible<T>::value
                      || !is_copy_assignable<E>::value || !is_copy_constructible<E>::value)>::type>
    expected_copy_assign_base& operator=(const expected_copy_assign_base&) = delete;
};

template <class E>
struct expected_void_copy_assign_base : expected_void_move_base<E>
{
    using expected_void_move_base<E>::expected_void_move_base;

    expected_void_copy_assign_base() = default;
    expected_void_copy_assign_base(const expected_void_copy_assign_base&) = default;
    expected_void_copy_assign_base(expected_void_copy_assign_base&&) = default;
    expected_void_copy_assign_base& operator=(const expected_void_copy_assign_base&) = default;
    expected_void_copy_assign_base& operator=(expected_void_copy_assign_base&&) = default;

    template <
        bool Dummy = true,
        class = typename enable_if<
            Dummy && (!is_copy_assignable<E>::value || !is_copy_constructible<E>::value)>::type>
    expected_void_copy_assign_base& operator=(const expected_void_copy_assign_base&) = delete;
};

template <class T, class E>
struct expected_move_assign_base : expected_copy_assign_base<T, E>
{
    using expected_copy_assign_base<T, E>::expected_copy_assign_base;

    expected_move_assign_base() = default;
    expected_move_assign_base(const expected_move_assign_base&) = default;
    expected_move_assign_base(expected_move_assign_base&&) = default;
    expected_move_assign_base& operator=(const expected_move_assign_base&) = default;
    expected_move_assign_base& operator=(expected_move_assign_base&&) = default;

    template <bool Dummy = true,
              class = typename enable_if<
                  Dummy
                  && (!is_move_assignable<T>::value || !is_move_constructible<T>::value
                      || !is_move_assignable<E>::value || !is_move_constructible<E>::value)>::type>
    expected_move_assign_base& operator=(expected_move_assign_base&&) = delete;
};

template <class E>
struct expected_void_move_assign_base : expected_void_copy_assign_base<E>
{
    using expected_void_copy_assign_base<E>::expected_void_copy_assign_base;

    expected_void_move_assign_base() = default;
    expected_void_move_assign_base(const expected_void_move_assign_base&) = default;
    expected_void_move_assign_base(expected_void_move_assign_base&&) = default;
    expected_void_move_assign_base& operator=(const expected_void_move_assign_base&) = default;
    expected_void_move_assign_base& operator=(expected_void_move_assign_base&&) = default;

    template <
        bool Dummy = true,
        class = typename enable_if<
            Dummy && (!is_move_assignable<E>::value || !is_move_constructible<E>::value)>::type>
    expected_void_move_assign_base& operator=(expected_void_move_assign_base&&) = delete;
};

template <class T, class E>
using expected_base = expected_move_assign_base<T, E>;

template <class E>
using expected_void_base = expected_void_move_assign_base<E>;

template <class T>
struct is_nothrow_swappable
{
    template <class U>
    static auto test(int) -> decltype(swap(declval<U&>(), declval<U&>()), true_type{});
    template <class>
    static false_type test(...);

    static constexpr bool value = noexcept(swap(declval<T&>(), declval<T&>()));
};

template <class T, class U>
struct is_same_decayed : is_same<typename decay<T>::type, typename decay<U>::type>
{
};

template <typename F, typename... Args>
struct invoke_result
{
    typedef decltype(std::declval<F>()(std::declval<Args>()...)) type;
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
    explicit bad_expected_access(E e) : err(std::move(e)) {}

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
        return std::move(err);
    }

    E&& error() && noexcept
    {
        return std::move(err);
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
        : val(std::forward<Err>(e))
    {
    }

    template <class... Args>
    constexpr explicit unexpected(detail::in_place_t, Args&&... args) noexcept(
        is_nothrow_constructible<E, Args...>::value)
        : val(std::forward<Args>(args)...)
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
        return std::move(val);
    }

    constexpr E&& error() && noexcept
    {
        return std::move(val);
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
    return unexpected<typename decay<E>::type>(std::forward<E>(e));
}

template <class T, class E>
class expected : private detail::expected_base<T, E>
{
    static_assert(!is_reference<T>::value, "T must not be a reference");
    static_assert(!is_function<T>::value, "T must not be a function");
    static_assert(!is_same<typename detail::remove_cvref<T>::type, detail::in_place_t>::value,
                  "T must not be in_place_t");
    static_assert(!detail::is_unexpected<typename detail::remove_cvref<T>::type>::value,
                  "T must not be unexpected");
    static_assert(!is_reference<E>::value, "E must not be a reference");
    static_assert(!is_function<E>::value, "E must not be a function");

    using base = detail::expected_base<T, E>;

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    template <class U>
    using rebind = expected<U, error_type>;

    constexpr expected() noexcept(is_nothrow_default_constructible<T>::value)
        : base(detail::in_place)
    {
    }

    constexpr expected(const expected&) = default;
    constexpr expected(expected&&) = default;


    template <class U, class G>
    constexpr expected(
        const expected<U, G>& rhs,
        typename enable_if<
            is_constructible<T, const U&>::value && is_constructible<E, const G&>::value
            && !is_constructible<T, expected<U, G>&>::value
            && !is_constructible<T, expected<U, G>>::value
            && !is_constructible<T, const expected<U, G>&>::value
            && !is_constructible<T, const expected<U, G>>::value
            && !is_convertible<expected<U, G>&, T>::value
            && !is_convertible<expected<U, G>, T>::value
            && !is_convertible<const expected<U, G>&, T>::value
            && !is_convertible<const expected<U, G>, T>::value
            && (is_convertible<const U&, T>::value && is_convertible<const G&, E>::value)>::type* =
            nullptr)
    {
        if (rhs.has_value())
        {
            ::new (static_cast<base*>(this)) base(detail::in_place, *rhs);
        }
        else
        {
            ::new (static_cast<base*>(this))
                base(detail::in_place_type_t<unexpected_type>{}, rhs.error());
        }
    }

    template <class U, class G>
    constexpr explicit expected(
        const expected<U, G>& rhs,
        typename enable_if<is_constructible<T, const U&>::value
                           && is_constructible<E, const G&>::value
                           && !is_constructible<T, expected<U, G>&>::value
                           && !is_constructible<T, expected<U, G>>::value
                           && !is_constructible<T, const expected<U, G>&>::value
                           && !is_constructible<T, const expected<U, G>>::value
                           && !is_convertible<expected<U, G>&, T>::value
                           && !is_convertible<expected<U, G>, T>::value
                           && !is_convertible<const expected<U, G>&, T>::value
                           && !is_convertible<const expected<U, G>, T>::value
                           && (!is_convertible<const U&, T>::value
                               || !is_convertible<const G&, E>::value)>::type* = nullptr)
    {
        if (rhs.has_value())
        {
            ::new (static_cast<base*>(this)) base(detail::in_place, *rhs);
        }
        else
        {
            ::new (static_cast<base*>(this))
                base(detail::in_place_type_t<unexpected_type>{}, rhs.error());
        }
    }

    template <class U, class G>
    constexpr expected(
        expected<U, G>&& rhs,
        typename enable_if<is_constructible<T, U&&>::value && is_constructible<E, G&&>::value
                           && !is_constructible<T, expected<U, G>&>::value
                           && !is_constructible<T, expected<U, G>>::value
                           && !is_constructible<T, const expected<U, G>&>::value
                           && !is_constructible<T, const expected<U, G>>::value
                           && !is_convertible<expected<U, G>&, T>::value
                           && !is_convertible<expected<U, G>, T>::value
                           && !is_convertible<const expected<U, G>&, T>::value
                           && !is_convertible<const expected<U, G>, T>::value
                           && (is_convertible<U&&, T>::value
                               && is_convertible<G&&, E>::value)>::type* = nullptr)
    {
        if (rhs.has_value())
        {
            ::new (static_cast<base*>(this)) base(detail::in_place, std::move(*rhs));
        }
        else
        {
            ::new (static_cast<base*>(this))
                base(detail::in_place_type_t<unexpected_type>{}, std::move(rhs.error()));
        }
    }

    template <class U, class G>
    constexpr explicit expected(
        expected<U, G>&& rhs,
        typename enable_if<is_constructible<T, U&&>::value && is_constructible<E, G&&>::value
                           && !is_constructible<T, expected<U, G>&>::value
                           && !is_constructible<T, expected<U, G>>::value
                           && !is_constructible<T, const expected<U, G>&>::value
                           && !is_constructible<T, const expected<U, G>>::value
                           && !is_convertible<expected<U, G>&, T>::value
                           && !is_convertible<expected<U, G>, T>::value
                           && !is_convertible<const expected<U, G>&, T>::value
                           && !is_convertible<const expected<U, G>, T>::value
                           && (!is_convertible<U&&, T>::value
                               || !is_convertible<G&&, E>::value)>::type* = nullptr)
    {
        if (rhs.has_value())
        {
            ::new (static_cast<base*>(this)) base(detail::in_place, std::move(*rhs));
        }
        else
        {
            ::new (static_cast<base*>(this))
                base(detail::in_place_type_t<unexpected_type>{}, std::move(rhs.error()));
        }
    }

    template <class U = T>
    constexpr expected(
        U&& v,
        typename enable_if<!detail::is_same_decayed<U, expected>::value
                           && !detail::is_same_decayed<U, detail::in_place_t>::value
                           && !detail::is_unexpected<typename detail::remove_cvref<U>::type>::value
                           && is_constructible<T, U&&>::value
                           && is_convertible<U&&, T>::value>::type* = nullptr)
        : base(detail::in_place, std::forward<U>(v))
    {
    }

    template <class U = T>
    constexpr explicit expected(
        U&& v,
        typename enable_if<!detail::is_same_decayed<U, expected>::value
                           && !detail::is_same_decayed<U, detail::in_place_t>::value
                           && !detail::is_unexpected<typename detail::remove_cvref<U>::type>::value
                           && is_constructible<T, U&&>::value
                           && !is_convertible<U&&, T>::value>::type* = nullptr)
        : base(detail::in_place, std::forward<U>(v))
    {
    }

    template <class G = E>
    constexpr expected(const unexpected<G>& e,
                       typename enable_if<is_constructible<E, const G&>::value
                                          && is_convertible<const G&, E>::value>::type* = nullptr)
        : base(detail::in_place_type_t<unexpected_type>{}, e.error())
    {
    }

    template <class G = E>
    constexpr explicit expected(
        const unexpected<G>& e,
        typename enable_if<is_constructible<E, const G&>::value
                           && !is_convertible<const G&, E>::value>::type* = nullptr)
        : base(detail::in_place_type_t<unexpected_type>{}, e.error())
    {
    }

    template <class G = E>
    constexpr expected(unexpected<G>&& e,
                       typename enable_if<is_constructible<E, G&&>::value
                                          && is_convertible<G&&, E>::value>::type* = nullptr)
        : base(detail::in_place_type_t<unexpected_type>{}, std::move(e.error()))
    {
    }

    template <class G = E>
    constexpr explicit expected(
        unexpected<G>&& e,
        typename enable_if<is_constructible<E, G&&>::value
                           && !is_convertible<G&&, E>::value>::type* = nullptr)
        : base(detail::in_place_type_t<unexpected_type>{}, std::move(e.error()))
    {
    }

    template <class... Args>
    constexpr explicit expected(detail::in_place_t, Args&&... args) noexcept(
        is_nothrow_constructible<T, Args...>::value)
        : base(detail::in_place, std::forward<Args>(args)...)
    {
    }

    template <class U, class... Args>
    constexpr explicit expected(
        detail::in_place_t,
        initializer_list<U> il,
        Args&&... args) noexcept(is_nothrow_constructible<T, initializer_list<U>&, Args...>::value)
        : base(detail::in_place, il, std::forward<Args>(args)...)
    {
    }

    template <class... Args>
    constexpr explicit expected(detail::in_place_type_t<unexpected_type>, Args&&... args) noexcept(
        is_nothrow_constructible<E, Args...>::value)
        : base(detail::in_place_type_t<unexpected_type>{}, std::forward<Args>(args)...)
    {
    }

    template <class U, class... Args>
    constexpr explicit expected(
        detail::in_place_type_t<unexpected_type>,
        initializer_list<U> il,
        Args&&... args) noexcept(is_nothrow_constructible<E, initializer_list<U>&, Args...>::value)
        : base(detail::in_place_type_t<unexpected_type>{}, il, std::forward<Args>(args)...)
    {
    }

    expected& operator=(const expected&) = default;
    expected& operator=(expected&&) = default;

    template <class U = T>
    typename enable_if<!detail::is_same_decayed<U, expected>::value
                           && !detail::is_unexpected<typename detail::remove_cvref<U>::type>::value
                           && is_constructible<T, U>::value && is_assignable<T&, U>::value
                           && (is_nothrow_constructible<T, U>::value
                               || is_nothrow_move_constructible<T>::value
                               || is_nothrow_move_constructible<E>::value),
                       expected&>::type
    operator=(U&& v)
    {
        if (has_value())
        {
            this->val = std::forward<U>(v);
        }
        else
        {
            if (is_nothrow_constructible<T, U>::value)
            {
                this->err.~E();
                ::new (static_cast<void*>(std::addressof(this->val))) T(std::forward<U>(v));
                this->has_val = true;
            }
            else if (is_nothrow_move_constructible<T>::value)
            {
                T tmp(std::forward<U>(v));
                this->err.~E();
                ::new (static_cast<void*>(std::addressof(this->val))) T(std::move(tmp));
                this->has_val = true;
            }
            else
            {
                E tmp(std::move(this->err));
                this->err.~E();
                try
                {
                    ::new (static_cast<void*>(std::addressof(this->val))) T(std::forward<U>(v));
                    this->has_val = true;
                }
                catch (...)
                {
                    ::new (static_cast<void*>(std::addressof(this->err))) E(std::move(tmp));
                    throw;
                }
            }
        }
        return *this;
    }

    template <class G>
    typename enable_if<is_constructible<E, const G&>::value && is_assignable<E&, const G&>::value
                           && (is_nothrow_constructible<E, const G&>::value
                               || is_nothrow_move_constructible<T>::value
                               || is_nothrow_move_constructible<E>::value),
                       expected&>::type
    operator=(const unexpected<G>& e)
    {
        if (has_value())
        {
            if (is_nothrow_constructible<E, const G&>::value)
            {
                this->val.~T();
                ::new (static_cast<void*>(std::addressof(this->err))) E(e.error());
                this->has_val = false;
            }
            else if (is_nothrow_move_constructible<E>::value)
            {
                E tmp(e.error());
                this->val.~T();
                ::new (static_cast<void*>(std::addressof(this->err))) E(std::move(tmp));
                this->has_val = false;
            }
            else
            {
                T tmp(std::move(this->val));
                this->val.~T();
                try
                {
                    ::new (static_cast<void*>(std::addressof(this->err))) E(e.error());
                    this->has_val = false;
                }
                catch (...)
                {
                    ::new (static_cast<void*>(std::addressof(this->val))) T(std::move(tmp));
                    throw;
                }
            }
        }
        else
        {
            this->err = e.error();
        }
        return *this;
    }


    template <class G>
    typename enable_if<is_constructible<E, G&&>::value && is_assignable<E&, G&&>::value
                           && (is_nothrow_constructible<E, G&&>::value
                               || is_nothrow_move_constructible<T>::value
                               || is_nothrow_move_constructible<E>::value),
                       expected&>::type
    operator=(unexpected<G>&& e)
    {
        if (has_value())
        {
            if (is_nothrow_constructible<E, G&&>::value)
            {
                this->val.~T();
                ::new (static_cast<void*>(std::addressof(this->err))) E(std::move(e.error()));
                this->has_val = false;
            }
            else if (is_nothrow_move_constructible<E>::value)
            {
                E tmp(std::move(e.error()));
                this->val.~T();
                ::new (static_cast<void*>(std::addressof(this->err))) E(std::move(tmp));
                this->has_val = false;
            }
            else
            {
                T tmp(std::move(this->val));
                this->val.~T();
                try
                {
                    ::new (static_cast<void*>(std::addressof(this->err))) E(std::move(e.error()));
                    this->has_val = false;
                }
                catch (...)
                {
                    ::new (static_cast<void*>(std::addressof(this->val))) T(std::move(tmp));
                    throw;
                }
            }
        }
        else
        {
            this->err = std::move(e.error());
        }
        return *this;
    }

    constexpr void swap(expected& rhs) noexcept(is_nothrow_move_constructible<T>::value
                                                && detail::is_nothrow_swappable<T>::value
                                                && is_nothrow_move_constructible<E>::value
                                                && detail::is_nothrow_swappable<E>::value)
    {
        if (has_value() && rhs.has_value())
        {
            using std::swap;
            swap(this->val, rhs.val);
        }
        else if (!has_value() && !rhs.has_value())
        {
            using std::swap;
            swap(this->err, rhs.err);
        }
        else if (has_value() && !rhs.has_value())
        {
            if (is_nothrow_move_constructible<E>::value)
            {
                E tmp(std::move(rhs.err));
                rhs.err.~E();
                try
                {
                    ::new (static_cast<void*>(std::addressof(rhs.val))) T(std::move(this->val));
                    this->val.~T();
                    ::new (static_cast<void*>(std::addressof(this->err))) E(std::move(tmp));
                    swap(this->has_val, rhs.has_val);
                }
                catch (...)
                {
                    ::new (static_cast<void*>(std::addressof(rhs.err))) E(std::move(tmp));
                    throw;
                }
            }
            else
            {
                T tmp(std::move(this->val));
                this->val.~T();
                try
                {
                    ::new (static_cast<void*>(std::addressof(this->err))) E(std::move(rhs.err));
                    rhs.err.~E();
                    ::new (static_cast<void*>(std::addressof(rhs.val))) T(std::move(tmp));
                    swap(this->has_val, rhs.has_val);
                }
                catch (...)
                {
                    ::new (static_cast<void*>(std::addressof(this->val))) T(std::move(tmp));
                    throw;
                }
            }
        }
        else
        {
            rhs.swap(*this);
        }
    }

    constexpr const T* operator->() const noexcept
    {
        return std::addressof(this->val);
    }

    constexpr T* operator->() noexcept
    {
        return std::addressof(this->val);
    }

    constexpr const T& operator*() const& noexcept
    {
        return this->val;
    }

    constexpr T& operator*() & noexcept
    {
        return this->val;
    }

    constexpr const T&& operator*() const&& noexcept
    {
        return std::move(this->val);
    }

    constexpr T&& operator*() && noexcept
    {
        return std::move(this->val);
    }

    constexpr explicit operator bool() const noexcept
    {
        return this->has_val;
    }

    constexpr bool has_value() const noexcept
    {
        return this->has_val;
    }

    constexpr const T& value() const&
    {
        if (!has_value())
            throw bad_expected_access<E>(error());
        return this->val;
    }

    constexpr T& value() &
    {
        if (!has_value())
            throw bad_expected_access<E>(error());
        return this->val;
    }

    constexpr const T&& value() const&&
    {
        if (!has_value())
            throw bad_expected_access<E>(std::move(error()));
        return std::move(this->val);
    }

    constexpr T&& value() &&
    {
        if (!has_value())
            throw bad_expected_access<E>(std::move(error()));
        return std::move(this->val);
    }

    constexpr const E& error() const& noexcept
    {
        return this->err;
    }

    constexpr E& error() & noexcept
    {
        return this->err;
    }

    constexpr const E&& error() const&& noexcept
    {
        return std::move(this->err);
    }

    constexpr E&& error() && noexcept
    {
        return std::move(this->err);
    }

    template <class U>
    constexpr T value_or(U&& v) const&
    {
        static_assert(is_copy_constructible<T>::value && is_convertible<U&&, T>::value,
                      "T must be copy constructible and convertible from U");
        return has_value() ? **this : static_cast<T>(forward<U>(v));
    }

    template <class U>
    constexpr T value_or(U&& v) &&
    {
        static_assert(is_move_constructible<T>::value && is_convertible<U&&, T>::value,
                      "T must be move constructible and convertible from U");
        return has_value() ? move(**this) : static_cast<T>(forward<U>(v));
    }

    template <class G = E>
    constexpr E error_or(G&& e) const&
    {
        static_assert(is_copy_constructible<E>::value && is_convertible<G&&, E>::value,
                      "E must be copy constructible and convertible from G");
        return has_value() ? static_cast<E>(forward<G>(e)) : error();
    }

    template <class G = E>
    constexpr E error_or(G&& e) &&
    {
        static_assert(is_move_constructible<E>::value && is_convertible<G&&, E>::value,
                      "E must be move constructible and convertible from G");
        return has_value() ? static_cast<E>(forward<G>(e)) : move(error());
    }

    template <class F>
    constexpr auto and_then(F&& f) & -> typename detail::invoke_result<F, T&>::type
    {
        static_assert(detail::is_expected<typename detail::invoke_result<F, T&>::type>::value,
                      "F must return expected");
        return has_value() ? forward<F>(f)(**this)
                           : typename detail::invoke_result<F, T&>::type(unexpected<E>(error()));
    }

    template <class F>
    constexpr auto and_then(F&& f) const& -> typename detail::invoke_result<F, const T&>::type
    {
        static_assert(detail::is_expected<typename detail::invoke_result<F, const T&>::type>::value,
                      "F must return expected");
        return has_value()
                   ? forward<F>(f)(**this)
                   : typename detail::invoke_result<F, const T&>::type(unexpected<E>(error()));
    }

    template <class F>
    constexpr auto and_then(F&& f) && -> typename detail::invoke_result<F, T&&>::type
    {
        static_assert(detail::is_expected<typename detail::invoke_result<F, T&&>::type>::value,
                      "F must return expected");
        return has_value()
                   ? forward<F>(f)(move(**this))
                   : typename detail::invoke_result<F, T&&>::type(unexpected<E>(move(error())));
    }

    template <class F>
    constexpr auto and_then(F&& f) const&& -> typename detail::invoke_result<F, const T&&>::type
    {
        static_assert(
            detail::is_expected<typename detail::invoke_result<F, const T&&>::type>::value,
            "F must return expected");
        return has_value() ? forward<F>(f)(move(**this))
                           : typename detail::invoke_result<F, const T&&>::type(
                                 unexpected<E>(move(error())));
    }

    template <class F>
    constexpr auto or_else(F&& f) & -> expected
    {
        static_assert(is_same<expected, typename detail::invoke_result<F, E&>::type>::value,
                      "F must return expected<T, E>");
        return has_value() ? *this : forward<F>(f)(error());
    }

    template <class F>
    constexpr auto or_else(F&& f) const& -> expected
    {
        static_assert(is_same<expected, typename detail::invoke_result<F, const E&>::type>::value,
                      "F must return expected<T, E>");
        return has_value() ? *this : forward<F>(f)(error());
    }

    template <class F>
    constexpr auto or_else(F&& f) && -> expected
    {
        static_assert(is_same<expected, typename detail::invoke_result<F, E&&>::type>::value,
                      "F must return expected<T, E>");
        return has_value() ? move(*this) : forward<F>(f)(move(error()));
    }

    template <class F>
    constexpr auto or_else(F&& f) const&& -> expected
    {
        static_assert(is_same<expected, typename detail::invoke_result<F, const E&&>::type>::value,
                      "F must return expected<T, E>");
        return has_value() ? move(*this) : forward<F>(f)(move(error()));
    }

    template <class F>
    constexpr auto transform(F&& f) & -> expected<typename detail::invoke_result<F>::type, E>
    {
        return has_value()
                   ? expected<typename detail::invoke_result<F>::type, E>(forward<F>(f)())
                   : expected<typename detail::invoke_result<F>::type, E>(unexpected<E>(error()));
    }

    template <class F>
    constexpr auto transform(F&& f) const& -> expected<typename detail::invoke_result<F>::type, E>
    {
        return has_value()
                   ? expected<typename detail::invoke_result<F>::type, E>(forward<F>(f)())
                   : expected<typename detail::invoke_result<F>::type, E>(unexpected<E>(error()));
    }

    template <class F>
    constexpr auto transform(F&& f) && -> expected<typename detail::invoke_result<F>::type, E>
    {
        return has_value() ? expected<typename detail::invoke_result<F>::type, E>(forward<F>(f)())
                           : expected<typename detail::invoke_result<F>::type, E>(
                                 unexpected<E>(move(error())));
    }

    template <class F>
    constexpr auto transform(F&& f) const&& -> expected<typename detail::invoke_result<F>::type, E>
    {
        return has_value() ? expected<typename detail::invoke_result<F>::type, E>(forward<F>(f)())
                           : expected<typename detail::invoke_result<F>::type, E>(
                                 unexpected<E>(move(error())));
    }

    template <class F>
    constexpr auto transform_error(
        F&& f) & -> expected<void, typename detail::invoke_result<F, E&>::type>
    {
        return has_value() ? expected<void, typename detail::invoke_result<F, E&>::type>()
                           : expected<void, typename detail::invoke_result<F, E&>::type>(
                                 unexpected<typename detail::invoke_result<F, E&>::type>(
                                     forward<F>(f)(error())));
    }

    template <class F>
    constexpr auto transform_error(
        F&& f) const& -> expected<void, typename detail::invoke_result<F, const E&>::type>
    {
        return has_value() ? expected<void, typename detail::invoke_result<F, const E&>::type>()
                           : expected<void, typename detail::invoke_result<F, const E&>::type>(
                                 unexpected<typename detail::invoke_result<F, const E&>::type>(
                                     forward<F>(f)(error())));
    }

    template <class F>
    constexpr auto transform_error(
        F&& f) && -> expected<void, typename detail::invoke_result<F, E&&>::type>
    {
        return has_value() ? expected<void, typename detail::invoke_result<F, E&&>::type>()
                           : expected<void, typename detail::invoke_result<F, E&&>::type>(
                                 unexpected<typename detail::invoke_result<F, E&&>::type>(
                                     forward<F>(f)(move(error()))));
    }

    template <class F>
    constexpr auto transform_error(
        F&& f) const&& -> expected<void, typename detail::invoke_result<F, const E&&>::type>
    {
        return has_value() ? expected<void, typename detail::invoke_result<F, const E&&>::type>()
                           : expected<void, typename detail::invoke_result<F, const E&&>::type>(
                                 unexpected<typename detail::invoke_result<F, const E&&>::type>(
                                     forward<F>(f)(move(error()))));
    }

    template <class T2, class E2>
    constexpr bool operator==(const expected<T2, E2>& rhs) const
    {
        if (has_value() != rhs.has_value())
            return false;
        return has_value() ? **this == *rhs : error() == rhs.error();
    }

    template <class T2, class E2>
    constexpr bool operator!=(const expected<T2, E2>& rhs) const
    {
        return !(*this == rhs);
    }

    template <class T2>
    constexpr bool operator==(const T2& v) const
    {
        return has_value() && **this == v;
    }

    template <class T2>
    constexpr bool operator!=(const T2& v) const
    {
        return !(*this == v);
    }

    template <class E2>
    constexpr bool operator==(const unexpected<E2>& e) const
    {
        return !has_value() && error() == e.error();
    }

    template <class E2>
    constexpr bool operator!=(const unexpected<E2>& e) const
    {
        return !(*this == e);
    }

private:
    template <class Tp>
    static constexpr Tp* addressof(Tp& r) noexcept
    {
        return reinterpret_cast<Tp*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(r)));
    }
};

}  // namespace std

#endif  // End of include guard: LIB_STD_EXPECTED_POLYFILL_CPP11_HPP_ztk3ue
