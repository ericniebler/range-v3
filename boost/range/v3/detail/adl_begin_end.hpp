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

#ifndef BOOST_RANGE_V3_DETAIL_ADL_BEGIN_END_HPP
#define BOOST_RANGE_V3_DETAIL_ADL_BEGIN_END_HPP

#include <iterator>
#include <boost/range/v3/range_fwd.hpp>

namespace boost
{
    namespace range
    {
        inline namespace v3
        {
            namespace adl_begin_end_detail
            {
                using std::begin;
                using std::end;

                constexpr struct begin_t
                {
                    template<typename Rng>
                    auto operator()(Rng && rng) const -> decltype(begin(rng))
                    {
                        return begin(rng);
                    }
                } adl_begin {};

                constexpr struct end_t
                {
                    template<typename Rng>
                    auto operator()(Rng && rng) const -> decltype(end(rng))
                    {
                        return end(rng);
                    }
                } adl_end {};

                constexpr struct cbegin_t
                {
                    template<typename Rng>
                    auto operator()(Rng const & rng) const -> decltype(begin(rng))
                    {
                        return begin(rng);
                    }
                } adl_cbegin {};

                constexpr struct cend_t
                {
                    template<typename Rng>
                    auto operator()(Rng const & rng) const -> decltype(end(rng))
                    {
                        return end(rng);
                    }
                } adl_cend {};
            }
        }
    }
}

#endif
