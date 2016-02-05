/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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
        template<typename I, typename O, typename C = equal_to, typename P = ident>
        using UniqueCopyable = meta::fast_and<
            InputIterator<I>,
            IndirectCallableRelation<C, Projected<I, P>>,
            WeaklyIncrementable<O>,
            IndirectlyCopyable<I, O>,
            meta::fast_or<
                ForwardIterator<I>,
                ForwardIterator<O>,
                IndirectlyCopyableStorable<I, O>>>;

        /// \addtogroup group-algorithms
        /// @{
        struct unique_copy_fn
        {
        private:
            template<typename I, typename S, typename O, typename C, typename P>
            static tagged_pair<tag::in(I), tag::out(O)> impl(I begin, S end, O out, C pred_, P proj_,
                concepts::InputIterator*, std::false_type)
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                if(begin != end)
                {
                    // Must save a copy into a local because we will need this value
                    // even after we advance the input iterator.
                    iterator_value_t<I> value = *begin; // This is guaranteed by IndirectlyCopyable
                    *out = value;
                    ++out;
                    while(++begin != end)
                    {
                        auto &&x = *begin;
                        if(!pred(proj(value), proj(x)))
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
            static tagged_pair<tag::in(I), tag::out(O)> impl(I begin, S end, O out, C pred_, P proj_,
                concepts::ForwardIterator*, std::false_type)
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                if(begin != end)
                {
                    I tmp = begin;
                    *out = *tmp;
                    ++out;
                    while(++begin != end)
                    {
                        auto &&x = *begin;
                        if(!pred(proj(*tmp), proj(x)))
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
            static tagged_pair<tag::in(I), tag::out(O)> impl(I begin, S end, O out, C pred_, P proj_,
                concepts::InputIterator*, std::true_type)
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                if(begin != end)
                {
                    *out = *begin;
                    while(++begin != end)
                    {
                        auto &&x = *begin;
                        if(!pred(proj(*out), proj(x)))
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
            /// \pre `C` is a model of the `CallableRelation` concept
            template<typename I, typename S, typename O, typename C = equal_to, typename P = ident,
                CONCEPT_REQUIRES_(UniqueCopyable<I, O, C, P>() && IteratorRange<I, S>())>
            tagged_pair<tag::in(I), tag::out(O)> operator()(I begin, S end, O out, C pred = C{}, P proj = P{}) const
            {
                return unique_copy_fn::impl(std::move(begin), std::move(end), std::move(out),
                    std::move(pred), std::move(proj), iterator_concept<I>(), ForwardIterator<O>());
            }

            /// \overload
            template<typename Rng, typename O, typename C = equal_to, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(UniqueCopyable<I, O, C, P>() && Range<Rng>())>
            tagged_pair<tag::in(range_safe_iterator_t<Rng>), tag::out(O)>
            operator()(Rng &&rng, O out, C pred = C{}, P proj = P{}) const
            {
                return unique_copy_fn::impl(begin(rng), end(rng), std::move(out),
                    std::move(pred), std::move(proj), iterator_concept<I>(), ForwardIterator<O>());
            }
        };

        /// \sa `unique_copy_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& unique_copy = static_const<with_braced_init_args<unique_copy_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
