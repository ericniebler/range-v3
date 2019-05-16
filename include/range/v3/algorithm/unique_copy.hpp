/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_UNIQUE_COPY_HPP
#define RANGES_V3_ALGORITHM_UNIQUE_COPY_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    template<typename I, typename O>
    using unique_copy_result = detail::in_out_result<I, O>;

    struct unique_copy_fn
    {
    private:
        template<typename I, typename S, typename O, typename C, typename P>
        static unique_copy_result<I, O> impl(I begin, S end, O out, C pred, P proj,
            detail::input_iterator_tag_, std::false_type)
        {
            if(begin != end)
            {
                // Must save a copy into a local because we will need this value
                // even after we advance the input iterator.
                iter_value_t<I> value = *begin; // This is guaranteed by IndirectlyCopyable
                *out = value;
                ++out;
                while(++begin != end)
                {
                    auto &&x = *begin;
                    if(!invoke(pred, invoke(proj, value), invoke(proj, x)))
                    {
                        value = (decltype(x) &&) x;
                        *out = value;
                        ++out;
                    }
                }
            }
            return {begin, out};
        }

        template<typename I, typename S, typename O, typename C, typename P>
        static unique_copy_result<I, O> impl(I begin, S end, O out, C pred, P proj,
            detail::forward_iterator_tag_, std::false_type)
        {
            if(begin != end)
            {
                I tmp = begin;
                *out = *tmp;
                ++out;
                while(++begin != end)
                {
                    auto &&x = *begin;
                    if(!invoke(pred, invoke(proj, *tmp), invoke(proj, x)))
                    {
                        *out = (decltype(x) &&) x;
                        ++out;
                        tmp = begin;
                    }
                }
            }
            return {begin, out};
        }

        template<typename I, typename S, typename O, typename C, typename P>
        static unique_copy_result<I, O> impl(I begin, S end, O out, C pred, P proj,
            detail::input_iterator_tag_, std::true_type)
        {
            if(begin != end)
            {
                *out = *begin;
                while(++begin != end)
                {
                    auto &&x = *begin;
                    if(!invoke(pred, invoke(proj, *out), invoke(proj, x)))
                        *++out = (decltype(x) &&) x;
                }
                ++out;
            }
            return {begin, out};
        }

    public:
        /// \brief template function unique_copy
        ///
        /// range-based version of the `unique_copy` std algorithm
        ///
        /// \pre InputView is a model of the `InputView` concept
        /// \pre `O` is a model of the `WeakOutputIterator` concept
        /// \pre `C` is a model of the `Relation` concept
        template<typename I, typename S, typename O, typename C = equal_to, typename P = identity>
        auto operator()(I begin, S end, O out, C pred = C{}, P proj = P{}) const ->
            CPP_ret(unique_copy_result<I, O>)(
                requires InputIterator<I> && Sentinel<S, I> &&
                    IndirectRelation<C, projected<I, P>> &&
                    WeaklyIncrementable<O> &&
                    IndirectlyCopyable<I, O> && (
                        ForwardIterator<I> ||
                        ForwardIterator<O> ||
                        IndirectlyCopyableStorable<I, O>))
        {
            return unique_copy_fn::impl(std::move(begin), std::move(end), std::move(out),
                std::move(pred), std::move(proj), iterator_tag_of<I>(),
                meta::bool_<ForwardIterator<O>>{});
        }

        /// \overload
        template<typename Rng, typename O, typename C = equal_to, typename P = identity>
        auto operator()(Rng &&rng, O out, C pred = C{}, P proj = P{}) const ->
            CPP_ret(unique_copy_result<safe_iterator_t<Rng>, O>)(
                requires Range<Rng> && InputIterator<iterator_t<Rng>> &&
                    IndirectRelation<C, projected<iterator_t<Rng>, P>> &&
                    WeaklyIncrementable<O> &&
                    IndirectlyCopyable<iterator_t<Rng>, O> && (
                        ForwardIterator<iterator_t<Rng>> ||
                        ForwardIterator<O> ||
                        IndirectlyCopyableStorable<iterator_t<Rng>, O>))
        {
            return unique_copy_fn::impl(begin(rng), end(rng), std::move(out),
                std::move(pred), std::move(proj), iterator_tag_of<iterator_t<Rng>>(),
                meta::bool_<ForwardIterator<O>>{});
        }
    };

    /// \sa `unique_copy_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(unique_copy_fn, unique_copy)

    namespace cpp20
    {
        using ranges::unique_copy_result;
        using ranges::unique_copy;
    }
    /// @}
} // namespace ranges

#endif // include guard
