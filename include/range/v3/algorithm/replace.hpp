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
#ifndef RANGES_V3_ALGORITHM_REPLACE_HPP
#define RANGES_V3_ALGORITHM_REPLACE_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename T0, typename T1, typename P = ident>
        using Replaceable = meta::strict_and<
            InputIterator<I>,
            IndirectRelation<equal_to, projected<I, P>, T0 const *>,
            Writable<I, T1 const &>>;

        /// \addtogroup group-algorithms
        /// @{
        struct replace_fn
        {
            template<typename I, typename S, typename T0, typename T1, typename P = ident,
                CONCEPT_REQUIRES_(Replaceable<I, T0, T1, P>() && Sentinel<S, I>())>
            I operator()(I begin, S end, T0 const & old_value, T1 const & new_value, P proj = {}) const
            {
                for(; begin != end; ++begin)
                    if(invoke(proj, *begin) == old_value)
                        *begin = new_value;
                return begin;
            }

            template<typename Rng, typename T0, typename T1, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(Replaceable<I, T0, T1, P>() && Range<Rng>())>
            safe_iterator_t<Rng>
            operator()(Rng &&rng, T0 const & old_value, T1 const & new_value, P proj = {}) const
            {
                return (*this)(begin(rng), end(rng), old_value, new_value, std::move(proj));
            }
        };

        /// \sa `replace_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<replace_fn>, replace)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
