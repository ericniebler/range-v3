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

#ifndef RANGES_V3_VIEW_TAKE_EXACTLY_HPP
#define RANGES_V3_VIEW_TAKE_EXACTLY_HPP

#include <type_traits>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/iterator/counted_iterator.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/interface.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename Rng>
        struct is_random_access_common_
          : meta::bool_<(bool)random_access_range<Rng> && (bool)common_range<Rng>>
        {};

        // BUGBUG Per the discussion in https://github.com/ericniebler/stl2/issues/63,
        // it's unclear if we can infer anything from random_access_range<Rng> &&
        // common_range<Rng>
        template<typename Rng,
                 bool IsRandomAccessCommon /*= is_random_access_common_<Rng>::value*/>
        struct take_exactly_view_
          : view_interface<take_exactly_view_<Rng, IsRandomAccessCommon>, finite>
        {
        private:
            Rng rng_;
            range_difference_t<Rng> n_;

        public:
            take_exactly_view_() = default;
            take_exactly_view_(Rng rng, range_difference_t<Rng> n)
              : rng_(std::move(rng))
              , n_(n)
            {
                RANGES_EXPECT(n >= 0);
            }
            counted_iterator<iterator_t<Rng>> begin()
            {
                return {ranges::begin(rng_), n_};
            }
            template<typename BaseRng = Rng>
            auto begin() const -> CPP_ret(counted_iterator<iterator_t<BaseRng const>>)( //
                requires range<BaseRng const>)
            {
                return {ranges::begin(rng_), n_};
            }
            default_sentinel_t end() const
            {
                return {};
            }
            auto size() const
            {
                return static_cast<detail::iter_size_t<iterator_t<Rng>>>(n_);
            }
            Rng base() const
            {
                return rng_;
            }
        };

        template<typename Rng>
        struct take_exactly_view_<Rng, true>
          : view_interface<take_exactly_view_<Rng, true>, finite>
        {
        private:
            Rng rng_;
            range_difference_t<Rng> n_;

        public:
            take_exactly_view_() = default;
            take_exactly_view_(Rng rng, range_difference_t<Rng> n)
              : rng_(std::move(rng))
              , n_(n)
            {
                RANGES_EXPECT(n >= 0);
                RANGES_EXPECT(!(bool)sized_range<Rng> || n <= ranges::distance(rng_));
            }
            iterator_t<Rng> begin()
            {
                return ranges::begin(rng_);
            }
            iterator_t<Rng> end()
            {
                return ranges::begin(rng_) + n_;
            }
            CPP_member
            auto CPP_fun(begin)()(const requires range<Rng const>)
            {
                return ranges::begin(rng_);
            }
            CPP_member
            auto CPP_fun(end)()(const requires range<Rng const>)
            {
                return ranges::begin(rng_) + n_;
            }
            detail::iter_size_t<iterator_t<Rng>> size() const
            {
                return static_cast<detail::iter_size_t<iterator_t<Rng>>>(n_);
            }
            Rng base() const
            {
                return rng_;
            }
        };
    } // namespace detail
    /// \endcond

    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    using take_exactly_view = detail::take_exactly_view_<Rng>;

    namespace views
    {
        struct take_exactly_fn
        {
        private:
            friend view_access;

            template<typename Int>
            static constexpr auto CPP_fun(bind)(take_exactly_fn take_exactly, Int n)( //
                requires integral<Int>)
            {
                return make_pipeable(bind_back(take_exactly, n));
            }

            template<typename Rng>
            static constexpr take_exactly_view<all_t<Rng>> impl_(
                Rng && rng, range_difference_t<Rng> n, input_range_tag)
            {
                return {all(static_cast<Rng &&>(rng)), n};
            }
            template<typename Rng>
            static constexpr auto impl_(Rng && rng, range_difference_t<Rng> n,
                                        random_access_range_tag)
                -> CPP_ret(subrange<iterator_t<Rng>>)( //
                    requires forwarding_range_<Rng>)
            {
                return {begin(rng), next(begin(rng), n)};
            }

        public:
            template<typename Rng>
            constexpr auto CPP_fun(operator())(Rng && rng, range_difference_t<Rng> n)(
                const requires viewable_range<Rng> && input_range<Rng>)
            {
                return take_exactly_fn::impl_(
                    static_cast<Rng &&>(rng), n, range_tag_of<Rng>{});
            }
        };

        /// \relates take_exactly_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<take_exactly_fn>, take_exactly)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::detail::take_exactly_view_)

#endif
