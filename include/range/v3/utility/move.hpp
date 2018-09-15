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
#if RANGES_BROKEN_CPO_LOOKUP
            void iter_move(); // unqualified name lookup block
#endif

            template<typename T,
                typename = decltype(iter_move(std::declval<T>()))>
            std::true_type try_adl_iter_move_(int);

            template<typename T>
            std::false_type try_adl_iter_move_(long);

            template<typename T>
            struct is_adl_indirectly_movable_
              : meta::id_t<decltype(adl_move_detail::try_adl_iter_move_<T>(42))>
            {};

            // TODO: investigate the breakage when these are made constexpr.
            // (Results in ODR-use of projected_readable::operator*)
            struct iter_move_fn
            {
                template<typename I,
                    typename = meta::if_c<is_adl_indirectly_movable_<I &>::value>>
                auto operator()(I &&i) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    iter_move(i)
                )

                template<typename I,
                    typename = meta::if_c<!is_adl_indirectly_movable_<I &>::value>,
                    typename R = reference_t<I>>
                auto operator()(I &&i) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    static_cast<aux::move_t<R>>(aux::move(*i))
                )
            };
        }
        /// \endcond

        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(adl_move_detail::iter_move_fn, iter_move)
        }

        /// \cond
        struct indirect_move_fn
        {
            template<typename I>
            RANGES_DEPRECATED("Please replace uses of ranges::indirect_move with ranges::iter_move.")
            void operator()(I &&i) const
            RANGES_AUTO_RETURN_NOEXCEPT
            (
                ranges::iter_move((I &&) i)
            )
        };

        RANGES_INLINE_VARIABLE(indirect_move_fn, indirect_move)

        namespace detail
        {
            template<typename I, typename O>
            using is_indirectly_movable_ =
                meta::bool_<
                    std::is_constructible<
                        meta::_t<value_type<I>>,
                        decltype(iter_move(std::declval<I &>()))>::value &&
                    std::is_assignable<
                        meta::_t<value_type<I>> &,
                        decltype(iter_move(std::declval<I &>()))>::value &&
                    std::is_assignable<
                        reference_t<O>,
                        meta::_t<value_type<I>>>::value &&
                    std::is_assignable<
                        reference_t<O>,
                        decltype(iter_move(std::declval<I &>()))>::value>;

            template<typename I, typename O>
            using is_nothrow_indirectly_movable_ =
                meta::bool_<
                    noexcept(iter_move(std::declval<I &>())) &&
                    std::is_nothrow_constructible<
                        meta::_t<value_type<I>>,
                        decltype(iter_move(std::declval<I &>()))>::value &&
                    std::is_nothrow_assignable<
                        meta::_t<value_type<I>> &,
                        decltype(iter_move(std::declval<I &>()))>::value &&
                    std::is_nothrow_assignable<
                        reference_t<O>,
                        meta::_t<value_type<I>>>::value &&
                    std::is_nothrow_assignable<
                        reference_t<O>,
                        decltype(iter_move(std::declval<I &>()))>::value>;
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
