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

#ifndef BOOST_RANGE_V3_RANGE_FWD_HPP
#define BOOST_RANGE_V3_RANGE_FWD_HPP

#include <iosfwd>

namespace boost
{
    namespace range
    {
        inline namespace v3
        {
            namespace adl_begin_end_detail
            {
                struct begin_t;
                extern begin_t const adl_begin;

                struct end_t;
                extern end_t const adl_end;

                struct cbegin_t;
                extern cbegin_t const adl_cbegin;

                struct cend_t;
                extern cend_t const adl_cend;
            }

            namespace detail
            {
                using adl_begin_end_detail::adl_begin;
                using adl_begin_end_detail::adl_end;
                using adl_begin_end_detail::adl_cbegin;
                using adl_begin_end_detail::adl_cend;

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

            struct iterator_range_maker;
            extern iterator_range_maker const make_iterator_range;

            template<typename Rng>
            struct const_range;

            struct conster;
            extern conster const const_;

            template<typename Rng, typename Pred>
            struct filter_range;

            struct filterer;
            extern filterer const filter;

            template<typename Rng, typename Fun>
            struct transform_range;

            struct transformer;
            extern transformer const transform;
        }
    }
}

#endif
