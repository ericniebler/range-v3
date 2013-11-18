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

            struct beginner
            {
                template<typename Rng>
                auto operator()(Rng && rng) const -> decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct ender
            {
                template<typename Rng>
                auto operator()(Rng && rng) const -> decltype(end(rng))
                {
                    return end(rng);
                }
            };

            struct cbeginner
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const -> decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct cender
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const -> decltype(end(rng))
                {
                    return end(rng);
                }
            };
        }

        constexpr bindable<adl_begin_end_detail::beginner> begin {};
        constexpr bindable<adl_begin_end_detail::ender> end {};
        constexpr bindable<adl_begin_end_detail::cbeginner> cbegin {};
        constexpr bindable<adl_begin_end_detail::cender> cend {};
    }
}

#endif
