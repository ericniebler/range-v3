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

#ifndef RANGES_V3_RANGE_FWD_HPP
#define RANGES_V3_RANGE_FWD_HPP

#include <climits>
#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/version.hpp>

/// \defgroup group-utility Utility
/// Utility classes

/// \defgroup group-core Core
/// Core range functionality

/// \defgroup group-algorithms Algorithms
/// Iterator- and range-based algorithms, like the standard algorithms

/// \defgroup group-views Views
/// Lazy, non-owning, non-mutating, composable range views

/// \defgroup group-actions Actions
/// Eager, mutating, composable algorithms

/// \defgroup group-concepts Concepts
/// Concept-checking classes and utilities

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT

namespace ranges
{
    inline namespace v3
    {
        inline namespace CPOs {}

        /// \cond
        namespace _end_
        {
            struct fn;
        }
        using end_fn = _end_::fn;

        namespace _size_
        {
            struct fn;
        }
        /// \endcond

        template<typename...>
        struct variant;

        template<typename I = void>
        struct dangling;

        template<typename ...Ts>
        struct common_type;

        template<typename ...Ts>
        using common_type_t = meta::_t<common_type<Ts...>>;

        template<typename T, typename U, typename TQual, typename UQual>
        struct basic_common_reference;

        template<typename ...Ts>
        struct common_reference;

        template<typename ...Ts>
        using common_reference_t = meta::_t<common_reference<Ts...>>;

        template<typename>
        struct result_of;

        template<typename Sig>
        using result_of_t = meta::_t<result_of<Sig>>;

        struct make_pipeable_fn;

        template<typename Derived>
        struct pipeable;

        template<typename First, typename Second>
        struct composed;

        template<typename ...Fns>
        struct overloaded;

        namespace action
        {
            template<typename Action>
            struct action;
        }

        namespace view
        {
            template<typename View>
            struct view;
        }

        namespace adl_advance_detail
        {
            struct advance_fn;
        }
        using adl_advance_detail::advance_fn;

        struct advance_to_fn;

        struct advance_bounded_fn;

        struct next_fn;

        struct prev_fn;

        struct distance_fn;

        struct iter_size_fn;

        template<typename T>
        struct difference_type;

        template<typename T>
        struct value_type;

        template<typename T>
        struct iterator_category;

        template<typename T>
        struct size_type;

        struct view_base
        {};

        /// \cond
        namespace detail
        {
            template<typename T = void>
            struct any_
            {
                any_() = default;
                any_(T &&)
                {}
            };

            template<>
            struct any_<void>
            {
                any_() = default;
                template<typename T>
                any_(T &&)
                {}
            };

            using any = any_<>;

            struct value_init
            {
                template<typename T>
                operator T () const
                {
                    return T{};
                }
            };

            struct make_compressed_pair_fn;

            template<typename T>
            constexpr T && forward(meta::_t<std::remove_reference<T>> & t) noexcept
            {
                return static_cast<T &&>(t);
            }

            template<typename T>
            constexpr T && forward(meta::_t<std::remove_reference<T>> && t) noexcept
            {
                // This is to catch way sketchy stuff like: forward<int const &>(42)
                static_assert(!std::is_lvalue_reference<T>::value, "You didn't just do that!");
                return static_cast<T &&>(t);
            }

            template<typename T>
            constexpr meta::_t<std::remove_reference<T>> &&
            move(T && t) noexcept
            {
                return static_cast<meta::_t<std::remove_reference<T>> &&>(t);
            }

            template<typename T>
            constexpr T const &as_const(T & t) noexcept
            {
                return t;
            }
            template<typename T>
            void as_const(T const &&) = delete;

            template<typename T>
            using decay_t = meta::_t<std::decay<T>>;

            template<typename T, typename R = meta::_t<std::remove_reference<T>>>
            using as_ref_t =
                meta::_t<std::add_lvalue_reference<meta::_t<std::remove_const<R>>>>;

            template<typename T, typename R = meta::_t<std::remove_reference<T>>>
            using as_cref_t =
                meta::_t<std::add_lvalue_reference<meta::_t<std::add_const<R>>>>;

            struct get_first;
            struct get_second;

            template<typename Val1, typename Val2>
            struct replacer_fn;

            template<typename Pred, typename Val>
            struct replacer_if_fn;

            template<typename I>
            struct move_into_cursor;

            template<typename Int>
            struct from_end_;

            template<typename ...Ts>
            constexpr int ignore_unused(Ts &&...)
            {
                return 42;
            }

            template<int I>
            struct priority_tag
              : priority_tag<I - 1>
            {};

            template<>
            struct priority_tag<0>
            {};

        #if defined(__clang__) && !defined(_LIBCPP_VERSION)
            template<typename T, typename... Args>
            using is_trivially_constructible =
                meta::bool_<__is_trivially_constructible(T, Args...)>;
            template<typename T>
            using is_trivially_default_constructible =
                is_trivially_constructible<T>;
            template<typename T>
            using is_trivially_copy_constructible =
                is_trivially_constructible<T, T const &>;
            template<typename T>
            using is_trivially_move_constructible =
                is_trivially_constructible<T, T>;
            template<typename T, typename U>
            using is_trivially_assignable =
                meta::bool_<__is_trivially_assignable(T, U)>;
            template<typename T>
            using is_trivially_copy_assignable =
                is_trivially_assignable<T &, T const &>;
            template<typename T>
            using is_trivially_move_assignable =
                is_trivially_assignable<T &, T>;
            template<typename T>
            using is_trivially_copyable =
                meta::bool_<__is_trivially_copyable(T)>;
        #elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5
            template<typename T>
            using is_trivially_default_constructible = std::is_trivial<T>;
            template<typename T>
            using is_trivially_copy_constructible = std::is_trivial<T>;
            template<typename T>
            using is_trivially_move_constructible = std::is_trivial<T>;
            template<typename T>
            using is_trivially_copy_assignable = std::is_trivial<T>;
            template<typename T>
            using is_trivially_move_assignable = std::is_trivial<T>;
            template<typename T>
            using is_trivially_copyable = std::is_trivial<T>;
        #else
            template<typename T>
            using is_trivially_default_constructible =
                std::is_trivially_constructible<T>;
            using std::is_trivially_copy_constructible;
            using std::is_trivially_move_constructible;
            using std::is_trivially_copy_assignable;
            using std::is_trivially_move_assignable;
            using std::is_trivially_copyable;
        #endif

        #if RANGES_CXX_LIB_IS_FINAL > 0
        # if defined(__clang__) && !defined(_LIBCPP_VERSION)
            template<typename T>
            using is_final = meta::bool_<__is_final(T)>;
        # else
            using std::is_final;
        # endif
        #else
            template<typename T>
            using is_final = std::false_type;
        #endif

            // Work around libc++'s buggy std::is_function
        #if !defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 3800
            using std::is_function;
        #else
            // Function types here:
            template<typename T>
            char (&is_function_impl_(priority_tag<0>))[1];

            // Array types here:
            template<typename T, typename = decltype((*(T*)0)[0])>
            char (&is_function_impl_(priority_tag<1>))[2];

            // Anything that can be returned from a function here (including
            // void and reference types):
            template<typename T, typename = T(*)()>
            char (&is_function_impl_(priority_tag<2>))[3];

            // Classes and unions (including abstract types) here:
            template<typename T, typename = int T::*>
            char (&is_function_impl_(priority_tag<3>))[4];

            template <typename T>
            struct is_function
              : meta::bool_<sizeof(detail::is_function_impl_<T>(priority_tag<3>{})) == 1>
            {};
        #endif

            template<typename T>
            struct remove_rvalue_reference
            {
                using type = T;
            };

            template<typename T>
            struct remove_rvalue_reference<T &&>
            {
                using type = T;
            };

            template<typename T>
            using remove_rvalue_reference_t = meta::_t<remove_rvalue_reference<T>>;
        }
        /// \endcond

        namespace concepts
        {
            template<typename Concept, typename...Ts>
            struct models;
        }

        struct begin_tag {};
        struct end_tag {};
        struct copy_tag {};
        struct move_tag {};

        template<typename T>
        using uncvref_t =
            meta::_t<std::remove_cv<meta::_t<std::remove_reference<T>>>>;

        struct not_equal_to;
        struct equal_to;
        struct less;
        struct ordered_less;
        struct ident;

        enum cardinality : std::ptrdiff_t
        {
            infinite = -3,
            unknown = -2,
            finite = -1
        };

        template<typename Rng, typename Void = void>
        struct range_cardinality;

        template<typename Rng>
        using is_finite = meta::bool_<range_cardinality<Rng>::value >= finite>;

        template<typename Rng>
        using is_infinite = meta::bool_<range_cardinality<Rng>::value == infinite>;

        template<typename T>
        struct enable_view;

        template<typename R>
        struct disable_sized_range;

        template<typename S, typename I>
        struct disable_sized_sentinel;

        template<typename Cur>
        struct basic_mixin;

        template<typename Cur>
        struct basic_iterator;

        template<cardinality>
        struct basic_view : view_base
        {};

        template<typename Derived, cardinality C = finite>
        struct view_facade;

        template<typename Derived,
                 typename BaseRng,
                 cardinality C = range_cardinality<BaseRng>::value>
        struct view_adaptor;

        template<typename I, typename S>
        struct common_iterator;

        template<typename I, typename S>
        using common_iterator_t =
            meta::if_<std::is_same<I, S>, I, common_iterator<I, S>>;

        template<typename First, typename Second>
        struct compressed_pair;

        template<typename T>
        struct bind_element;

        template<typename T>
        using bind_element_t = meta::_t<bind_element<T>>;

        template<typename Derived, cardinality = finite>
        struct view_interface;

        template<typename T>
        struct istream_range;

    #if !RANGES_CXX_VARIABLE_TEMPLATES
        template<typename T>
        istream_range<T> istream(std::istream & sin);
    #endif

        template<typename I, typename S = I>
        struct iterator_range;

        template<typename I, typename S = I>
        struct sized_iterator_range;

        template<typename T>
        struct reference_wrapper;

        template<typename>
        struct is_reference_wrapper;

        // Views
        //
        template<typename Rng, typename Pred>
        struct adjacent_filter_view;

        namespace view
        {
            struct adjacent_filter_fn;
        }

        template<typename Rng, typename Pred>
        struct adjacent_remove_if_view;

        namespace view
        {
            struct adjacent_remove_if_fn;
        }

        namespace view
        {
            struct all_fn;
        }

        template<typename Rng>
        struct bounded_view;

        namespace view
        {
            struct bounded_fn;
        }

        template<typename Rng>
        struct const_view;

        namespace view
        {
            struct const_fn;
        }

        template<typename I>
        struct counted_view;

        namespace view
        {
            struct counted_fn;
        }

        struct default_sentinel { };

        template<typename I>
        struct move_iterator;

        template<typename I>
        using move_into_iterator =
            basic_iterator<detail::move_into_cursor<I>>;

        template<typename Rng>
        struct cycled_view;

        namespace view
        {
            struct cycle_fn;
        }

        /// \cond
        namespace detail
        {
            template<typename I> struct reverse_cursor;
        }
        /// \endcond

        template<typename I>
        using reverse_iterator = basic_iterator<detail::reverse_cursor<I>>;

        template<typename T>
        struct empty_view;

        namespace view
        {
            struct empty_fn;
        }

        template<typename Rng, typename Pred>
        struct filter_view;

        namespace view
        {
            struct filter_fn;
        }

        template<typename Rng, typename Fun>
        struct group_by_view;

        namespace view
        {
            struct group_by_fn;
        }

        template<typename Rng>
        struct indirect_view;

        namespace view
        {
            struct indirect_fn;
        }

        template<typename From, typename To = void>
        struct iota_view;

        template<typename From, typename To = void>
        struct closed_iota_view;

        namespace view
        {
            struct iota_fn;
            struct closed_iota_fn;
        }

        template<typename Rng, typename ValRng = void>
        struct join_view;

        namespace view
        {
            struct join_fn;
        }

        template<typename...Rngs>
        struct concat_view;

        namespace view
        {
            struct concat_fn;
        }

        template<typename Rng, typename Fun>
        struct partial_sum_view;

        namespace view
        {
            struct partial_sum_fn;
        }

        template<typename Rng>
        struct move_view;

        namespace view
        {
            struct move_fn;
        }

        template<typename Val>
        struct repeat_view;

        namespace view
        {
            struct repeat_fn;
        }

        template<typename Rng>
        struct reverse_view;

        namespace view
        {
            struct reverse_fn;
        }

        template<typename Rng>
        struct slice_view;

        namespace view
        {
            struct slice_fn;
        }

        template<typename Rng, typename Fun>
        struct split_view;

        namespace view
        {
            struct split_fn;
        }

        template<typename Rng>
        struct single_view;

        namespace view
        {
            struct single_fn;
        }

        template<typename Rng>
        struct stride_view;

        namespace view
        {
            struct stride_fn;
        }

        template<typename Rng>
        struct take_view;

        namespace view
        {
            struct take_fn;
        }

        /// \cond
        namespace detail
        {
            template<typename Rng>
            struct is_random_access_bounded_;

            template<typename Rng,
                bool IsRandomAccessBounded = is_random_access_bounded_<Rng>::value>
            struct take_exactly_view_;
        }
        /// \endcond

        template<typename Rng>
        using take_exactly_view = detail::take_exactly_view_<Rng>;

        namespace view
        {
            struct take_exactly_fn;
        }

        template<typename Rng, typename Pred>
        struct iter_take_while_view;

        template<typename Rng, typename Pred>
        struct take_while_view;

        namespace view
        {
            struct iter_take_while_fn;
            struct take_while_fn;
        }

        template<typename Rng, typename Regex, typename SubMatchRange>
        struct tokenize_view;

        namespace view
        {
            struct tokenize_fn;
        }

        template<typename Rng, typename Fun>
        struct iter_transform_view;

        template<typename Rng, typename Fun>
        struct transform_view;

        namespace view
        {
            struct transform_fn;
        }

        template<typename Rng, typename Val1, typename Val2>
        using replace_view = iter_transform_view<Rng, detail::replacer_fn<Val1, Val2>>;

        template<typename Rng, typename Pred, typename Val>
        using replace_if_view = iter_transform_view<Rng, detail::replacer_if_fn<Pred, Val>>;

        namespace view
        {
            struct replace_fn;

            struct replace_if_fn;
        }

        template<typename I>
        struct unbounded_view;

        namespace view
        {
            struct unbounded_fn;
        }

        template<typename Rng>
        using unique_view = adjacent_filter_view<Rng, not_equal_to>;

        namespace view
        {
            struct unique_fn;
        }

        template<typename Rng>
        using keys_range_view = transform_view<Rng, detail::get_first>;

        template<typename Rng>
        using values_view = transform_view<Rng, detail::get_second>;

        namespace view
        {
            struct keys_fn;

            struct values_fn;
        }

        template<typename Fun, typename...Rngs>
        struct iter_zip_with_view;

        template<typename Fun, typename ...Rngs>
        struct zip_with_view;

        template<typename ...Rngs>
        struct zip_view;

        namespace view
        {
            struct iter_zip_with_fn;

            struct zip_with_fn;

            struct zip_fn;
        }
    }
}

RANGES_DIAGNOSTIC_POP

#endif
