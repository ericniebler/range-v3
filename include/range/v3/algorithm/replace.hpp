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
#ifndef RANGES_V3_ALGORITHM_REPLACE_HPP
#define RANGES_V3_ALGORITHM_REPLACE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename T0, typename T1, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using Replaceable = meta::fast_and<
            InputIterator<I>,
            EqualityComparable<X, T0>,
            Writable<I, T1>>;

        /// \addtogroup group-algorithms
        /// @{
        struct replace_fn
        {
            template<typename I, typename S, typename T0, typename T1, typename P = ident,
                CONCEPT_REQUIRES_(Replaceable<I, T0, T1, P>() && IteratorRange<I, S>())>
            I operator()(I begin, S end, T0 const & old_value, T1 const & new_value, P proj_ = {}) const
            {
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin)
                    if(proj(*begin) == old_value)
                        *begin = new_value;
                return begin;
            }

            template<typename Rng, typename T0, typename T1, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Replaceable<I, T0, T1, P>() && Iterable<Rng &>())>
            I operator()(Rng & rng, T0 const & old_value, T1 const & new_value, P proj = {}) const
            {
                return (*this)(begin(rng), end(rng), old_value, new_value, std::move(proj));
            }
        };

        /// \sa `replace_fn`
        /// \ingroup group-algorithms
        constexpr replace_fn replace{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
