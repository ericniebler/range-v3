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

#ifndef RANGES_V3_VIEW_TAKE_HPP
#define RANGES_V3_VIEW_TAKE_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/iterator/counted_iterator.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    template<typename Rng>
    struct take_view
      : view_interface<take_view<Rng>>
    {
    private:
        CPP_assert(View<Rng>);
        Rng base_ = Rng();
        range_difference_t<Rng> count_ = 0;
        template<bool Const>
        struct sentinel
        {
        private:
            using Base = detail::if_then_t<Const, Rng const, Rng>;
            using CI = counted_iterator<iterator_t<Base>>;
            sentinel_t<Base> end_ = sentinel_t<Base>();
        public:
            sentinel() = default;
            constexpr explicit sentinel(sentinel_t<Base> end)
              : end_(std::move(end))
            {}
            template<bool Other>
            constexpr CPP_ctor(sentinel)(sentinel<Other> that)(
                requires Const && (!Other) &&
                    ConvertibleTo<sentinel_t<Rng>, sentinel_t<Base>>)
              : end_(std::move(that.end_))
            {}
            constexpr sentinel_t<Base> base() const
            {
                return end_;
            }
            friend constexpr bool operator==(sentinel const &x, CI const &y)
            {
                return y.count() == 0 || y.base() == x.end_;
            }
            friend constexpr bool operator==(CI const &y, sentinel const &x)
            {
                return y.count() == 0 || y.base() == x.end_;
            }
            friend constexpr bool operator!=(sentinel const &x, CI const &y)
            {
                return y.count() != 0 && y.base() != x.end_;
            }
            friend constexpr bool operator!=(CI const &y, sentinel const &x)
            {
                return y.count() != 0 && y.base() != x.end_;
            }
        };

#if RANGES_CXX_IF_CONSTEXPR < RANGES_CXX_IF_CONSTEXPR_17
        template<typename Take>
        static auto begin_random_access_(Take &take, std::true_type)
        {
            return ranges::begin(take.base_);
        }
        template<typename Take>
        static auto begin_random_access_(Take &take, std::false_type)
        {
            auto s = static_cast<range_difference_t<Rng>>(take.size());
            return make_counted_iterator(ranges::begin(take.base_), s);
        }
        template<typename Take>
        static auto begin_sized_(Take &take, std::true_type)
        {
            return begin_random_access_(
                take,
                meta::bool_<RandomAccessRange<decltype((take.base_))>>{});
        }
        template<typename Take>
        static auto begin_sized_(Take &take, std::false_type)
        {
            return make_counted_iterator(ranges::begin(take.base_), take.count_);
        }

        template<typename Take>
        static auto end_random_access_(Take &take, std::true_type)
        {
            return ranges::begin(take.base_) + static_cast<range_difference_t<Rng>>(take.size());
        }
        static auto end_random_access_(detail::ignore_t, std::false_type)
        {
            return default_sentinel;
        }
        template<typename Take>
        static auto end_sized_(Take &take, std::true_type, std::false_type) // sized
        {
            return end_random_access_(
                take,
                meta::bool_<RandomAccessRange<decltype((take.base_))>>{});
        }
        static auto end_sized_(detail::ignore_t, std::false_type, std::true_type) // infinite
        {
            return default_sentinel;
        }
        static auto end_sized_(take_view &take, std::false_type, std::false_type)
        {
            return sentinel<false>{ranges::end(take.base_)};
        }
        static auto end_sized_(take_view const &take, std::false_type, std::false_type)
        {
            return sentinel<true>{ranges::end(take.base_)};
        }
#endif
    public:
        take_view() = default;

        constexpr take_view(Rng base, range_difference_t<Rng> count)
          : base_(std::move(base))
          , count_(count)
        {}

        constexpr Rng base() const
        {
            return base_;
        }

        CPP_member
        constexpr auto CPP_fun(begin)() (
            requires (!simple_view<Rng>()))
        {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
            if constexpr(SizedRange<Rng>)
                if constexpr(RandomAccessRange<Rng>)
                    return ranges::begin(base_);
                else
                {
                    auto s = static_cast<range_difference_t<Rng>>(size());
                    return make_counted_iterator(ranges::begin(base_), s);
                }
            else
                return make_counted_iterator(ranges::begin(base_), count_);
#else
            return begin_sized_(*this, meta::bool_<SizedRange<Rng>>{});
#endif
        }

        CPP_member
        constexpr auto CPP_fun(begin)() (const
            requires Range<Rng const>)
        {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
            if constexpr(SizedRange<Rng const>)
                if constexpr(RandomAccessRange<Rng const>)
                    return ranges::begin(base_);
                else
                {
                    auto s = static_cast<range_difference_t<Rng>>(size());
                    return make_counted_iterator(ranges::begin(base_), s);
                }
            else
                return make_counted_iterator(ranges::begin(base_), count_);
#else
            return begin_sized_(*this, meta::bool_<SizedRange<Rng const>>{});
#endif
        }

        CPP_member
        constexpr auto CPP_fun(end)() (
            requires (!simple_view<Rng>()))
        {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
            if constexpr(SizedRange<Rng>)
                if constexpr(RandomAccessRange<Rng>)
                    return ranges::begin(base_) + static_cast<range_difference_t<Rng>>(size());
                else
                    return default_sentinel;
            // Not to spec: Infinite ranges:
            else if constexpr(is_infinite<Rng>::value)
                return default_sentinel;
            else
                return sentinel<false>{ranges::end(base_)};
#else
            return end_sized_(
                *this,
                meta::bool_<SizedRange<Rng>>{},
                is_infinite<Rng>{});
#endif
        }

        CPP_member
        constexpr auto CPP_fun(end)() (const
            requires Range<Rng const>)
        {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
            if constexpr(SizedRange<Rng const>)
                if constexpr(RandomAccessRange<Rng const>)
                    return ranges::begin(base_) + static_cast<range_difference_t<Rng>>(size());
                else
                    return default_sentinel;
            // Not to spec: Infinite ranges:
            else if constexpr(is_infinite<Rng const>::value)
                return default_sentinel;
            else
                return sentinel<true>{ranges::end(base_)};
#else
            return end_sized_(
                *this,
                meta::bool_<SizedRange<Rng const>>{},
                is_infinite<Rng const>{});
#endif
        }

        CPP_member
        constexpr auto CPP_fun(size)() (requires SizedRange<Rng>)
        {
            auto n = ranges::size(base_);
            return ranges::min(n, static_cast<decltype(n)>(count_));
        }
        CPP_member
        constexpr auto CPP_fun(size)() (const requires SizedRange<Rng const>)
        {
            auto n = ranges::size(base_);
            return ranges::min(n, static_cast<decltype(n)>(count_));
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng)(
        requires ViewableRange<Rng>)
    take_view(Rng &&, range_difference_t<Rng>) ->
        take_view<view::all_t<Rng>>;
#endif

    namespace view
    {
        struct take_fn
        {
        private:
            friend view_access;

            template<typename Int>
            static auto CPP_fun(bind)(take_fn take, Int n)(
                requires Integral<Int>)
            {
                return make_pipeable(std::bind(take, std::placeholders::_1, n));
            }

        public:
            template<typename Rng>
            auto operator()(Rng &&rng, range_difference_t<Rng> n) const ->
                CPP_ret(take_view<all_t<Rng>>)(
                    requires ViewableRange<Rng>)
            {
                return {all(static_cast<Rng &&>(rng)), n};
            }
        };

        /// \relates take_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<take_fn>, take)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::take_view)

#endif
