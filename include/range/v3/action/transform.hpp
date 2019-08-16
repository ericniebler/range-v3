/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_TRANSFORM_HPP
#define RANGES_V3_ACTION_TRANSFORM_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct transform_fn
        {
        private:
            friend action_access;
            template<typename F, typename P = identity>
            static auto CPP_fun(bind)(transform_fn transform, F fun, P proj = P{})( //
                requires(!range<F>))
            {
                return bind_back(transform, std::move(fun), std::move(proj));
            }

        public:
            template<typename Rng, typename F, typename P = identity>
            auto operator()(Rng && rng, F fun, P proj = P{}) const -> CPP_ret(Rng)( //
                requires input_range<Rng> && copy_constructible<F> &&
                    writable<iterator_t<Rng>,
                             indirect_result_t<F &, projected<iterator_t<Rng>, P>>>)
            {
                ranges::transform(rng, begin(rng), std::move(fun), std::move(proj));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \relates transform_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<transform_fn>, transform)
    } // namespace actions
    /// @}
} // namespace ranges

#endif
