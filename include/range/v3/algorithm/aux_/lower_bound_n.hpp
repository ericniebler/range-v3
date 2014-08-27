//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_AUX_LOWER_BOUND_N_HPP
#define RANGES_V3_ALGORITHM_AUX_LOWER_BOUND_N_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace aux
        {
            struct lower_bound_n_fn
            {
                template<typename I, typename V2, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(BinarySearchable<I, V2, C, P>())>
                I operator()(I begin, iterator_difference_t<I> d, V2 const &val, C pred = C{},
                    P proj = P{}) const
                {
                    RANGES_ASSERT(0 <= d);
                    auto &&ipred = invokable(pred);
                    auto &&iproj = invokable(proj);
                    while(0 != d)
                    {
                        auto half = d / 2;
                        auto middle = next(begin, half);
                        if(ipred(iproj(*middle), val))
                        {
                            begin = std::move(++middle);
                            d -= half + 1;
                        }
                        else
                            d = half;
                    }
                    return begin;
                }

                /// \overload
                template<typename Rng, typename V2, typename C = ordered_less, typename P = ident,
                    typename I = range_iterator_t<Rng>,
                    CONCEPT_REQUIRES_(Iterable<Rng>() && BinarySearchable<I, V2, C, P>())>
                I operator()(Rng &rng, iterator_difference_t<I> d, V2 const &val, C pred = C{},
                    P proj = P{}) const
                {
                    static_assert(!is_infinite<Rng>::val, "Trying to binary search an infinite range");
                    RANGES_ASSERT(0 <= d);
                    RANGES_ASSERT(d <= distance(rng));
                    return (*this)(begin(rng), d, std::move(pred), std::move(proj));
                }
            };

            RANGES_CONSTEXPR range_algorithm<lower_bound_n_fn> lower_bound_n{};
        }
    } // namespace v3
} // namespace ranges

#endif // include guard
