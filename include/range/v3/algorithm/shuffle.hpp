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
#ifndef RANGES_V3_ALGORITHM_SHUFFLE_HPP
#define RANGES_V3_ALGORITHM_SHUFFLE_HPP

#include <random>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace concepts
        {
            struct UniformRandomNumberGenerator
            {
                template<typename Gen>
                auto requires_(Gen rand) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<UnsignedIntegral>(val<Gen>()())
                    ));
            };
        }

        template<typename Gen>
        using UniformRandomNumberGenerator = concepts::models<concepts::UniformRandomNumberGenerator, Gen>;

        struct shuffle_fn
        {
            template<typename I, typename S, typename Gen,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() &&
                                  Permutable<I>() &&
                                  UniformRandomNumberGenerator<Gen>())>
            I operator()(I begin, S end_, Gen && gen) const
            {
                I end = next_to(begin, end_);
                auto d = end - begin;
                if(d > 1)
                {
                    using param_t = std::uniform_int_distribution<std::ptrdiff_t>::param_type;
                    std::uniform_int_distribution<std::ptrdiff_t> uid;
                    for(--end, --d; begin < end; ++begin, --d)
                    {
                        auto i = uid(gen, param_t{0, d});
                        if(i != 0)
                            ranges::iter_swap(begin, begin + i);
                    }
                }
                return end;
            }

            template<typename Rng, typename Gen, typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng &>() &&
                                  Permutable<I>() &&
                                  UniformRandomNumberGenerator<Gen>())>
            I operator()(Rng & rng, Gen && rand) const
            {
                return (*this)(begin(rng), end(rng), std::forward<Gen>(rand));
            }
        };

        constexpr shuffle_fn shuffle {};

    } // namespace v3
} // namespace ranges

#endif // include guard
