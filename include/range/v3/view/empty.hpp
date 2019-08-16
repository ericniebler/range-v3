/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_EMPTY_HPP
#define RANGES_V3_VIEW_EMPTY_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/view/interface.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        struct empty_view_base
        {
            template<typename T>
            friend constexpr T * begin(empty_view<T>) noexcept
            {
                return nullptr;
            }
            template<typename T>
            friend constexpr T * end(empty_view<T>) noexcept
            {
                return nullptr;
            }
        };
    } // namespace detail
    /// \endcond

    template<typename T>
    struct empty_view
      : view_interface<empty_view<T>, (cardinality)0>
      , private detail::empty_view_base
    {
        static_assert(std::is_object<T>::value,
                      "The template parameter to empty_view must be an object type.");
        empty_view() = default;
        constexpr static T * begin() noexcept
        {
            return nullptr;
        }
        constexpr static T * end() noexcept
        {
            return nullptr;
        }
        static constexpr std::size_t size() noexcept
        {
            return 0u;
        }
        static constexpr T * data() noexcept
        {
            return nullptr;
        }
        RANGES_DEPRECATED(
            "Replace views::empty<T>() with views::empty<>. "
            "It is now a variable template.")
        empty_view operator()() const
        {
            return *this;
        }
    };

    namespace views
    {
        template<typename T>
        RANGES_INLINE_VAR constexpr empty_view<T> empty{};
    }

    namespace cpp20
    {
        namespace views
        {
            using ranges::views::empty;
        }
        CPP_template(typename T)(              //
            requires std::is_object<T>::value) //
            using empty_view = ranges::empty_view<T>;
    } // namespace cpp20
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::empty_view)

#endif
