//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
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

namespace ranges
{
    inline namespace v3
    {
        struct copy_fn
        {
            template<typename I, typename S, typename O, typename P = ident,
                CONCEPT_REQUIRES_(
                    InputIterator<I, S>()                   &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(I begin, S end, O out, P proj = P{}) const
            {
                auto &&iproj = invokable(proj);
                for(; begin != end; ++begin, ++out)
                    *out = iproj(*begin);
                return {begin, out};
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng>()                         &&
                    InputIterator<I>()                      &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(Rng &rng, O out, P proj = P{}) const
            {
                return (*this)(ranges::begin(rng), ranges::end(rng), std::move(out),
                    std::move(proj));
            }

            template<typename V, typename O, typename P = ident,
                typename I = V const *,
                CONCEPT_REQUIRES_(
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(std::initializer_list<V> rng, O out, P proj = P{}) const
            {
                return (*this)(rng.begin(), rng.end(), std::move(out), std::move(proj));
            }
        };

        RANGES_CONSTEXPR copy_fn copy{};

    } // namespace v3
} // namespace ranges

#endif // include guard
