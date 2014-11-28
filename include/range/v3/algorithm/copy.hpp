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
#ifndef RANGES_V3_ALGORITHM_COPY_HPP
#define RANGES_V3_ALGORITHM_COPY_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/copy.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct copy_fn : aux::copy_fn
        {
            using aux::copy_fn::operator();

            template<typename I, typename S, typename O, typename P = ident,
                CONCEPT_REQUIRES_(
                    InputIterator<I>() && IteratorRange<I, S>() &&
                    WeaklyIncrementable<O>() &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(I begin, S end, O out, P proj_ = P{}) const
            {
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin, ++out)
                    *out = proj(*begin);
                return {begin, out};
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    InputIterable<Rng &>() &&
                    WeaklyIncrementable<O>() &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(Rng &rng, O out, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(proj));
            }
        };

        constexpr copy_fn copy{};

    } // namespace v3
} // namespace ranges

#endif // include guard
