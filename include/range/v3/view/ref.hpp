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

#ifndef RANGES_V3_VIEW_REF_HPP
#define RANGES_V3_VIEW_REF_HPP

#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/size.hpp>
#include <range/v3/data.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>

RANGES_DISABLE_WARNINGS

namespace ranges
{
    inline namespace v3
    {
        /// \cond
#if RANGES_BROKEN_CPO_LOOKUP
        namespace _ref_view_ { template<typename> struct adl_hook {}; }
#endif

        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct ref_view
          : view_interface<ref_view<Rng>>
#if RANGES_BROKEN_CPO_LOOKUP
          , private _ref_view_::adl_hook<ref_view<Rng>>
#endif
        {
        private:
            CPP_assert(Range<Rng>);
            static_assert(std::is_object<Rng>::value, "");
            Rng *rng_ = nullptr; // exposition only
        public:
            constexpr ref_view() noexcept = default;
            constexpr ref_view(Rng &rng) noexcept
              : rng_(std::addressof(rng))
            {}
            constexpr Rng &base() const noexcept
            {
                return *rng_;
            }
            constexpr iterator_t<Rng> begin() const
                noexcept(noexcept(ranges::begin(*rng_)))
            {
                return ranges::begin(*rng_);
            }
            constexpr sentinel_t<Rng> end() const
                noexcept(noexcept(ranges::end(*rng_)))
            {
                return ranges::end(*rng_);
            }
            constexpr auto empty() const
                noexcept(noexcept(ranges::empty(*rng_))) ->
                CPP_ret(bool)(
                    requires detail::CanEmpty<Rng>)
            {
                return ranges::empty(*rng_);
            }
            CPP_member
            constexpr auto CPP_fun(size)() (const
                noexcept(noexcept(ranges::size(*rng_)))
                requires SizedRange<Rng>)
            {
                return ranges::size(*rng_);
            }
            CPP_member
            constexpr auto CPP_fun(data)() (const
                noexcept(noexcept(ranges::data(*rng_)))
                requires ContiguousRange<Rng>)
            {
                return ranges::data(*rng_);
            }
#if !RANGES_BROKEN_CPO_LOOKUP
            friend constexpr iterator_t<Rng> begin(ref_view &&rng)
                noexcept(noexcept(rng.begin()))
            {
                return rng.begin();
            }
            friend constexpr sentinel_t<Rng> end(ref_view &&rng)
                noexcept(noexcept(rng.end()))
            {
                return rng.end();
            }
#endif
        };

#if RANGES_BROKEN_CPO_LOOKUP
        namespace _ref_view_
        {
            template<typename Rng>
            constexpr iterator_t<Rng> begin(ref_view<Rng> &&rng)
                noexcept(noexcept(rng.begin()))
            {
                return rng.begin();
            }
            template<typename Rng>
            constexpr sentinel_t<Rng> end(ref_view<Rng> &&rng)
                noexcept(noexcept(rng.end()))
            {
                return rng.end();
            }
        }
#endif

        namespace view
        {
            struct ref_fn
            {
                template<typename Rng>
                constexpr auto operator()(Rng &rng) const noexcept ->
                    CPP_ret(ref_view<Rng>)(
                        requires Range<Rng>)
                {
                    return ref_view<Rng>(rng);
                }
            };

            /// \relates const_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<ref_fn>, ref)
        }
    }
}

RANGES_RE_ENABLE_WARNINGS

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::ref_view)

#endif
