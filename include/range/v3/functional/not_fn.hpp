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
#ifndef RANGES_V3_FUNCTIONAL_NOT_FN_HPP
#define RANGES_V3_FUNCTIONAL_NOT_FN_HPP

#include <type_traits>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/concepts.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-functional
    /// @{
    template<typename FD>
    struct logical_negate
    {
    private:
        CPP_assert(same_as<FD, detail::decay_t<FD>> && move_constructible<FD>);
        RANGES_NO_UNIQUE_ADDRESS FD pred_;

    public:
        CPP_member
        constexpr CPP_ctor(logical_negate)()(                          //
            noexcept(std::is_nothrow_default_constructible<FD>::value) //
            requires default_constructible<FD>)
        {}
        template<typename T>
        explicit constexpr CPP_ctor(logical_negate)(T && pred)( //
            requires(!defer::same_as<detail::decay_t<T>, logical_negate>) &&
            defer::constructible_from<FD, T>)
          : pred_(static_cast<T &&>(pred))
        {}

        template<typename... Args>
        constexpr auto operator()(Args &&... args) & -> CPP_ret(bool)( //
            requires predicate<FD &, Args...>)
        {
            return !invoke(pred_, static_cast<Args &&>(args)...);
        }
        /// \overload
        template<typename... Args>
        constexpr auto operator()(Args &&... args) const & -> CPP_ret(bool)( //
            requires predicate<FD const &, Args...>)
        {
            return !invoke(pred_, static_cast<Args &&>(args)...);
        }
        /// \overload
        template<typename... Args>
        constexpr auto operator()(Args &&... args) && -> CPP_ret(bool)( //
            requires predicate<FD, Args...>)
        {
            return !invoke(static_cast<FD &&>(pred_), static_cast<Args &&>(args)...);
        }
    };

    struct not_fn_fn
    {
        template<typename Pred>
        constexpr auto operator()(Pred && pred) const
            -> CPP_ret(logical_negate<detail::decay_t<Pred>>)( //
                requires move_constructible<detail::decay_t<Pred>> &&
                    constructible_from<detail::decay_t<Pred>, Pred>)
        {
            return logical_negate<detail::decay_t<Pred>>{(Pred &&) pred};
        }
    };

    /// \ingroup group-functional
    /// \sa `not_fn_fn`
    RANGES_INLINE_VARIABLE(not_fn_fn, not_fn)

    namespace cpp20
    {
        using ranges::not_fn;
    }
    /// @}
} // namespace ranges

#endif
