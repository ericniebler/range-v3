/// \file
// Range v3 library
//
//  Copyright Hui Xie 2021
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_RANGE_VIEW_MAYBE_HPP
#define RANGES_V3_RANGE_VIEW_MAYBE_HPP

#include <type_traits>

#include <range/v3/range_fwd.hpp>

#include <range/v3/range/maybe.hpp>
#include <range/v3/utility/addressof.hpp>
#include <range/v3/utility/in_place.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/view/interface.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{

    /// \addtogroup group-views
    /// @{
    template<typename Maybe>
    struct maybe_view : view_interface<maybe_view<Maybe>, (cardinality)1>
    {
    private:
        CPP_assert(copy_constructible<Maybe>);
        static_assert(ranges::maybe<Maybe>,
                      "The template parameter of maybe_view must satisfy `maybe`");
        using T = std::decay_t<decltype(ranges::get_just(declval(Maybe &)))>;

        semiregular_box_t<Maybe> value_;
        template<typename... Args>
        constexpr maybe_view(in_place_t, std::true_type, Args &&... args)
          : value_{static_cast<Args &&>(args)...}
        {}
        template<typename... Args>
        constexpr maybe_view(in_place_t, std::false_type, Args &&... args)
          : value_{in_place, static_cast<Args &&>(args)...}
        {}

    public:
        maybe_view() = default;
        constexpr explicit maybe_view(Maybe const & t)
          : value_(t)
        {}
        constexpr explicit maybe_view(Maybe && t)
          : value_(std::move(t))
        {}
        template(class... Args)(
            /// \pre
            requires constructible_from<Maybe,
                                        Args...>) constexpr maybe_view(in_place_t,
                                                                       Args &&... args)
          : maybe_view{in_place,
                       meta::bool_<(bool)semiregular<Maybe>>{},
                       static_cast<Args &&>(args)...}
        {}
        constexpr T * begin() noexcept
        {
            return data();
        }
        constexpr T const * begin() const noexcept
        {
            return data();
        }
        constexpr T * end() noexcept
        {
            return data() + size();
        }
        constexpr T const * end() const noexcept
        {
            return data() + size();
        }
        constexpr std::size_t size() const noexcept
        {
            return ranges::is_just(static_cast<Maybe const &>(value_)) ? 1u : 0u;
        }
        constexpr T * data() noexcept
        {
            auto & m = static_cast<Maybe &>(value_);
            return ranges::is_just(m) ? detail::addressof(ranges::get_just(m)) : nullptr;
        }
        constexpr T const * data() const noexcept
        {
            const auto & m = static_cast<Maybe const &>(value_);
            return ranges::is_just(m) ? detail::addressof(ranges::get_just(m)) : nullptr;
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    template<class T>
    explicit maybe_view(T &&) //
        ->maybe_view<detail::decay_t<T>>;
#endif

    namespace views
    {
        struct maybe_fn
        {
            template(typename Val)(
                /// \pre
                requires copy_constructible<Val> AND ranges::maybe<Val>) //
                maybe_view<Val>
                operator()(Val value) const
            {
                return maybe_view<Val>{std::move(value)};
            }
        };

        /// \relates maybe_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(maybe_fn, maybe)
    } // namespace views

    /// @}
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::maybe_view)

#endif
