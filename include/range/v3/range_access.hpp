/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
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
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SemiRegular, T>(),
                        concepts::model_of<concepts::SemiRegular, mixin_base_t<T>>(),
                        concepts::model_of<concepts::Constructible, mixin_base_t<T>, T>(),
                        concepts::model_of<concepts::Constructible, mixin_base_t<T>, T const &>()
                    ));
                    // Axiom: mixin_base_t<T> has a member get(), accessible to derived classes,
                    //   which perfectly-returns the contained cursor object and does not throw
                    //   exceptions.
            };
            struct HasCursorNext
            {
                template<typename T>
                auto requires_(T &t) -> decltype(
                    concepts::valid_expr(
                        (t.next(), concepts::void_)
                    ));
            };
            struct CursorSentinel
              : concepts::refines<concepts::SemiRegular(concepts::_1), Cursor(concepts::_2)>
            {
                template<typename S, typename C>
                auto requires_(S &s, C &c) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(c.equal(s))
                    ));
            };
            struct ReadableCursor
            {
                template<typename T>
                auto requires_(T &t) -> decltype(
                    concepts::valid_expr(
                        t.read()
                    ));
            };
            struct HasCursorArrow
            {
                template<typename C>
                auto requires_(C const &c) -> decltype(
                    concepts::valid_expr(
                        c.arrow()
                    ));
            };
            struct WritableCursor
            {
                template<typename T, typename U>
                auto requires_(T &t, U &&u) -> decltype(
                    concepts::valid_expr(
                        (t.write((U &&) u), 42)
                    ));
            };
            struct SizedCursorSentinel
              : concepts::refines<CursorSentinel>
            {
                template<typename S, typename C>
                auto requires_(S &s, C &c) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SignedIntegral>(c.distance_to(s))
                    ));
            };
            struct OutputCursor
              : concepts::refines<WritableCursor, Cursor(concepts::_1)>
            {};
            struct InputCursor
              : concepts::refines<ReadableCursor, Cursor, HasCursorNext>
            {};
            struct ForwardCursor
              : concepts::refines<InputCursor, CursorSentinel(concepts::_1, concepts::_1)>
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_false(single_pass_t<uncvref_t<T>>())
                    ));
            };
            struct BidirectionalCursor
              : concepts::refines<ForwardCursor>
            {
                template<typename T>
                auto requires_(T &t) -> decltype(
                    concepts::valid_expr(
                        (t.prev(), concepts::void_)
                    ));
            };
            struct RandomAccessCursor
              : concepts::refines<BidirectionalCursor, SizedCursorSentinel(concepts::_1, concepts::_1)>
            {
                template<typename T>
                auto requires_(T &t) -> decltype(
                    concepts::valid_expr(
                        (t.advance(t.distance_to(t)), concepts::void_)
                    ));
            };
            struct InfiniteCursor
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(typename T::is_infinite{})
                    ));
            };

            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_cursor(Rng &rng)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                rng.begin_cursor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_cursor(Rng &rng)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                rng.end_cursor()
            )

            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_adaptor(Rng &rng)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                rng.begin_adaptor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_adaptor(Rng &rng)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                rng.end_adaptor()
            )

            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto read(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.read()
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto arrow(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.arrow()
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto move(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.move()
            )
            template<typename Cur, typename T>
            static RANGES_CXX14_CONSTEXPR auto write(Cur &pos, T && t)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.write((T &&) t)
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto next(Cur & pos)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.next()
            )
            template<typename Cur, typename O>
            static RANGES_CXX14_CONSTEXPR auto equal(Cur const &pos, O const &other)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.equal(other)
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
            template<typename Cur, typename O>
            static RANGES_CXX14_CONSTEXPR auto distance_to(Cur const &pos, O const &other)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.distance_to(other)
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

            template<typename T>
            using cursor_reference_t = decltype(std::declval<T const &>().read());

            template<typename T>
            static meta::id<uncvref_t<cursor_reference_t<T>>> cursor_value_2_(long);
            template<typename T>
            static meta::id<typename T::value_type> cursor_value_2_(int);

#ifdef RANGES_WORKAROUND_CWG_1554
            template<typename Cur>
            struct cursor_difference
            {
                using type = decltype(range_access::cursor_difference_2_<Cur>(42));
            };

            template<typename Cur>
            struct cursor_value
              : decltype(range_access::cursor_value_2_<Cur>(42))
            {};
#endif // RANGES_WORKAROUND_CWG_1554
        public:
#ifdef RANGES_WORKAROUND_CWG_1554
            template<typename Cur>
            using cursor_difference_t = meta::_t<cursor_difference<Cur>>;

            template<typename Cur>
            using cursor_value_t = meta::_t<cursor_value<Cur>>;
#else // ^^^ workaround ^^^ / vvv no workaround vvv
            template<typename Cur>
            using cursor_difference_t = decltype(range_access::cursor_difference_2_<Cur>(42));

            template<typename Cur>
            using cursor_value_t = meta::_t<decltype(range_access::cursor_value_2_<Cur>(42))>;
#endif // RANGES_WORKAROUND_CWG_1554

            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR Cur &pos(basic_iterator<Cur> &it) noexcept
            {
                return it.pos();
            }
            template<typename Cur>
            static constexpr Cur const &pos(basic_iterator<Cur> const &it) noexcept
            {
                return it.pos();
            }
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR Cur &&pos(basic_iterator<Cur> &&it) noexcept
            {
                return detail::move(it.pos());
            }

            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR Cur cursor(basic_iterator<Cur> it)
            {
                return std::move(it.pos());
            }
            /// endcond
        };
        /// @}

        /// \cond
        namespace detail
        {
            template<typename T>
            using Cursor =
                concepts::models<range_access::Cursor, T>;

            template<typename S, typename C>
            using CursorSentinel =
                concepts::models<range_access::CursorSentinel, S, C>;

            template<typename T>
            using ReadableCursor =
                concepts::models<range_access::ReadableCursor, T>;

            template<typename T, typename U>
            using WritableCursor =
                concepts::models<range_access::WritableCursor, T, U>;

            template<typename T>
            using HasCursorNext =
                concepts::models<range_access::HasCursorNext, T>;

            template<typename T>
            using HasCursorArrow =
                concepts::models<range_access::HasCursorArrow, T>;

            template<typename S, typename C>
            using SizedCursorSentinel =
                concepts::models<range_access::SizedCursorSentinel, S, C>;

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
              : WritableCursor<Cur, range_access::cursor_value_t<Cur>>
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
