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
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        template<typename Rng>
        struct stride_view;

        namespace detail
        {
            template<typename Rng>
            using stride_view_adaptor = view_adaptor<stride_view<Rng>, Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>;

            // Bidirectional stride views need to remember the distance between
            // the penultimate iterator and the end iterator - which may be less
            // than the stride - so that decrementing an end iterator properly
            // produces the penultimate iterator. stride_view_base specializes on
            // that distinction so that only Bidirectional stride views have the
            // data member "offset_".
            template<typename Rng, bool = BidirectionalRange<Rng>()>
            struct stride_view_base
              : stride_view_adaptor<Rng>
            {
                stride_view_base() = default;
                RANGES_CXX14_CONSTEXPR
                stride_view_base(Rng &&rng, range_difference_type_t<Rng> const stride)
                  : stride_view_adaptor<Rng>{std::move(rng)},
                    stride_{(RANGES_EXPECT(0 < stride), stride)},
                    offset_{calc_offset(SizedRange<Rng>())}
                {}
            protected:
                RANGES_CXX14_CONSTEXPR
                void set_offset(range_difference_type_t<Rng> const delta) noexcept
                {
                    RANGES_EXPECT(0 <= delta && delta < stride_);
                    if(0 > offset_) offset_ = delta;
                    else RANGES_EXPECT(offset_ == delta);
                }
                RANGES_CXX14_CONSTEXPR
                void set_offset(range_difference_type_t<Rng> const) const noexcept
                {}
                RANGES_CXX14_CONSTEXPR
                range_difference_type_t<Rng> get_offset(bool check = true) const noexcept
                {
                    RANGES_EXPECT(!check || 0 <= offset_);
                    return offset_;
                }

                range_difference_type_t<Rng> stride_;
                range_difference_type_t<Rng> offset_ = -1;
            private:
                RANGES_CXX14_CONSTEXPR
                range_difference_type_t<Rng> calc_offset(std::true_type)
                {
                    if(auto const rem = ranges::distance(this->base()) % stride_)
                        return stride_ - rem;
                    else
                        return 0;
                }
                RANGES_CXX14_CONSTEXPR
                range_difference_type_t<Rng> calc_offset(std::false_type) const noexcept
                {
                    return -1;
                }
            };

            template<typename Rng>
            struct stride_view_base<Rng, false>
              : stride_view_adaptor<Rng>
            {
                stride_view_base() = default;
                constexpr stride_view_base(Rng &&rng, range_difference_type_t<Rng> const stride)
                  : stride_view_adaptor<Rng>{std::move(rng)},
                    stride_{(RANGES_EXPECT(0 < stride), stride)}
                {}
            protected:
                RANGES_CXX14_CONSTEXPR
                void set_offset(range_difference_type_t<Rng> const) const noexcept
                {}
                RANGES_CXX14_CONSTEXPR
                range_difference_type_t<Rng> get_offset(bool = true) const noexcept
                {
                    return 0;
                }

                range_difference_type_t<Rng> stride_;
            };
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct stride_view
          : detail::stride_view_base<Rng>
        {
        private:
            friend range_access;

            // stride_view const models Range if Rng const models Range, and
            // either (1) Rng is sized, so we can pre-calculate offset_, or (2)
            // Rng is not Bidirectional, so it does not need offset_.
#ifdef RANGES_WORKAROUND_MSVC_711347
            static constexpr bool const_iterable = Range<Rng const>() &&
                (SizedRange<Rng const>() || !BidirectionalRange<Rng const>());
#else // ^^^ workaround / no workaround vvv
            static constexpr bool const_iterable() noexcept
            {
                return Range<Rng const>() &&
                    (SizedRange<Rng const>() || !BidirectionalRange<Rng const>());
            }
#endif // RANGES_WORKAROUND_MSVC_711347

            // If the underlying range doesn't model BoundedRange, then we can't
            // decrement the end and there's no reason to adapt the sentinel. Strictly
            // speaking, we don't have to adapt the end iterator of Input and Forward
            // Ranges, but in the interests of making the resulting stride view model
            // BoundedView, adapt it anyway.
#ifdef RANGES_WORKAROUND_MSVC_711347
            template<bool Const, class CRng = meta::const_if_c<Const, Rng>>
            static constexpr bool can_bound = BoundedRange<CRng>()
                    && (SizedRange<CRng>() || !BidirectionalRange<CRng>());
#else // ^^^ workaround / no workaround vvv
            template<bool Const>
            static constexpr bool can_bound() noexcept
            {
                using CRng = meta::const_if_c<Const, Rng>;
                return BoundedRange<CRng>()
                    && (SizedRange<CRng>() || !BidirectionalRange<CRng>());
            }
#endif // RANGES_WORKAROUND_MSVC_711347

            template<bool Const>
            struct adaptor : adaptor_base
            {
            private:
                using CRng = meta::const_if_c<Const, Rng>;
                using stride_view_t = meta::const_if_c<Const, stride_view>;
                stride_view_t *rng_;
            public:
                adaptor() = default;
                constexpr adaptor(stride_view_t &rng) noexcept
                  : rng_(&rng)
                {}
                template<bool Other,
                    CONCEPT_REQUIRES_(Const && !Other)>
                adaptor(adaptor<Other> that)
                  : rng_(that.rng_)
                {}
                RANGES_CXX14_CONSTEXPR void next(iterator_t<CRng> &it)
                {
                    auto const last = ranges::end(rng_->base());
                    RANGES_EXPECT(it != last);
                    auto const delta = ranges::advance(it, rng_->stride_, last);
                    if(it == last)
                    {
                        rng_->set_offset(delta);
                    }
                }
                CONCEPT_REQUIRES(BidirectionalRange<CRng>())
                RANGES_CXX14_CONSTEXPR void prev(iterator_t<CRng> &it)
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
                template<class Other,
                    CONCEPT_REQUIRES_(SizedSentinel<Other, iterator_t<CRng>>())>
                RANGES_CXX14_CONSTEXPR range_difference_type_t<Rng> distance_to(
                    iterator_t<CRng> const &here, Other const &there) const
                {
                    range_difference_type_t<Rng> delta = there - here;
                    if(delta < 0)
                        delta -= rng_->stride_ - 1;
                    else
                        delta += rng_->stride_ - 1;
                    return delta / rng_->stride_;
                }
                CONCEPT_REQUIRES(RandomAccessRange<CRng>())
                RANGES_CXX14_CONSTEXPR void advance(
                    iterator_t<CRng> &it, range_difference_type_t<Rng> n)
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
                            // advance hit the end of the base range.
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
            RANGES_CXX14_CONSTEXPR adaptor<false> begin_adaptor()
            {
                return adaptor<false>{*this};
            }
#ifdef RANGES_WORKAROUND_MSVC_711347
            CONCEPT_REQUIRES(const_iterable)
#else // ^^^ workaround / no workaround vvv
            CONCEPT_REQUIRES(const_iterable())
#endif // RANGES_WORKAROUND_MSVC_711347
            constexpr adaptor<true> begin_adaptor() const
            {
                return adaptor<true>{*this};
            }

            RANGES_CXX14_CONSTEXPR
#ifdef RANGES_WORKAROUND_MSVC_711347
            meta::if_c<can_bound<false>, adaptor<false>, adaptor_base> end_adaptor()
#else // ^^^ workaround / no workaround vvv
            meta::if_c<can_bound<false>(), adaptor<false>, adaptor_base> end_adaptor()
#endif // RANGES_WORKAROUND_MSVC_711347
            {
                return {*this};
            }
#ifdef RANGES_WORKAROUND_MSVC_711347
            CONCEPT_REQUIRES(const_iterable)
            constexpr
            meta::if_c<can_bound<true>, adaptor<true>, adaptor_base> end_adaptor() const
#else // ^^^ workaround / no workaround vvv
            CONCEPT_REQUIRES(const_iterable())
            constexpr
            meta::if_c<can_bound<true>(), adaptor<true>, adaptor_base> end_adaptor() const
#endif // RANGES_WORKAROUND_MSVC_711347
            {
                return {*this};
            }

            constexpr range_size_type_t<Rng> size_(range_size_type_t<Rng> const n) const noexcept
            {
                return (n + static_cast<range_size_type_t<Rng>>(this->stride_) - 1) /
                    static_cast<range_size_type_t<Rng>>(this->stride_);
            }
        public:
            stride_view() = default;
            constexpr stride_view(Rng rng, range_difference_type_t<Rng> const stride)
              : detail::stride_view_base<Rng>{std::move(rng), stride}
            {}
            CONCEPT_REQUIRES(SizedRange<Rng>())
            RANGES_CXX14_CONSTEXPR range_size_type_t<Rng> size()
            {
                return size_(ranges::size(this->base()));
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            constexpr range_size_type_t<Rng> size() const
            {
                return size_(ranges::size(this->base()));
            }
        };

        namespace view
        {
            struct stride_fn
            {
            private:
                friend view_access;
                template<typename Difference, CONCEPT_REQUIRES_(Integral<Difference>())>
                RANGES_CXX14_CONSTEXPR
                static auto bind(stride_fn stride, Difference step)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    make_pipeable(std::bind(stride, std::placeholders::_1, std::move(step)))
                )
            public:
                template<typename Rng, CONCEPT_REQUIRES_(InputRange<Rng>())>
                constexpr auto operator()(Rng &&rng, range_difference_type_t<Rng> step) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    stride_view<all_t<Rng>>{all(static_cast<Rng &&>(rng)), step}
                )

                // For the purpose of better error messages:
            #ifndef RANGES_DOXYGEN_INVOKED
            private:
                template<typename Difference, CONCEPT_REQUIRES_(!Integral<Difference>())>
                static detail::null_pipe bind(stride_fn, const Difference &)
                {
                    CONCEPT_ASSERT_MSG(Integral<Difference>(),
                        "The value to be used as the step in a call to view::stride must be a "
                        "model of the Integral concept that is convertible to the range's "
                        "difference type.");
                    return {};
                }
            public:
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!InputRange<Rng>())>
                void operator()(Rng &&, T &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object to be operated on by view::stride should be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(Integral<T>(),
                        "The value to be used as the step in a call to view::stride must be a "
                        "model of the Integral concept that is convertible to the range's "
                        "difference type.");
                }
            #endif
            };

            /// \relates stride_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<stride_fn>, stride)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::stride_view)

#endif
