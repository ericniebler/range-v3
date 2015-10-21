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
            template<typename T, typename Enable = void>
            struct single_pass_2_
            {
                using type = std::false_type;
            };

            template<typename T>
            struct single_pass_2_<T, meta::void_<typename T::single_pass>>
            {
                using type = typename T::single_pass;
            };

            template<typename T>
            struct single_pass_
              : single_pass_2_<T>
            {};

            template<typename Cur, typename Enable = void>
            struct mixin_base_2_
            {
                using type = basic_mixin<Cur>;
            };

            template<typename Cur>
            struct mixin_base_2_<Cur, meta::void_<typename Cur::mixin>>
            {
                using type = typename Cur::mixin;
            };

            template<typename Cur>
            struct mixin_base_
              : mixin_base_2_<Cur>
            {};

            struct HasDoneCursor_
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t.done())
                    ));
            };

            struct HasEqualCursor_
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t.equal(t))
                    ));
            };

        public:
            template<typename Cur>
            using single_pass_t = meta::_t<single_pass_<Cur>>;

            template<typename Cur>
            using mixin_base_t = meta::_t<mixin_base_<Cur>>;

            //
            // Concepts that the range cursor must model
            //
            struct WeakCursor
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        (t.next(), concepts::void_)
                    ));
            };
            struct Cursor
              : concepts::refines<WeakCursor>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(
                            meta::or_<
                                concepts::models<HasDoneCursor_, T>,
                                concepts::models<HasEqualCursor_, T> >())
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
            struct WeakOutputCursor
              : concepts::refines<WritableCursor, WeakCursor(concepts::_1)>
            {};
            struct OutputCursor
              : concepts::refines<WeakOutputCursor, Cursor(concepts::_1)>
            {};
            struct WeakInputCursor
              : concepts::refines<WeakCursor, ReadableCursor>
            {};
            struct InputCursor
              : concepts::refines<WeakInputCursor, Cursor>
            {};
            struct ForwardCursor
              : concepts::refines<WeakInputCursor, HasEqualCursor_>
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
              : concepts::refines<BidirectionalCursor>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SignedIntegral>(t.distance_to(t)),
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
            static RANGES_CXX14_CONSTEXPR auto set(Cur const &pos, T &&t)
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
            RANGES_CXX14_CONSTEXPR
            static auto distance_to(Cur const &pos0, Cur const &pos1)
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
                using type = uncvref_t<decltype(std::declval<Cur const &>().get())>;
            };

            template<typename Cur>
            struct cursor_value<Cur, meta::void_<typename Cur::value_type>>
            {
                using type = typename Cur::value_type;
            };
        public:
            template<typename Cur>
            using cursor_difference_t = typename cursor_difference<Cur>::type;

            template<typename Cur>
            using cursor_value_t = typename cursor_value<Cur>::type;

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
            struct view_facade
            {
                using type = typename RangeFacade::view_facade_t;
            };
            template<typename RangeAdaptor>
            struct view_adaptor
            {
                using type = typename RangeAdaptor::view_adaptor_t;
            };
            /// endcond
        };
        /// @}

        /// \cond
        namespace detail
        {
            template<typename T>
            using ReadableCursor =
                concepts::models<range_access::ReadableCursor, T>;

            template<typename T, typename U>
            using WritableCursor =
                concepts::models<range_access::WritableCursor, T, U>;

            template<typename T>
            using WeakCursor =
                concepts::models<range_access::WeakCursor, T>;

            template<typename T>
            using Cursor =
                concepts::models<range_access::Cursor, T>;

            template<typename T, typename U>
            using WeakOutputCursor =
                concepts::models<range_access::WeakOutputCursor, T, U>;

            template<typename T, typename U>
            using OutputCursor =
                concepts::models<range_access::OutputCursor, T, U>;

            template<typename T>
            using WeakInputCursor =
                concepts::models<range_access::WeakInputCursor, T>;

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
                        range_access::WeakInputCursor,
                        range_access::Cursor,
                        range_access::WeakCursor>, T>;

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
