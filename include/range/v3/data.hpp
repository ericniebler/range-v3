/// \file
// Range v3 library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_DATA_HPP
#define RANGES_V3_DATA_HPP

#include <string>
#include <type_traits>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Customization point data
        /// \cond
        namespace data_detail
        {
            class data_fn
            {
                template<typename Rng,
                    typename Ptr = decltype(begin(std::declval<Rng &>())),
                    CONCEPT_REQUIRES_(std::is_pointer<Ptr>())>
                static constexpr Ptr impl(Rng &rng, detail::priority_tag<0>)
                    noexcept(noexcept(begin(rng)))
                {
                    return begin(rng);
                }
                template<typename Rng,
                    typename Ptr = detail::decay_t<decltype(data(std::declval<Rng &>()))>,
                    CONCEPT_REQUIRES_(std::is_pointer<Ptr>())>
                static constexpr Ptr impl(Rng &rng, detail::priority_tag<1>)
                    noexcept(noexcept(data(rng)))
                {
                    return data(rng);
                }
                template<typename Rng,
                    typename Ptr = detail::decay_t<decltype(std::declval<Rng &>().data())>,
                    CONCEPT_REQUIRES_(std::is_pointer<Ptr>())>
                static constexpr Ptr impl(Rng &rng, detail::priority_tag<2>)
                    noexcept(noexcept(rng.data()))
                {
                    return rng.data();
                }
                template<typename T, std::size_t N>
                static constexpr T *impl(T (&a)[N], detail::priority_tag<2>) noexcept
                {
                    return a + 0;
                }
                template<typename charT, typename Traits, typename Alloc>
                static constexpr charT *impl(
                    std::basic_string<charT, Traits, Alloc> &s,
                    detail::priority_tag<2>) noexcept
                {
                    // string doesn't have non-const data before C++17
                    return const_cast<charT *>(detail::as_const(s).data());
                }
            public:
                template<typename Rng>
                constexpr auto operator()(Rng &rng) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    data_fn::impl(rng, detail::priority_tag<2>{})
                )
            };
        }
        /// \endcond

        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(data_detail::data_fn, data)
        }
    }
}

#endif // RANGES_V3_DATA_HPP
