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
#ifndef RANGES_V3_ALGORITHM_REPLACE_COPY_HPP
#define RANGES_V3_ALGORITHM_REPLACE_COPY_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        CPP_def
        (
            template(typename I, typename O, typename T0, typename T1, typename P = ident)
            (concept ReplaceCopyable)(I, O, T0, T1, P),
                InputIterator<I> &&
                OutputIterator<O, T1 const &> &&
                IndirectlyCopyable<I, O> &&
                IndirectRelation<equal_to, projected<I, P>, T0 const *>
        );

        /// \addtogroup group-algorithms
        /// @{
        struct replace_copy_fn
        {
            template<typename I, typename S, typename O, typename T0, typename T1,
                typename P = ident>
            auto operator()(I begin, S end, O out, T0 const & old_value, T1 const & new_value,
                    P proj = {}) const ->
                CPP_ret(tagged_pair<tag::in(I), tag::out(O)>)(
                    requires ReplaceCopyable<I, O, T0, T1, P> && Sentinel<S, I>)
            {
                for(; begin != end; ++begin, ++out)
                {
                    auto &&x = *begin;
                    if(invoke(proj, x) == old_value)
                        *out = new_value;
                    else
                        *out = (decltype(x) &&) x;
                }
                return {begin, out};
            }

            template<typename Rng, typename O, typename T0, typename T1, typename P = ident>
            auto operator()(Rng &&rng, O out, T0 const & old_value, T1 const & new_value,
                    P proj = {}) const ->
                CPP_ret(tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>)(
                    requires ReplaceCopyable<iterator_t<Rng>, O, T0, T1, P> && Range<Rng>)
            {
                return (*this)(begin(rng), end(rng), std::move(out), old_value, new_value,
                    std::move(proj));
            }
        };

        /// \sa `replace_copy_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<replace_copy_fn>,
                               replace_copy)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
