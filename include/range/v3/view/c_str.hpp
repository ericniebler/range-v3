/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_C_STR_HPP
#define RANGES_V3_VIEW_C_STR_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct c_str_fn
            {
                // Fixed-length
                template<std::size_t N>
                ranges::range<char *> operator()(char (&sz)[N]) const
                {
                    return {&sz[0], &sz[N-1]};
                }
                template<std::size_t N>
                ranges::range<char const*> operator()(char const (&sz)[N]) const
                {
                    return {&sz[0], &sz[N-1]};
                }
                template<std::size_t N>
                ranges::range<wchar_t *> operator()(wchar_t (&sz)[N]) const
                {
                    return {&sz[0], &sz[N-1]};
                }
                template<std::size_t N>
                ranges::range<wchar_t const*> operator()(wchar_t const (&sz)[N]) const
                {
                    return {&sz[0], &sz[N-1]};
                }
                template<std::size_t N>
                ranges::range<char16_t *> operator()(char16_t (&sz)[N]) const
                {
                    return {&sz[0], &sz[N-1]};
                }
                template<std::size_t N>
                ranges::range<char16_t const*> operator()(char16_t const (&sz)[N]) const
                {
                    return {&sz[0], &sz[N-1]};
                }
                template<std::size_t N>
                ranges::range<char32_t *> operator()(char32_t (&sz)[N]) const
                {
                    return {&sz[0], &sz[N-1]};
                }
                template<std::size_t N>
                ranges::range<char32_t const*> operator()(char32_t const (&sz)[N]) const
                {
                    return {&sz[0], &sz[N-1]};
                }

                // Null-terminated
                ranges::delimit_view<ranges::range<char *, ranges::unreachable>, char>
                operator()(char *sz) const
                {
                    return ranges::view::delimit(sz, '\0');
                }
                ranges::delimit_view<ranges::range<char const *, ranges::unreachable>, char>
                operator()(char const *sz) const
                {
                    return ranges::view::delimit(sz, '\0');
                }
                ranges::delimit_view<ranges::range<wchar_t *, ranges::unreachable>, wchar_t>
                operator()(wchar_t *sz) const
                {
                    return ranges::view::delimit(sz, L'\0');
                }
                ranges::delimit_view<ranges::range<wchar_t const *, ranges::unreachable>, wchar_t>
                operator()(wchar_t const *sz) const
                {
                    return ranges::view::delimit(sz, L'\0');
                }
                ranges::delimit_view<ranges::range<char16_t *, ranges::unreachable>, char16_t>
                operator()(char16_t *sz) const
                {
                    return ranges::view::delimit(sz, u'\0');
                }
                ranges::delimit_view<ranges::range<char16_t const *, ranges::unreachable>, char16_t>
                operator()(char16_t const *sz) const
                {
                    return ranges::view::delimit(sz, u'\0');
                }
                ranges::delimit_view<ranges::range<char32_t *, ranges::unreachable>, char32_t>
                operator()(char32_t *sz) const
                {
                    return ranges::view::delimit(sz, U'\0');
                }
                ranges::delimit_view<ranges::range<char32_t const *, ranges::unreachable>, char32_t>
                operator()(char32_t const *sz) const
                {
                    return ranges::view::delimit(sz, U'\0');
                }
            };

            namespace
            {
                constexpr auto&& c_str = static_const<c_str_fn>::value;
            }
        }
    }
}

#endif
