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
#include <initializer_list>
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

            // Handle char*, std::pair<iterator, iterator>, std::pair<interator, int> ?

            struct begin_fn : bindable<begin_fn>
            {
                template<typename Rng>
                static auto invoke(begin_fn, Rng && rng) -> decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct end_fn : bindable<end_fn>
            {
                template<typename Rng>
                static auto invoke(end_fn, Rng && rng) -> decltype(end(rng))
                {
                    return end(rng);
                }
            };

            struct cbegin_fn : bindable<cbegin_fn>
            {
                template<typename Rng>
                static auto invoke(cbegin_fn, Rng const & rng) -> decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct cend_fn : bindable<cend_fn>
            {
                template<typename Rng>
                static auto invoke(cend_fn, Rng const & rng) -> decltype(end(rng))
                {
                    return end(rng);
                }
            };
        }

        RANGES_CONSTEXPR adl_begin_end_detail::begin_fn begin {};
        RANGES_CONSTEXPR adl_begin_end_detail::end_fn end {};
        RANGES_CONSTEXPR adl_begin_end_detail::cbegin_fn cbegin {};
        RANGES_CONSTEXPR adl_begin_end_detail::cend_fn cend {};
    }
}

#endif
