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
#include <type_traits>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename T>
        struct mutable_
        {
            mutable T value;
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
        };

        static_assert(std::is_trivial<constant<int, 0>>::value, "Expected constant to be trivial");

        template<typename Element, typename Tag, bool Empty = std::is_empty<Element>::value>
        struct box
        {
            Element value;

            box()
              : value{}
            {}

            template<typename E,
                     typename std::enable_if<
                         std::is_constructible<Element, E &&>::value, int>::type = 0>
            constexpr explicit box(E && e)
              : value(detail::forward<E>(e))
            {}
        };

        template<typename Element, typename Tag>
        struct box<Element, Tag, true>
          : Element
        {
            box() = default;

            template<typename E,
                     typename std::enable_if<
                         std::is_constructible<Element, E &&>::value, int>::type = 0>
            constexpr explicit box(E && e)
              : Element(detail::forward<E>(e))
            {}
        };

        // Get by tag type
        template<typename Element>
        Element & get(meta::eval<meta::id<Element>> & value)
        {
            return value;
        }

        template<typename Element>
        Element const & get(meta::eval<meta::id<Element>> const & value)
        {
            return value;
        }

        template<typename Element>
        Element && get(meta::eval<meta::id<Element>> && value)
        {
            return std::move(value);
        }

        template<typename Tag, typename Element>
        Element & get(box<Element, Tag, false> & b)
        {
            return b.value;
        }

        template<typename Tag, typename Element>
        constexpr Element const & get(box<Element, Tag, false> const & b)
        {
            return b.value;
        }

        template<typename Tag, typename Element>
        constexpr Element && get(box<Element, Tag, false> && b)
        {
            return detail::move(b).value;
        }

        template<typename Tag, typename Element>
        Element & get(box<Element, Tag, true> & b)
        {
            return b;
        }

        template<typename Tag, typename Element>
        constexpr Element const & get(box<Element, Tag, true> const & b)
        {
            return b;
        }

        template<typename Tag, typename Element>
        constexpr Element && get(box<Element, Tag, true> && b)
        {
            return detail::move(b);
        }

        // Get by index
        template<std::size_t I, typename Element>
        Element & get(box<Element, std::integral_constant<std::size_t, I>, false> & b)
        {
            return b.value;
        }

        template<std::size_t I, typename Element>
        constexpr Element const & get(box<Element, std::integral_constant<std::size_t, I>, false> const & b)
        {
            return b.value;
        }

        template<std::size_t I, typename Element>
        constexpr Element && get(box<Element, std::integral_constant<std::size_t, I>, false> && b)
        {
            return detail::move(b).value;
        }

        template<std::size_t I, typename Element>
        Element & get(box<Element, std::integral_constant<std::size_t, I>, true> & b)
        {
            return b;
        }

        template<std::size_t I, typename Element>
        constexpr Element const & get(box<Element, std::integral_constant<std::size_t, I>, true> const & b)
        {
            return b;
        }

        template<std::size_t I, typename Element>
        constexpr Element && get(box<Element, std::integral_constant<std::size_t, I>, true> && b)
        {
            return detail::move(b);
        }
    }
}

#endif
