//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_COPY_N_HPP
#define RANGES_V3_ALGORITHM_COPY_N_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct copy_n_fn
        {
            template<typename I, typename O, typename P = ident,
                CONCEPT_REQUIRES_(
                    InputIterator<I>()                      &&
                    WeaklyIncrementable<O>()                &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(I begin, iterator_difference_t<I> n, O out, P proj = P{}) const
            {
                RANGES_ASSERT(0 <= n);
                auto &&iproj = invokable(proj);
                for(; n != 0; ++begin, ++out, --n)
                    *out = iproj(*begin);
                return {begin, out};
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng>()                         &&
                    InputIterator<I>()                      &&
                    WeaklyIncrementable<O>()                &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(Rng &rng, iterator_difference_t<I> n, O out, P proj = P{}) const
            {
                RANGES_ASSERT(!ForwardIterator<I>() || n <= distance(rng));
                return (*this)(begin(rng), n, std::move(out), std::move(proj));
            }

            template<typename V, typename O, typename P = ident,
                typename I = V const *,
                CONCEPT_REQUIRES_(
                    WeaklyIncrementable<O>()                &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(std::initializer_list<V> rng, iterator_difference_t<I> n, O out, P proj = P{}) const
            {
                RANGES_ASSERT(n <= rng.size());
                return (*this)(rng.begin(), n, std::move(out), std::move(proj));
            }
        };

        RANGES_CONSTEXPR copy_n_fn copy_n{};

    } // namespace v3
} // namespace ranges

#endif // include guard
