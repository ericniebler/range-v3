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
            CPP_requires(SizedRange<Rng const>)
            constexpr range_size_type_t<Rng> size() const
                noexcept(noexcept(ranges::size(std::declval<Rng const &>())))
            {
                return ranges::size(this->base());
            }
            CPP_requires(not SizedRange<Rng const> && (SizedRange<Rng> ||
                SizedSentinel<iterator_t<Rng>, iterator_t<Rng>>))
            RANGES_CXX14_CONSTEXPR range_size_type_t<Rng> size()
                noexcept(noexcept(std::declval<reverse_view &>().size_(
                    meta::bool_<SizedRange<Rng>>{})))
            {
                return size_(meta::bool_<SizedRange<Rng>>{});
            }
        private:
            friend range_access;

            CPP_requires(BoundedRange<Rng const>)
            constexpr iterator_t<Rng> get_end() const
                noexcept(noexcept(ranges::end(std::declval<Rng const &>())))
            {
                return ranges::end(this->base());
            }
            RANGES_CXX14_CONSTEXPR iterator_t<Rng> get_end_(std::true_type)
                noexcept(noexcept(ranges::end(std::declval<Rng &>())))
            {
                CPP_assert(BoundedRange<Rng>);
                return ranges::end(this->base());
            }
            RANGES_CXX14_CONSTEXPR iterator_t<Rng> get_end_(std::false_type)
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
            CPP_requires(not BoundedRange<Rng const>)
            RANGES_CXX14_CONSTEXPR iterator_t<Rng> get_end()
                noexcept(noexcept(std::declval<reverse_view &>().get_end_(
                    meta::bool_<BoundedRange<Rng>>{})))
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
                RANGES_CXX14_CONSTEXPR static auto begin(Parent &rng)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    rng.get_end()
                )
                RANGES_CXX14_CONSTEXPR static auto end(Parent &rng)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::begin(rng.base())
                )
                RANGES_CXX14_CONSTEXPR auto read(iterator_t<Rng> it) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    *--it
                )
                RANGES_CXX14_CONSTEXPR void next(iterator_t<Rng> &it) const
                    noexcept(noexcept(--it))
                {
                    RANGES_ASSERT(it != ranges::begin(rng_->base()));
                    --it;
                }
                RANGES_CXX14_CONSTEXPR void prev(iterator_t<Rng> &it) const
                    noexcept(noexcept(++it))
                {
                    RANGES_ASSERT(it != ranges::end(rng_->base()));
                    ++it;
                }
                CPP_requires(RandomAccessRange<Rng>)
                RANGES_CXX14_CONSTEXPR
                void advance(iterator_t<Rng> &it, range_difference_type_t<Rng> n) const
                    noexcept(noexcept(ranges::advance(it, -n)))
                {
                    RANGES_ASSERT(n <= it - ranges::begin(rng_->base()));
                    RANGES_ASSERT(it - rng_->get_end() <= n);
                    ranges::advance(it, -n);
                }
                CPP_requires(SizedSentinel<iterator_t<Rng>, iterator_t<Rng>>)
                RANGES_CXX14_CONSTEXPR range_difference_type_t<Rng>
                distance_to(iterator_t<Rng> const &here, iterator_t<Rng> const &there,
                    adaptor const &other_adapt) const
                    noexcept(noexcept(here - there))
                {
                    RANGES_ASSERT(rng_ == other_adapt.rng_); (void)other_adapt;
                    return here - there;
                }
            };
            CPP_requires(BoundedRange<Rng const>)
            RANGES_CXX14_CONSTEXPR adaptor begin_adaptor() const noexcept
            {
                return {*this};
            }
            CPP_requires(BoundedRange<Rng const>)
            RANGES_CXX14_CONSTEXPR adaptor end_adaptor() const noexcept
            {
                return {*this};
            }
            CPP_requires(not BoundedRange<Rng const>)
            RANGES_CXX14_CONSTEXPR adaptor begin_adaptor() noexcept
            {
                return {*this};
            }
            CPP_requires(not BoundedRange<Rng const>)
            RANGES_CXX14_CONSTEXPR adaptor end_adaptor() noexcept
            {
                return {*this};
            }
            // SizedRange == true
            RANGES_CXX14_CONSTEXPR range_size_type_t<Rng> size_(std::true_type)
                noexcept(noexcept(ranges::size(std::declval<Rng &>())))
            {
                return ranges::size(this->base());
            }
            // SizedRange == false, SizedSentinel == true
            RANGES_CXX14_CONSTEXPR range_size_type_t<Rng> size_(std::false_type)
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
                RANGES_CXX14_CONSTEXPR auto
#else
                CPP_template(typename Rng)(
                    requires BidirectionalRange<Rng>)
                constexpr auto
#endif
                operator()(Rng &&rng) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    reverse_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))}
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                CPP_template(typename Rng)(
                    requires not BidirectionalRange<Rng>)
                void operator()(Rng &&) const
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
