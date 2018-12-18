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
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
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
            template<typename R = Rng const>
            auto end_(std::false_type) const ->
                CPP_ret(sentinel_t<R>)(
                    requires Range<R &>)
            {
                return ranges::end(rng_);
            }
            template<typename R = Rng const>
            auto end_(std::true_type) const ->
                CPP_ret(iterator_t<R>)(
                    requires Range<R &>)
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

            template<typename R = Rng const>
            auto begin() const ->
                CPP_ret(detail::common_view_iterator_t<R>)(
                    requires Range<R &>)
            {
                return detail::common_view_iterator_t<R>{ranges::begin(rng_)};
            }
            template<typename R = Rng const>
            auto end() const ->
                CPP_ret(detail::common_view_iterator_t<R>)(
                    requires Range<R &>)
            {
                return detail::common_view_iterator_t<R>{
                    end_(meta::bool_<detail::RA_and_Sized<R>>{})};
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
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::common_view)

#endif
