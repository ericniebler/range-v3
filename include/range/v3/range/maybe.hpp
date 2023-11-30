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

#ifndef RANGES_V3_RANGE_MAYBE_HPP
#define RANGES_V3_RANGE_MAYBE_HPP

#include <concepts/concepts.hpp>

#include <range/v3/detail/config.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{
    namespace _is_just_
    {
        // clang-format off
        template<typename T>
        CPP_requires(has_non_member_is_just_,
            requires(T const& t) //
            (
                static_cast<bool>(is_just(t))
            ));
        template<typename T>
        CPP_concept has_non_member_is_just =
            CPP_requires_ref(_is_just_::has_non_member_is_just_, T);
        // clang-format on

        struct fn
        {
            template(typename M)(
                /// \pre
                requires has_non_member_is_just<M>) //
                constexpr bool
                operator()(M && m) const //
                noexcept(noexcept(is_just(m)))
            {
                return is_just(m);
            }

            template(typename M)(
                /// \pre
                requires(!has_non_member_is_just<M>)
                    AND concepts::explicitly_convertible_to<M, bool>) //
                constexpr bool
                operator()(M && m) const //
                noexcept(noexcept(static_cast<bool>(m)))
            {
                return static_cast<bool>(m);
            }
        };
    } // namespace _is_just_

    RANGES_DEFINE_CPO(_is_just_::fn, is_just)

    namespace _get_just_
    {

        // clang-format off
        template<typename T>
        CPP_requires(has_non_member_get_just_,
            requires(T&& t) //
            (
                get_just(static_cast<T&&>(t))
            ));
        template<typename T>
        CPP_concept has_non_member_get_just =
            CPP_requires_ref(_get_just_::has_non_member_get_just_, T);
        // clang-format on

        struct fn
        {
        private:
            struct _non_member_result_
            {
                template<typename M>
                using invoke = decltype(get_just(declval(M &)));
            };

            struct _operator_result_
            {
                template<typename M>
                using invoke = decltype(*(declval(M &)));
            };

            template<typename M>
            using _result_t =
                meta::invoke<meta::conditional_t<has_non_member_get_just<M>,
                                                 _non_member_result_, _operator_result_>,
                             M>;

        public:
            template(typename M)(
                /// \pre
                requires has_non_member_get_just<M>) //
                constexpr _result_t<M>
                operator()(M && m) const //
                noexcept(noexcept(get_just(static_cast<M &&>(m))))
            {
                return get_just(static_cast<M &&>(m));
            }

            template(typename M)(
                /// \pre
                requires(!has_non_member_get_just<M>) AND detail::dereferenceable_<M>) //
                constexpr _result_t<M>
                operator()(M && m) const //
                noexcept(noexcept(*(static_cast<M &&>(m))))
            {
                return *(static_cast<M &&>(m));
            }
        };
    } // namespace _get_just_

    RANGES_DEFINE_CPO(_get_just_::fn, get_just)

    // clang-format off
    template<typename T>
    CPP_requires(_maybe_,
        requires(T & t) //
        (
            ranges::is_just(static_cast<const T&>(t)),
            ranges::get_just(t)
        ));
    template<typename T>
    CPP_concept maybe =
        CPP_requires_ref(ranges::_maybe_, T);
    // clang-format on

} // namespace ranges

#include <range/v3/detail/epilogue.hpp>

#endif
