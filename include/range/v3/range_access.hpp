/// \file
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

#ifndef RANGES_V3_RANGE_ACCESS_HPP
#define RANGES_V3_RANGE_ACCESS_HPP

#include <cstddef>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{
        struct range_access
        {
            /// \cond

            //
            // Concepts that the range cursor must model
            // BUGBUG this doesn't handle weak cursors.
            //
            struct InputCursorConcept
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        //t.done(),
                        t.current(),
                        (t.next(), concepts::void_)
                    ));
            };
            struct ForwardCursorConcept
              : concepts::refines<InputCursorConcept>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t.equal(t))
                    ));
            };
            struct BidirectionalCursorConcept
              : concepts::refines<ForwardCursorConcept>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        (t.prev(), concepts::void_)
                    ));
            };
            struct RandomAccessCursorConcept
              : concepts::refines<BidirectionalCursorConcept>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SignedIntegral>(t.distance_to(t)),
                        (t.advance(t.distance_to(t)), concepts::void_)
                    ));
            };
            struct InfiniteCursorConcept
            {
                template<typename T>
                auto requires_(T&&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(typename T::is_infinite{})
                    ));
            };

            template<typename Rng>
            static RANGES_RELAXED_CONSTEXPR auto begin_cursor(Rng & rng, long)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                rng.begin_cursor()
            )
            template<typename Rng>
            static RANGES_RELAXED_CONSTEXPR auto begin_cursor(Rng & rng, int)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                static_cast<Rng const &>(rng).begin_cursor()
            )
            template<typename Rng>
            static RANGES_RELAXED_CONSTEXPR auto end_cursor(Rng & rng, long)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                rng.end_cursor()
            )
            template<typename Rng>
            static RANGES_RELAXED_CONSTEXPR auto end_cursor(Rng & rng, int)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                static_cast<Rng const &>(rng).end_cursor()
            )

            template<typename Rng>
            static RANGES_RELAXED_CONSTEXPR auto begin_adaptor(Rng & rng, long)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                rng.begin_adaptor()
            )
            template<typename Rng>
            static RANGES_RELAXED_CONSTEXPR auto begin_adaptor(Rng & rng, int)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                static_cast<Rng const &>(rng).begin_adaptor()
            )
            template<typename Rng>
            static RANGES_RELAXED_CONSTEXPR auto end_adaptor(Rng & rng, long)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                rng.end_adaptor()
            )
            template<typename Rng>
            static RANGES_RELAXED_CONSTEXPR auto end_adaptor(Rng & rng, int)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                static_cast<Rng const &>(rng).end_adaptor()
            )

            template<typename Cur>
            static RANGES_RELAXED_CONSTEXPR auto current(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.current()
            )
            template<typename Cur>
            static RANGES_RELAXED_CONSTEXPR auto next(Cur & pos)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.next()
            )
            template<typename Cur>
            static constexpr auto done(Cur const & pos)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.done()
            )
            template<typename Cur>
            static RANGES_RELAXED_CONSTEXPR auto equal(Cur const &pos0, Cur const &pos1)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos0.equal(pos1)
            )
            template<typename Cur, typename S>
            static constexpr auto empty(Cur const &pos, S const &end)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                end.equal(pos)
            )
            template<typename Cur>
            static RANGES_RELAXED_CONSTEXPR auto prev(Cur & pos)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.prev()
            )
            template<typename Cur, typename D>
            static RANGES_RELAXED_CONSTEXPR auto advance(Cur & pos, D n)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.advance(n)
            )
            template<typename Cur>
            static RANGES_RELAXED_CONSTEXPR auto distance_to(Cur const &pos0, Cur const &pos1)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos0.distance_to(pos1)
            )

        private:
            template<typename Cur>
            using random_access_cursor_difference_t =
                decltype(range_access::distance_to(std::declval<Cur>(), std::declval<Cur>()));

            template<typename Cur, typename Enable = void>
            struct cursor_difference2
            {
                using type = std::ptrdiff_t;
            };

            template<typename Cur>
            struct cursor_difference2<Cur, meta::void_<random_access_cursor_difference_t<Cur>>>
            {
                using type = random_access_cursor_difference_t<Cur>;
            };

            template<typename Cur, typename Enable = void>
            struct cursor_difference
              : cursor_difference2<Cur>
            {};

            template<typename Cur>
            struct cursor_difference<Cur, meta::void_<typename Cur::difference_type>>
            {
                using type = typename Cur::difference_type;
            };

            template<typename Cur, typename Enable = void>
            struct cursor_value
            {
                using type = uncvref_t<decltype(std::declval<Cur const &>().current())>;
            };

            template<typename Cur>
            struct cursor_value<Cur, meta::void_<typename Cur::value_type>>
            {
                using type = typename Cur::value_type;
            };

            template<typename T, typename Enable = void>
            struct single_pass
            {
                using type = std::false_type;
            };

            template<typename T>
            struct single_pass<T, meta::void_<typename T::single_pass>>
            {
                using type = typename T::single_pass;
            };
        public:
            template<typename Cur>
            using cursor_difference_t = typename cursor_difference<Cur>::type;

            template<typename Cur>
            using cursor_value_t = typename cursor_value<Cur>::type;

            template<typename Cur>
            using single_pass_t = typename single_pass<Cur>::type;

            template<typename Cur, typename S>
            static RANGES_RELAXED_CONSTEXPR Cur cursor(basic_iterator<Cur, S> it)
            {
                return std::move(it.pos());
            }
            template<typename S>
            static RANGES_RELAXED_CONSTEXPR S sentinel(basic_sentinel<S> s)
            {
                return std::move(s.end());
            }

            template<typename RangeAdaptor>
            struct base_range
            {
                using type = typename RangeAdaptor::base_range_t;
            };
            template<typename RangeAdaptor>
            struct base_range<RangeAdaptor const>
            {
                using type = typename RangeAdaptor::base_range_t const;
            };
            template<typename RangeFacade>
            struct range_facade
            {
                using type = typename RangeFacade::range_facade_t;
            };
            template<typename RangeAdaptor>
            struct range_adaptor
            {
                using type = typename RangeAdaptor::range_adaptor_t;
            };
            /// endcond
        };
        /// @}

        /// \cond
        namespace detail
        {
            template<typename T>
            using InputCursor =
                concepts::models<range_access::InputCursorConcept, T>;

            template<typename T>
            using ForwardCursor =
                concepts::models<range_access::ForwardCursorConcept, T>;

            template<typename T>
            using BidirectionalCursor =
                concepts::models<range_access::BidirectionalCursorConcept, T>;

            template<typename T>
            using RandomAccessCursor =
                concepts::models<range_access::RandomAccessCursorConcept, T>;

            template<typename T>
            using InfiniteCursor =
                concepts::models<range_access::InfiniteCursorConcept, T>;

            template<typename T>
            using cursor_concept =
                concepts::most_refined<
                    meta::list<
                        range_access::RandomAccessCursorConcept,
                        range_access::BidirectionalCursorConcept,
                        range_access::ForwardCursorConcept,
                        range_access::InputCursorConcept>, T>;

            template<typename T>
            using cursor_concept_t = meta::eval<cursor_concept<T>>;
        }
        /// \endcond
    }
}

#endif
