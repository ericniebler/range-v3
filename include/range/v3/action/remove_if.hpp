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

#ifndef RANGES_V3_ACTION_REMOVE_IF_HPP
#define RANGES_V3_ACTION_REMOVE_IF_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/remove_if.hpp>

namespace ranges
{
    inline namespace v3 
    {
        // TODO Look at all the special cases handled by erase_if in Library Fundamentals 2

        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct remove_if_fn
            {
            private:
                friend action_access;
                template<typename C, typename P = ident, CONCEPT_REQUIRES_(!Iterable<C>())>
                static auto bind(remove_if_fn remove_if, C pred, P proj = P{})
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(remove_if, std::placeholders::_1, protect(std::move(pred)),
                        protect(std::move(proj)))
                )
            public:
                template<typename Rng, typename C, typename P = ident,
                    typename I = range_iterator_t<Rng>,
                    CONCEPT_REQUIRES_(ForwardIterable<Rng &>() &&
                        EraseableIterable<Rng &, I, I>() && RemovableIf<I, C, P>())>
                void operator()(Rng & rng, C pred, P proj = P{}) const
                {
                    auto it = ranges::remove_if(rng, std::move(pred), std::move(proj));
                    ranges::erase(rng, it, ranges::end(rng));
                }
            };

            /// \ingroup group-actions
            /// \sa action
            /// \sa with_braced_init_args
            constexpr action<remove_if_fn> remove_if{};
        }
        /// @}
    }
}

#endif
