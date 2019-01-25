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
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/view/facade.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/iterator/default_sentinel.hpp>

namespace ranges
{
    CPP_def
    (
        template(typename I)
        concept BidirectionalIncrementable,
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
        template(typename S, typename I)
        concept SizedIncrementableSentinel,
            requires (I i, S s)
            (
                i - s,
                s - i,
                concepts::requires_<Integral<decltype(i - s)>>,
                concepts::requires_<Integral<decltype(s - i)>>
            ) &&
            WeaklyEqualityComparableWith<I, S> &&
            Semiregular<S> &&
            WeaklyIncrementable<I>
    );

    CPP_def
    (
        template(typename I)
        concept RandomAccessIncrementable,
            requires (I i)
            (
                i - i,
                i += (i - i),
                i -= (i - i),
                i - (i - i),
                i + (i - i),
                (i - i) + i,
                concepts::requires_<Integral<decltype(i - i)>>,
                concepts::requires_<Same<I&, decltype(i += (i - i))>>,
                concepts::requires_<Same<I&, decltype(i -= (i - i))>>,
                concepts::requires_<ConvertibleTo<decltype(i - (i - i)), I>>,
                concepts::requires_<ConvertibleTo<decltype(i + (i - i)), I>>,
                concepts::requires_<ConvertibleTo<decltype((i - i) + i), I>>
            ) &&
            BidirectionalIncrementable<I>
    );

    using weakly_incrementable_tag =
        concepts::tag<WeaklyIncrementableConcept>;

    using incrementable_tag =
        concepts::tag<IncrementableConcept, weakly_incrementable_tag>;

    using bidirectional_incrementable_tag =
        concepts::tag<BidirectionalIncrementableConcept, incrementable_tag>;

    using random_access_incrementable_tag =
        concepts::tag<RandomAccessIncrementableConcept, bidirectional_incrementable_tag>;

    template<typename T>
    using incrementable_tag_of =
        concepts::tag_of<
            meta::list<
                RandomAccessIncrementableConcept,
                BidirectionalIncrementableConcept,
                IncrementableConcept,
                WeaklyIncrementableConcept>,
            T>;

    /// \cond
    namespace detail
    {
        template<typename Val, typename Iota = incrementable_tag_of<Val>,
            bool IsIntegral = std::is_integral<Val>::value>
        struct iota_difference_
          : ranges::difference_type<Val>
        {};

        template<typename Val>
        struct iota_difference_<Val, random_access_incrementable_tag, true>
        {
        private:
            using difference_t = decltype(std::declval<Val>() - std::declval<Val>());
            static constexpr std::size_t bits = sizeof(difference_t) * CHAR_BIT;
        public:
            using type =
                meta::if_<
                    meta::not_<std::is_same<Val, difference_t>>,
                    meta::_t<std::make_signed<difference_t>>,
                    meta::if_c<
                        (bits < 16),
                        std::int_fast16_t,
                        meta::if_c<
                            (bits < 32),
                            std::int_fast32_t,
                            std::int_fast64_t>>>;
        };

        template<typename Val>
        struct iota_difference
          : iota_difference_<Val>
        {};

        template<typename Val>
        using iota_difference_t = meta::_t<iota_difference<Val>>;

        CPP_template(typename To, typename From)(
            requires SizedIncrementableSentinel<To, From>)
        auto CPP_auto_fun(iota_minus_)(To const &to, From const &from)
        (
            return to - from
        )

        template<typename Val>
        auto iota_minus_(Val const &v0, Val const &v1) ->
            CPP_ret(iota_difference_t<Val>)(
                requires SignedIntegral<Val>)
        {
            using D = iota_difference_t<Val>;
            return (D) v0 - (D) v1;
        }

        template<typename Val>
        auto iota_minus_(Val const &v0, Val const &v1) ->
            CPP_ret(iota_difference_t<Val>)(
                requires UnsignedIntegral<Val>)
        {
            using D = iota_difference_t<Val>;
            return v0 < v1
                ? -static_cast<D>(v1 - v0)
                :  static_cast<D>(v0 - v1);
        }

        template<typename Val>
        constexpr /*c++14*/ auto ints_open_distance_(Val from, Val to) noexcept ->
            CPP_ret(iota_difference_t<Val>)(
                requires SignedIntegral<Val>)
        {
            using D = iota_difference_t<Val>;
            RANGES_EXPECT(from <= to);
            static_assert(sizeof(iota_difference_t<Val>) >= sizeof(Val),
                "iota_difference_type must be at least as wide as the signed integer type; "
                "otherwise the expression below might overflow when to - from would not.");
            return static_cast<D>(to) - static_cast<D>(from);
        }

        template<typename Val>
        constexpr /*c++14*/ auto ints_open_distance_(Val from, Val to) noexcept ->
            CPP_ret(iota_difference_t<Val>)(
                requires UnsignedIntegral<Val>)
        {
            using D = iota_difference_t<Val>;
            using UD = meta::_t<std::make_unsigned<D>>;
            // Disable wrap-semantics for UnsignedIntegral types.
            // See discussion in: https://github.com/ericniebler/range-v3/pull/593
            RANGES_EXPECT(from <= to);
            // view::take_exactly(Rng rng, iter_difference_t<Rng> n) takes a
            // number of type iter_difference_t<Rng>, which for view::iota is
            // equal to iota_difference_t<Val>.
            //
            // The expression (to - from) shall not overflow this signed
            // difference type. This is ensured by the RANGES_EXPECT below,
            // in which:
            // - the expression is evaluated using the unsigned integer type of the inputs, and
            // - compared against the maximum value representable by the
            //   signed difference_type (which is always positive and can
            //   always be represented by an unsigned integer type of the
            //   same width).
            //
            static_assert(sizeof(D) >= sizeof(Val),
                "the difference type of view::iota must be at least as wide as the UnsignedIntegral type");
            RANGES_EXPECT(Val(to - from) <= static_cast<UD>(std::numeric_limits<D>::max()));
            return static_cast<D>(Val(to - from));
        }

        template<typename Val>
        constexpr /*c++14*/ auto ints_closed_distance_(Val from, Val to) noexcept ->
            CPP_ret(iota_difference_t<Val>)(
                requires Integral<Val>)
        {
            using D = iota_difference_t<Val>;
            auto dist = ints_open_distance_(from, to);
            // Check whether dist + 1 would overflow the signed integer type,
            // introducing undefined behavior:
            RANGES_EXPECT(dist < std::numeric_limits<D>::max());
            return dist + D(1);
        }

        CPP_template(typename Val,
            typename C = common_type_t<Val, iota_difference_t<Val>>)(
            requires Integral<Val>)
        auto CPP_auto_fun(iota_plus)(Val const &v, iota_difference_t<Val> n)
        (
            return static_cast<Val>(static_cast<C>(v) + static_cast<C>(n))
        )

        CPP_template(typename Val)(
            requires not Integral<Val>)
        auto CPP_auto_fun(iota_plus)(Val const &v, iota_difference_t<Val> n)
        (
            return v + n
        )
    }
    /// \endcond

    /// \addtogroup group-views
    /// @{

    /// An iota view in a closed range with non-random access iota value type
    template<typename From, typename To /* = From */>
    struct closed_iota_view
      : view_facade<closed_iota_view<From, To>, finite>
    {
    private:
        friend range_access;
        using difference_type_ = detail::iota_difference_t<From>;

        From from_;
        To to_;
        bool done_ = false;

        From read() const
        {
            RANGES_EXPECT(!done_);
            return from_;
        }
        void next()
        {
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
        auto equal(closed_iota_view const &that) const ->
            CPP_ret(bool)(
                requires Incrementable<From>)
        {
            return that.from_ == from_ && that.done_ == done_;
        }
        CPP_member
        auto prev() -> CPP_ret(void)(
            requires BidirectionalIncrementable<From>)
        {
            if(done_)
                done_ = false;
            else
                --from_;
        }
    public:
        closed_iota_view() = default;
        constexpr closed_iota_view(From from, To to)
          : from_(detail::move(from)), to_(detail::move(to))
        {}
    };

    template<typename From, typename To /* = void*/>
    struct iota_view
      : view_facade<iota_view<From, To>, finite>
    {
    private:
        friend range_access;
        using difference_type_ = detail::iota_difference_t<From>;

        From from_;
        To to_;

        From read() const
        {
            return from_;
        }
        void next()
        {
            ++from_;
        }
        bool equal(default_sentinel_t) const
        {
            return from_ == to_;
        }
        CPP_member
        auto equal(iota_view const &that) const ->
            CPP_ret(bool)(
                requires Incrementable<From>)
        {
            return that.from_ == from_;
        }
        CPP_member
        auto prev() -> CPP_ret(void)(
            requires BidirectionalIncrementable<From>)
        {
            --from_;
        }
        CPP_member
        auto check_advance_(difference_type_ n) -> CPP_ret(void)(
            requires SizedIncrementableSentinel<To, From>)
        {
            detail::ignore_unused(n);
            RANGES_EXPECT(detail::iota_minus_(to_, from_) >= n);
        }
        template<typename = void>
        void check_advance_(difference_type_) const
        {
        }
        CPP_member
        auto advance(difference_type_ n) -> CPP_ret(void)(
            requires RandomAccessIncrementable<From>)
        {
            this->check_advance_(n);
            from_ = detail::iota_plus(from_, n);
        }
        CPP_member
        auto distance_to(iota_view const &that) const ->
            CPP_ret(difference_type_)(
                requires RandomAccessIncrementable<From>)
        {
            return detail::iota_minus_(that.from_, from_);
        }
    public:
        iota_view() = default;
        constexpr iota_view(From from, To to)
          : from_(detail::move(from)), to_(detail::move(to))
        {}
    };

    template<typename From>
    struct iota_view<From, void>
      : view_facade<iota_view<From, void>, infinite>
    {
    private:
        using incrementable_tag_of = ranges::incrementable_tag_of<From>;
        friend range_access;
        using difference_type_ = detail::iota_difference_t<From>;

        From value_;

        From read() const
        {
            return value_;
        }
        void next()
        {
            ++value_;
        }
        constexpr bool equal(default_sentinel_t) const
        {
            return false;
        }
        CPP_member
        auto equal(iota_view const &that) const -> CPP_ret(bool)(
            requires Incrementable<From>)
        {
            return that.value_ == value_;
        }
        CPP_member
        auto prev() -> CPP_ret(void)(
            requires BidirectionalIncrementable<From>)
        {
            --value_;
        }
        CPP_member
        auto advance(difference_type_ n) -> CPP_ret(void)(
            requires RandomAccessIncrementable<From>)
        {
            value_ = detail::iota_plus(value_, n);
        }
        CPP_member
        auto distance_to(iota_view const &that) const ->
            CPP_ret(difference_type_)(
                requires RandomAccessIncrementable<From>)
        {
            return detail::iota_minus_(that.value_, value_);
        }
    public:
        iota_view() = default;
        constexpr explicit iota_view(From value)
          : value_(detail::move(value))
        {}
    };

    namespace view
    {
        struct iota_fn
        {
        private:
            template<typename From>
            static detail::take_exactly_view_<iota_view<From>, true>
            impl_(From from, From to, random_access_incrementable_tag)
            {
                auto n = detail::iota_minus_(std::move(to), from);
                return {iota_view<From>{std::move(from)}, n};
            }
            template<typename From, typename To>
            static iota_view<From, To>
            impl_(From from, To to, weakly_incrementable_tag)
            {
                return {std::move(from), std::move(to)};
            }
        public:
            template<typename From>
            auto operator()(From value) const ->
                CPP_ret(iota_view<From>)(
                    requires WeaklyIncrementable<From>)
            {
                return iota_view<From>{std::move(value)};
            }
            template<typename From, typename To>
            auto CPP_fun(operator())(From from, To to) (const
                requires WeaklyIncrementable<From> && WeaklyEqualityComparableWith<From, To>)
            {
                return iota_fn::impl_(
                    std::move(from),
                    std::move(to),
                    incrementable_tag_of<From>{});
            }
        };

        struct closed_iota_fn
        {
        private:
            template<typename From>
            static detail::take_exactly_view_<iota_view<From>, true>
            impl_(From from, From to, random_access_incrementable_tag)
            {
                return {
                    iota_view<From>{std::move(from)},
                    detail::iota_minus_(to, from) + 1
                };
            }
            template<typename From, typename To>
            static closed_iota_view<From, To>
            impl_(From from, To to, weakly_incrementable_tag)
            {
                return {std::move(from), std::move(to)};
            }
        public:
            template<typename From, typename To>
            auto CPP_fun(operator())(From from, To to) (const
                requires WeaklyIncrementable<From> &&
                    WeaklyEqualityComparableWith<From, To>)
            {
                return closed_iota_fn::impl_(
                    std::move(from),
                    std::move(to),
                    incrementable_tag_of<From>{});
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
            auto operator()(Val value) const ->
                CPP_ret(iota_view<Val>)(
                    requires Integral<Val>)
            {
                return iota_view<Val>{value};
            }
            template<typename Val>
            auto CPP_fun(operator())(Val from, Val to) (const
                requires Integral<Val>)
            {
                return detail::take_exactly_view_<iota_view<Val>, true>
                    {iota_view<Val>{from}, detail::ints_open_distance_(from, to)};
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
