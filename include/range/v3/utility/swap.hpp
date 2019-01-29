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
#include <concepts/swap.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/iterator/associated_types.hpp>

namespace ranges
{
    template<typename T>
    using is_swappable = concepts::is_swappable<T>;

    template<typename T>
    using is_nothrow_swappable = concepts::is_nothrow_swappable<T>;

    template<typename T, typename U>
    using is_swappable_with = concepts::is_swappable_with<T, U>;

    template<typename T, typename U>
    using is_nothrow_swappable_with = concepts::is_nothrow_swappable_with<T, U>;

    template<typename T, typename U = T>
    struct is_indirectly_swappable;

    template<typename T, typename U = T>
    struct is_nothrow_indirectly_swappable;

    using concepts::exchange;

    /// \ingroup group-utility
    /// \relates concepts::adl_swap_detail::swap_fn
    RANGES_DEFINE_CPO(decltype(concepts::swap), swap)

    /// \cond
    namespace detail
    {
        template<typename Derived>
        struct member_swap
        {
            friend constexpr /*c++14*/ void swap(Derived &a, Derived &b)
                noexcept(noexcept(a.swap(b)))
            {
                a.swap(b);
            }
        };
    }

    namespace adl_swap_detail
    {
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
        // unconstrained.
        template<typename T, typename U>
        void iter_swap(T, U) = delete;

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
            constexpr /*c++14*/
            meta::if_c<is_adl_indirectly_swappable_<T, U>::value>
            operator()(T &&t, U &&u) const
                noexcept(noexcept(iter_swap((T &&) t, (U &&) u)))
            {
                (void) iter_swap((T &&) t, (U &&) u);
            }

            // *Otherwise*, for Readable types with swappable reference
            // types, call ranges::swap(*a, *b)
            template<typename I0, typename I1>
            constexpr /*c++14*/
            meta::if_c<
                !is_adl_indirectly_swappable_<I0, I1>::value &&
                is_swappable_with<iter_reference_t<I0>, iter_reference_t<I1>>::value>
            operator()(I0 &&a, I1 &&b) const
                noexcept(noexcept(ranges::swap(*a, *b)))
            {
                ranges::swap(*a, *b);
            }

            // *Otherwise*, for Readable types that are mutually
            // IndirectlyMovableStorable, implement as:
            //      iter_value_t<T0> tmp = iter_move(a);
            //      *a = iter_move(b);
            //      *b = std::move(tmp);
            template<typename I0, typename I1>
            constexpr /*c++14*/
            meta::if_c<
                !is_adl_indirectly_swappable_<I0, I1>::value &&
                !is_swappable_with<iter_reference_t<I0>, iter_reference_t<I1>>::value &&
                is_indirectly_movable<I0, I1>::value &&
                is_indirectly_movable<I1, I0>::value>
            operator()(I0 &&a, I1 &&b) const
                noexcept(
                    is_nothrow_indirectly_movable<I0, I1>::value &&
                    is_nothrow_indirectly_movable<I1, I0>::value)
            {
                iter_value_t<I0> v0 = iter_move(a);
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

    /// \ingroup group-utility
    /// \relates adl_swap_detail::iter_swap_fn
    RANGES_DEFINE_CPO(adl_swap_detail::iter_swap_fn, iter_swap)
}

#endif
