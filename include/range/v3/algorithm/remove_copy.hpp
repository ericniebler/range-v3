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
#ifndef RANGES_V3_ALGORITHM_REMOVE_COPY_HPP
#define RANGES_V3_ALGORITHM_REMOVE_COPY_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/result_types.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        CPP_def
        (
            template(typename I, typename O, typename T, typename P = ident)
            (concept RemoveCopyable)(I, O, T, P),
                InputIterator<I> &&
                WeaklyIncrementable<O> &&
                IndirectRelation<equal_to, projected<I, P>, T const *> &&
                IndirectlyCopyable<I, O>
        );

        /// \addtogroup group-algorithms
        /// @{
        template<typename I, typename O>
        using remove_copy_result = detail::in_out_result<I, O>;

        struct remove_copy_fn
        {
            template<typename I, typename S, typename O, typename T, typename P = ident>
            auto operator()(I begin, S end, O out, T const &val, P proj = P{}) const ->
                CPP_ret(remove_copy_result<I, O>)(
                    requires RemoveCopyable<I, O, T, P> && Sentinel<S, I>)
            {
                for(; begin != end; ++begin)
                {
                    auto &&x = *begin;
                    if(!(invoke(proj, x) == val))
                    {
                        *out = (decltype(x) &&) x;
                        ++out;
                    }
                }
                return {begin, out};
            }

            template<typename Rng, typename O, typename T, typename P = ident>
            auto operator()(Rng &&rng, O out, T const &val, P proj = P{}) const ->
                CPP_ret(remove_copy_result<safe_iterator_t<Rng>, O>)(
                    requires RemoveCopyable<iterator_t<Rng>, O, T, P> && InputRange<Rng>)
            {
                return (*this)(begin(rng), end(rng), std::move(out), val, std::move(proj));
            }
        };

        /// \sa `remove_copy_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<remove_copy_fn>, remove_copy)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
