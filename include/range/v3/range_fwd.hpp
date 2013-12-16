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
            struct beginner;
            struct ender;
            struct cbeginner;
            struct cender;
        }

        template<typename Sig>
        using result_of_t = typename std::result_of<Sig>::type;

        template<typename Derived>
        struct bindable;

        template<typename Derived>
        struct pipeable;

        extern adl_begin_end_detail::beginner const begin;
        extern adl_begin_end_detail::ender const end;
        extern adl_begin_end_detail::cbeginner const cbegin;
        extern adl_begin_end_detail::cender const cend;

        struct nexter;
        struct prever;

        extern nexter const next;
        extern prever const prev;

        namespace detail
        {
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
        }

        template<typename First, typename Second>
        struct compressed_pair;

        struct invokable_maker;
        extern invokable_maker const make_invokable;

        struct bind_maker;
        extern bind_maker const bind;

        template<typename T>
        using invokable_t = decltype(make_invokable(std::declval<T>()));

        template<typename T>
        struct istream_range;

        template<typename T>
        istream_range<T> istream(std::istream & sin);

        template<typename Iter>
        struct iterator_range;

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

        struct adjacent_finder;
        extern adjacent_finder const adjacent_find;

        struct binary_searcher;
        extern binary_searcher const binary_search;

        struct copier;
        extern copier const copy;

        struct backward_copier;
        extern backward_copier const copy_backward;

        struct counter;
        extern counter const count;

        struct counter_if;
        extern counter_if const count_if;

        struct equaler;
        extern equaler const equal;

        struct equal_ranger;
        extern equal_ranger const equal_range;

        struct filler;
        extern filler const fill;

        struct filler_n;
        extern filler_n const fill_n;

        struct finder;
        extern finder const find;

        struct end_finder;
        extern end_finder const find_end;

        struct first_of_finder;
        extern first_of_finder const find_first_of;

        struct finder_if;
        extern finder_if const find_if;

        struct for_eacher;
        extern for_eacher const for_each;

        struct generator;
        extern generator const generate;

        struct heap_pusher;
        extern heap_pusher const push_heap;

        struct heap_popper;
        extern heap_popper const pop_heap;

        struct heap_maker;
        extern heap_maker const make_heap;

        struct heap_sorter;
        extern heap_sorter const sort_heap;

        struct inplace_merger;
        extern inplace_merger const inplace_merge;

        struct lexicographical_comparer;
        extern lexicographical_comparer const lexicographical_compare;

        struct lower_bound_finder;
        extern lower_bound_finder const lower_bound;

        struct max_element_finder;
        extern max_element_finder const max_element;

        struct merger;
        extern merger const merge;

        struct min_element_finder;
        extern min_element_finder const min_element;

        struct mismatcher;
        extern mismatcher const mismatch;

        struct nth_element_finder;
        extern nth_element_finder const nth_element;

        struct partial_sorter;
        extern partial_sorter const partial_sort;

        struct partial_sorter_copier;
        extern partial_sorter_copier const partial_sort_copy;

        struct partitioner;
        extern partitioner const partition;

        struct next_permutation_finder;
        extern next_permutation_finder const next_permutation;

        struct prev_permutation_finder;
        extern prev_permutation_finder const prev_permutation;

        struct random_shuffler;
        extern random_shuffler const random_shuffle;

        struct remover;
        extern remover const remove;

        struct remover_copier;
        extern remover_copier const remove_copy;

        struct remover_copier_if;
        extern remover_copier_if const remove_copy_if;

        struct remover_if;
        extern remover_if const remove_if;

        struct replacer;
        extern replacer const replace;

        struct replacer_copier;
        extern replacer_copier const replace_copy;

        struct replacer_copier_if;
        extern replacer_copier_if const replace_copy_if;

        struct replacer_if;
        extern replacer_if const replace_if;

        struct reverser;
        extern reverser const reverse;

        struct reverser_copier;
        extern reverser_copier const reverse_copy;

        struct rotater;
        extern rotater const rotate;

        struct rotater_copier;
        extern rotater_copier const rotate_copy;

        struct searcher;
        extern searcher const search;

        struct searcher_n;
        extern searcher_n const search_n;

        struct includer;
        extern includer const includes;

        struct set_unioner;
        extern set_unioner const set_union;

        struct set_intersecter;
        extern set_intersecter const set_intersection;

        struct set_differencer;
        extern set_differencer const set_difference;

        struct set_symmetric_differencer;
        extern set_symmetric_differencer const set_symmetric_difference;

        struct sorter;
        extern sorter const sort;

        struct stable_partitioner;
        extern stable_partitioner const stable_partition;

        struct stable_sorter;
        extern stable_sorter const stable_sort;

        struct range_swapper;
        extern range_swapper const swap_ranges;

        struct transformer;
        extern transformer const transform;

        struct uniquer;
        extern uniquer const unique;

        struct uniquer_copier;
        extern uniquer_copier const unique_copy;

        struct upper_bound_finder;
        extern upper_bound_finder const upper_bound;

        // Views
        //
        template<typename Rng>
        struct const_range_view;

        namespace view
        {
            struct conster;
            extern conster const const_;
        }

        template<typename Rng, typename Pred>
        struct filter_range_view;

        namespace view
        {
            struct filterer;
            extern filterer const filter;
        }

        template<typename Rng0, typename Rng1>
        struct join_range_view;

        namespace view
        {
            struct joiner;
            extern joiner const join;
        }

        template<typename Rng>
        struct reverse_range_view;

        namespace view
        {
            struct reverser;
            extern reverser const reverse;
        }

        template<typename Rng, typename Fun>
        struct transform_range_view;

        namespace view
        {
            struct transformer;
            extern transformer const transform;
        }
    }
}

#endif
