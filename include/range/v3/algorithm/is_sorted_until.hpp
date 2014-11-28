// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/
#ifndef RANGES_V3_ALGORITHM_IS_SORTED_UNTIL_HPP
#define RANGES_V3_ALGORITHM_IS_SORTED_UNTIL_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct is_sorted_until_fn
        {
            /// \brief template function \c is_sorted_until_fn::operator()
            ///
            /// range-based version of the \c is_sorted_until std algorithm
            ///
            /// Works on ForwardRanges
            ///
            /// \pre \c Rng is a model of the ForwardRange concept
            /// \pre \c I is a model of the ForwardIterator concept
            /// \pre \c S is a model of the Sentinel<I> concept
            /// \pre \c R is a model of the Relation<Value_Type<I>> concept
            ///
            template <typename I, typename S, typename R = ordered_less,
                      typename P = ident, typename V = iterator_value_t<I>,
                      CONCEPT_REQUIRES_(
                       ForwardIterator<I>() && IteratorRange<I, S>() && Invokable<P, V>() &&
                       InvokableRelation<R, concepts::Invokable::result_t<P, V>>())>
            I operator()(I begin, S end, R rel = R{}, P proj_ = P{}) const
            {
                auto &&irel = invokable(rel);
                auto &&iproj = invokable(proj_);
                auto i = begin;
                if(begin != end)
                {
                    while(++i != end)
                    {
                        if(irel(iproj(*i), iproj(*begin)))
                            return i;
                        begin = i;
                    }
                }
                return i;
            }

            template <typename Rng, typename R = ordered_less, typename P = ident,
                      typename I = range_iterator_t<Rng>,
                      typename V = iterator_value_t<I>,
                      CONCEPT_REQUIRES_(
                       ForwardIterable<Rng &>() && Invokable<P, V>() &&
                       InvokableRelation<R, concepts::Invokable::result_t<P, V>>())>
            I operator()(Rng &rng, R rel = R{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(rel), std::move(proj));
            }
        };

        constexpr is_sorted_until_fn is_sorted_until{};

    } // namespace v3
} // namespace ranges

#endif // include guard
