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

#ifndef RANGES_V3_VIEW_REVERSE_HPP
#define RANGES_V3_VIEW_REVERSE_HPP

#include <iterator>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct reverse_view
          : view_interface<reverse_view<Rng>, range_cardinality<Rng>::value>
          , private detail::non_propagating_cache<
                iterator_t<Rng>, reverse_view<Rng>, !BoundedRange<Rng>>
        {
        private:
            CPP_assert(BidirectionalRange<Rng>);
            Rng rng_;
            constexpr /*c++14*/
            reverse_iterator<iterator_t<Rng>> begin_(std::true_type)
            {
                return make_reverse_iterator(ranges::end(rng_));
            }
            constexpr /*c++14*/
            reverse_iterator<iterator_t<Rng>> begin_(std::false_type)
            {
                using cache_t = detail::non_propagating_cache<iterator_t<Rng>, reverse_view<Rng>>;
                auto &end_ = static_cast<cache_t &>(*this);
                if(!end_)
                    end_ = ranges::next(ranges::begin(rng_), ranges::end(rng_));
                return make_reverse_iterator(*end_);
            }
            template<typename T>
            using not_self_ =
                meta::if_c<!std::is_same<reverse_view, detail::decay_t<T>>::value, T>;
        public:
            reverse_view() = default;
            explicit constexpr reverse_view(Rng rng)
              : rng_(detail::move(rng))
            {}
            template<typename O>
            explicit constexpr CPP_ctor(reverse_view)(O&& o)(
                requires ViewableRange<not_self_<O>> &&
                    BidirectionalRange<O> && Constructible<Rng, view::all_t<O>>)
              : rng_(view::all(static_cast<O &&>(o)))
            {}
            Rng base() const
            {
                return rng_;
            }
            constexpr /*c++14*/
            reverse_iterator<iterator_t<Rng>> begin()
            {
                return begin_(meta::bool_<(bool) BoundedRange<Rng>>{});
            }
            template<typename CRng = Rng const>
            constexpr auto begin() const ->
                CPP_ret(reverse_iterator<iterator_t<CRng>>)(
                    requires BoundedRange<CRng>)
            {
                return make_reverse_iterator(ranges::end(rng_));
            }
            constexpr /*c++14*/
            reverse_iterator<iterator_t<Rng>> end()
            {
                return make_reverse_iterator(ranges::begin(rng_));
            }
            template<typename CRng = Rng const>
            constexpr auto end() const ->
                CPP_ret(reverse_iterator<iterator_t<CRng>>)(
                    requires BoundedRange<CRng>)
            {
                return make_reverse_iterator(ranges::begin(rng_));
            }
            CPP_member
            constexpr /*c++14*/ auto CPP_fun(size)() (
                requires SizedRange<Rng>)
            {
                return ranges::size(rng_);
            }
            CPP_member
            constexpr auto CPP_fun(size)() (const
                requires SizedRange<Rng const>)
            {
                return ranges::size(rng_);
            }
        };

        namespace view
        {
            struct reverse_fn
            {
                template<typename Rng>
                constexpr /*c++14*/ auto operator()(Rng &&rng) const ->
                    CPP_ret(reverse_view<all_t<Rng>>)(
                        requires BidirectionalRange<Rng>)
                {
                    return reverse_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Rng>
                auto operator()(Rng &&) const ->
                    CPP_ret(void)(
                        requires not BidirectionalRange<Rng>)
                {
                    CPP_assert_msg(BidirectionalRange<Rng>,
                        "The object on which view::reverse operates must model the "
                        "BidirectionalRange concept.");
                }
            #endif
            };

            /// \relates reverse_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<reverse_fn>, reverse)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::reverse_view)

#endif
