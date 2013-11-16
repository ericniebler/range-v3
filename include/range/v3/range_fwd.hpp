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

#ifndef RANGE_V3_RANGE_FWD_HPP
#define RANGE_V3_RANGE_FWD_HPP

#include <iosfwd>

namespace ranges
{
    inline namespace v3
    {
        namespace adl_begin_end_detail
        {
            struct begin_t;
            struct end_t;
            struct cbegin_t;
            struct cend_t;
        }

        template<typename Fun>
        struct bindable;

        template<typename ...T>
        struct is_bind_expression;

        extern bindable<adl_begin_end_detail::begin_t> const begin;
        extern bindable<adl_begin_end_detail::end_t> const end;
        extern bindable<adl_begin_end_detail::cbegin_t> const cbegin;
        extern bindable<adl_begin_end_detail::cend_t> const cend;

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
        using range_iterator_t = typename range_iterator<Rng>::type;

        template<typename Rng>
        struct range_value;

        template<typename Rng>
        using range_value_t = typename range_value<Rng>::type;

        template<typename Rng>
        struct range_reference;

        template<typename Rng>
        using range_reference_t = typename range_reference<Rng>::type;

        template<typename Rng>
        struct range_category;

        template<typename Rng>
        using range_category_t = typename range_category<Rng>::type;

        template<typename Rng>
        struct range_difference;

        template<typename Rng>
        using range_difference_t = typename range_difference<Rng>::type;

        template<typename Rng>
        struct range_pointer;

        template<typename Rng>
        using range_pointer_t = typename range_pointer<Rng>::type;

        struct adjacent_finder;
        extern bindable<adjacent_finder> const adjacent_find;
    }
}

#endif
