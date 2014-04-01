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

        struct nexter;
        extern nexter const next;

        struct prever;
        extern prever const prev;

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

            struct compressed_pair_maker;
            extern compressed_pair_maker const make_compressed_pair;

            template<typename Ret, typename PMFN>
            struct member_function_wrapper;

            template<typename ...T>
            struct is_placeholder;

            template<typename T>
            struct is_binder;

            struct bind_wrapper_maker;
            extern bind_wrapper_maker const wrap_bind;

            template<typename Bind>
            struct bind_wrapper;

            struct unwrap_binder;
            extern unwrap_binder const unwrap_bind;

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

            template<bool Infinite>
            struct is_infinite
            {};

            template<typename Iterator, typename Sentinel>
            struct common_cursor;

            template<typename InputIterator>
            struct counted_cursor;

            template<typename InputIterator>
            struct counted_sentinel;
        }

        namespace concepts
        {
            struct Iterable;
            struct Range;
        }

        struct begin_tag {};
        struct end_tag {};

        struct use_default;

        template<typename Iterable, typename Void = void>
        struct is_infinite;

        struct range_base
        {};

        template<typename Cursor, typename Sentinel = Cursor>
        struct basic_range_iterator;

        template<typename Sentinel>
        struct basic_range_sentinel;

        template<typename Derived, bool Infinite = false>
        struct range_facade;

        template<typename Derived,
                 typename BaseIterable,
                 bool Infinite = is_infinite<BaseIterable>::value>
        struct range_adaptor;

        template<typename Iterator, typename Sentinel>
        using common_range_iterator =
            basic_range_iterator<detail::common_cursor<Iterator, Sentinel>>;

        struct public_t;

        template<typename First, typename Second>
        class compressed_pair;

        template<typename...Ts>
        struct compressed_tuple;

        struct make_invokable_fn;
        extern make_invokable_fn const make_invokable;

        struct bind_maker;
        extern bind_maker const bind;

        template<typename T>
        using invokable_t = decltype(make_invokable(std::declval<T>()));

        template<typename T>
        struct istream_iterable;

        template<typename T>
        istream_iterable<T> istream(std::istream & sin);

        template<typename Iterator, typename Sentinel = Iterator>
        struct iterator_range;

        template<typename Iterator, typename Sentinel = Iterator>
        struct sized_iterator_range;

        struct ranger;
        extern ranger const range;

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
        template<typename ForwardRange, typename BinaryPredicate>
        struct adjacent_filtered_view;

        namespace view
        {
            struct adjacent_filterer;
            extern adjacent_filterer const adjacent_filter;
        }

        namespace view
        {
            struct all_getter;
            extern all_getter const all;
        }

        template<typename Iterable>
        struct as_range_view;

        namespace view
        {
            struct as_ranger;
            extern as_ranger const as_range;
        }

        template<typename Iterable>
        struct const_view;

        namespace view
        {
            struct conster;
            extern conster const const_;
        }

        template<typename InputIterator>
        struct counted_view;

        namespace view
        {
            struct counted_maker;
            extern counted_maker const counted;
        }

        template<typename InputIterator>
        using counted_iterator =
            basic_range_iterator<detail::counted_cursor<InputIterator>, detail::counted_sentinel<InputIterator>>;

        template<typename InputIterator>
        using counted_sentinel =
            basic_range_sentinel<detail::counted_sentinel<InputIterator>>;

        template<typename Rng, typename Pred>
        struct filtered_view;

        namespace view
        {
            struct filterer;
            extern filterer const filter;
        }

        template<typename InputRange>
        struct indirect_view;

        namespace view
        {
            struct indirecter;
            extern indirecter const indirect;
        }

        template<typename InputRange>
        struct iota_view;

        namespace view
        {
            struct iota_maker;
            extern iota_maker const iota;
        }

        template<typename...InputIterables>
        struct joined_view;

        namespace view
        {
            struct joiner;
            extern joiner const join;
        }

        template<typename InputRange>
        struct move_view;

        namespace view
        {
            struct mover;
            extern mover const move;
        }

        template<typename Value>
        struct repeated_view;

        namespace view
        {
            struct repeater;
            extern repeater const repeat;
        }

        namespace view
        {
            struct replacer;
            extern replacer const replace;

            struct replacer_if;
            extern replacer_if const replace_if;
        }

        template<typename Rng>
        struct reversed_view;

        namespace view
        {
            struct reverser;
            extern reverser const reverse;
        }

        template<typename InputRange>
        struct sliced_view;

        namespace view
        {
            struct slicer;
            extern slicer const slice;
        }

        template<typename InputRange>
        struct strided_view;

        namespace view
        {
            struct strider;
            extern strider const stride;
        }

        namespace view
        {
            struct taker;
            extern taker const take;
        }

        template<typename BidirectionalRange, typename Regex, typename SubMatchRange>
        struct tokenized_view;

        namespace view
        {
            struct tokenizer;
            extern tokenizer const tokenize;
        }

        template<typename Rng, typename Fun>
        struct transformed_view;

        namespace view
        {
            struct transformer;
            extern transformer const transform;
        }

        template<typename InputIterator>
        struct unbounded_view;

        namespace view
        {
            struct unbounded_maker;
            extern unbounded_maker const unbounded;
        }

        template<typename ForwardRange>
        using uniqued_view = adjacent_filtered_view<ForwardRange, detail::not_equal_to>;

        namespace view
        {
            struct uniquer;
            extern uniquer const unique;
        }

        template<typename InputRange>
        using keys_range_view = transformed_view<InputRange, detail::get_first>;

        template<typename InputRange>
        using values_range_view = transformed_view<InputRange, detail::get_second>;

        namespace view
        {
            struct map_keys;
            extern map_keys const keys;

            struct map_values;
            extern map_values const values;
        }

        template<typename...InputRanges>
        struct zip_range_view;

        namespace view
        {
            struct zipper;
            extern zipper const zip;
        }
    }
}

#endif
