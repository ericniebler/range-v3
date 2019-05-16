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

#ifndef RANGES_V3_VIEW_IOTA_HPP
#define RANGES_V3_VIEW_IOTA_HPP

#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <meta/meta.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/iterator/diffmax_t.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<std::size_t N, typename = void>
        struct promote_as_signed_
        {
            // This shouldn't cause us to LOSE precision, but maybe it doesn't
            // net us any either.
            static_assert(sizeof(std::intmax_t) * CHAR_BIT >= N,
                "Possible extended integral type?");
            using difference_type = diffmax_t;
        };

        template<std::size_t N>
        struct promote_as_signed_<N, enable_if_t<(N < 16)>>
        {
            using difference_type = std::int_fast16_t;
        };

        template<std::size_t N>
        struct promote_as_signed_<N, enable_if_t<(N >= 16 && N < 32)>>
        {
            using difference_type = std::int_fast32_t;
        };

        template<std::size_t N>
        struct promote_as_signed_<N, enable_if_t<(N >= 32 && N < 64)>>
        {
            using difference_type = std::int_fast64_t;
        };

        template<typename I>
        using iota_difference_t =
            typename if_then_t<
                std::is_integral<I>::value && sizeof(I) == sizeof(iter_difference_t<I>),
                promote_as_signed_<sizeof(iter_difference_t<I>) * CHAR_BIT>,
                with_difference_type_<iter_difference_t<I>>>::difference_type;

        CPP_def
        (
            template(typename I)
            concept Decrementable_,
                requires (I i)
                (
                    --i,
                    i--,
                    concepts::requires_<Same<I&, decltype(--i)>>,
                    concepts::requires_<Same<I, decltype(i--)>>
                ) &&
                Incrementable<I>
        );

        CPP_def
        (
            template(typename I)
            concept Advanceable_,
                requires (I i, I const j, iota_difference_t<I> const n)
                (
                    j - j,
                    i += n,
                    i -= n,
                    static_cast<I>(j - n),
                    static_cast<I>(j + n),
                    static_cast<I>(n + j),
                    // NOT TO SPEC:
                    // Unsigned integers are advanceable, but subtracting them results in
                    // an unsigned integral, which is not the same as the difference type,
                    // which is signed.
                    concepts::requires_<ConvertibleTo<decltype(j - j), iota_difference_t<I>>>,
                    concepts::requires_<Same<I&, decltype(i += n)>>,
                    concepts::requires_<Same<I&, decltype(i -= n)>> //,
                    // concepts::requires_<ConvertibleTo<decltype(i - n), I>>,
                    // concepts::requires_<ConvertibleTo<decltype(i + n), I>>,
                    // concepts::requires_<ConvertibleTo<decltype(n + i), I>>
                ) &&
                Decrementable_<I> && StrictTotallyOrdered<I>
        );

        template<typename I>
        auto iota_advance_(I &i, iota_difference_t<I> n) ->
            CPP_ret(void)(
                requires (!UnsignedIntegral<I>))
        {
            // TODO: bounds-check this
            i += n;
        }

        template<typename Int>
        auto iota_advance_(Int &i, iota_difference_t<Int> n) ->
            CPP_ret(void)(
                requires UnsignedIntegral<Int>)
        {
            // TODO: bounds-check this
            if(n >= 0)
                i += static_cast<Int>(n);
            else
                i -= static_cast<Int>(-n);
        }

        template<typename I>
        auto iota_distance_(I const &i, I const &s) ->
            CPP_ret(iota_difference_t<I>)(
                requires Advanceable_<I> && (!Integral<I>))
        {
            return static_cast<iota_difference_t<I>>(s - i);
        }

        template<typename Int>
        auto iota_distance_(Int i0, Int i1) ->
            CPP_ret(iota_difference_t<Int>)(
                requires SignedIntegral<Int>)
        {
            // TODO: bounds-check this
            return static_cast<iota_difference_t<Int>>(
                static_cast<iota_difference_t<Int>>(i1) -
                static_cast<iota_difference_t<Int>>(i0));
        }

        template<typename Int>
        auto iota_distance_(Int i0, Int i1) ->
            CPP_ret(iota_difference_t<Int>)(
                requires UnsignedIntegral<Int>)
        {
            // TODO: bounds-check this
            return (i0 > i1)
              ? static_cast<iota_difference_t<Int>>(-static_cast<iota_difference_t<Int>>(i0 - i1))
              : static_cast<iota_difference_t<Int>>(i1 - i0);
        }
    }
    /// \endcond

    /// \cond
    namespace iota_view_detail
    {
        struct adl_hook
        {};

        // Extension: iota_view models forwarding-range, as suggested by
        // https://github.com/ericniebler/stl2/issues/575
        template<class From, class To>
        constexpr auto begin(iota_view<From, To> r)
        {
            return r.begin();
        }
        template<class From, class To>
        constexpr auto end(iota_view<From, To> r)
        {
            return r.end();
        }

        template<class From, class To>
        constexpr auto begin(closed_iota_view<From, To> r)
        {
            return r.begin();
        }
        template<class From, class To>
        constexpr auto end(closed_iota_view<From, To> r)
        {
            return r.end();
        }
    }
    /// \endcond

    /// \addtogroup group-views
    /// @{

    /// An iota view in a closed range
    template<typename From, typename To /* = From */>
    struct RANGES_EMPTY_BASES closed_iota_view
      : view_facade<closed_iota_view<From, To>, finite>
      , private iota_view_detail::adl_hook
    {
    private:
        friend range_access;

        From from_ = From();
        RANGES_NO_UNIQUE_ADDRESS To to_ = To();

        struct cursor
        {
            using difference_type = detail::iota_difference_t<From>;
        private:
            friend range_access;
            From from_ = From();
            RANGES_NO_UNIQUE_ADDRESS To to_ = To();
            bool done_ = false;

            From read() const
            {
                RANGES_EXPECT(!done_);
                return from_;
            }
            void next()
            {
                RANGES_EXPECT(!done_);
                if(from_ == to_)
                    done_ = true;
                else
                    ++from_;
            }
            bool equal(default_sentinel_t) const
            {
                return done_;
            }
            CPP_member
            auto equal(cursor const &that) const ->
                CPP_ret(bool)(
                    requires EqualityComparable<From>)
            {
                return that.from_ == from_ && that.done_ == done_;
            }
            CPP_member
            auto prev() -> CPP_ret(void)(
                requires detail::Decrementable_<From>)
            {
                if(done_)
                    done_ = false;
                else
                    --from_;
            }
            CPP_member
            auto advance(difference_type n) ->
                CPP_ret(void)(
                    requires detail::Advanceable_<From>)
            {
                if(n > 0)
                {
                    RANGES_ENSURE(detail::iota_distance_(from_, to_) >= n - !done_);
                    detail::iota_advance_(
                        from_,
                        n - (done_ = (detail::iota_distance_(from_, to_) <= n - !done_)));
                }
                else if(n < 0)
                    detail::iota_advance_(from_, n + std::exchange(done_, false));
            }
            CPP_member
            auto distance_to(cursor const &that) const ->
                CPP_ret(difference_type)(
                    requires detail::Advanceable_<From>)
            {
                using D = difference_type;
                return static_cast<D>(detail::iota_distance_(from_, that.from_))
                    + ((D) that.done_ - (D) done_);
            }
            CPP_member
            auto distance_to(default_sentinel_t) const ->
                CPP_ret(difference_type)(
                    requires SizedSentinel<To, From>)
            {
                return difference_type(to_ - from_) + !done_;
            }
        public:
            cursor() = default;
            constexpr cursor(From from, To to, bool done = false)
              : from_(std::move(from))
              , to_(std::move(to))
              , done_(done)
            {}
        };

        cursor begin_cursor() const
        {
            return {from_, to_};
        }
        CPP_member
        auto end_cursor() const ->
            CPP_ret(cursor)(
                requires Same<From, To>)
        {
            return {to_, to_, true};
        }
        CPP_member
        auto end_cursor() const ->
            CPP_ret(default_sentinel_t)(
                requires (!Same<From, To>))
        {
            return {};
        }

        constexpr /*c++14*/ void check_bounds_(std::true_type)
        {
            RANGES_EXPECT(from_ <= to_);
        }
        constexpr /*c++14*/ void check_bounds_(std::false_type)
        {}
    public:
        closed_iota_view() = default;
        constexpr closed_iota_view(meta::id_t<From> from, meta::id_t<To> to)
          : from_(std::move(from))
          , to_(std::move(to))
        {
            check_bounds_(meta::bool_<StrictTotallyOrderedWith<From, To>>{});
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename From, typename To)(
        requires WeaklyIncrementable<From> && Semiregular<To> &&
            (!Integral<From> || !Integral<To> ||
                std::is_signed<From>::value == std::is_signed<To>::value))
    closed_iota_view(From, To) -> closed_iota_view<From, To>;
#endif

    template<typename From, typename To /* = unreachable_sentinel_t*/>
    struct RANGES_EMPTY_BASES iota_view
      : view_facade<
            iota_view<From, To>,
            Same<To, unreachable_sentinel_t>
              ? infinite
              : std::is_integral<From>::value && std::is_integral<To>::value
                  ? finite
                  : unknown>
      , private iota_view_detail::adl_hook
    {
    private:
        friend range_access;
        From from_ = From();
        RANGES_NO_UNIQUE_ADDRESS To to_ = To();

        struct cursor;
        struct sentinel
        {
        private:
            friend struct cursor;
            RANGES_NO_UNIQUE_ADDRESS To to_;
        public:
            sentinel() = default;
            constexpr explicit sentinel(To to)
              : to_(std::move(to))
            {}
        };

        struct cursor
        {
            using difference_type = detail::iota_difference_t<From>;
        private:
            friend range_access;
            From from_;

            From read() const
            {
                return from_;
            }
            void next()
            {
                ++from_;
            }
            bool equal(sentinel const &that) const
            {
                return from_ == that.to_;
            }
            CPP_member
            auto equal(cursor const &that) const ->
                CPP_ret(bool)(
                    requires EqualityComparable<From>)
            {
                return that.from_ == from_;
            }
            CPP_member
            auto prev() -> CPP_ret(void)(
                requires detail::Decrementable_<From>)
            {
                --from_;
            }
            CPP_member
            auto advance(difference_type n) -> CPP_ret(void)(
                requires detail::Advanceable_<From>)
            {
                detail::iota_advance_(from_, n);
            }
            // Not to spec: TODO the relational operators will effectively be constrained
            // with Advanceable, but they should be constrained with StrictTotallyOrdered.
            // Reimplement iota_view without view_facade or basic_iterator.
            CPP_member
            auto distance_to(cursor const &that) const ->
                CPP_ret(difference_type)(
                    requires detail::Advanceable_<From>)
            {
                return detail::iota_distance_(from_, that.from_);
            }
            // Extension: see https://github.com/ericniebler/stl2/issues/613
            CPP_member
            auto distance_to(sentinel const &that) const ->
                CPP_ret(difference_type)(
                    requires SizedSentinel<To, From>)
            {
                return that.to_ - from_;
            }
        public:
            cursor() = default;
            constexpr explicit cursor(From from)
              : from_(std::move(from))
            {}
        };
        cursor begin_cursor() const
        {
            return cursor{from_};
        }
        // Not to spec: see https://github.com/ericniebler/stl2/issues/615
        CPP_member
        auto CPP_fun(end_cursor)() (const
            requires (Same<To, unreachable_sentinel_t>))
        {
            return unreachable;
        }
        CPP_member
        auto CPP_fun(end_cursor)() (const
            requires (!Same<To, unreachable_sentinel_t>))
        {
            return detail::if_then_t<Same<From, To>, cursor, sentinel>{to_};
        }
        constexpr /*c++14*/ void check_bounds_(std::true_type)
        {
            RANGES_EXPECT(from_ <= to_);
        }
        constexpr /*c++14*/ void check_bounds_(std::false_type)
        {}
    public:
        iota_view() = default;
        constexpr explicit iota_view(From from)
          : from_(std::move(from))
        {}
        constexpr iota_view(meta::id_t<From> from, meta::id_t<To> to)
          : from_(std::move(from)), to_(std::move(to))
        {
            check_bounds_(meta::bool_<StrictTotallyOrderedWith<From, To>>{});
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename From, typename To)(
        requires WeaklyIncrementable<From> && Semiregular<To> &&
            (!Integral<From> || !Integral<To> ||
                std::is_signed<From>::value == std::is_signed<To>::value))
    iota_view(From, To) -> iota_view<From, To>;
#endif

    namespace view
    {
        struct iota_fn
        {
            template<typename From>
            auto operator()(From value) const ->
                CPP_ret(iota_view<From>)(
                    requires WeaklyIncrementable<From>)
            {
                return iota_view<From>{std::move(value)};
            }
            template<typename From, typename To>
            auto operator()(From from, To to) const ->
                CPP_ret(iota_view<From, To>)(
                    requires WeaklyIncrementable<From> && Semiregular<To> &&
                        detail::WeaklyEqualityComparableWith_<From, To> &&
                        (!Integral<From> || !Integral<To> ||
                            std::is_signed<From>::value == std::is_signed<To>::value))
            {
                return {std::move(from), std::move(to)};
            }
        };

        struct closed_iota_fn
        {
            template<typename From, typename To>
            auto operator()(From from, To to) const ->
                CPP_ret(closed_iota_view<From, To>)(
                    requires WeaklyIncrementable<From> && Semiregular<To> &&
                        detail::WeaklyEqualityComparableWith_<From, To> &&
                        (!Integral<From> || !Integral<To> ||
                            std::is_signed<From>::value == std::is_signed<To>::value))
            {
                return {std::move(from), std::move(to)};
            }
        };

        /// \relates iota_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(iota_fn, iota)

        /// \relates closed_iota_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(closed_iota_fn, closed_iota)

        struct ints_fn
          : iota_view<int>
        {
            ints_fn() = default;

            template<typename Val>
            RANGES_DEPRECATED("This potentially confusing API is deprecated. Prefer to "
                "explicitly specify the upper bound as with ranges::unreachable, as in "
                "view::ints( n, unreachable )")
            constexpr auto operator()(Val value) const ->
                CPP_ret(iota_view<Val>)(
                    requires Integral<Val>)
            {
                return iota_view<Val>{value};
            }
            template<typename Val>
            constexpr auto operator()(Val value, unreachable_sentinel_t) const ->
                CPP_ret(iota_view<Val>)(
                    requires Integral<Val>)
            {
                return iota_view<Val>{value};
            }
            template<typename Val>
            constexpr auto operator()(Val from, Val to) const ->
                CPP_ret(iota_view<Val, Val>)(
                    requires Integral<Val>)
            {
                return {from, to};
            }
        };

        /// \relates ints_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(ints_fn, ints)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::closed_iota_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::iota_view)

#endif
