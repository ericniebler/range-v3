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

#ifndef RANGES_V3_ACTION_SLICE_HPP
#define RANGES_V3_ACTION_SLICE_HPP

#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct slice_fn
            {
            private:
                friend action_access;

                template <typename D>
                using Diff = range_difference_type_t<D>;

                // Overloads for the pipe syntax: rng | action::slice(from, to)
                template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
                static auto bind(slice_fn slice, D from, D to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(slice, std::placeholders::_1, from, to)
                )
                template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
                static auto bind(slice_fn slice, D from, detail::from_end_<D> to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(slice, std::placeholders::_1, from, to)
                )
                template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
                static auto bind(slice_fn slice, detail::from_end_<D> from, detail::from_end_<D> to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(slice, std::placeholders::_1, from, to)
                )
                template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
                static auto bind(slice_fn slice, D from, end_fn const &to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(slice, std::placeholders::_1, from, to)
                )
                template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
                static auto bind(slice_fn slice, detail::from_end_<D> from, end_fn const &to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(slice, std::placeholders::_1, from, to)
                )
            public:
                template<typename Rng,
                    typename I = iterator_t<Rng>,
                    CONCEPT_REQUIRES_(ForwardRange<Rng>() && ErasableRange<Rng, I, I>())>
                Rng operator()(Rng &&rng, Diff<Rng> from, Diff<Rng> to) const
                {
                    RANGES_EXPECT(0 <= from && 0 <= to && from <= to);
                    RANGES_EXPECT(!SizedRange<Rng>::value || to <= distance(rng));
                    ranges::action::erase(rng, begin(rng), next(begin(rng), from));
                    ranges::action::erase(rng, next(begin(rng), to - from), end(rng));
                    return static_cast<Rng &&>(rng);
                }

                template<typename Rng,
                    typename I = iterator_t<Rng>,
                    CONCEPT_REQUIRES_(BidirectionalRange<Rng>() && ErasableRange<Rng, I, I>())>
                Rng operator()(Rng &&rng, Diff<Rng> from, detail::from_end_<Diff<Rng>> to) const
                {
                    RANGES_EXPECT(0 <= from && to.dist_ <= 0);
                    RANGES_EXPECT(!SizedRange<Rng>::value || from - to.dist_ <= distance(rng));
                    ranges::action::erase(rng, begin(rng), next(begin(rng), from));
                    if (to.dist_ != 0)
                    {
                        auto const last = next(begin(rng), end(rng));
                        ranges::action::erase(rng, prev(last, -to.dist_), last);
                    }
                    return static_cast<Rng &&>(rng);
                }

                template<typename Rng,
                    typename I = iterator_t<Rng>,
                    CONCEPT_REQUIRES_(BidirectionalRange<Rng>() && ErasableRange<Rng, I, I>())>
                Rng operator()(Rng &&rng, detail::from_end_<Diff<Rng>> from, detail::from_end_<Diff<Rng>> to) const
                {
                    RANGES_EXPECT(from.dist_ <= 0 && to.dist_ <= 0 && from.dist_ <= to.dist_);
                    RANGES_EXPECT(!SizedRange<Rng>::value || 0 <= distance(rng) + from.dist_);
                    auto last = next(begin(rng), end(rng));
                    ranges::action::erase(rng, prev(last, -to.dist_), last);
                    last = next(begin(rng), end(rng));
                    ranges::action::erase(rng, begin(rng), prev(last, to.dist_ - from.dist_));
                    return static_cast<Rng &&>(rng);
                }

                template<typename Rng,
                    typename I = iterator_t<Rng>,
                    CONCEPT_REQUIRES_(ForwardRange<Rng>() && ErasableRange<Rng, I, I>())>
                Rng operator()(Rng &&rng, Diff<Rng> from, end_fn const &) const
                {
                    RANGES_EXPECT(0 <= from);
                    RANGES_EXPECT(!SizedRange<Rng>::value || from <= distance(rng));
                    ranges::action::erase(rng, begin(rng), next(begin(rng), from));
                    return static_cast<Rng &&>(rng);
                }

                template<typename Rng,
                    typename I = iterator_t<Rng>,
                    CONCEPT_REQUIRES_(BidirectionalRange<Rng>() && ErasableRange<Rng, I, I>())>
                Rng operator()(Rng &&rng, detail::from_end_<Diff<Rng>> from, end_fn const &) const
                {
                    RANGES_EXPECT(from.dist_ <= 0);
                    RANGES_EXPECT(!SizedRange<Rng>::value || 0 <= distance(rng) + from.dist_);
                    auto const last = next(begin(rng), end(rng));
                    ranges::action::erase(rng, begin(rng), prev(last, -from.dist_));
                    return static_cast<Rng &&>(rng);
                }

                template<typename A>
                using is_end_type = meta::or_<meta::is<A, detail::from_end_>, std::is_same<A, end_fn>>;

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T, typename U,
                    typename I = iterator_t<Rng>,
                    CONCEPT_REQUIRES_(!ForwardRange<Rng>() || !ErasableRange<Rng, I, I>() || (!BidirectionalRange<Rng>() && (is_end_type<T>() || is_end_type<U>())))>
                void operator()(Rng &&, T &&, U &&) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                                       "The range upon which action::slice operates must model "
                                       "ForwardRange.");
                    CONCEPT_ASSERT_MSG(ErasableRange<Rng, I, I>(),
                                       "The range upon which action::slice operates must allow element "
                                       "removal.");
                    CONCEPT_ASSERT_MSG(meta::or_<BidirectionalRange<Rng>,
                                                 meta::negate<meta::or_<is_end_type<T>, is_end_type<U>>>>(),
                                       "The range upon which action::slice operates must model "
                                       "BidrectionalRange to slice from the end.");

                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates slice_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<slice_fn>, slice)
        }
        /// @}
    }
}

#endif
