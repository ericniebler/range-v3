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
#ifndef RANGES_V3_VIEW_C_STR_HPP
#define RANGES_V3_VIEW_C_STR_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<class T>
            struct is_char_type_
                : std::false_type
            {};

            template<>
            struct is_char_type_<char>
                : std::true_type
            {};

            template<>
            struct is_char_type_<wchar_t>
                : std::true_type
            {};

            template<>
            struct is_char_type_<char16_t>
                : std::true_type
            {};

            template<>
            struct is_char_type_<char32_t>
                : std::true_type
            {};

            template<class T>
            using is_char_type = is_char_type_<meta::_t<std::remove_cv<T>>>;
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        namespace view
        {
            /// View a `\0`-terminated C string (e.g. from a const char*) as a
            /// range.
            struct c_str_fn
            {
                // Fixed-length
                template<typename Char, std::size_t N,
                    CONCEPT_REQUIRES_(detail::is_char_type<Char>())>
                ranges::iterator_range<Char *> operator()(Char (&sz)[N]) const
                {
                    return {&sz[0], &sz[N-1]};
                }

                // Null-terminated
                template<typename Char,
                    CONCEPT_REQUIRES_(detail::is_char_type<Char>())>
                ranges::delimit_view<
                    ranges::iterator_range<Char *, ranges::unreachable>,
                    meta::_t<std::remove_cv<Char>>>
                operator()(Char *sz) const volatile
                {
                    using ch_t = meta::_t<std::remove_cv<Char>>;
                    return ranges::view::delimit(sz, ch_t(0));
                }
            };

            /// \relates c_str_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(c_str_fn, c_str)
        }
    }
}

#endif
