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

        private:

            template<typename T>
            static std::false_type single_pass_2_(long);
            template<typename T>
            static typename T::single_pass single_pass_2_(int);

            template<typename T>
            struct single_pass_
            {
                using type = decltype(range_access::single_pass_2_<T>(42));
            };

            template<typename T>
            static meta::id<basic_mixin<T>> mixin_base_2_(long);
            template<typename T>
            static meta::id<typename T::mixin> mixin_base_2_(int);

            template<typename Cur>
            struct mixin_base_
              : decltype(range_access::mixin_base_2_<Cur>(42))
            {};

        public:
            template<typename Cur>
            using single_pass_t = meta::_t<single_pass_<Cur>>;

            template<typename Cur>
            using mixin_base_t = meta::_t<mixin_base_<Cur>>;

            //
            // Concepts that the range cursor must model
            //
            struct Cursor
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SemiRegular, T>(),
                        concepts::model_of<concepts::SemiRegular, mixin_base_t<T>>(),
                        concepts::model_of<concepts::Constructible, mixin_base_t<T>, T &&>(),
                        concepts::model_of<concepts::Constructible, mixin_base_t<T>, T const &>(),
                        (t.next(), concepts::void_)
                    ));
            };
            struct HasEqualCursor
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t.equal(t))
                    ));
            };
            struct ReadableCursor
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        t.get()
                    ));
            };
            struct WritableCursor
            {
                template<typename T, typename U>
                auto requires_(T&& t, U&& u) -> decltype(
                    concepts::valid_expr(
                        (t.set((U &&) u), 42)
                    ));
            };
            struct SizedCursor
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SignedIntegral>(t.distance_to(t))
                    ));
            };
            struct SizedCursorRange
            {
                template<typename C, typename S>
                auto requires_(C&& c, S&& s) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SignedIntegral>(s.distance_from(c))
                    ));
            };
            struct OutputCursor
              : concepts::refines<WritableCursor, Cursor(concepts::_1)>
            {};
            struct InputCursor
              : concepts::refines<ReadableCursor, Cursor>
            {};
            struct ForwardCursor
              : concepts::refines<InputCursor, HasEqualCursor>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::is_false(single_pass_t<uncvref_t<T>>())
                    ));
            };
            struct BidirectionalCursor
              : concepts::refines<ForwardCursor>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        (t.prev(), concepts::void_)
                    ));
            };
            struct RandomAccessCursor
              : concepts::refines<BidirectionalCursor, SizedCursor>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        (t.advance(t.distance_to(t)), concepts::void_)
                    ));
            };
            struct InfiniteCursor
            {
                template<typename T>
                auto requires_(T&&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(typename T::is_infinite{})
                    ));
            };

            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_cursor(Rng & rng, long)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                rng.begin_cursor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_cursor(Rng & rng, int)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                static_cast<Rng const &>(rng).begin_cursor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_cursor(Rng & rng, long)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                rng.end_cursor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_cursor(Rng & rng, int)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                static_cast<Rng const &>(rng).end_cursor()
            )

            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_adaptor(Rng & rng, long)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                rng.begin_adaptor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_adaptor(Rng & rng, int)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                static_cast<Rng const &>(rng).begin_adaptor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_adaptor(Rng & rng, long)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                rng.end_adaptor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_adaptor(Rng & rng, int)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                static_cast<Rng const &>(rng).end_adaptor()
            )

            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto get(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.get()
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto move(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.move()
            )
            template<typename Cur, typename T>
            static RANGES_CXX14_CONSTEXPR auto set(Cur &pos, T &&t)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.set((T &&) t)
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto next(Cur & pos)
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
            static RANGES_CXX14_CONSTEXPR auto equal(Cur const &pos0, Cur const &pos1)
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
            static RANGES_CXX14_CONSTEXPR auto prev(Cur & pos)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.prev()
            )
            template<typename Cur, typename D>
            static RANGES_CXX14_CONSTEXPR auto advance(Cur & pos, D n)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.advance(n)
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto distance_to(
                Cur const &pos0, Cur const &pos1)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos0.distance_to(pos1)
            )
            template<typename Cur, typename S>
            static RANGES_CXX14_CONSTEXPR auto distance_to(
                Cur const &pos, S const &end)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                end.distance_from(pos)
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto distance_remaining(
                Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.distance_remaining()
            )

        private:
            template<typename Cur>
            using sized_cursor_difference_t =
                decltype(range_access::distance_to(std::declval<Cur>(), std::declval<Cur>()));

            template<typename T>
            static std::ptrdiff_t cursor_difference_2_(detail::any);
            template<typename T>
            static sized_cursor_difference_t<T> cursor_difference_2_(long);
            template<typename T>
            static typename T::difference_type cursor_difference_2_(int);

            template<typename Cur>
            struct cursor_difference
            {
                using type = decltype(range_access::cursor_difference_2_<Cur>(42));
            };

            template<typename T>
            using cursor_reference_t = decltype(std::declval<T const &>().get());

            template<typename T>
            static meta::id<uncvref_t<cursor_reference_t<T>>> cursor_value_2_(long);
            template<typename T>
            static meta::id<typename T::value_type> cursor_value_2_(int);

            template<typename Cur>
            struct cursor_value
              : decltype(range_access::cursor_value_2_<Cur>(42))
            {};

        public:
            template<typename Cur>
            using cursor_difference_t = typename cursor_difference<Cur>::type;

            template<typename Cur>
            using cursor_value_t = typename cursor_value<Cur>::type;

            template<typename BI,
                CONCEPT_REQUIRES_(meta::is<meta::_t<std::decay<BI>>, basic_iterator>())>
            static RANGES_CXX14_CONSTEXPR auto pos(BI&& it)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
                std::forward<BI>(it).pos()
            )
            template<typename BS,
                CONCEPT_REQUIRES_(meta::is<meta::_t<std::decay<BS>>, basic_sentinel>())>
            static RANGES_CXX14_CONSTEXPR auto end(BS&& s)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
                std::forward<BS>(s).end()
            )

            template<typename Cur, typename S>
            static RANGES_CXX14_CONSTEXPR Cur cursor(basic_iterator<Cur, S> it)
            {
                return std::move(it.pos());
            }
            template<typename S>
            static RANGES_CXX14_CONSTEXPR S sentinel(basic_sentinel<S> s)
            {
                return std::move(s.end());
            }

        private:
            template<typename RangeAdaptor>
            static meta::id<typename RangeAdaptor::base_range_t> base_range_2_();
            template<typename RangeFacade>
            static meta::id<typename RangeFacade::view_facade_t> view_facade_2_();
        public:
            template<typename RangeAdaptor>
            struct base_range
              : decltype(range_access::base_range_2_<RangeAdaptor>())
            {};
            template<typename RangeAdaptor>
            struct base_range<RangeAdaptor const>
              : std::add_const<meta::_t<base_range<RangeAdaptor>>>
            {};
            template<typename RangeFacade>
            struct view_facade
              : decltype(range_access::view_facade_2_<RangeFacade>())
            {};
            /// endcond
        };
        /// @}

        /// \cond
        namespace detail
        {
            template<typename T>
            using Cursor =
                concepts::models<range_access::Cursor, T>;

            template<typename T>
            using HasEqualCursor =
                concepts::models<range_access::HasEqualCursor, T>;

            template<typename T>
            using ReadableCursor =
                concepts::models<range_access::ReadableCursor, T>;

            template<typename T, typename U>
            using WritableCursor =
                concepts::models<range_access::WritableCursor, T, U>;

            template<typename T>
            using SizedCursor =
                concepts::models<range_access::SizedCursor, T>;

            template<typename T, typename U>
            using SizedCursorRange =
              concepts::models<range_access::SizedCursorRange, T, U>;

            template<typename T, typename U>
            using OutputCursor =
                concepts::models<range_access::OutputCursor, T, U>;

            template<typename T>
            using InputCursor =
                concepts::models<range_access::InputCursor, T>;

            template<typename T>
            using ForwardCursor =
                concepts::models<range_access::ForwardCursor, T>;

            template<typename T>
            using BidirectionalCursor =
                concepts::models<range_access::BidirectionalCursor, T>;

            template<typename T>
            using RandomAccessCursor =
                concepts::models<range_access::RandomAccessCursor, T>;

            template<typename T>
            using InfiniteCursor =
                concepts::models<range_access::InfiniteCursor, T>;

            template<typename T>
            using cursor_concept =
                concepts::most_refined<
                    meta::list<
                        range_access::RandomAccessCursor,
                        range_access::BidirectionalCursor,
                        range_access::ForwardCursor,
                        range_access::InputCursor,
                        range_access::Cursor>, T>;

            template<typename T>
            using cursor_concept_t = meta::_t<cursor_concept<T>>;

            template<typename Cur, bool Readable = (bool) ReadableCursor<Cur>()>
            struct is_writable_cursor_
              : std::true_type
            {};

            template<typename Cur>
            struct is_writable_cursor_<Cur, true>
              : WritableCursor<Cur, range_access::cursor_value_t<Cur> &&>
            {};

            template<typename Cur>
            struct is_writable_cursor
              : detail::is_writable_cursor_<Cur>
            {};
        }
        /// \endcond
    }
}

#endif
