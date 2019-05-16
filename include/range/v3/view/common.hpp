/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_COMMON_HPP
#define RANGES_V3_VIEW_COMMON_HPP

#include <type_traits>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/view/interface.hpp>
#include <range/v3/iterator/common_iterator.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{

    namespace detail
    {
        /// \cond
        CPP_def
        (
            template(typename R)
            concept RA_and_Sized,
                RandomAccessRange<R> && SizedRange<R>
        );

        template<typename R>
        using common_view_iterator_t = meta::if_c<RA_and_Sized<R>,
            iterator_t<R>,
            common_iterator_t<iterator_t<R>, sentinel_t<R>>>;

        template<typename Rng>
        struct is_common_range
          : meta::bool_<CommonRange<Rng>>
        {};
        /// \endcond
    }

    template<typename Rng, bool = detail::is_common_range<Rng>::value>
    struct common_view
      : view_interface<common_view<Rng>, range_cardinality<Rng>::value>
    {
    private:
        CPP_assert(View<Rng>);
        CPP_assert(!CommonView<Rng>);
        Rng rng_;

        sentinel_t<Rng> end_(std::false_type)
        {
            return ranges::end(rng_);
        }
        iterator_t<Rng> end_(std::true_type)
        {
            return ranges::begin(rng_) + ranges::distance(rng_);
        }
        template<bool Const = true>
        auto end_(std::false_type) const ->
            CPP_ret(sentinel_t<meta::const_if_c<Const, Rng>>)(
                requires Const && Range<meta::const_if_c<Const, Rng>>)
        {
            return ranges::end(rng_);
        }
        template<bool Const = true>
        auto end_(std::true_type) const ->
            CPP_ret(iterator_t<meta::const_if_c<Const, Rng>>)(
                requires Const && Range<meta::const_if_c<Const, Rng>>)
        {
            return ranges::begin(rng_) + ranges::distance(rng_);
        }
    public:
        common_view() = default;
        explicit common_view(Rng rng)
          : rng_(detail::move(rng))
        {}
        Rng base() const
        {
            return rng_;
        }

        detail::common_view_iterator_t<Rng> begin()
        {
            return detail::common_view_iterator_t<Rng>{ranges::begin(rng_)};
        }
        detail::common_view_iterator_t<Rng> end()
        {
            return detail::common_view_iterator_t<Rng>{
                end_(meta::bool_<detail::RA_and_Sized<Rng>>{})};
        }
        CPP_member
        auto CPP_fun(size)() (
            requires SizedRange<Rng>)
        {
            return ranges::size(rng_);
        }

        template<bool Const = true>
        auto begin() const ->
            CPP_ret(detail::common_view_iterator_t<meta::const_if_c<Const, Rng>>)(
                requires Range<meta::const_if_c<Const, Rng>>)
        {
            return detail::common_view_iterator_t<meta::const_if_c<Const, Rng>>{
                ranges::begin(rng_)};
        }
        template<bool Const = true>
        auto end() const ->
            CPP_ret(detail::common_view_iterator_t<meta::const_if_c<Const, Rng>>)(
                requires Range<meta::const_if_c<Const, Rng>>)
        {
            return detail::common_view_iterator_t<meta::const_if_c<Const, Rng>>{
                end_(meta::bool_<detail::RA_and_Sized<meta::const_if_c<Const, Rng>>>{})};
        }
        CPP_member
        auto CPP_fun(size)() (const
            requires SizedRange<Rng const>)
        {
            return ranges::size(rng_);
        }
    };

    template<typename Rng>
    struct common_view<Rng, true>
      : identity_adaptor<Rng>
    {
        CPP_assert(CommonRange<Rng>);
        using identity_adaptor<Rng>::identity_adaptor;
    };

    namespace view
    {
        struct cpp20_common_fn
        {
            template<typename Rng>
            auto operator()(Rng &&rng) const ->
                CPP_ret(common_view<all_t<Rng>>)(
                    requires ViewableRange<Rng> && (!CommonRange<Rng>))
            {
                return common_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
            }
        };

        struct common_fn
        {
            template<typename Rng>
            auto operator()(Rng &&rng) const ->
                CPP_ret(common_view<all_t<Rng>>)(
                    requires ViewableRange<Rng>)
            {
                return common_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
            }
        };

        /// \relates common_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<common_fn>, common)
    }
    /// @}

    /// \cond
    template<typename Rng>
    using bounded_view
        RANGES_DEPRECATED("The name bounded_view is deprecated. "
                          "Please use common_view instead.") =
            common_view<Rng>;
    /// \endcond

    namespace view
    {
        /// \cond
        namespace
        {
            RANGES_DEPRECATED("The name view::bounded is deprecated. "
                              "Please use view::common instead.")
            RANGES_INLINE_VAR constexpr auto &bounded = common;
        }

        template<typename Rng>
        using bounded_t
            RANGES_DEPRECATED("The name view::bounded_t is deprecated.") =
                decltype(common(std::declval<Rng>()));
        /// \endcond
    }

    namespace cpp20
    {
        namespace view
        {
            RANGES_INLINE_VARIABLE(ranges::view::view<ranges::view::cpp20_common_fn>, common)
        }
        CPP_template(typename Rng)(
            requires View<Rng> && (!CommonRange<Rng>))
        using common_view = ranges::common_view<Rng>;
    }
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::common_view)

#endif
