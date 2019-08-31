/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_STRIDE_HPP
#define RANGES_V3_VIEW_STRIDE_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \cond
    template<typename Rng>
    struct stride_view;

    namespace detail
    {
        template<typename Rng>
        using stride_view_adaptor =
            view_adaptor<stride_view<Rng>, Rng,
                         is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>;

        // Bidirectional stride views need to remember the distance between
        // the penultimate iterator and the last iterator - which may be less
        // than the stride - so that decrementing an last iterator properly
        // produces the penultimate iterator. stride_view_base specializes on
        // that distinction so that only Bidirectional stride views have the
        // data member "offset_".
        template<typename Rng, bool BidiRange>
        struct stride_view_base_;
        template<typename Rng>
        using stride_view_base = stride_view_base_<Rng, (bool)bidirectional_range<Rng>>;

        template<typename Rng, bool /*= bidirectional_range<Rng>*/>
        struct stride_view_base_ : stride_view_adaptor<Rng>
        {
            stride_view_base_() = default;
            constexpr stride_view_base_(Rng && rng, range_difference_t<Rng> const stride)
              : stride_view_adaptor<Rng>{std::move(rng)}
              , stride_{(RANGES_EXPECT(0 < stride), stride)}
              , offset_{calc_offset(meta::bool_<sized_range<Rng>>{})}
            {}

        protected:
            constexpr void set_offset(range_difference_t<Rng> const delta) noexcept
            {
                RANGES_EXPECT(0 <= delta && delta < stride_);
                if(0 > offset_)
                    offset_ = delta;
                else
                    RANGES_EXPECT(offset_ == delta);
            }
            constexpr void set_offset(range_difference_t<Rng> const) const noexcept
            {}
            constexpr range_difference_t<Rng> get_offset(bool check = true) const noexcept
            {
                RANGES_EXPECT(!check || 0 <= offset_);
                return offset_;
            }

            range_difference_t<Rng> stride_;
            range_difference_t<Rng> offset_ = -1;

        private:
            constexpr range_difference_t<Rng> calc_offset(std::true_type)
            {
                if(auto const rem = ranges::distance(this->base()) % stride_)
                    return stride_ - rem;
                else
                    return 0;
            }
            constexpr range_difference_t<Rng> calc_offset(std::false_type) const noexcept
            {
                return -1;
            }
        };

        template<typename Rng>
        struct stride_view_base_<Rng, false> : stride_view_adaptor<Rng>
        {
            stride_view_base_() = default;
            constexpr stride_view_base_(Rng && rng, range_difference_t<Rng> const stride)
              : stride_view_adaptor<Rng>{std::move(rng)}
              , stride_{(RANGES_EXPECT(0 < stride), stride)}
            {}

        protected:
            constexpr void set_offset(range_difference_t<Rng> const) const noexcept
            {}
            constexpr range_difference_t<Rng> get_offset(bool = true) const noexcept
            {
                return 0;
            }

            range_difference_t<Rng> stride_;
        };
    } // namespace detail
    /// \endcond

    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    struct stride_view : detail::stride_view_base<Rng>
    {
    private:
        friend range_access;

        // stride_view const models Range if Rng const models Range, and
        // either (1) Rng is sized, so we can pre-calculate offset_, or (2)
        // Rng is !Bidirectional, so it does not need offset_.
        static constexpr bool const_iterable() noexcept
        {
            return range<Rng const> &&
                   (sized_range<Rng const> || !bidirectional_range<Rng const>);
        }

        // If the underlying range doesn't model common_range, then we can't
        // decrement the last and there's no reason to adapt the sentinel. Strictly
        // speaking, we don't have to adapt the last iterator of input and forward
        // ranges, but in the interests of making the resulting stride view model
        // common_range, adapt it anyway.
        template<bool Const>
        static constexpr bool can_bound() noexcept
        {
            using CRng = meta::const_if_c<Const, Rng>;
            return common_range<CRng> &&
                   (sized_range<CRng> || !bidirectional_range<CRng>);
        }

        template<bool Const>
        struct adaptor : adaptor_base
        {
        private:
            friend struct adaptor<!Const>;
            using CRng = meta::const_if_c<Const, Rng>;
            using stride_view_t = meta::const_if_c<Const, stride_view>;
            stride_view_t * rng_;

        public:
            adaptor() = default;
            constexpr adaptor(stride_view_t * rng) noexcept
              : rng_(rng)
            {}
            CPP_template(bool Other)( //
                requires Const && (!Other)) adaptor(adaptor<Other> that)
              : rng_(that.rng_)
            {}
            constexpr void next(iterator_t<CRng> & it)
            {
                auto const last = ranges::end(rng_->base());
                RANGES_EXPECT(it != last);
                auto const delta = ranges::advance(it, rng_->stride_, last);
                if(it == last)
                {
                    rng_->set_offset(delta);
                }
            }
            CPP_member
            constexpr auto prev(iterator_t<CRng> & it) -> CPP_ret(void)( //
                requires bidirectional_range<CRng>)
            {
                RANGES_EXPECT(it != ranges::begin(rng_->base()));
                auto delta = -rng_->stride_;
                if(it == ranges::end(rng_->base()))
                {
                    RANGES_EXPECT(rng_->get_offset() >= 0);
                    delta += rng_->get_offset();
                }
                ranges::advance(it, delta);
            }
            template<typename Other>
            constexpr auto distance_to(iterator_t<CRng> const & here,
                                       Other const & there) const
                -> CPP_ret(range_difference_t<Rng>)( //
                    requires sized_sentinel_for<Other, iterator_t<CRng>>)
            {
                range_difference_t<Rng> delta = there - here;
                if(delta < 0)
                    delta -= rng_->stride_ - 1;
                else
                    delta += rng_->stride_ - 1;
                return delta / rng_->stride_;
            }
            CPP_member
            constexpr auto advance(iterator_t<CRng> & it,
                                   range_difference_t<Rng> n) -> CPP_ret(void)( //
                requires random_access_range<CRng>)
            {
                if(0 == n)
                    return;
                n *= rng_->stride_;
                auto const last = ranges::end(rng_->base());
                if(it == last)
                {
                    RANGES_EXPECT(n < 0);
                    RANGES_EXPECT(rng_->get_offset() >= 0);
                    n += rng_->get_offset();
                }
                if(0 < n)
                {
                    auto delta = ranges::advance(it, n, last);
                    if(it == last)
                    {
                        // advance hit the last of the base range.
                        rng_->set_offset(delta % rng_->stride_);
                    }
                }
                else if(0 > n)
                {
#ifdef NDEBUG
                    ranges::advance(it, n);
#else
                    auto const first = ranges::begin(rng_->base());
                    auto const delta = ranges::advance(it, n, first);
                    RANGES_EXPECT(delta == 0);
#endif
                }
            }
        };
        constexpr auto begin_adaptor() noexcept -> adaptor<false>
        {
            return adaptor<false>{this};
        }
        CPP_member
        constexpr auto begin_adaptor() const noexcept
            -> CPP_ret(adaptor<true>)(requires(const_iterable()))
        {
            return adaptor<true>{this};
        }

        constexpr auto end_adaptor() noexcept
            -> meta::if_c<can_bound<false>(), adaptor<false>, adaptor_base>
        {
            return {this};
        }
        CPP_member
        constexpr auto end_adaptor() const noexcept
            -> CPP_ret(meta::if_c<can_bound<true>(), adaptor<true>, adaptor_base>)( //
                requires(const_iterable()))
        {
            return {this};
        }

    public:
        stride_view() = default;
        constexpr stride_view(Rng rng, range_difference_t<Rng> const stride)
          : detail::stride_view_base<Rng>{std::move(rng), stride}
        {}
        CPP_member
        constexpr auto CPP_fun(size)()(requires sized_range<Rng>)
        {
            using size_type = range_size_t<Rng>;
            auto const n = ranges::size(this->base());
            return (n + static_cast<size_type>(this->stride_) - 1) /
                   static_cast<size_type>(this->stride_);
        }
        CPP_member
        constexpr auto CPP_fun(size)()(const requires sized_range<Rng const>)
        {
            using size_type = range_size_t<Rng const>;
            auto const n = ranges::size(this->base());
            return (n + static_cast<size_type>(this->stride_) - 1) /
                   static_cast<size_type>(this->stride_);
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng>
    stride_view(Rng &&, range_difference_t<Rng>)->stride_view<views::all_t<Rng>>;
#endif

    namespace views
    {
        struct stride_fn
        {
        private:
            friend view_access;
            template<typename Difference>
            constexpr static auto CPP_fun(bind)(stride_fn stride, Difference step)( //
                requires integral<Difference>)
            {
                return make_pipeable(bind_back(stride, std::move(step)));
            }

        public:
            template<typename Rng>
            constexpr auto operator()(Rng && rng, range_difference_t<Rng> step) const
                -> CPP_ret(stride_view<all_t<Rng>>)( //
                    requires viewable_range<Rng> && input_range<Rng>)
            {
                return stride_view<all_t<Rng>>{all(static_cast<Rng &&>(rng)), step};
            }
        };

        /// \relates stride_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<stride_fn>, stride)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::stride_view)

#endif
