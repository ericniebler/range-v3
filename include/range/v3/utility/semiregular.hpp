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
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/utility/optional.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{

        /// \cond
        namespace detail
        {
            template<typename T>
            struct semiregular_move_assign
              : optional<T>
            {
                using optional<T>::optional;

                semiregular_move_assign() = default;
                semiregular_move_assign(semiregular_move_assign const &) = default;
                semiregular_move_assign(semiregular_move_assign &&) = default;
                semiregular_move_assign &operator=(semiregular_move_assign const &) = default;
                RANGES_CXX14_CONSTEXPR
                semiregular_move_assign &operator=(semiregular_move_assign &&that)
                    noexcept(std::is_nothrow_move_constructible<T>::value)
                {
                    this->reset();
                    if (that)
                        this->emplace(detail::move(*that));
                    return *this;
                }
            };

            template<typename T>
            using semiregular_move_layer =
                meta::if_<Movable<T>, optional<T>, semiregular_move_assign<T>>;

            template<typename T>
            struct semiregular_copy_assign
              : semiregular_move_layer<T>
            {
                using semiregular_move_layer<T>::semiregular_move_layer;

                semiregular_copy_assign() = default;
                semiregular_copy_assign(semiregular_copy_assign const &) = default;
                semiregular_copy_assign(semiregular_copy_assign &&) = default;
                RANGES_CXX14_CONSTEXPR
                semiregular_copy_assign &operator=(semiregular_copy_assign const &that)
                    noexcept(std::is_nothrow_copy_constructible<T>::value)
                {
                    this->reset();
                    if (that)
                        this->emplace(*that);
                    return *this;
                }
                semiregular_copy_assign &operator=(semiregular_copy_assign &&) = default;
            };

            template<typename T>
            using semiregular_copy_layer =
                meta::if_<Copyable<T>, optional<T>, semiregular_copy_assign<T>>;
        } // namespace detail
        /// \endcond

        template<typename T>
        struct semiregular
          : detail::semiregular_copy_layer<T>
        {
            using detail::semiregular_copy_layer<T>::semiregular_copy_layer;

            constexpr semiregular()
                noexcept(std::is_nothrow_default_constructible<T>::value ||
                    !std::is_default_constructible<T>::value)
              : semiregular(tag{}, std::is_default_constructible<T>{})
            {}
            RANGES_CXX14_CONSTEXPR T &get() & noexcept
            {
                return **this;
            }
            constexpr T const &get() const & noexcept
            {
                return **this;
            }
            RANGES_CXX14_CONSTEXPR T &&get() && noexcept
            {
                return *detail::move(*this);
            }
            constexpr T const &&get() const && noexcept
            {
                return *detail::move(*this);
            }
            RANGES_CXX14_CONSTEXPR operator T &() & noexcept
            {
                return **this;
            }
            constexpr operator T const &() const & noexcept
            {
                return **this;
            }
            RANGES_CXX14_CONSTEXPR operator T &&() && noexcept
            {
                return *detail::move(*this);
            }
            constexpr operator T const &&() const && noexcept
            {
                return *detail::move(*this);
            }
            template<typename...Args>
            RANGES_CXX14_CONSTEXPR auto operator()(Args &&...args) &
            RANGES_DECLTYPE_NOEXCEPT(std::declval<T &>()(static_cast<Args &&>(args)...))
            {
                return (**this)(static_cast<Args&&>(args)...);
            }
            template<typename...Args>
            constexpr auto operator()(Args &&...args) const &
            RANGES_DECLTYPE_NOEXCEPT(std::declval<T const &>()(static_cast<Args &&>(args)...))
            {
                return (**this)(static_cast<Args&&>(args)...);
            }
            template<typename...Args>
            RANGES_CXX14_CONSTEXPR auto operator()(Args &&...args) &&
            RANGES_DECLTYPE_NOEXCEPT(std::declval<T &&>()(static_cast<Args &&>(args)...))
            {
                return (*detail::move(*this))(static_cast<Args&&>(args)...);
            }
            template<typename...Args>
            constexpr auto operator()(Args &&...args) const &&
            RANGES_DECLTYPE_NOEXCEPT(std::declval<T const &&>()(static_cast<Args &&>(args)...))
            {
                return (*detail::move(*this))(static_cast<Args&&>(args)...);
            }
        private:
            struct tag {};
            constexpr semiregular(tag, std::false_type) noexcept
            {}
            constexpr semiregular(tag, std::true_type)
                noexcept(std::is_nothrow_default_constructible<T>::value)
              : detail::semiregular_copy_layer<T>{in_place}
            {}
        };

        template<typename T>
        struct semiregular<T &>
          : private ranges::reference_wrapper<T &>
        {
            semiregular() = default;
            template<typename Arg,
                CONCEPT_REQUIRES_(Constructible<ranges::reference_wrapper<T &>, Arg &>())>
            semiregular(in_place_t, Arg &arg)
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
        {
            semiregular() = default;
            template<typename Arg,
                CONCEPT_REQUIRES_(Constructible<ranges::reference_wrapper<T &&>, Arg>())>
            semiregular(in_place_t, Arg &&arg)
              : ranges::reference_wrapper<T &>(static_cast<Arg &&>(arg))
            {}
            using ranges::reference_wrapper<T &&>::reference_wrapper;
            using ranges::reference_wrapper<T &&>::get;
            using ranges::reference_wrapper<T &&>::operator T &&;
            using ranges::reference_wrapper<T &&>::operator();
        };

        template<typename T>
        using semiregular_t =
            meta::if_<SemiRegular<T>, T, semiregular<T>>;

        template<typename T>
        using movesemiregular_t =
            meta::if_c<
                Movable<T>() && DefaultConstructible<T>(),
                T,
                semiregular<T>>;

        template<typename T, bool IsConst = false>
        using semiregular_ref_or_val_t =
            meta::if_<
                SemiRegular<T>,
                meta::if_c<IsConst, T, reference_wrapper<T>>,
                reference_wrapper<meta::if_c<IsConst, semiregular<T> const, semiregular<T>>>>;

        template<typename T>
        auto get(meta::id_t<semiregular<T>> &t)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            t.get()
        )

        template<typename T>
        auto get(meta::id_t<semiregular<T>> const &t)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            t.get()
        )

        template<typename T>
        auto get(meta::id_t<semiregular<T>> &&t)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            detail::move(t).get()
        )
        /// @}
    }
}

#endif
