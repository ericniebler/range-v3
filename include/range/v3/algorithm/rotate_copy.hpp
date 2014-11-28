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
#ifndef RANGES_V3_ALGORITHM_ROTATE_COPY_HPP
#define RANGES_V3_ALGORITHM_ROTATE_COPY_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/copy.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct rotate_copy_fn
        {
            template<typename I, typename S, typename O, typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && IteratorRange<I, S>() && WeaklyIncrementable<O>() &&
                    IndirectlyProjectedCopyable<I, P, O>())>
            std::pair<I, O> operator()(I begin, I middle, S end, O out, P proj_ = P{}) const
            {
                auto &&proj = invokable(proj_);
                auto res = copy(middle, std::move(end), std::move(out), std::ref(proj));
                return {std::move(res.first), copy(std::move(begin), middle, std::move(res.second), std::ref(proj)).second};
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Iterable<Rng &>() && WeaklyIncrementable<O>() &&
                    IndirectlyProjectedCopyable<I, P, O>())>
            std::pair<I, O> operator()(Rng & rng, I middle, O out, P proj = P{}) const
            {
                return (*this)(begin(rng), std::move(middle), end(rng), std::move(out), std::move(proj));
            }
        };

        constexpr rotate_copy_fn rotate_copy{};

    } // namespace v3
} // namespace ranges

#endif // include guard
