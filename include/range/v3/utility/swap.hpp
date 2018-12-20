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
// The implementation of swap (see below) has been adapted from libc++
// (http://libcxx.llvm.org).

#ifndef RANGES_V3_UTILITY_SWAP_HPP
#define RANGES_V3_UTILITY_SWAP_HPP

#include <tuple>
#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/associated_types.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            struct is_movable_
              : meta::bool_<
                    std::is_object<T>::value &&
                    std::is_move_constructible<T>::value &&
                    std::is_move_assignable<T>::value>
            {};
        }
        /// \endcond

        template<typename T>
        struct is_swappable;

        template<typename T>
        struct is_nothrow_swappable;

        template<typename T, typename U>
        struct is_swappable_with;

        template<typename T, typename U>
        struct is_nothrow_swappable_with;

        template<typename T, typename U = T>
        struct is_indirectly_swappable;

        template<typename T, typename U = T>
        struct is_nothrow_indirectly_swappable;

        template<typename T, typename U = T>
        RANGES_CXX14_CONSTEXPR
        meta::if_c<
            std::is_move_constructible<T>::value &&
            std::is_assignable<T &, U>::value, T>
        exchange(T &t, U &&u)
            noexcept(
                std::is_nothrow_move_constructible<T>::value &&
                std::is_nothrow_assignable<T &, U>::value)
        {
            T tmp((T &&) t);
            t = (U &&) u;
            return tmp;
        }

        /// \cond
        namespace adl_swap_detail
        {
            // Intentionally create an ambiguity with std::swap, which is
            // (possibly) unconstrained.
            template<typename T>
            void swap(T &, T &) = delete;

            template<typename T, std::size_t N>
            void swap(T (&)[N], T (&)[N]) = delete;

#ifdef RANGES_WORKAROUND_MSVC_620035
            void swap();
#endif

            template<typename T, typename U,
                typename = decltype(swap(std::declval<T>(), std::declval<U>()))>
            std::true_type try_adl_swap_(int);

            template<typename T, typename U>
            std::false_type try_adl_swap_(long);

            template<typename T, typename U = T>
            struct is_adl_swappable_
              : meta::id_t<decltype(adl_swap_detail::try_adl_swap_<T, U>(42))>
            {};

            struct swap_fn
            {
                // Dispatch to user-defined swap found via ADL:
                template<typename T, typename U>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<is_adl_swappable_<T, U>::value>
                operator()(T &&t, U &&u) const
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    (void) swap((T &&) t, (U &&) u)
                )

                // For intrinsically swappable (i.e., movable) types for which
                // a swap overload cannot be found via ADL, swap by moving.
                template<typename T>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<
                    !is_adl_swappable_<T &>::value &&
                    detail::is_movable_<T>::value>
                operator()(T &a, T &b) const
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    (void)(b = ranges::exchange(a, (T &&) b))
                )

                // For arrays of intrinsically swappable (i.e., movable) types
                // for which a swap overload cannot be found via ADL, swap array
                // elements by moving.
                template<typename T, typename U, std::size_t N>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<
                    !is_adl_swappable_<T (&)[N], U (&)[N]>::value &&
                    is_swappable_with<T &, U &>::value>
                operator()(T (&t)[N], U (&u)[N]) const
                    noexcept(is_nothrow_swappable_with<T &, U &>::value)
                {
                    for(std::size_t i = 0; i < N; ++i)
                        (*this)(t[i], u[i]);
                }

                // For rvalue pairs and tuples of swappable types, swap the
                // members. This permits code like:
                //   ranges::swap(std::tie(a,b,c), std::tie(d,e,f));
                template<typename F0, typename S0, typename F1, typename S1>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<is_swappable_with<F0, F1>::value && is_swappable_with<S0, S1>::value>
                operator()(std::pair<F0, S0> &&left, std::pair<F1, S1> &&right) const
                    noexcept(
                        is_nothrow_swappable_with<F0, F1>::value &&
                        is_nothrow_swappable_with<S0, S1>::value)
                {
                    swap_fn{}(detail::move(left).first, detail::move(right).first);
                    swap_fn{}(detail::move(left).second, detail::move(right).second);
                }

                template<typename ...Ts, typename ...Us>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<meta::and_c<is_swappable_with<Ts, Us>::value...>::value>
                operator()(std::tuple<Ts...> &&left, std::tuple<Us...> &&right) const
                    noexcept(meta::and_c<is_nothrow_swappable_with<Ts, Us>::value...>::value)
                {
                    swap_fn::impl(detail::move(left), detail::move(right),
                        meta::make_index_sequence<sizeof...(Ts)>{});
                }

            private:
                template<typename T, typename U, std::size_t ...Is>
                RANGES_CXX14_CONSTEXPR
                static void impl(T &&left, U &&right, meta::index_sequence<Is...>)
                {
                    (void) detail::ignore_unused(
                        (swap_fn{}(std::get<Is>(detail::move(left)),
                                   std::get<Is>(detail::move(right))), 42)...);
                }
            };

            template<typename T, typename U, typename = void>
            struct is_swappable_with_
              : std::false_type
            {};

            template<typename T, typename U>
            struct is_swappable_with_<T, U, meta::void_<
                decltype(swap_fn{}(std::declval<T>(), std::declval<U>())),
                decltype(swap_fn{}(std::declval<U>(), std::declval<T>()))>>
              : std::true_type
            {};

            template<typename T, typename U>
            struct is_nothrow_swappable_with_
              : meta::bool_<noexcept(swap_fn{}(std::declval<T>(), std::declval<U>())) &&
                            noexcept(swap_fn{}(std::declval<U>(), std::declval<T>()))>
            {};

            // Q: Should std::reference_wrapper be considered a proxy wrt swapping rvalues?
            // A: No. Its operator= is currently defined to reseat the references, so
            //    std::swap(ra, rb) already means something when ra and rb are (lvalue)
            //    reference_wrappers. That reseats the reference wrappers but leaves the
            //    referents unmodified. Treating rvalue reference_wrappers differently would
            //    be confusing.

            // Q: Then why is it OK to "re"-define swap for pairs and tuples of references?
            // A: Because as defined above, swapping an rvalue tuple of references has the same
            //    semantics as swapping an lvalue tuple of references. Rather than reseat the
            //    references, assignment happens *through* the references.

            // Q: But I have an iterator whose operator* returns an rvalue
            //    std::reference_wrapper<T>. How do I make it model IndirectlySwappable?
            // A: With an overload of iter_swap.

            // Intentionally create an ambiguity with std::iter_swap, which is
            // (possibly) unconstrained.
            template<typename T>
            void iter_swap(T, T) = delete;

#ifdef RANGES_WORKAROUND_MSVC_620035
            void iter_swap();
#endif

            template<typename T, typename U,
                typename = decltype(iter_swap(std::declval<T>(), std::declval<U>()))>
            std::true_type try_adl_iter_swap_(int);

            template<typename T, typename U>
            std::false_type try_adl_iter_swap_(long);

            // Test whether an overload of iter_swap for a T and a U can be found
            // via ADL with the iter_swap overload above participating in the
            // overload set. This depends on user-defined iter_swap overloads
            // being a better match than the overload in namespace std.
            template<typename T, typename U>
            struct is_adl_indirectly_swappable_
              : meta::id_t<decltype(adl_swap_detail::try_adl_iter_swap_<T, U>(42))>
            {};

            struct iter_swap_fn
            {
                // *If* a user-defined iter_swap is found via ADL, call that:
                template<typename T, typename U>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<is_adl_indirectly_swappable_<T, U>::value>
                operator()(T &&t, U &&u) const
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    (void) iter_swap((T &&) t, (U &&) u)
                )

                // *Otherwise*, for Readable types with swappable reference
                // types, call ranges::swap(*a, *b)
                template<typename I0, typename I1>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<
                    !is_adl_indirectly_swappable_<I0, I1>::value &&
                    is_swappable_with<reference_t<I0>, reference_t<I1>>::value>
                operator()(I0 &&a, I1 &&b) const
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    swap_fn{}(*a, *b)
                )

                // *Otherwise*, for Readable types that are mutually
                // IndirectlyMovableStorable, implement as:
                //      value_type_t<T0> tmp = iter_move(a);
                //      *a = iter_move(b);
                //      *b = std::move(tmp);
                template<typename I0, typename I1>
                RANGES_CXX14_CONSTEXPR
                meta::if_c<
                    !is_adl_indirectly_swappable_<I0, I1>::value &&
                    !is_swappable_with<reference_t<I0>, reference_t<I1>>::value &&
                    is_indirectly_movable<I0, I1>::value &&
                    is_indirectly_movable<I1, I0>::value>
                operator()(I0 &&a, I1 &&b) const
                    noexcept(
                        is_nothrow_indirectly_movable<I0, I1>::value &&
                        is_nothrow_indirectly_movable<I1, I0>::value)
                {
                    meta::_t<value_type<I0>> v0 = iter_move(a);
                    *a = iter_move(b);
                    *b = detail::move(v0);
                }
            };

            template<typename T, typename U, typename Enable = void>
            struct is_indirectly_swappable_
              : std::false_type
            {};

            template<typename T, typename U>
            struct is_indirectly_swappable_<T, U, meta::void_<
                decltype(iter_swap_fn{}(std::declval<T>(), std::declval<U>()))>>
              : std::true_type
            {};

            template<typename T, typename U>
            struct is_nothrow_indirectly_swappable_
              : meta::bool_<noexcept(iter_swap_fn{}(std::declval<T>(), std::declval<U>()))>
            {};
        }
        /// \endcond

        /// \ingroup group-utility
        template<typename T, typename U>
        struct is_swappable_with
          : adl_swap_detail::is_swappable_with_<T, U>
        {};

        /// \ingroup group-utility
        template<typename T, typename U>
        struct is_nothrow_swappable_with
          : meta::and_<
                is_swappable_with<T, U>,
                adl_swap_detail::is_nothrow_swappable_with_<T, U>>
        {};

        /// \ingroup group-utility
        template<typename T>
        struct is_swappable
          : is_swappable_with<T &, T &>
        {};

        /// \ingroup group-utility
        template<typename T>
        struct is_nothrow_swappable
          : is_nothrow_swappable_with<T &, T &>
        {};

        /// \ingroup group-utility
        template<typename T, typename U>
        struct is_indirectly_swappable
          : adl_swap_detail::is_indirectly_swappable_<T, U>
        {};

        /// \ingroup group-utility
        template<typename T, typename U>
        struct is_nothrow_indirectly_swappable
          : meta::and_<
                is_indirectly_swappable<T, U>,
                adl_swap_detail::is_nothrow_indirectly_swappable_<T, U>>
        {};

        inline namespace CPOs
        {
            /// \ingroup group-utility
            /// \relates adl_swap_detail::swap_fn
            RANGES_INLINE_VARIABLE(adl_swap_detail::swap_fn, swap)

            /// \ingroup group-utility
            /// \relates adl_swap_detail::iter_swap_fn
            RANGES_INLINE_VARIABLE(adl_swap_detail::iter_swap_fn, iter_swap)
        }


        /// \cond
        struct indirect_swap_fn
        {
            template<typename I0, typename I1>
            RANGES_DEPRECATED("Please replace uses of ranges::indirect_swap with ranges::iter_swap.")
            void operator()(I0 &&i0, I1 &&i1) const
            RANGES_AUTO_RETURN_NOEXCEPT
            (
                ranges::iter_swap((I0 &&) i0, (I1 &&) i1)
            )
        };

        RANGES_INLINE_VARIABLE(indirect_swap_fn, indirect_swap)
        /// \endcond
    }
}

#endif
