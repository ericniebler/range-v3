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
#ifndef RANGES_V3_ALGORITHM_GENERATE_N_HPP
#define RANGES_V3_ALGORITHM_GENERATE_N_HPP

#include <tuple>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct generate_n_fn
        {
            template<typename O, typename F,
                CONCEPT_REQUIRES_(Function<F>() &&
                    WeakOutputIterator<O, concepts::Function::result_t<F>>())>
            std::pair<O, F> operator()(O begin, iterator_difference_t<O> n, F fun) const
            {
                RANGES_ASSERT(n >= 0);
                auto norig = n;
                auto b = uncounted(begin);
                for(; 0 != n; ++b, --n)
                    *b = fun();
                return {recounted(begin, b, norig), fun};
            }

            //template<typename O, typename S, typename F,
            //    CONCEPT_REQUIRES_(Function<F>() &&
            //        OutputIterator<O, concepts::Function::result_t<F>>() &&
            //        IteratorRange<O, S>())>
            //std::tuple<O, F, iterator_difference_t<O>>
            //operator()(O begin, S end, iterator_difference_t<O> n, F fun) const
            //{
            //    RANGES_ASSERT(n >= 0);
            //    for(; begin != end && n != 0; ++begin, --n)
            //        *begin = fun();
            //    return std::tuple<O, F, iterator_difference_t<O>>{begin, fun, n};
            //}

            //template<typename Rng, typename F,
            //    typename O = range_iterator_t<Rng>,
            //    CONCEPT_REQUIRES_(Function<F>() &&
            //        OutputIterable<Rng, concepts::Function::result_t<F>>())>
            //std::tuple<O, F, iterator_difference_t<O>>
            //operator()(Rng & rng, iterator_difference_t<O> n, F fun) const
            //{
            //    return (*this)(begin(rng), end(rng), n, std::move(fun));
            //}
        };

        constexpr generate_n_fn generate_n{};
    } // namespace v3
} // namespace ranges

#endif // include guard
