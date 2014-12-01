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
#ifndef RANGES_V3_ALGORITHM_COUNT_HPP
#define RANGES_V3_ALGORITHM_COUNT_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct count_fn
        {
            template<typename I, typename S, typename V1, typename P = ident,
                typename V0 = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V0>,
                CONCEPT_REQUIRES_(
                    InputIterator<I>() && IteratorRange<I, S>() &&
                    Invokable<P, V0>() &&
                    EqualityComparable<X, V1>()
                )>
            iterator_difference_t<I>
            operator()(I begin, S end, V1 const & val, P proj_ = P{}) const
            {
                auto &&proj = invokable(proj_);
                iterator_difference_t<I> n = 0;
                for(; begin != end; ++begin)
                    if(proj(*begin) == val)
                        ++n;
                return n;
            }

            template<typename Rng, typename V1, typename P = ident,
                typename I = range_iterator_t<Rng>,
                typename V0 = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V0>,
                CONCEPT_REQUIRES_(
                    InputIterable<Rng>() &&
                    Invokable<P, V0>() &&
                    EqualityComparable<X, V1>()
                )>
            iterator_difference_t<I>
            operator()(Rng &&rng, V1 const & val, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), val, std::move(proj));
            }
        };

        /// \sa `count_fn`
        /// \ingroup group-algorithms
        constexpr with_braced_init_args<count_fn> count{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
