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

#ifndef RANGES_V3_ALGORITHM_UNIQUE_HPP
#define RANGES_V3_ALGORITHM_UNIQUE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct unique_fn
        {
            /// \brief template function \c unique_fn::operator()
            ///
            /// range-based version of the \c unique std algorithm
            ///
            /// \pre `Rng` is a model of the `ForwardRange` concept
            /// \pre `I` is a model of the `ForwardIterator` concept
            /// \pre `S` is a model of the `Sentinel` concept
            /// \pre `C` is a model of the `CallableRelation` concept
            ///
            template<typename I, typename S, typename C = equal_to, typename P = ident,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && IteratorRange<I, S>())>
            RANGES_CXX14_CONSTEXPR
            I operator()(I begin, S end, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);

                begin = adjacent_find(std::move(begin), end, ranges::ref(pred), ranges::ref(proj));

                if(begin != end)
                {
                    for(I i = next(begin); ++i != end;)
                        if(!pred(proj(*begin), proj(*i)))
                            *++begin = iter_move(i);
                    ++begin;
                }
                return begin;
            }

            template<typename Rng, typename C = equal_to, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Sortable<I, C, P>() && Iterable<Rng>())>
            RANGES_CXX14_CONSTEXPR
            range_safe_iterator_t<Rng>
            operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `unique_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& unique = static_const<with_braced_init_args<unique_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
