// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

// DISCLAIMER: This code is incredibly dumb, slow, inefficient
// and probably causes tooth decay, hair loss, and love handles.
//
//            DO NOT USE THIS CODE FOR ANYTHING REAL
//
// (Can't say you weren't warned.)

#ifndef RANGES_V3_EXT_VIEW_RECURSIVE_RANGE_HPP
#define RANGES_V3_EXT_VIEW_RECURSIVE_RANGE_HPP

#ifndef __CYGWIN__
#include <mutex>
#endif
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/size.hpp>
#include <range/v3/range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_interface.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/view/any_range.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/concat.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace ext
        {
        template<typename Ref>
        struct recursive_range_fn
        {
        private:
            using value_type = meta::eval<std::remove_reference<Ref>>;
            std::function<any_input_range<int>()> fun_;

            struct impl
              : range_interface<impl>
            {
            private:
                friend recursive_range_fn;
                std::function<any_input_range<int>()> const *fun_;
                mutable optional<any_input_range<int>> rng_;
            #ifndef __CYGWIN__
                mutable std::mutex mtx_;
            #endif
                any_input_range<int> const &rng() const
                {
                #ifndef __CYGWIN__
                    std::lock_guard<std::mutex> lock{mtx_};
                #endif
                    if(!rng_)
                        rng_ = (*fun_)();
                    return *rng_;
                }
                impl(std::function<any_input_range<int>()> const &fun)
                  : fun_(&fun), rng_{}
                #ifndef __CYGWIN__
                  , mtx_{}
                #endif
                {}
            public:
                impl() = default;
                impl(impl &&) = default;
                impl(impl const &that)
                  : fun_(that.fun_), rng_{}
                #ifndef __CYGWIN__
                  , mtx_{}
                #endif
                {}
                impl &operator=(impl &&) = default;
                impl &operator=(impl const &that)
                {
                    fun_ = that.fun_;
                    rng_ = that.rng_;
                    return *this;
                }
                range_iterator_t<any_input_range<int>> begin() const
                {
                    return ranges::begin(rng());
                }
                range_sentinel_t<any_input_range<int>> end() const
                {
                    return ranges::end(rng());
                }
                any_input_range<int> & base()
                {
                    return rng();
                }
                any_input_range<int> const & base() const
                {
                    return rng();
                }
            };
        public:
            template<typename Fun,
                CONCEPT_REQUIRES_(Function<Fun>() &&
                                  Convertible<
                                    concepts::Function::result_t<Fun>,
                                    any_input_range<Ref>
                                  >())>
            explicit recursive_range_fn(Fun fun)
              : fun_{[=](){return view::concat(fun(), view::empty<value_type>());}}
            {}
            recursive_range_fn(recursive_range_fn const &) = delete;
            recursive_range_fn &operator=(recursive_range_fn const &) = delete;

            impl operator()() const
            {
                return {fun_};
            }
        };
        }
    }
}

#endif
