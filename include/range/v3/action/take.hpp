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

#ifndef RANGES_V3_ACTION_TAKE_HPP
#define RANGES_V3_ACTION_TAKE_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
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
                template(typename Rng, typename T)
                concept TakeActionConcept,
                    ForwardRange<Rng> &&
                    ErasableRange<Rng &, iterator_t<Rng>, sentinel_t<Rng>> &&
                    ConvertibleTo<T, range_difference_t<Rng>>
            );

            struct take_fn
            {
            private:
                friend action_access;
                template<typename Int>
                static auto CPP_fun(bind)(take_fn take, Int n)(
                    requires Integral<Int>)
                {
                    return std::bind(take, std::placeholders::_1, n);
                }
            public:
                CPP_template(typename Rng, typename D)(
                    requires TakeActionConcept<Rng, D>)
                Rng operator()(Rng &&rng, D &&d) const
                {
                    range_difference_t<Rng> n = d;
                    RANGES_EXPECT(n >= 0);
                    ranges::action::erase(rng, ranges::next(begin(rng), n, end(rng)), end(rng));
                    return static_cast<Rng &&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                CPP_template(typename Rng, typename T)(
                    requires not TakeActionConcept<Rng, T>)
                void operator()(Rng &&, T &&) const
                {
                    CPP_assert_msg(ForwardRange<Rng>,
                        "The object on which action::take operates must be a model of the "
                        "ForwardRange concept.");
                    using I = iterator_t<Rng>;
                    using S = sentinel_t<Rng>;
                    CPP_assert_msg(ErasableRange<Rng &, I, S>,
                        "The object on which action::take operates must allow element removal.");
                    CPP_assert_msg(ConvertibleTo<T, range_difference_t<Rng>>,
                        "The stride argument to action::take must be convertible to the range's "
                        "difference type.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates take_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<take_fn>, take)
        }
        /// @}
    }
}

#endif
