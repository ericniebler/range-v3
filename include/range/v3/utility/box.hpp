/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_BOX_HPP
#define RANGES_V3_UTILITY_BOX_HPP

#include <atomic>
#include <utility>
#include <cstdlib>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility Utility
        /// @{
        ///
        template<typename T>
        struct mutable_
        {
            mutable T value;
            CONCEPT_REQUIRES(std::is_default_constructible<T>::value)
            constexpr mutable_()
              : value{}
            {}
            constexpr explicit mutable_(T const &t)
              : value(t)
            {}
            constexpr explicit mutable_(T &&t)
              : value(detail::move(t))
            {}
            mutable_ const &operator=(T const &t) const
            {
                value = t;
                return *this;
            }
            mutable_ const &operator=(T &&t) const
            {
                value = detail::move(t);
                return *this;
            }
            constexpr operator T &() const &
            {
                return value;
            }
        };

        template<typename T>
        struct mutable_<std::atomic<T>>
        {
            mutable std::atomic<T> value;
            mutable_() = default;
            mutable_(mutable_ const &that)
              : value(static_cast<T>(that.value))
            {}
            constexpr explicit mutable_(T &&t)
              : value(detail::move(t))
            {}
            constexpr explicit mutable_(T const &t)
              : value(t)
            {}
            mutable_ const &operator=(mutable_ const &that) const
            {
                value = static_cast<T>(that.value);
                return *this;
            }
            mutable_ const &operator=(T &&t) const
            {
                value = std::move(t);
                return *this;
            }
            mutable_ const &operator=(T const &t) const
            {
                value = t;
                return *this;
            }
            operator T() const
            {
                return value;
            }
            T exchange(T desired)
            {
                return value.exchange(desired);
            }
            operator std::atomic<T> &() const &
            {
                return value;
            }
        };

        template<typename T, T v>
        struct constant
        {
            constant() = default;
            constexpr explicit constant(T const &)
            {}
            constant &operator=(T const &)
            {
                return *this;
            }
            constant const &operator=(T const &) const
            {
                return *this;
            }
            constexpr operator T() const
            {
                return v;
            }
            constexpr T exchange(T const &) const
            {
                return v;
            }
        };

        static_assert(std::is_trivial<constant<int, 0>>::value, "Expected constant to be trivial");

        template<typename Element, typename Tag = Element,
            bool Empty = std::is_empty<Element>::value && !detail::is_final<Element>::value>
        struct box
        {
        private:
            Element value;

        public:
            CONCEPT_REQUIRES(std::is_default_constructible<Element>::value)
            constexpr box()
              : value{}
            {}

            template<typename E,
                CONCEPT_REQUIRES_(std::is_constructible<Element, E &&>::value)>
            constexpr explicit box(E && e)
              : value(detail::forward<E>(e))
            {}

            RANGES_CXX14_CONSTEXPR Element &get() & noexcept
            {
                return value;
            }
            constexpr Element const &get() const & noexcept
            {
                return value;
            }
            RANGES_CXX14_CONSTEXPR Element &&get() && noexcept
            {
                return detail::move(value);
            }
        };

        template<typename Element, typename Tag>
        struct box<Element, Tag, true>
          : private Element
        {
            CONCEPT_REQUIRES(std::is_default_constructible<Element>::value)
            constexpr box()
              : Element{}
            {}

            template<typename E,
                CONCEPT_REQUIRES_(std::is_constructible<Element, E &&>::value)>
            constexpr explicit box(E && e)
              : Element(detail::forward<E>(e))
            {}

            RANGES_CXX14_CONSTEXPR Element &get() & noexcept
            {
                return *this;
            }
            constexpr Element const &get() const & noexcept
            {
                return *this;
            }
            RANGES_CXX14_CONSTEXPR Element &&get() && noexcept
            {
                return detail::move(*this);
            }
        };

        template<typename Element, typename Tag = Element, bool Inherit = true>
        using box_if =
            box<Element, Tag, Inherit && std::is_empty<Element>::value &&
                                         !detail::is_final<Element>::value>;

        // Get by tag type
        template<typename Tag, typename Element, bool Empty>
        RANGES_CXX14_CONSTEXPR Element & get(box<Element, Tag, Empty> & b) noexcept
        {
            return b.get();
        }

        template<typename Tag, typename Element, bool Empty>
        constexpr Element const & get(box<Element, Tag, Empty> const & b) noexcept
        {
            return b.get();
        }

        template<typename Tag, typename Element, bool Empty>
        RANGES_CXX14_CONSTEXPR Element && get(box<Element, Tag, Empty> && b) noexcept
        {
            return detail::move(b).get();
        }

        // Get by index
        template<std::size_t I, typename Element, bool Empty>
        RANGES_CXX14_CONSTEXPR Element & get(box<Element, meta::size_t<I>, Empty> & b) noexcept
        {
            return b.get();
        }

        template<std::size_t I, typename Element, bool Empty>
        constexpr Element const & get(box<Element, meta::size_t<I>, Empty> const & b) noexcept
        {
            return b.get();
        }

        template<std::size_t I, typename Element, bool Empty>
        RANGES_CXX14_CONSTEXPR Element && get(box<Element, meta::size_t<I>, Empty> && b) noexcept
        {
            return detail::move(b).get();
        }
        /// @}
    }
}

#endif
