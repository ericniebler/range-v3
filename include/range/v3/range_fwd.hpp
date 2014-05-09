// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_RANGE_FWD_HPP
#define RANGES_V3_RANGE_FWD_HPP

#include <iosfwd>
#include <type_traits>
#include <utility>

#ifndef RANGES_ASSERT
# include <cassert>
# define RANGES_ASSERT assert
#endif

#if __GNUC__ == 4 && __GNUC_MINOR__ < 9
#define RANGES_CONSTEXPR const
#else
#define RANGES_CONSTEXPR constexpr
#endif

#define RANGES_DECLTYPE_AUTO_RETURN(...)    \
    -> decltype(__VA_ARGS__)                \
    { return (__VA_ARGS__); }               \
    /**/

namespace ranges
{
    inline namespace v3
    {
        namespace adl_begin_end_detail
        {
            struct begin_fn;
            struct end_fn;
            struct cbegin_fn;
            struct cend_fn;
        }

        struct size_fn;

        template<typename ...Ts>
        struct common_type;

        template<typename ...Ts>
        using common_type_t = typename common_type<Ts...>::type;

        template<typename Sig>
        using result_of_t = typename std::result_of<Sig>::type;

        template<bool Condition, typename T = int>
        using enable_if_t =
            typename std::enable_if<Condition, T>::type;

        template<typename Derived>
        struct bindable;

        template<typename Derived>
        struct pipeable;

        extern adl_begin_end_detail::begin_fn const begin;
        extern adl_begin_end_detail::end_fn const end;
        extern adl_begin_end_detail::cbegin_fn const cbegin;
        extern adl_begin_end_detail::cend_fn const cend;

        struct advance_fn;
        extern advance_fn const advance;

        struct advance_bounded_fn;
        extern advance_bounded_fn const advance_bounded;

        struct next_fn;
        extern next_fn const next;

        struct prev_fn;
        extern prev_fn const prev;

        struct distance_fn;
        extern distance_fn const distance;

        extern size_fn const size;

        namespace detail
        {
            struct empty
            {
                template<typename ...Ts>
                empty(Ts &&...)
                {}
            };

            template<typename T = void>
            struct any_
            {
                any_(T &&)
                {}
            };

            template<>
            struct any_<void>
            {
                template<typename T>
                any_(T &&)
                {}
            };

            using any = any_<>;

            template<typename T>
            using uncvref_t =
                typename std::remove_cv<typename std::remove_reference<T>::type>::type;

            struct make_compressed_pair_fn;
            extern make_compressed_pair_fn const make_compressed_pair;

            template<typename Ret, typename PMFN>
            struct member_function_wrapper;

            template<typename ...T>
            struct is_placeholder;

            template<typename T>
            struct is_binder;

            struct wrap_bind_fn;
            extern wrap_bind_fn const wrap_bind;

            template<typename Bind>
            struct bind_wrapper;

            struct unwrap_bind_fn;
            extern unwrap_bind_fn const unwrap_bind;

            template<typename T>
            struct identity
            {
                using type = T;
            };

            template<typename T>
            constexpr T && forward(typename std::remove_reference<T>::type & t) noexcept
            {
                return static_cast<T &&>(t);
            }

            template<typename T>
            constexpr T && forward(typename std::remove_reference<T>::type && t) noexcept
            {
                // This is to catch way sketchy stuff like: forward<int const &>(42)
                static_assert(!std::is_lvalue_reference<T>::value, "You didn't just do that!");
                return static_cast<T &&>(t);
            }

            template<typename T>
            constexpr typename std::remove_reference<T>::type &&
            move(T && t) noexcept
            {
                return static_cast<typename std::remove_reference<T>::type &&>(t);
            }

            template<typename Fn, typename...Args>
            struct binder;

            ////////////////////////////////////////////////////////////////////////////////////
            // always
            template<typename A, typename...Rest>
            struct always
            {
                using type = A;
            };

            template<typename A, typename...Rest>
            using always_t = typename always<A, Rest...>::type;

            struct not_equal_to;

            template<bool B, typename T, typename U>
            using conditional_t = typename std::conditional<B, T, U>::type;

            template<bool B, typename T, typename U>
            using lazy_conditional_t = typename conditional_t<B, T, U>::type;

            template<typename T, bool B>
            using add_const_if_t = conditional_t<B, T const, T>;

            template<typename T>
            using not_t = std::integral_constant<bool, !T::value>;

            template<typename T, typename U>
            using and_t = std::integral_constant<bool, T::value && U::value>;

            template<typename T, typename U>
            using or_t = std::integral_constant<bool, T::value || U::value>;

            template<typename T>
            using decay_t = typename std::decay<T>::type;

            template<typename T>
            using as_ref_t =
                typename std::remove_const<typename std::remove_reference<T>::type>::type &;

            template<typename T>
            using as_cref_t =
                typename std::add_const<typename std::remove_reference<T>::type>::type &;

            struct get_first;
            struct get_second;

            template<typename...Ts>
            void valid_exprs(Ts &&...);

            template<bool Inf>
            struct is_infinite
            {};

            template<typename I, typename S>
            struct common_cursor;

            template<typename I>
            struct counted_cursor;

            template<typename I>
            struct counted_sentinel;
        }

        namespace concepts
        {
            template<typename Concept, typename...Ts>
            struct models;

            struct Iterable;
            struct Range;
        }

        struct begin_tag {};
        struct end_tag {};

        struct use_default;

        template<typename Rng, typename Void = void>
        struct is_infinite;

        struct range_base
        {};

        template<typename Cur, typename S = Cur>
        struct basic_range_iterator;

        template<typename S>
        struct basic_range_sentinel;

        template<typename Derived, bool Inf = false>
        struct range_facade;

        template<typename Derived,
                 typename BaseRng,
                 bool Inf = is_infinite<BaseRng>::value>
        struct range_adaptor;

        template<typename I, typename S>
        using common_range_iterator =
            basic_range_iterator<detail::common_cursor<I, S>>;

        template<typename First, typename Second>
        class compressed_pair;

        template<typename...Ts>
        struct compressed_tuple;

        struct make_invokable_fn;
        extern make_invokable_fn const invokable;

        struct bind_fn;
        extern bind_fn const bind;

        template<typename T>
        using invokable_t = decltype(invokable(std::declval<T>()));

        template<typename T>
        struct istream_iterable;

        template<typename T>
        istream_iterable<T> istream(std::istream & sin);

        template<typename I, typename S = I>
        struct iterator_range;

        template<typename I, typename S = I>
        struct sized_iterator_range;

        struct range_fn;
        extern range_fn const range;

        template<typename Rng>
        struct range_iterator;

        template<typename Rng>
        struct range_value;

        template<typename Rng>
        struct range_reference;

        template<typename Rng>
        struct range_category;

        template<typename Rng>
        struct range_difference;

        template<typename Rng>
        struct range_pointer;

        // Views
        //
        template<typename Rng, typename BinaryPredicate>
        struct adjacent_filtered_view;

        namespace view
        {
            struct adjacent_filter_fn;
            extern adjacent_filter_fn const adjacent_filter;
        }

        namespace view
        {
            struct all_fn;
            extern all_fn const all;
        }

        template<typename Rng>
        struct as_range_view;

        namespace view
        {
            struct as_range_fn;
            extern as_range_fn const as_range;
        }

        template<typename Rng>
        struct const_view;

        namespace view
        {
            struct const_fn;
            extern const_fn const const_;
        }

        template<typename I>
        struct counted_view;

        namespace view
        {
            struct counted_fn;
            extern counted_fn const counted;
        }

        template<typename I>
        using counted_iterator =
            basic_range_iterator<detail::counted_cursor<I>, detail::counted_sentinel<I>>;

        template<typename I>
        using counted_sentinel =
            basic_range_sentinel<detail::counted_sentinel<I>>;

        template<typename Rng, typename Pred>
        struct filtered_view;

        namespace view
        {
            struct filter_fn;
            extern filter_fn const filter;
        }

        template<typename Rng>
        struct indirect_view;

        namespace view
        {
            struct indirect_fn;
            extern indirect_fn const indirect;
        }

        template<typename Rng>
        struct iota_view;

        namespace view
        {
            struct iota_fn;
            extern iota_fn const iota;
        }

        template<typename...Rngs>
        struct joined_view;

        namespace view
        {
            struct join_fn;
            extern join_fn const join;
        }

        template<typename Rng>
        struct move_view;

        namespace view
        {
            struct move_fn;
            extern move_fn const move;
        }

        template<typename Val>
        struct repeated_view;

        namespace view
        {
            struct repeat_fn;
            extern repeat_fn const repeat;
        }

        namespace view
        {
            struct replace_fn;
            extern replace_fn const replace;

            struct replace_if_fn;
            extern replace_if_fn const replace_if;
        }

        template<typename Rng>
        struct reversed_view;

        namespace view
        {
            struct reverse_fn;
            extern reverse_fn const reverse;
        }

        template<typename Rng>
        struct sliced_view;

        namespace view
        {
            struct slice_fn;
            extern slice_fn const slice;
        }

        template<typename Rng>
        struct single_view;

        namespace view
        {
            struct single_fn;
            extern single_fn const single;
        }

        template<typename Rng>
        struct strided_view;

        namespace view
        {
            struct stride_fn;
            extern stride_fn const stride;
        }

        template<typename Rng>
        struct take_view;

        namespace view
        {
            struct take_fn;
            extern take_fn const take;
        }

        template<typename Rng, typename Regex, typename SubMatchRange>
        struct tokenized_view;

        namespace view
        {
            struct tokenize_fn;
            extern tokenize_fn const tokenize;
        }

        template<typename Rng, typename Fun>
        struct transformed_view;

        namespace view
        {
            struct transform_fn;
            extern transform_fn const transform;
        }

        template<typename I>
        struct unbounded_view;

        namespace view
        {
            struct unbounded_fn;
            extern unbounded_fn const unbounded;
        }

        template<typename Rng>
        using uniqued_view = adjacent_filtered_view<Rng, detail::not_equal_to>;

        namespace view
        {
            struct unique_fn;
            extern unique_fn const unique;
        }

        template<typename Rng>
        using keys_range_view = transformed_view<Rng, detail::get_first>;

        template<typename Rng>
        using values_range_view = transformed_view<Rng, detail::get_second>;

        namespace view
        {
            struct keys_fn;
            extern keys_fn const keys;

            struct values_fn;
            extern values_fn const values;
        }

        template<typename...InputRanges>
        struct zip_range_view;

        namespace view
        {
            struct zip_fn;
            extern zip_fn const zip;
        }
    }
}

#endif
