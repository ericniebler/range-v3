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

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename O, typename C = equal_to, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using UniqueCopyable = meta::fast_and<
            InputIterator<I>,
            Invokable<P, V>,
            InvokableRelation<C, X>,
            WeakOutputIterator<O, V>>; // BUGBUG V or X?

        /// \addtogroup group-algorithms
        /// @{
        struct unique_copy_fn
        {
        private:
            template<typename I, typename S, typename O, typename C, typename P>
            static std::pair<I, O> impl(I begin, S end, O out, C pred_, P proj_,
                concepts::InputIterator*, std::false_type)
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                if(begin != end)
                {
                    iterator_value_t<I> value = *begin;
                    *out = value;
                    ++out;
                    while(++begin != end)
                    {
                        if(!pred(proj(value), proj(*begin)))
                        {
                            value = *begin;
                            *out = value;
                            ++out;
                        }
                    }
                }
                return {begin, out};
            }

            template<typename I, typename S, typename O, typename C, typename P>
            static std::pair<I, O> impl(I begin, S end, O out, C pred_, P proj_,
                concepts::ForwardIterator*, std::false_type)
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                if(begin != end)
                {
                    I tmp = begin;
                    *out = *tmp;
                    ++out;
                    while(++begin != end)
                    {
                        if(!pred(proj(*tmp), proj(*begin)))
                        {
                            *out = *begin;
                            ++out;
                            tmp = begin;
                        }
                    }
                }
                return {begin, out};
            }

            template<typename I, typename S, typename O, typename C, typename P>
            static std::pair<I, O> impl(I begin, S end, O out, C pred_, P proj_,
                concepts::InputIterator*, std::true_type)
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                if(begin != end)
                {
                    *out = *begin;
                    while(++begin != end)
                        if(!pred(proj(*out), proj(*begin)))
                            *++out = *begin;
                    ++out;
                }
                return {begin, out};
            }

        public:
            /// \brief template function unique_copy
            ///
            /// range-based version of the `unique_copy` std algorithm
            ///
            /// \pre InputRange is a model of the `InputRange` concept
            /// \pre `O` is a model of the `WeakOutputIterator` concept
            /// \pre `C` is a model of the `InvokableRelation` concept
            template<typename I, typename S, typename O, typename C = equal_to, typename P = ident,
                CONCEPT_REQUIRES_(UniqueCopyable<I, O, C, P>() && IteratorRange<I, S>())>
            std::pair<I, O> operator()(I begin, S end, O out, C pred = C{}, P proj = P{}) const
            {
                return unique_copy_fn::impl(std::move(begin), std::move(end), std::move(out),
                    std::move(pred), std::move(proj), iterator_concept<I>(), ForwardIterator<O>());
            }

            /// \overload
            template<typename Rng, typename O, typename C = equal_to, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(UniqueCopyable<I, O, C, P>() && Iterable<Rng &>())>
            std::pair<I, O> operator()(Rng & rng, O out, C pred = C{}, P proj = P{}) const
            {
                return unique_copy_fn::impl(begin(rng), end(rng), std::move(out),
                    std::move(pred), std::move(proj), iterator_concept<I>(), ForwardIterator<O>());
            }
        };

        /// \sa `unique_copy_fn`
        /// \ingroup group-algorithms
        constexpr unique_copy_fn unique_copy{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
