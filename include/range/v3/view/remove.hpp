/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_REMOVE_HPP
#define RANGES_V3_VIEW_REMOVE_HPP

#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/functional/bind.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    namespace view
    {
        struct remove_fn
        {
        private:
            friend view_access;

            template<typename Value>
            static auto bind(remove_fn remove, Value value)
            {
                return make_pipeable(
                    std::bind(remove, std::placeholders::_1, std::move(value)));
            }
            template<typename Value, typename Proj>
            static auto CPP_fun(bind)(remove_fn remove, Value value, Proj proj)(
                requires (!Range<Value>))
            {
                return make_pipeable(
                    std::bind(remove, std::placeholders::_1, std::move(value),
                        protect(std::move(proj))));
            }

            template<typename Value>
            struct pred
            {
                Value value_;
                template<typename T>
                auto operator()(T &&other) const ->
                    CPP_ret(bool)(
                        requires EqualityComparableWith<T, Value const &>)
                {
                    return static_cast<T &&>(other) == value_;
                }
            };
        public:
            template<typename Rng, typename Value>
            constexpr /*c++14*/
            auto CPP_fun(operator())(Rng&& rng, Value value) (const
                requires MoveConstructible<Value> && ViewableRange<Rng> && InputRange<Rng> &&
                    IndirectlyComparable<iterator_t<Rng>, Value const *, equal_to>)
            {
                return remove_if(static_cast<Rng&&>(rng), pred<Value>{std::move(value)});
            }

            template<typename Rng, typename Value, typename Proj>
            constexpr /*c++14*/
            auto CPP_fun(operator())(Rng&& rng, Value value, Proj proj) (const
                requires MoveConstructible<Value> && ViewableRange<Rng> && InputRange<Rng> &&
                    IndirectlyComparable<iterator_t<Rng>, Value const *, equal_to, Proj>)
            {
                return remove_if(static_cast<Rng&&>(rng), pred<Value>{std::move(value)},
                    std::move(proj));
            }
        };

        /// \relates remove_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<remove_fn>, remove)
    }
    /// @}
}

#endif //RANGES_V3_VIEW_REMOVE_HPP
