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

#ifndef RANGES_V3_ACTION_TAKE_WHILE_HPP
#define RANGES_V3_ACTION_TAKE_WHILE_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/utility/iterator.hpp>
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
            struct take_while_fn
            {
            private:
                friend action_access;
                template<typename Fun, CONCEPT_REQUIRES_(!Iterable<Fun>())>
                static auto bind(take_while_fn take_while, Fun fun)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(take_while, std::placeholders::_1, std::move(fun))
                )
            public:
                template<typename Rng, typename Fun,
                    typename I = range_iterator_t<Rng>,
                    CONCEPT_REQUIRES_(ForwardIterable<Rng>() && EraseableIterable<Rng, I, I>() &&
                        InvokablePredicate<Fun, range_value_t<Rng>>())>
                void operator()(Rng & rng, Fun fun) const
                {
                    ranges::action::erase(rng, find_if_not(begin(rng), end(rng), std::move(fun)), end(rng));
                }
            };

            /// \ingroup group-actions
            /// \sa take_while_fn
            /// \sa action
            constexpr action<take_while_fn> take_while{};
        }
        /// @}
    }
}

#endif
