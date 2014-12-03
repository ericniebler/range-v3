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

#ifndef RANGES_V3_ACTION_TRANSFORM_HPP
#define RANGES_V3_ACTION_TRANSFORM_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct transform_fn
            {
            private:
                friend action_access;
                template<typename F, typename P = ident, CONCEPT_REQUIRES_(!Iterable<F>())>
                static auto bind(transform_fn transform, F fun, P proj = P{})
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(transform, std::placeholders::_1, protect(std::move(fun)),
                        protect(std::move(proj)))
                )
            public:
                template<typename Rng, typename F, typename P = ident,
                    typename I = range_iterator_t<Rng>,
                    CONCEPT_REQUIRES_(Iterable<Rng &>() && Transformable1<I, I, F, P>())>
                void operator()(Rng & rng, F fun, P proj = P{}) const
                {
                    ranges::transform(rng, begin(rng), std::move(fun), std::move(proj));
                }
            };

            /// \ingroup group-actions
            /// \sa transform_fn
            /// \sa action
            constexpr action<transform_fn> transform{};
        }
        /// @}
    }
}

#endif
