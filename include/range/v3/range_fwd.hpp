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

        template<typename Fun>
        struct bindable;

        template<typename ...T>
        struct is_bind_expression;

        extern bindable<adl_begin_end_detail::beginner> const begin;
        extern bindable<adl_begin_end_detail::ender> const end;
        extern bindable<adl_begin_end_detail::cbeginner> const cbegin;
        extern bindable<adl_begin_end_detail::cender> const cend;

        struct nexter;
        struct prever;

        extern bindable<nexter> const next;
        extern bindable<prever> const prev;

        namespace detail
        {
            template<typename First, typename Second, typename Enable = void>
            struct compressed_pair;

            struct compressed_pair_maker;
            extern compressed_pair_maker const make_compressed_pair;

            template<typename Ret, typename PMFN>
            struct member_function_wrapper;

            template<typename Fun>
            struct function_wrapper;

            template<typename ...T>
            struct is_placeholder;

            template<typename T>
            struct is_binder;

            struct binder;
            extern binder const bind;

            template<typename Bind>
            struct bind_wrapper;

            struct unwrap_binder;
            extern unwrap_binder const unwrap_bind;

            template<bool B>
            using bool_ = std::integral_constant<bool, B>;

            using true_ = bool_<true>;
            using false_ = bool_<false>;

            // Thanks to  Louis Dionne for this clever hack for a quick-to-compile
            // implementation of and_ and or_
            template<typename ...T>
            true_ and_impl_(T*...);

            template<typename ...T>
            false_ and_impl_(T...);

            template<bool ...Bools>
            struct and_
              : decltype(detail::and_impl_(
                    typename std::conditional<Bools, int*, int>::type{}...))
            {};

            template<>
            struct and_<>
              : true_
            {};

            template<typename ...T>
            false_ or_impl_(T*...);

            template<typename ...T>
            true_ or_impl_(T...);
                
            template<bool ...Bools>
            struct or_
              : decltype(detail::or_impl_(
                    typename std::conditional<Bools, int, int*>::type{}...))
            {};

            template<>
            struct or_<>
              : false_
            {};

            template<typename T>
            struct identity
            {
                using type = T;
            };

            template<typename T>
            constexpr T && forward(typename std::remove_reference<T>::type& t) noexcept
            {
                return static_cast<T&&>(t);
            }

            template<typename T>
            constexpr T && forward(typename std::remove_reference<T>::type&& t) noexcept
            {
                static_assert(!std::is_lvalue_reference<T>::value, "You didn't just do that!");
                return static_cast<T&&>(t);
            }

            template<typename T>
            constexpr typename std::remove_reference<T>::type &&
            move(T&& t) noexcept
            {
                return static_cast<typename std::remove_reference<T>::type&&>(t);
            }
        }

        template<typename T>
        struct istream_range;

        template<typename T>
        istream_range<T> istream(std::istream & sin);

        template<typename Iter>
        struct iterator_range;

        struct ranger;
        extern bindable<ranger> const range;

        template<typename Rng0, typename Rng1>
        struct join_range;

        struct joiner;
        extern bindable<joiner> const join;

        template<typename Rng>
        struct const_range;

        struct conster;
        extern bindable<conster> const const_;

        template<typename Rng, typename Pred>
        struct filter_range;

        struct filterer;
        extern bindable<filterer> const filter;

        template<typename Rng>
        struct reverse_range;

        struct reverser;
        extern bindable<reverser> const reverse;

        template<typename Rng, typename Fun>
        struct transform_range;

        struct transformer;
        extern bindable<transformer> const transform;

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
        extern bindable<adjacent_finder> const adjacent_find;

        struct binary_searcher;
        extern bindable<binary_searcher> const binary_search;

        struct copier;
        extern bindable<copier> const copy;

        struct backward_copier;
        extern bindable<backward_copier> const copy_backward;

        struct counter;
        extern bindable<counter> const count;

        struct counter_if;
        extern bindable<counter_if> const count_if;

        struct equaler;
        extern bindable<equaler> const equal;

        struct equal_ranger;
        extern bindable<equal_ranger> const equal_range;

        struct filler;
        extern bindable<filler> const fill;

        struct filler_n;
        extern bindable<filler_n> const fill_n;

        struct finder;
        extern bindable<finder> const find;

        struct end_finder;
        extern bindable<end_finder> const find_end;

        struct first_of_finder;
        extern bindable<first_of_finder> const find_first_of;

        struct finder_if;
        extern bindable<finder_if> const find_if;

        struct for_eacher;
        extern bindable<for_eacher> const for_each;

        struct generator;
        extern bindable<generator> const generate;

        struct heap_pusher;
        extern bindable<heap_pusher> const push_heap;

        struct heap_popper;
        extern bindable<heap_popper> const pop_heap;

        struct heap_maker;
        extern bindable<heap_maker> const make_heap;

        struct heap_sorter;
        extern bindable<heap_sorter> const sort_heap;

        struct inplace_merger;
        extern bindable<inplace_merger> const inplace_merge;

        struct lexicographical_comparer;
        extern bindable<lexicographical_comparer> const lexicographical_compare;

        struct lower_bound_finder;
        extern bindable<lower_bound_finder> const lower_bound;
    }
}

#endif
