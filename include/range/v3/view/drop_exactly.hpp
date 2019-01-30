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

#ifndef RANGES_V3_VIEW_DROP_EXACTLY_HPP
#define RANGES_V3_VIEW_DROP_EXACTLY_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/view/interface.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/subrange.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    struct RANGES_EMPTY_BASES drop_exactly_view
      : view_interface<
            drop_exactly_view<Rng>,
            is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
      , private detail::non_propagating_cache<
            iterator_t<Rng>, drop_exactly_view<Rng>, !RandomAccessRange<Rng>>
    {
    private:
        using difference_type_ = range_difference_t<Rng>;
        Rng rng_;
        difference_type_ n_;

        // RandomAccessRange == true
        template<bool Const = true>
        auto get_begin_(std::true_type) const ->
            CPP_ret(iterator_t<meta::const_if_c<Const, Rng>>)(
                requires Const &&
                    RandomAccessRange<meta::const_if_c<Const, Rng>>)
        {
            return next(ranges::begin(rng_), n_);
        }
        iterator_t<Rng> get_begin_(std::true_type)
        {
            return next(ranges::begin(rng_), n_);
        }
        // RandomAccessRange == false
        iterator_t<Rng> get_begin_(std::false_type)
        {
            using cache_t = detail::non_propagating_cache<
                iterator_t<Rng>, drop_exactly_view<Rng>>;
            auto &begin_ = static_cast<cache_t&>(*this);
            if(!begin_)
                begin_ = next(ranges::begin(rng_), n_);
            return *begin_;
        }
    public:
        drop_exactly_view() = default;
        drop_exactly_view(Rng rng, difference_type_ n)
          : rng_(std::move(rng)), n_(n)
        {
            RANGES_EXPECT(n >= 0);
        }
        iterator_t<Rng> begin()
        {
            return this->get_begin_(meta::bool_<RandomAccessRange<Rng>>{});
        }
        sentinel_t<Rng> end()
        {
            return ranges::end(rng_);
        }
        template<bool Const = true>
        auto begin() const ->
            CPP_ret(iterator_t<meta::const_if_c<Const, Rng>>)(
                requires Const && RandomAccessRange<meta::const_if_c<Const, Rng>>)
        {
            return this->get_begin_(std::true_type{});
        }
        template<bool Const = true>
        auto end() const ->
            CPP_ret(sentinel_t<meta::const_if_c<Const, Rng>>)(
                requires Const && RandomAccessRange<meta::const_if_c<Const, Rng>>)
        {
            return ranges::end(rng_);
        }
        CPP_member
        auto CPP_fun(size)() (const
            requires SizedRange<Rng const>)
        {
            return ranges::size(rng_) - static_cast<range_size_t<Rng const>>(n_);
        }
        CPP_member
        auto CPP_fun(size)() (
            requires SizedRange<Rng>)
        {
            return ranges::size(rng_) - static_cast<range_size_t<Rng>>(n_);
        }
        Rng base() const
        {
            return rng_;
        }
    };

    namespace view
    {
        struct drop_exactly_fn
        {
        private:
            friend view_access;
            template<typename Int>
            static auto CPP_fun(bind)(drop_exactly_fn drop_exactly, Int n)(
                requires Integral<Int>)
            {
                return make_pipeable(std::bind(drop_exactly, std::placeholders::_1, n));
            }
            template<typename Rng>
            static auto impl_(Rng &&rng, range_difference_t<Rng> n, input_range_tag) ->
                drop_exactly_view<all_t<Rng>>
            {
                return {all(static_cast<Rng &&>(rng)), n};
            }
            template<typename Rng>
            static auto impl_(Rng &&rng, range_difference_t<Rng> n, random_access_range_tag) ->
                CPP_ret(subrange<iterator_t<Rng>, sentinel_t<Rng>>)(
                    requires ForwardingRange_<Rng>)
            {
                return {begin(rng) + n, end(rng)};
            }
        public:
            template<typename Rng>
            auto CPP_fun(operator())(Rng &&rng, range_difference_t<Rng> n) (const
                requires ViewableRange<Rng> && InputRange<Rng>)
            {
                return drop_exactly_fn::impl_(static_cast<Rng &&>(rng), n, range_tag_of<Rng>{});
            }
        };

        /// \relates drop_exactly_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<drop_exactly_fn>, drop_exactly)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::drop_exactly_view)

#endif
