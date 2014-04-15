//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
// Copyright (c) 2009 Alexander Stepanov and Paul McJones
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. The authors make no
// representations about the suitability of this software for any
// purpose. It is provided "as is" without express or implied
// warranty.
//
// Algorithms from
// Elements of Programming
// by Alexander Stepanov and Paul McJones
// Addison-Wesley Professional, 2009
#ifndef RANGES_V3_ALGORITHM_COMBINE_COPY_N_HPP
#define RANGES_V3_ALGORITHM_COMBINE_COPY_N_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/algorithm/copy_n.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct combine_copy_n_fn
        {
            template<typename I0, typename I1, typename O, typename R, typename P0 = ident,
                typename P1 = ident,
                typename V0 = concepts::Readable::value_t<I0>,
                typename V1 = concepts::Readable::value_t<I1>,
                typename X0 = concepts::Invokable::result_t<P0, V0>,
                typename X1 = concepts::Invokable::result_t<P1, V1>,
                CONCEPT_REQUIRES_(
                    InputIterator<I0>()                         &&
                    InputIterator<I1>()                         &&
                    WeaklyIncrementable<O>()                    &&
                    IndirectlyProjectedCopyable<I0, P0, O>()    &&
                    IndirectlyProjectedCopyable<I1, P1, O>()    &&
                    InvokableRelation<R, X1, X0>()
                )>
            std::tuple<I0, I1, O>
            operator()(I0 f_i0, iterator_difference_t<I0> n_i0,
                       I1 f_i1, iterator_difference_t<I1> n_i1,
                       O f_o, R r, P0 p0 = P0{}, P1 p1 = P1{}) const
            {
                auto &&ir = invokable(r);
                auto &&ip0 = invokable(p0);
                auto &&ip1 = invokable(p1);
                while(true)
                {
                    if(0 == n_i0)
                    {
                        auto p = copy_n(f_i1, n_i1, f_o, ip1);
                        return {f_i0, p.first, p.second};
                    }
                    if(0 == n_i1)
                    {
                        auto p = copy_n(f_i0, n_i0, f_o, ip0);
                        return {p.first, f_i1, p.second};
                    }
                    if(ir(ip1(*f_i1), ip0(*f_i0)))
                    {
                        *f_o = ip1(*f_i1);
                        ++f_i1; ++f_o; --n_i1;
                    }
                    else
                    {
                        *f_o = ip0(*f_i0);
                        ++f_i0; ++f_o; --n_i0;
                    }
                }
            }

            template<typename Rng0, typename Rng1, typename O, typename R, typename P0 = ident,
                typename P1 = ident,
                typename I0 = range_iterator_t<Rng0>,
                typename I1 = range_iterator_t<Rng1>,
                typename V0 = concepts::Readable::value_t<I0>,
                typename V1 = concepts::Readable::value_t<I1>,
                typename X0 = concepts::Invokable::result_t<P0, V0>,
                typename X1 = concepts::Invokable::result_t<P1, V1>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng0>()                            &&
                    Iterable<Rng1>()                            &&
                    InputIterator<I0>()                         &&
                    InputIterator<I1>()                         &&
                    WeaklyIncrementable<O>()                    &&
                    IndirectlyProjectedCopyable<I0, P0, O>()    &&
                    IndirectlyProjectedCopyable<I1, P1, O>()    &&
                    InvokableRelation<R, X1, X0>()
                )>
            std::tuple<I0, I1, O>
            operator()(Rng0 &rng0, iterator_difference_t<I0> n_i0,
                       Rng1 &rng1, iterator_difference_t<I1> n_i1,
                       O out, R r, P0 p0 = P0{}, P1 p1 = P1{}) const
            {
                RANGES_ASSERT(!ForwardIterator<I0>() || n_i0 <= distance(rng0));
                RANGES_ASSERT(!ForwardIterator<I1>() || n_i1 <= distance(rng1));
                return (*this)(begin(rng0), n_i0, begin(rng1), n_i1, std::move(out),
                    std::move(r), std::move(p0), std::move(p1));
            }

            // TODO overloads for initializer_lists
        };

        RANGES_CONSTEXPR combine_copy_n_fn combine_copy_n{};
    }
}

#endif
