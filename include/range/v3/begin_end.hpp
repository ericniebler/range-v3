// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_BEGIN_END_HPP
#define RANGES_V3_BEGIN_END_HPP

#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace adl_begin_end_detail
        {
            using std::begin;
            using std::end;

            struct beginner : bindable<beginner>
            {
                template<typename Rng>
                static auto invoke(beginner, Rng && rng) -> decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct ender : bindable<ender>
            {
                template<typename Rng>
                static auto invoke(ender, Rng && rng) -> decltype(end(rng))
                {
                    return end(rng);
                }
            };

            struct cbeginner : bindable<cbeginner>
            {
                template<typename Rng>
                static auto invoke(cbeginner, Rng const & rng) -> decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct cender : bindable<cender>
            {
                template<typename Rng>
                static auto invoke(cender, Rng const & rng) -> decltype(end(rng))
                {
                    return end(rng);
                }
            };
        }

        RANGES_CONSTEXPR adl_begin_end_detail::beginner begin {};
        RANGES_CONSTEXPR adl_begin_end_detail::ender end {};
        RANGES_CONSTEXPR adl_begin_end_detail::cbeginner cbegin {};
        RANGES_CONSTEXPR adl_begin_end_detail::cender cend {};
    }
}

#endif
