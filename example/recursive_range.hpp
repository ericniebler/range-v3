// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/size.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/view/any_view.hpp>
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
            using value_type = meta::_t<std::remove_reference<Ref>>;
            std::function<any_input_view<int>()> fun_;

            struct impl
              : view_interface<impl>
            {
            private:
                friend recursive_range_fn;
                std::function<any_input_view<int>()> const *fun_;
                optional<any_input_view<int>> rng_;

                any_input_view<int> &rng()
                {
                    if(!rng_)
                        rng_ = (*fun_)();
                    return *rng_;
                }
                impl(std::function<any_input_view<int>()> const &fun)
                  : fun_(&fun), rng_{}
                {}
            public:
                impl() = default;
                impl(impl &&) = default;
                impl(impl const &that)
                  : fun_(that.fun_)
                  , rng_{}
                {}
                impl &operator=(impl &&) = default;
                impl &operator=(impl const &that)
                {
                    fun_ = that.fun_;
                    rng_.reset();
                    return *this;
                }
                iterator_t<any_input_view<int>> begin()
                {
                    return ranges::begin(rng());
                }
                sentinel_t<any_input_view<int>> end()
                {
                    return ranges::end(rng());
                }
                any_input_view<int> & base()
                {
                    return rng();
                }
            };
        public:
            template<typename Fun,
                CONCEPT_REQUIRES_(CopyConstructible<Fun>() && Invocable<Fun&>() &&
                    ConvertibleTo<invoke_result_t<Fun&>, any_input_view<Ref>>())>
            explicit recursive_range_fn(Fun fun)
              : fun_{[=]{return view::concat(invoke(fun), view::empty<value_type>());}}
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
