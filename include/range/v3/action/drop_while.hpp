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

#ifndef RANGES_V3_ACTION_DROP_WHILE_HPP
#define RANGES_V3_ACTION_DROP_WHILE_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            CPP_def
            (
                template(typename Rng, typename Fun)
                concept DropWhileActionConcept,
                    ForwardRange<Rng> &&
                    IndirectPredicate<Fun, iterator_t<Rng>> &&
                    ErasableRange<Rng &, iterator_t<Rng>, iterator_t<Rng>>
            );

            struct drop_while_fn
            {
            private:
                friend action_access;
                template<typename Fun>
                static auto CPP_fun(bind)(drop_while_fn drop_while, Fun fun)(
                    requires !Range<Fun>)
                {
                    return std::bind(drop_while, std::placeholders::_1, std::move(fun));
                }
            public:
                template<typename Rng, typename Fun>
                auto operator()(Rng &&rng, Fun fun) const -> CPP_ret(Rng)(
                    requires DropWhileActionConcept<Rng, Fun>)
                {
                    ranges::action::erase(rng, begin(rng), find_if_not(begin(rng), end(rng),
                        std::move(fun)));
                    return static_cast<Rng &&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Fun>
                auto operator()(Rng &&, Fun &&) const -> CPP_ret(void)(
                    requires not DropWhileActionConcept<Rng, Fun>)
                {
                    CPP_assert_msg(ForwardRange<Rng>,
                        "The object on which action::drop_while operates must be a model of the "
                        "ForwardRange concept.");
                    CPP_assert_msg(IndirectPredicate<Fun, iterator_t<Rng>>,
                        "The function passed to action::drop_while must be callable with objects "
                        "of the range's common reference type, and it must return something convertible to "
                        "bool.");
                    using I = iterator_t<Rng>;
                    CPP_assert_msg(ErasableRange<Rng &, I, I>,
                        "The object on which action::drop_while operates must allow element "
                        "removal.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates drop_while_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<drop_while_fn>, drop_while)
        }
        /// @}
    }
}

#endif
