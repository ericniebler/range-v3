/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_SEMIREGULAR_HPP
#define RANGES_V3_UTILITY_SEMIREGULAR_HPP

#include <utility>
#include <meta/meta.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/utility/in_place.hpp>

namespace ranges
{
    /// \cond
    template<typename T>
    struct semiregular;

    namespace detail
    {
        struct semiregular_get
        {
            template<typename T>
            friend auto CPP_auto_fun(get)(meta::id_t<semiregular<T>> &t)
            (
                return t.get()
            )
            template<typename T>
            friend auto CPP_auto_fun(get)(meta::id_t<semiregular<T>> const &t)
            (
                return t.get()
            )
            template<typename T>
            friend auto CPP_auto_fun(get)(meta::id_t<semiregular<T>> &&t)
            (
                return detail::move(t).get()
            )
        };
    }
    /// \endcond

    /// \addtogroup group-utility
    /// @{
    template<typename T>
    struct semiregular
      : private detail::semiregular_get
    {
    private:
        struct tag {};
        template<typename... Args>
        void construct_from(Args &&... args)
        {
            new ((void*) std::addressof(data_)) T(static_cast<Args &&>(args)...);
            engaged_ = true;
        }
        void move_assign(T &&t, std::true_type)
        {
            data_ = detail::move(t);
        }
        void move_assign(T &&t, std::false_type)
        {
            reset();
            construct_from(detail::move(t));
        }
        void copy_assign(T const &t, std::true_type)
        {
            data_ = t;
        }
        void copy_assign(T &&t, std::false_type)
        {
            reset();
            construct_from(t);
        }
        constexpr semiregular(tag, std::false_type) noexcept
        {}
        constexpr semiregular(tag, std::true_type)
            noexcept(std::is_nothrow_default_constructible<T>::value)
          : data_{}
          , engaged_(true)
        {}
        void reset()
        {
            if(engaged_)
            {
                data_.~T();
                engaged_ = false;
            }
        }
        union
        {
            char ch_{};
            T data_;
        };
        bool engaged_ {false};
    public:
        constexpr semiregular()
            noexcept(std::is_nothrow_default_constructible<T>::value ||
                !std::is_default_constructible<T>::value)
          : semiregular(tag{}, std::is_default_constructible<T>{})
        {}
        semiregular(semiregular &&that)
            noexcept(std::is_nothrow_move_constructible<T>::value)
        {
            if (that.engaged_)
                this->construct_from(detail::move(that.data_));
        }
        semiregular(semiregular const &that)
        {
            if (that.engaged_)
                this->construct_from(that.data_);
        }
#if defined(__cpp_conditional_explicit) && 0 < __cpp_conditional_explicit
        template<typename U>
        explicit(!ConvertibleTo<U, T>)
        constexpr CPP_ctor(semiregular)(U &&u)(
            noexcept(std::is_nothrow_constructible<T, U>::value)
                requires (!defer::Same<uncvref_t<U>, semiregular>) &&
                    defer::Constructible<T, U>)
          : semiregular(in_place, static_cast<U &&>(u))
        {}
#else
        template<typename U>
        explicit constexpr CPP_ctor(semiregular)(U &&u)(
            noexcept(std::is_nothrow_constructible<T, U>::value)
                requires (!defer::Same<uncvref_t<U>, semiregular>) &&
                    defer::Constructible<T, U> &&
                    (!defer::ConvertibleTo<U, T>))
          : semiregular(in_place, static_cast<U &&>(u))
        {}
        template<typename U>
        constexpr CPP_ctor(semiregular)(U &&u)(
            noexcept(std::is_nothrow_constructible<T, U>::value)
                requires (!defer::Same<uncvref_t<U>, semiregular>) &&
                    defer::Constructible<T, U> &&
                    defer::ConvertibleTo<U, T>)
          : semiregular(in_place, static_cast<U &&>(u))
        {}
#endif
        CPP_template(typename... Args)(
            requires Constructible<T, Args...>)
        constexpr semiregular(in_place_t, Args &&...args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
          : data_(static_cast<Args &&>(args)...)
          , engaged_(true)
        {}
        ~semiregular()
        {
            reset();
        }
        semiregular &operator=(semiregular &&that)
            noexcept(std::is_nothrow_move_constructible<T>::value &&
                (!std::is_move_assignable<T>::value ||
                    std::is_nothrow_move_assignable<T>::value))
        {
            if (engaged_ && that.engaged_)
                this->move_assign(detail::move(that.data_), std::is_move_assignable<T>());
            else if (that.engaged_)
                this->construct_from(detail::move(that.data_));
            else if (engaged_)
                this->reset();
            return *this;
        }
        semiregular &operator=(semiregular const &that)
            noexcept(std::is_nothrow_copy_constructible<T>::value &&
                (!std::is_copy_assignable<T>::value ||
                    std::is_nothrow_copy_assignable<T>::value))
        {
            if (engaged_ && that.engaged_)
                this->copy_assign(that.data_, std::is_copy_assignable<T>());
            else if (that.engaged_)
                this->construct_from(that.data_);
            else if (engaged_)
                this->reset();
            return *this;
        }
        semiregular &operator=(T t)
            noexcept(std::is_nothrow_move_constructible<T>::value &&
                (!std::is_move_assignable<T>::value ||
                    std::is_nothrow_move_assignable<T>::value))
        {
            if (engaged_)
                this->move_assign(detail::move(t), std::is_move_assignable<T>());
            else
                this->construct_from(detail::move(t));
            return *this;
        }
        constexpr /*c++14*/ T &get() & noexcept
        {
            return RANGES_ENSURE(engaged_), data_;
        }
        constexpr T const &get() const & noexcept
        {
            return RANGES_ENSURE(engaged_), data_;
        }
        constexpr /*c++14*/ T &&get() && noexcept
        {
            return RANGES_ENSURE(engaged_), detail::move(data_);
        }
        T const &&get() const && = delete;
        constexpr /*c++14*/ operator T &() & noexcept
        {
            return get();
        }
        constexpr operator T const &() const & noexcept
        {
            return get();
        }
        constexpr /*c++14*/ operator T &&() && noexcept
        {
            return detail::move(get());
        }
        operator T const &&() const && = delete;
        template<typename...Args>
        constexpr /*c++14*/ auto CPP_auto_fun(operator())(Args &&...args) (mutable &)
        (
            return data_(static_cast<Args &&>(args)...)
        )
        template<typename...Args>
        constexpr auto CPP_auto_fun(operator())(Args &&...args) (const &)
        (
            return ((T const &) data_)(static_cast<Args &&>(args)...)
        )
        template<typename...Args>
        constexpr /*c++14*/ auto CPP_auto_fun(operator())(Args &&...args) (mutable &&)
        (
            return ((T &&) data_)(static_cast<Args &&>(args)...)
        )
        template<typename...Args>
        constexpr void operator()(Args &&...args) const && = delete;
    };

    template<typename T>
    struct semiregular<T &>
      : private ranges::reference_wrapper<T &>
      , private detail::semiregular_get
    {
        semiregular() = default;
        template<typename Arg>
        CPP_ctor(semiregular)(in_place_t, Arg &arg)(
            noexcept(true)
            requires Constructible<ranges::reference_wrapper<T &>, Arg &>)
          : ranges::reference_wrapper<T &>(arg)
        {}
        using ranges::reference_wrapper<T &>::reference_wrapper;
        using ranges::reference_wrapper<T &>::get;
        using ranges::reference_wrapper<T &>::operator T &;
        using ranges::reference_wrapper<T &>::operator();
    };

    template<typename T>
    struct semiregular<T &&>
      : private ranges::reference_wrapper<T &&>
      , private detail::semiregular_get
    {
        semiregular() = default;
        template<typename Arg>
        CPP_ctor(semiregular)(in_place_t, Arg &&arg)(
            noexcept(true)
            requires Constructible<ranges::reference_wrapper<T &&>, Arg>)
          : ranges::reference_wrapper<T &&>(static_cast<Arg &&>(arg))
        {}
        using ranges::reference_wrapper<T &&>::reference_wrapper;
        using ranges::reference_wrapper<T &&>::get;
        using ranges::reference_wrapper<T &&>::operator T &&;
        using ranges::reference_wrapper<T &&>::operator();
    };

    template<typename T>
    using semiregular_t =
        meta::if_c<(bool) Semiregular<T>, T, semiregular<T>>;

    template<typename T, bool IsConst = false>
    using semiregular_ref_or_val_t =
        meta::if_c<
            (bool) Semiregular<T>,
            meta::if_c<IsConst, T, reference_wrapper<T>>,
            reference_wrapper<meta::if_c<IsConst, semiregular<T> const, semiregular<T>>>>;
    /// @}
}

#endif
