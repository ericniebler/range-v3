/// \file
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
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct is_sorted_until_fn
        {
            /// \brief template function \c is_sorted_until_fn::operator()
            ///
            /// range-based version of the \c is_sorted_until std algorithm
            ///
            /// Works on ForwardViews
            ///
            /// \pre `Rng` is a model of the `ForwardView` concept
            /// \pre `I` is a model of the `ForwardIterator` concept
            /// \pre `S` is a model of the `Sentinel<I>` concept
            /// \pre `R` is a model of the `Relation<Value_Type<I>>` concept
            ///
            template<typename I, typename S, typename R = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && Sentinel<S, I>() &&
                    IndirectCallableRelation<R, projected<I, P>>())>
            I operator()(I begin, S end, R pred_ = R{}, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                auto i = begin;
                if(begin != end)
                {
                    while(++i != end)
                    {
                        if(pred(proj(*i), proj(*begin)))
                            return i;
                        begin = i;
                    }
                }
                return i;
            }

            template<typename Rng, typename R = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(ForwardRange<Rng>() &&
                    IndirectCallableRelation<R, projected<I, P>>())>
            range_safe_iterator_t<Rng> operator()(Rng &&rng, R pred = R{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `is_sorted_until_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<is_sorted_until_fn>,
                               is_sorted_until)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
