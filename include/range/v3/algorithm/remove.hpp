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
#ifndef RANGES_V3_ALGORITHM_REMOVE_HPP
#define RANGES_V3_ALGORITHM_REMOVE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/algorithm/find.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename T, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using Removable = meta::fast_and<
            ForwardIterator<I>,
            Invokable<P, V>,
            EqualityComparable<T, X>,
            Permutable<I>>;

        struct remove_fn
        {
            template<typename I, typename S, typename T, typename P = ident,
                CONCEPT_REQUIRES_(Removable<I, T const &, P>() && IteratorRange<I, S>())>
            I operator()(I begin, S end, T const &val, P proj_ = P{}) const
            {
                auto &&proj = invokable(proj_);
                begin = find(std::move(begin), end, val, std::ref(proj));
                if(begin != end)
                {
                    for(I i = next(begin); i != end; ++i)
                    {
                        if(!(proj(*i) == val))
                        {
                            *begin = std::move(*i);
                            ++begin;
                        }
                    }
                }
                return begin;
            }

            template<typename Rng, typename T, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Removable<I, T const &, P>() && ForwardIterable<Rng &>())>
            I operator()(Rng &rng, T const &val, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), val, std::move(proj));
            }
        };

        constexpr remove_fn remove{};

    } // namespace v3
} // namespace ranges

#endif // include guard
