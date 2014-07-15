//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef RANGES_V3_ALGORITHM_UNIQUE_HPP
#define RANGES_V3_ALGORITHM_UNIQUE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct unique_fn
        {
            /// \brief template function \c unique_fn::operator()
            ///
            /// range-based version of the \c unique std algorithm
            ///
            /// \pre \c Rng is a model of the ForwardIterable concept
            /// \pre \c I is a model of the ForwardIterator concept
            /// \pre \c S is a model of the Sentinel<I> concept
            /// \pre \c R is a model of the BinaryPredicate concept
            ///
            template <typename I, typename S, typename R = equal_to, typename P = ident,
                      typename V = iterator_value_t<I>,
                      CONCEPT_REQUIRES_(
                       ForwardIterator<I, S>() && Permutable<I>() && Invokable<P, V>() &&
                       InvokableRelation<R, concepts::Invokable::result_t<P, V>>())>
            I operator()(I begin, S end, R pred = R{}, P proj_ = P{}) const
            {
                auto &&ipred = invokable(pred);
                auto &&iproj = invokable(proj_);

                begin = adjacent_find.template operator()<
                 I, S, typename std::add_lvalue_reference<R>::type>(begin, end, pred);

                if(begin != end)
                {
                    auto i = begin;
                    for(++i; ++i != end;)
                        if(!ipred(iproj(*begin), iproj(*i)))
                            *++begin = std::move(*i);
                    ++begin;
                }
                return begin;
            }

            template <typename Rng, typename R = equal_to, typename P = ident,
                      typename I = range_iterator_t<Rng>,
                      typename V = iterator_value_t<I>,
                      CONCEPT_REQUIRES_(
                       ForwardIterable<Rng>() && Permutable<I>() && Invokable<P, V>() &&
                       InvokableRelation<R, concepts::Invokable::result_t<P, V>>())>
            I operator()(Rng &rng, R pred = R{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR unique_fn unique{};

    } // namespace v3
} // namespace ranges

#endif // include guard
