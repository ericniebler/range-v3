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

#ifndef RANGES_V3_RANGE_FWD_HPP
#define RANGES_V3_RANGE_FWD_HPP

#include <utility>
#include <type_traits>
#include <range/v3/detail/config.hpp>

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

        using adl_begin_end_detail::begin_fn;
        using adl_begin_end_detail::end_fn;
        using adl_begin_end_detail::cbegin_fn;
        using adl_begin_end_detail::cend_fn;

        namespace adl_size_detail
        {
            struct size_fn;
        }

        using adl_size_detail::size_fn;

        template<typename ...Ts>
        struct common_type;

        template<typename ...Ts>
        using common_type_t = typename common_type<Ts...>::type;

        template<typename Sig>
        using result_of_t = typename std::result_of<Sig>::type;

        template<bool Condition, typename T = int>
        using enable_if_t =
            typename std::enable_if<Condition, T>::type;

        struct make_pipeable_fn;

        template<typename Derived>
        struct pipeable;

        namespace action
        {
            template<typename Action>
            struct action;
        }

        struct advance_fn;

        struct advance_to_fn;

        struct advance_bounded_fn;

        struct next_fn;

        struct prev_fn;

        struct distance_fn;

        struct iter_size_fn;

        template<typename I>
        struct iterator_difference;

        struct range_base
        {};

        template<bool Inf>
        struct basic_range : range_base
        {};

        namespace detail
        {
            struct empty
            {};

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

            template<typename ...T>
            struct is_placeholder;

            template<typename T>
            struct is_binder;

            struct wrap_bind_fn;

            template<typename Bind>
            struct bind_wrapper;

            struct unwrap_bind_fn;

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
            // void_
            template<typename...Rest>
            struct always_void
            {
                using type = void;
            };

            template<typename...Rest>
            using void_t = typename always_void<Rest...>::type;

            struct not_equal_to;

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

            template<typename I, typename S>
            struct common_cursor;

            template<typename I, typename D = typename iterator_difference<I>::type>
            struct counted_cursor;

            struct counted_sentinel;

            template<typename Int>
            struct from_end_;

#if !defined(__GLIBCXX__)
            template<typename T>
            using is_trivially_copy_assignable = std::is_trivially_copy_assignable<T>;

            template<typename T>
            using is_trivially_move_assignable = std::is_trivially_move_assignable<T>;
#else
            template<typename T>
            struct is_trivially_copy_assignable
              : std::is_trivial<T>
            {};

            template<typename T>
            struct is_trivially_move_assignable
              : std::is_trivial<T>
            {};
#endif
        }

        namespace concepts
        {
            template<typename Concept, typename...Ts>
            struct models;
        }

        template<typename T>
        using uncvref_t =
            typename std::remove_cv<typename std::remove_reference<T>::type>::type;

        struct begin_tag {};
        struct end_tag {};

        template<typename Rng, typename Void = void>
        struct is_infinite;

        template<typename T, typename Enable = void>
        struct is_range;

        template<typename T, typename Enable = void>
        struct is_sized_iterable;

        template<typename Cur>
        struct basic_mixin;

        template<typename Cur, typename S = Cur>
        struct basic_iterator;

        template<typename S>
        struct basic_sentinel;

        template<typename Derived, bool Inf = false>
        struct range_facade;

        template<typename Derived,
                 typename BaseRng,
                 bool Inf = is_infinite<BaseRng>::value>
        struct range_adaptor;

        template<typename I, typename S>
        using common_iterator =
            typename std::conditional<
                std::is_same<I, S>::value,
                I,
                basic_iterator<detail::common_cursor<I, S>>>::type;

        template<typename First, typename Second>
        struct compressed_pair;

        template<typename...Ts>
        struct compressed_tuple;

        struct make_invokable_fn;

        struct bind_fn;

        template<typename Derived, bool Inf = false>
        struct range_interface;

        template<typename T>
        struct istream_range;

        template<typename T>
        istream_range<T> istream(std::istream & sin);

        template<typename I, typename S = I>
        struct range;

        template<typename I, typename S = I>
        struct sized_range;

        struct make_range_fn;

        template<typename I>
        struct iterator_value;

        template<typename I>
        struct iterator_reference;

        template<typename I>
        struct iterator_category;

        template<typename I>
        struct iterator_difference;

        template<typename I>
        struct iterator_pointer;

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

        template<typename T>
        struct reference_wrapper;

        // Views
        //
        template<typename Rng, typename BinaryPredicate>
        struct adjacent_filtered_view;

        namespace view
        {
            struct adjacent_filter_fn;
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

        template<typename I, typename D = typename iterator_difference<I>::type>
        struct counted_view;

        namespace view
        {
            struct counted_fn;
        }

        template<typename I, typename D = typename iterator_difference<I>::type>
        using counted_iterator =
            basic_iterator<detail::counted_cursor<I, D>, detail::counted_sentinel>;

        using counted_sentinel =
            basic_sentinel<detail::counted_sentinel>;

        template<typename Rng, typename Pred>
        struct filtered_view;

        namespace view
        {
            struct filter_fn;
        }

        template<typename Rng>
        struct indirect_view;

        namespace view
        {
            struct indirect_fn;
        }

        template<typename Rng>
        struct iota_view;

        namespace view
        {
            struct iota_fn;
        }

        template<typename...Rngs>
        struct joined_view;

        namespace view
        {
            struct join_fn;
        }

        template<typename Rng>
        struct move_view;

        namespace view
        {
            struct move_fn;
        }

        template<typename Val>
        struct repeated_view;

        namespace view
        {
            struct repeat_fn;
        }

        namespace view
        {
            struct replace_fn;

            struct replace_if_fn;
        }

        template<typename Rng>
        struct reversed_view;

        namespace view
        {
            struct reverse_fn;
        }

        template<typename Rng>
        struct sliced_view;

        namespace view
        {
            struct slice_fn;
        }

        template<typename Rng>
        struct single_view;

        namespace view
        {
            struct single_fn;
        }

        template<typename Rng>
        struct strided_view;

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

        template<typename Rng, typename Regex, typename SubMatchRange>
        struct tokenized_view;

        namespace view
        {
            struct tokenize_fn;
        }

        template<typename Rng, typename Fun>
        struct transformed_view;

        namespace view
        {
            struct transform_fn;
        }

        template<typename I>
        struct unbounded_view;

        namespace view
        {
            struct unbounded_fn;
        }

        template<typename Rng>
        using uniqued_view = adjacent_filtered_view<Rng, detail::not_equal_to>;

        namespace view
        {
            struct unique_fn;
        }

        template<typename Rng>
        using keys_range_view = transformed_view<Rng, detail::get_first>;

        template<typename Rng>
        using values_range_view = transformed_view<Rng, detail::get_second>;

        namespace view
        {
            struct keys_fn;

            struct values_fn;
        }

        template<typename Fun, typename...Rngs>
        struct zipped_with_view;

        template<typename...Rngs>
        struct zipped_view;

        namespace view
        {
            struct zip_fn;

            struct zip_with_fn;
        }
    }
}

#endif
