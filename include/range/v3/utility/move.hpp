/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_MOVE_HPP
#define RANGES_V3_UTILITY_MOVE_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/associated_types.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace aux
        {
            /// \ingroup group-utility
            struct move_fn : move_tag
            {
                template<typename T,
                    typename U = meta::_t<std::remove_reference<T>>>
                U && operator()(T && t) const noexcept
                {
                    return static_cast<U &&>(t);
                }
            };

            /// \ingroup group-utility
            /// \sa `move_fn`
            RANGES_INLINE_VARIABLE(move_fn, move)

            /// \ingroup group-utility
            /// \sa `move_fn`
            template<typename T>
            meta::_t<std::remove_reference<T>> && operator|(T && t, move_fn move) noexcept
            {
                return move(t);
            }

            /// \ingroup group-utility
            /// \sa `move_fn`
            template<typename R>
            using move_t =
                meta::if_<
                    std::is_reference<R>,
                    meta::_t<std::remove_reference<R>> &&,
                    detail::decay_t<R>>;
        }

        /// \cond
        namespace adl_move_detail
        {
            // TODO: investigate the breakage when these are made constexpr.
            // (Results in ODR-use of projected_readable::operator*)

            // Default indirect_move overload.
            template<typename I,
                typename R = decltype(*std::declval<I const &>())>
            aux::move_t<R> indirect_move(I const &i)
                noexcept(noexcept(static_cast<aux::move_t<R>>(aux::move(*i))))
            {
                return aux::move(*i);
            }

            struct indirect_move_fn
            {
                template<typename I>
                auto operator()(I const &i) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    indirect_move(i)
                )
            };
        }
        /// \endcond

        RANGES_INLINE_VARIABLE(adl_move_detail::indirect_move_fn, indirect_move)

        /// \cond
        namespace detail
        {
            template<typename I, typename O>
            using is_indirectly_movable_ =
                meta::bool_<
                    std::is_constructible<
                        meta::_t<value_type<I>>,
                        decltype(indirect_move(std::declval<I &>()))>::value &&
                    std::is_assignable<
                        meta::_t<value_type<I>> &,
                        decltype(indirect_move(std::declval<I &>()))>::value &&
                    std::is_assignable<
                        decltype(*std::declval<O &>()),
                        meta::_t<value_type<I>>>::value &&
                    std::is_assignable<
                        decltype(*std::declval<O &>()),
                        decltype(indirect_move(std::declval<I &>()))>::value>;

            template<typename I, typename O>
            using is_nothrow_indirectly_movable_ =
                meta::bool_<
                    noexcept(iter_move(std::declval<I &>())) &&
                    std::is_nothrow_constructible<
                        meta::_t<value_type<I>>,
                        decltype(indirect_move(std::declval<I &>()))>::value &&
                    std::is_nothrow_assignable<
                        meta::_t<value_type<I>> &,
                        decltype(indirect_move(std::declval<I &>()))>::value &&
                    std::is_nothrow_assignable<
                        decltype(*std::declval<O &>()),
                        meta::_t<value_type<I>>>::value &&
                    std::is_nothrow_assignable<
                        decltype(*std::declval<O &>()),
                        decltype(indirect_move(std::declval<I &>()))>::value>;
        }
        /// \endcond

        template<typename I, typename O>
        struct is_indirectly_movable
          : meta::_t<meta::if_<
                meta::is_trait<meta::defer<detail::is_indirectly_movable_, I, O>>,
                meta::defer<detail::is_indirectly_movable_, I, O>,
                std::false_type>>
        {};

        template<typename I, typename O>
        struct is_nothrow_indirectly_movable
          : meta::_t<meta::if_<
                meta::is_trait<meta::defer<detail::is_nothrow_indirectly_movable_, I, O>>,
                meta::defer<detail::is_nothrow_indirectly_movable_, I, O>,
                std::false_type>>
        {};
    }
}

#endif
