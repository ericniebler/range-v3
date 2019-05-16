/// \file
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

#ifndef RANGES_V3_VIEW_FOR_EACH_HPP
#define RANGES_V3_VIEW_FOR_EACH_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/generate_n.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/transform.hpp>

namespace ranges
{
    namespace view
    {
        /// Lazily applies an unary function to each element in the source
        /// range that returns another range (possibly empty), flattening
        /// the result.
        struct for_each_fn
        {
        private:
            friend view_access;
            template<typename Fun>
            static auto bind(for_each_fn for_each, Fun fun)
            {
                return make_pipeable(std::bind(for_each, std::placeholders::_1,
                    protect(std::move(fun))));
            }
        public:
            template<typename Rng, typename Fun>
            auto CPP_fun(operator())(Rng &&rng, Fun fun) (const
                requires ViewableRange<Rng> &&
                    TransformableRange<Rng, Fun> &&
                    JoinableRange<transform_view<all_t<Rng>, Fun>>)
            {
                return join(transform(static_cast<Rng &&>(rng), std::move(fun)));
            }
        };

        /// \relates for_each_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<for_each_fn>, for_each)
    }

    struct yield_fn
    {
        template<typename V>
        auto operator()(V v) const ->
            CPP_ret(single_view<V>)(
                requires CopyConstructible<V>)
        {
            return view::single(std::move(v));
        }
    };

    /// \relates yield_fn
    /// \ingroup group-views
    RANGES_INLINE_VARIABLE(yield_fn, yield)

    struct yield_from_fn
    {
        template<typename Rng>
        auto operator()(Rng rng) const ->
            CPP_ret(Rng)(
                requires View<Rng>)
        {
            return rng;
        }
    };

    /// \relates yield_from_fn
    /// \ingroup group-views
    RANGES_INLINE_VARIABLE(yield_from_fn, yield_from)

    struct yield_if_fn
    {
        template<typename V>
        repeat_n_view<V> operator()(bool b, V v) const
        {
            return view::repeat_n(std::move(v), b ? 1 : 0);
        }
    };

    /// \relates yield_if_fn
    /// \ingroup group-views
    RANGES_INLINE_VARIABLE(yield_if_fn, yield_if)

    struct lazy_yield_if_fn
    {
        template<typename F>
        auto operator()(bool b, F f) const ->
            CPP_ret(generate_n_view<F>)(
                requires Invocable<F &>)
        {
            return view::generate_n(std::move(f), b ? 1 : 0);
        }
    };

    /// \relates lazy_yield_if_fn
    /// \ingroup group-views
    RANGES_INLINE_VARIABLE(lazy_yield_if_fn, lazy_yield_if)
    /// @}

    /// \cond
    CPP_template(typename Rng, typename Fun)(
        requires ViewableRange<Rng> &&
            view::TransformableRange<Rng, Fun> &&
            InputRange<invoke_result_t<Fun&, range_reference_t<Rng>>>)
    auto operator >>= (Rng &&rng, Fun fun)
    {
        return view::for_each(static_cast<Rng &&>(rng), std::move(fun));
    }
    /// \endcond
}

#endif
