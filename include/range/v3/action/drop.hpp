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

#ifndef RANGES_V3_ACTION_DROP_HPP
#define RANGES_V3_ACTION_DROP_HPP

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
                concept DropActionConcept,
                    ForwardRange<Rng> &&
                    ErasableRange<Rng &, iterator_t<Rng>, iterator_t<Rng>> &&
                    ConvertibleTo<T, range_difference_t<Rng>>
            );

            struct drop_fn
            {
            private:
                friend action_access;
                template<typename Int>
                static auto CPP_fun(bind)(drop_fn drop, Int n)(
                    requires Integral<Int>)
                {
                    return std::bind(drop, std::placeholders::_1, n);
                }
            public:
                template<typename Rng>
                auto operator()(Rng &&rng, range_difference_t<Rng> n) const ->
                    CPP_ret(Rng)(
                    requires DropActionConcept<Rng, range_difference_t<Rng>>)
                {
                    RANGES_EXPECT(n >= 0);
                    ranges::action::erase(rng, begin(rng), ranges::next(begin(rng), n, end(rng)));
                    return static_cast<Rng &&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T>
                auto operator()(Rng &&, T &&) const -> CPP_ret(void)(
                    requires not DropActionConcept<Rng, T>)
                {
                    CPP_assert_msg(ForwardRange<Rng>,
                        "The object on which action::drop operates must be a model of the "
                        "ForwardRange concept.");
                    using I = iterator_t<Rng>;
                    CPP_assert_msg(ErasableRange<Rng &, I, I>,
                        "The object on which action::drop operates must allow element removal.");
                    CPP_assert_msg(ConvertibleTo<T, range_difference_t<Rng>>,
                        "The count passed to action::drop must be an integral type.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates drop_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<drop_fn>, drop)
        }
        /// @}
    }
}

#endif
