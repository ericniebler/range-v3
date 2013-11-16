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

#ifndef RANGE_V3_BEGIN_END_HPP
#define RANGE_V3_BEGIN_END_HPP

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

            struct begin_t
            {
                template<typename Rng>
                auto operator()(Rng && rng) const -> decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct end_t
            {
                template<typename Rng>
                auto operator()(Rng && rng) const -> decltype(end(rng))
                {
                    return end(rng);
                }
            };

            struct cbegin_t
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const -> decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct cend_t
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const -> decltype(end(rng))
                {
                    return end(rng);
                }
            };
        }

        constexpr bindable<adl_begin_end_detail::begin_t> begin {};
        constexpr bindable<adl_begin_end_detail::end_t> end {};
        constexpr bindable<adl_begin_end_detail::cbegin_t> cbegin {};
        constexpr bindable<adl_begin_end_detail::cend_t> cend {};
    }
}

#endif
