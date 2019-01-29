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

#ifndef RANGES_V3_VIEW_CHUNK_HPP
#define RANGES_V3_VIEW_CHUNK_HPP

#include <functional>
#include <limits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/optional.hpp> // for non_propagating_cache
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/iterator/operations.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename Rng, bool Const>
        constexpr bool can_sized_sentinel_() noexcept
        {
            using I = iterator_t<meta::const_if_c<Const, Rng>>;
            return (bool) SizedSentinel<I, I>;
        }
    }
    /// \endcond

    /// \addtogroup group-views
    /// @{
    template<typename Rng, bool IsForwardRange>
    struct chunk_view_
      : view_adaptor<
            chunk_view_<Rng, IsForwardRange>,
            Rng,
            is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
    {
    private:
        friend range_access;
        CPP_assert(ForwardRange<Rng>);

        template<bool Const>
        using offset_t =
            meta::if_c<
                BidirectionalRange<meta::const_if_c<Const, Rng>> ||
                    detail::can_sized_sentinel_<Rng, Const>(),
                range_difference_t<Rng>,
                constant<range_difference_t<Rng>, 0>>;

        range_difference_t<Rng> n_ = 0;

        template<bool Const>
        struct RANGES_EMPTY_BASES adaptor
          : adaptor_base
          , private box<offset_t<Const>>
        {
        private:
            friend adaptor<!Const>;
            using CRng = meta::const_if_c<Const, Rng>;

            range_difference_t<CRng> n_;
            sentinel_t<CRng> end_;

            constexpr /*c++14*/
            offset_t<Const> const &offset() const
            {
                offset_t<Const> const &result = this->box<offset_t<Const>>::get();
                RANGES_EXPECT(0 <= result && result < n_);
                return result;
            }
            constexpr /*c++14*/
            offset_t<Const> &offset()
            {
                return const_cast<offset_t<Const> &>(
                    const_cast<adaptor const &>(*this).offset());
            }
        public:
            adaptor() = default;
            constexpr adaptor(meta::const_if_c<Const, chunk_view_> &cv)
              : box<offset_t<Const>>{0}
              , n_((RANGES_EXPECT(0 < cv.n_), cv.n_))
              , end_(ranges::end(cv.base()))
            {}
            template<bool Other>
            constexpr CPP_ctor(adaptor)(adaptor<Other> that)(
                requires Const && (!Other))
              : box<offset_t<Const>>(that.offset())
              , n_(that.n_)
              , end_(that.end_)
            {}
            constexpr /*c++14*/
            auto read(iterator_t<CRng> const &it) const ->
                decltype(view::take(make_subrange(it, end_), n_))
            {
                RANGES_EXPECT(it != end_);
                RANGES_EXPECT(0 == offset());
                return view::take(make_subrange(it, end_), n_);
            }
            constexpr /*c++14*/
            void next(iterator_t<CRng> &it)
            {
                RANGES_EXPECT(it != end_);
                RANGES_EXPECT(0 == offset());
                offset() = ranges::advance(it, n_, end_);
            }
            CPP_member
            constexpr /*c++14*/
            auto prev(iterator_t<CRng> &it) -> CPP_ret(void)(
                requires BidirectionalRange<CRng>)
            {
                ranges::advance(it, -n_ + offset());
                offset() = 0;
            }
            CPP_member
            constexpr /*c++14*/
            auto distance_to(iterator_t<CRng> const &here, iterator_t<CRng> const &there,
                adaptor const &that) const ->
                    CPP_ret(range_difference_t<Rng>)(
                        requires detail::can_sized_sentinel_<Rng, Const>())
            {
                auto const delta = (there - here) + (that.offset() - offset());
                // This can fail for cyclic base ranges when the chunk size does not divide the
                // cycle length. Such iterator pairs are NOT in the domain of -.
                RANGES_ENSURE(0 == delta % n_);
                return delta / n_;
            }
            CPP_member
            constexpr /*c++14*/
            auto advance(iterator_t<CRng> &it, range_difference_t<Rng> n) ->
                CPP_ret(void)(
                    requires RandomAccessRange<CRng>)
            {
                using Limits = std::numeric_limits<range_difference_t<CRng>>;
                if(0 < n)
                {
                    RANGES_EXPECT(0 == offset());
                    RANGES_EXPECT(n <= Limits::max() / n_);
                    auto const remainder = ranges::advance(it, n * n_, end_) % n_;
                    RANGES_EXPECT(0 <= remainder && remainder < n_);
                    offset() = remainder;
                }
                else if(0 > n)
                {
                    RANGES_EXPECT(n >= Limits::min() / n_);
                    ranges::advance(it, n * n_ + offset());
                    offset() = 0;
                }
            }
        };

        constexpr /*c++14*/
        adaptor<simple_view<Rng>()> begin_adaptor()
        {
            return adaptor<simple_view<Rng>()>{*this};
        }
        CPP_member
        constexpr auto begin_adaptor() const ->
            CPP_ret(adaptor<true>)(
                requires ForwardRange<Rng const>)
        {
            return adaptor<true>{*this};
        }
        template<typename Size>
        constexpr /*c++14*/ Size size_(Size base_size) const
        {
            auto const n = static_cast<Size>(n_);
            return base_size / n + (0 != (base_size % n));
        }
    public:
        chunk_view_() = default;
        constexpr chunk_view_(Rng rng, range_difference_t<Rng> n)
          : chunk_view_::view_adaptor(detail::move(rng))
          , n_((RANGES_EXPECT(0 < n), n))
        {}
        CPP_member constexpr /*c++14*/
        auto CPP_fun(size)() (const
            requires SizedRange<Rng const>)
        {
            return size_(ranges::size(this->base()));
        }
        CPP_member constexpr /*c++14*/
        auto CPP_fun(size)() (
            requires SizedRange<Rng>)
        {
            return size_(ranges::size(this->base()));
        }
    };

    template<typename Rng>
    struct chunk_view_<Rng, false>
      : view_facade<
            chunk_view_<Rng, false>,
            is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
    {
    private:
        friend range_access;
        CPP_assert(InputRange<Rng> && !ForwardRange<Rng>);

        using iter_cache_t = detail::non_propagating_cache<iterator_t<Rng>>;

        Rng base_;
        range_difference_t<Rng> n_;
        range_difference_t<Rng> remainder_;
        mutable iter_cache_t it_cache_;

        constexpr /*c++14*/ iterator_t<Rng> &it() noexcept
        {
            return *it_cache_;
        }
        constexpr iterator_t<Rng> const &it() const noexcept
        {
            return *it_cache_;
        }

        struct outer_cursor
        {
        private:
            struct inner_view
              : view_facade<inner_view, finite>
            {
            private:
                friend range_access;

                using value_type = range_value_t<Rng>;

                chunk_view_ *rng_ = nullptr;

                constexpr /*c++14*/
                bool done() const noexcept
                {
                    RANGES_EXPECT(rng_);
                    return rng_->remainder_ == 0;
                }
                constexpr /*c++14*/
                bool equal(default_sentinel_t) const noexcept
                {
                    return done();
                }
                constexpr /*c++14*/
                iter_reference_t<iterator_t<Rng>> read() const
                {
                    RANGES_EXPECT(!done());
                    return *rng_->it();
                }
                constexpr /*c++14*/
                iter_rvalue_reference_t<iterator_t<Rng>> move() const
                {
                    RANGES_EXPECT(!done());
                    return ranges::iter_move(rng_->it());
                }
                constexpr /*c++14*/
                void next()
                {
                    RANGES_EXPECT(!done());
                    ++rng_->it();
                    --rng_->remainder_;
                    if(rng_->remainder_ != 0 && rng_->it() == ranges::end(rng_->base_))
                        rng_->remainder_ = 0;
                }
                CPP_member
                constexpr /*c++14*/
                auto distance_to(default_sentinel_t) const ->
                    CPP_ret(range_difference_t<Rng>)(
                        requires SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>)
                {
                    RANGES_EXPECT(rng_);
                    auto const d = ranges::end(rng_->base_) - rng_->it();
                    return ranges::min(d, rng_->remainder_);
                }
            public:
                inner_view() = default;
                constexpr explicit inner_view(chunk_view_ &view) noexcept
                  : rng_{&view}
                {}
                CPP_member constexpr /*c++14*/
                auto CPP_fun(size)() (
                    requires SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>)
                {
                    using size_type = meta::_t<std::make_unsigned<range_difference_t<Rng>>>;
                    return static_cast<size_type>(distance_to(default_sentinel_t{}));
                }
            };

            chunk_view_ *rng_ = nullptr;

        public:
            using value_type = inner_view;

            outer_cursor() = default;
            constexpr explicit outer_cursor(chunk_view_ &view) noexcept
              : rng_{&view}
            {}
            constexpr /*c++14*/
            inner_view read() const
            {
                RANGES_EXPECT(!done());
                return inner_view{*rng_};
            }
            constexpr /*c++14*/
            bool done() const
            {
                RANGES_EXPECT(rng_);
                return rng_->it() == ranges::end(rng_->base_) && rng_->remainder_ != 0;
            }
            constexpr /*c++14*/
            bool equal(default_sentinel_t) const
            {
                return done();
            }
            constexpr /*c++14*/
            void next()
            {
                RANGES_EXPECT(!done());
                ranges::advance(rng_->it(), rng_->remainder_, ranges::end(rng_->base_));
                rng_->remainder_ = rng_->n_;
            }
            CPP_member
            constexpr /*c++14*/
            auto distance_to(default_sentinel_t) const ->
                CPP_ret(range_difference_t<Rng>)(
                    requires SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>)
            {
                RANGES_EXPECT(rng_);
                auto d = ranges::end(rng_->base_) - rng_->it();
                if(d < rng_->remainder_)
                    return 1;

                d -= rng_->remainder_;
                d = (d + rng_->n_ - 1) / rng_->n_;
                d += (rng_->remainder_ != 0);
                return d;
            }
        };

        constexpr /*c++14*/
        outer_cursor begin_cursor() noexcept
        {
            it_cache_ = ranges::begin(base_);
            return outer_cursor{*this};
        }
        template<typename Size>
        constexpr /*c++14*/ Size size_(Size base_size) const
        {
            auto const n = static_cast<Size>(this->n_);
            return base_size / n + (0 != base_size % n);
        }
    public:
        chunk_view_() = default;
        constexpr /*c++14*/
        chunk_view_(Rng rng, range_difference_t<Rng> n)
          : base_(detail::move(rng)), n_((RANGES_EXPECT(0 < n), n)), remainder_(n), it_cache_{nullopt}
        {}
        CPP_member
        constexpr auto CPP_fun(size)() (const
            requires SizedRange<Rng const>)
        {
            return size_(ranges::size(base_));
        }
        CPP_member
        constexpr /*c++14*/ auto CPP_fun(size)() (
            requires SizedRange<Rng>)
        {
            return size_(ranges::size(base_));
        }
        Rng base() const
        {
            return base_;
        }
    };

    template<typename Rng>
    struct chunk_view
      : chunk_view_<Rng, (bool) ForwardRange<Rng>>
    {
        using chunk_view::chunk_view_::chunk_view_;
    };

    namespace view
    {
        // In:  Range<T>
        // Out: Range<Range<T>>, where each inner range has $n$ elements.
        //                       The last range may have fewer.
        struct chunk_fn
        {
        private:
            friend view_access;
            template<typename Int>
            static auto CPP_fun(bind)(chunk_fn chunk, Int n)(
                requires Integral<Int>)
            {
                return make_pipeable(std::bind(chunk, std::placeholders::_1, n));
            }
        public:
            template<typename Rng>
            auto operator()(Rng &&rng, range_difference_t<Rng> n) const ->
                CPP_ret(chunk_view<all_t<Rng>>)(
                    requires ViewableRange<Rng> && InputRange<Rng>)
            {
                return {all(static_cast<Rng &&>(rng)), n};
            }
        };

        /// \relates chunk_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<chunk_fn>, chunk)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::chunk_view)

#endif
