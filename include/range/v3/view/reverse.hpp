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
          : view_adaptor<reverse_view<Rng>, Rng>
          , private detail::non_propagating_cache<
                iterator_t<Rng>, reverse_view<Rng>, !BoundedRange<Rng>>
        {
            CPP_assert(BidirectionalRange<Rng>);

            reverse_view() = default;
            explicit constexpr reverse_view(Rng rng)
                noexcept(std::is_nothrow_constructible<
                    typename reverse_view::view_adaptor, Rng>::value)
              : reverse_view::view_adaptor{detail::move(rng)}
            {}
            CPP_member
            constexpr auto size() const
                noexcept(noexcept(ranges::size(std::declval<Rng const &>()))) ->
                CPP_ret(range_size_type_t<Rng>)(
                    requires SizedRange<Rng const>)
            {
                return ranges::size(this->base());
            }
            CPP_member
            constexpr /*c++14*/ auto size()
                noexcept(noexcept(std::declval<reverse_view &>().size_(
                    meta::bool_<SizedRange<Rng>>{}))) ->
                CPP_ret(range_size_type_t<Rng>)(
                    requires not SizedRange<Rng const> && (SizedRange<Rng> ||
                        SizedSentinel<iterator_t<Rng>, iterator_t<Rng>>))
            {
                return size_(meta::bool_<SizedRange<Rng>>{});
            }
        private:
            friend range_access;

            CPP_member
            constexpr auto get_end() const
                noexcept(noexcept(ranges::end(std::declval<Rng const &>()))) ->
                CPP_ret(iterator_t<Rng>)(
                    requires BoundedRange<Rng const>)
            {
                return ranges::end(this->base());
            }
            constexpr /*c++14*/ iterator_t<Rng> get_end_(std::true_type)
                noexcept(noexcept(ranges::end(std::declval<Rng &>())))
            {
                CPP_assert(BoundedRange<Rng>);
                return ranges::end(this->base());
            }
            constexpr /*c++14*/ iterator_t<Rng> get_end_(std::false_type)
                noexcept(noexcept(iterator_t<Rng>(ranges::next(
                    ranges::begin(std::declval<Rng &>()),
                    ranges::end(std::declval<Rng &>())))))
            {
                CPP_assert(!BoundedRange<Rng>);
                using cache_t = detail::non_propagating_cache<iterator_t<Rng>, reverse_view<Rng>>;
                auto &end_ = static_cast<cache_t &>(*this);
                if(!end_)
                    end_ = ranges::next(ranges::begin(this->base()), ranges::end(this->base()));
                return *end_;
            }
            CPP_member
            constexpr /*c++14*/ auto get_end()
                noexcept(noexcept(std::declval<reverse_view &>().get_end_(
                    meta::bool_<BoundedRange<Rng>>{}))) ->
                CPP_ret(iterator_t<Rng>)(
                    requires not BoundedRange<Rng const>)
            {
                return get_end_(meta::bool_<BoundedRange<Rng>>{});
            }

            struct adaptor : adaptor_base
            {
            private:
                using Parent = meta::const_if_c<BoundedRange<Rng const>, reverse_view>;
                using Base = meta::const_if_c<BoundedRange<Rng const>, Rng>;
#ifndef NDEBUG
                Parent *rng_;
#endif
            public:
                adaptor() = default;
#ifndef NDEBUG
                constexpr adaptor(Parent &rng) noexcept
                  : rng_(&rng)
                {}
#else
                constexpr adaptor(Parent &) noexcept
                {}
#endif
                constexpr /*c++14*/ static auto CPP_auto_fun(begin)(Parent &rng)
                (
                    return rng.get_end()
                )
                constexpr /*c++14*/ static auto CPP_auto_fun(end)(Parent &rng)
                (
                    return ranges::begin(rng.base())
                )
                constexpr /*c++14*/ auto CPP_auto_fun(read)(iterator_t<Rng> it) (const)
                (
                    return *--it
                )
                constexpr /*c++14*/ void next(iterator_t<Rng> &it) const
                    noexcept(noexcept(--it))
                {
                    RANGES_ASSERT(it != ranges::begin(rng_->base()));
                    --it;
                }
                constexpr /*c++14*/ void prev(iterator_t<Rng> &it) const
                    noexcept(noexcept(++it))
                {
                    RANGES_ASSERT(it != ranges::end(rng_->base()));
                    ++it;
                }
                CPP_member
                constexpr /*c++14*/
                auto advance(iterator_t<Rng> &it, range_difference_type_t<Rng> n) const
                    noexcept(noexcept(ranges::advance(it, -n))) ->
                    CPP_ret(void)(
                        requires RandomAccessRange<Rng>)
                {
                    RANGES_ASSERT(n <= it - ranges::begin(rng_->base()));
                    RANGES_ASSERT(it - rng_->get_end() <= n);
                    ranges::advance(it, -n);
                }
                CPP_member
                constexpr /*c++14*/ auto
                distance_to(iterator_t<Rng> const &here, iterator_t<Rng> const &there,
                    adaptor const &other_adapt) const
                    noexcept(noexcept(here - there)) ->
                    CPP_ret(range_difference_type_t<Rng>)(
                        requires SizedSentinel<iterator_t<Rng>, iterator_t<Rng>>)
                {
                    RANGES_ASSERT(rng_ == other_adapt.rng_); (void)other_adapt;
                    return here - there;
                }
            };
            CPP_member
            constexpr /*c++14*/ auto begin_adaptor() const noexcept ->
                CPP_ret(adaptor)(
                    requires BoundedRange<Rng const>)
            {
                return {*this};
            }
            CPP_member
            constexpr /*c++14*/ auto end_adaptor() const noexcept ->
                CPP_ret(adaptor)(
                    requires BoundedRange<Rng const>)
            {
                return {*this};
            }
            CPP_member
            constexpr /*c++14*/ auto begin_adaptor() noexcept ->
                CPP_ret(adaptor)(
                    requires not BoundedRange<Rng const>)
            {
                return {*this};
            }
            CPP_member
            constexpr /*c++14*/ auto end_adaptor() noexcept ->
                CPP_ret(adaptor)(
                    requires not BoundedRange<Rng const>)
            {
                return {*this};
            }
            // SizedRange == true
            constexpr /*c++14*/ range_size_type_t<Rng> size_(std::true_type)
                noexcept(noexcept(ranges::size(std::declval<Rng &>())))
            {
                return ranges::size(this->base());
            }
            // SizedRange == false, SizedSentinel == true
            constexpr /*c++14*/ range_size_type_t<Rng> size_(std::false_type)
                noexcept(noexcept(ranges::iter_size(
                    std::declval<reverse_view &>().begin(),
                    std::declval<reverse_view &>().end())))
            {
                // NB: This may trigger the O(N) walk over the sequence to find
                // last iterator. That cost is amortized over all calls to size()
                // and end, so we'll squint and call it "amortized O(1)."
                return ranges::iter_size(this->begin(), this->end());
            }
        };

        namespace view
        {
            struct reverse_fn
            {
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 5
                // Avoid GCC5 bug that ODR-uses std::declval?!?
                CPP_template(typename Rng)(
                    requires BidirectionalRange<Rng>)
                constexpr /*c++14*/
#else
                CPP_template(typename Rng)(
                    requires BidirectionalRange<Rng>)
                constexpr
#endif
                auto CPP_auto_fun(operator())(Rng &&rng) (const)
                (
                    return reverse_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))}
                )
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
