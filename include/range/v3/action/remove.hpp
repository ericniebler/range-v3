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
#ifndef RANGES_V3_ACTION_REMOVE_HPP
#define RANGES_V3_ACTION_REMOVE_HPP

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/remove.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct remove_fn
        {
        private:
            friend action_access;
            template<typename V, typename P>
            static auto CPP_fun(bind)(remove_fn remove, V && value, P proj)( //
                requires(!range<V>))
            {
                return bind_back(remove, static_cast<V &&>(value), std::move(proj));
            }
            template<typename V>
            static auto bind(remove_fn remove, V && value)
            {
                return bind_back(remove, static_cast<V &&>(value), identity{});
            }

        public:
            template<typename Rng, typename V, typename P = identity>
            auto operator()(Rng && rng, V const & value, P proj = {}) const
                -> CPP_ret(Rng)( //
                    requires forward_range<Rng> && permutable<iterator_t<Rng>> &&
                        erasable_range<Rng, iterator_t<Rng>, sentinel_t<Rng>> &&
                            indirect_relation<equal_to, projected<iterator_t<Rng>, P>,
                                              V const *>)
            {
                auto it = ranges::remove(rng, value, std::move(proj));
                ranges::erase(rng, it, ranges::end(rng));
                return static_cast<Rng &&>(rng);
            }
        };
        /// \ingroup group-actions
        /// \sa action
        /// \sa with_braced_init_args
        RANGES_INLINE_VARIABLE(action<remove_fn>, remove)
    } // namespace actions
    /// @}
} // namespace ranges

#endif // include guard
