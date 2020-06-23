/// \file
// Range v3 library
//
//  Copyright Semir Vrana 2020-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_SUCCESSOR_HPP
#define RANGES_V3_VIEW_SUCCESSOR_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/unreachable_sentinel.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/facade.hpp>

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{

    /// Iteratively calls a function on an initial value
    /// successor_view(f, x) -> [x, f(x), f(f(x)), ...]
    template<typename G, typename T>
    struct successor_view : view_facade<successor_view<G, T>, infinite>
    {
    private:
        friend range_access;
        using result_t = T;
        semiregular_box_t<G> gen_;
        semiregular_box_t<T> val_;
        struct cursor
        {
        private:
            successor_view * view_;

        public:
            cursor() = default;
            explicit cursor(successor_view * view)
              : view_(view)
            {}
            const result_t & read() const
            {
                return view_->val_;
            }
            void next()
            {
                view_->val_ = ranges::invoke(view_->gen_, std::move(view_->val_));
            }
        };
        cursor begin_cursor()
        {
            return cursor{this};
        }
        unreachable_sentinel_t end_cursor() const
        {
            return {};
        }

    public:
        successor_view() = default;
        explicit successor_view(G g, T x)
          : gen_(std::move(g))
          , val_(std::move(x))
        {}
        const result_t & cached()
        {
            return *val_;
        }
    };

    namespace views
    {
        struct successor_fn
        {
            template<typename G, typename T>
            auto operator()(G g, T x) const -> CPP_ret(successor_view<G, T>)( //
                requires invocable<G &, T &&> && copy_constructible<G> &&
                    copy_constructible<T> &&
                        assignable_from<T &, invoke_result_t<G &, T &&>>)
            {
                return successor_view<G, T>{std::move(g), std::move(x)};
            }
        };

        /// \relates successor_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(successor_fn, successor)
    } // namespace views
      /// \@}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::successor_view)

#endif
