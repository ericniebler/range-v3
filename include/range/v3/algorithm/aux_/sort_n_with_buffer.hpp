/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
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
#ifndef RANGES_V3_ALGORITHM_AUX_SORT_N_WITH_BUFFER_HPP
#define RANGES_V3_ALGORITHM_AUX_SORT_N_WITH_BUFFER_HPP

#include <tuple>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/algorithm/aux_/merge_n_with_buffer.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace aux
        {
            struct sort_n_with_buffer_fn
            {
                template<typename I, typename B, typename C = ordered_less, typename P = ident,
                    typename VI = iter_common_reference_t<I>,
                    typename VB = iter_common_reference_t<B>,
                    CONCEPT_REQUIRES_(
                        Same<VI, VB>() &&
                        IndirectlyCopyable<I, B>() &&
                        Mergeable<B, I, I, C, P, P>()
                    )>
                I operator()(I begin, difference_type_t<I> n, B buff, C r = C{}, P p = P{}) const
                {
                    auto half = n / 2;
                    if(0 == half)
                        return next(begin, n);
                    I m = (*this)(begin, half, buff, r, p);
                          (*this)(m, n - half, buff, r, p);
                    return merge_n_with_buffer(begin, half, m, n - half, buff, r, p);
                }
            };

            RANGES_INLINE_VARIABLE(sort_n_with_buffer_fn, sort_n_with_buffer)
        } // namespace aux
    } // namespace v3
} // namespace ranges

#endif // include guard
