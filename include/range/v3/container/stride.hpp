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

#ifndef RANGES_V3_CONTAINER_STRIDE_HPP
#define RANGES_V3_CONTAINER_STRIDE_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/container/action.hpp>
#include <range/v3/container/erase.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace cont
        {
            struct stride_fn
            {
            private:
                friend action_access;
                template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
                static auto bind(stride_fn stride, D step)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(stride, std::placeholders::_1, step)
                )
            public:
                template<typename Rng, typename I = range_iterator_t<Rng>,
                    typename S = range_sentinel_t<Rng>,
                    CONCEPT_REQUIRES_(EraseableIterable<Rng &, I, S>() && Permutable<I>())>
                void operator()(Rng & rng, range_difference_t<Rng> const step) const
                {
                    RANGES_ASSERT(0 < step);
                    if(1 < step)
                    {
                        I begin = ranges::begin(rng);
                        S const end = ranges::end(rng);
                        if(begin != end)
                        {
                            for(I i = next_bounded(++begin, step-1, end); i != end;
                                advance_bounded(i, step, end), ++begin)
                            {
                                *begin = std::move(*i);
                            }
                        }
                        cont::erase(rng, begin, end);
                    }
                }
            };

            RANGES_CONSTEXPR action<stride_fn> stride{};
        }
    }
}

#endif
