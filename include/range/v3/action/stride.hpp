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

#ifndef RANGES_V3_ACTION_STRIDE_HPP
#define RANGES_V3_ACTION_STRIDE_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
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
                concept StrideActionConcept,
                    ForwardRange<Rng> &&
                    ErasableRange<Rng &, iterator_t<Rng>, sentinel_t<Rng>> &&
                    ConvertibleTo<T, range_difference_t<Rng>> &&
                    Permutable<iterator_t<Rng>>
            );

            struct stride_fn
            {
            private:
                friend action_access;
                template<typename D>
                static auto CPP_fun(bind)(stride_fn stride, D step)(
                    requires Integral<D>)
                {
                    return std::bind(stride, std::placeholders::_1, step);
                }
            public:
                CPP_template(typename Rng, typename D = range_difference_t<Rng>)(
                    requires StrideActionConcept<Rng, D>)
                Rng operator()(Rng &&rng, range_difference_t<Rng> const step) const
                {
                    using I = iterator_t<Rng>;
                    using S = sentinel_t<Rng>;
                    RANGES_EXPECT(0 < step);
                    if(1 < step)
                    {
                        I begin = ranges::begin(rng);
                        S const end = ranges::end(rng);
                        if(begin != end)
                        {
                            for(I i = ranges::next(++begin, step-1, end); i != end;
                                advance(i, step, end), ++begin)
                            {
                                *begin = iter_move(i);
                            }
                        }
                        ranges::action::erase(rng, begin, end);
                    }
                    return static_cast<Rng &&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                CPP_template(typename Rng, typename T)(
                    requires not StrideActionConcept<Rng, T>)
                void operator()(Rng &&, T &&) const
                {
                    CPP_assert_msg(ForwardRange<Rng>,
                        "The object on which action::stride operates must be a model of the "
                        "ForwardRange concept.");
                    using I = iterator_t<Rng>;
                    using S = sentinel_t<Rng>;
                    CPP_assert_msg(ErasableRange<Rng &, I, S>,
                        "The object on which action::stride operates must allow element removal.");
                    CPP_assert_msg(ConvertibleTo<T, range_difference_t<Rng>>,
                        "The stride argument to action::stride must be convertible to the range's "
                        "difference type.");
                    CPP_assert_msg(Permutable<I>,
                        "The iterator type of the range passed to action::stride must allow its "
                        "elements to be permutaed; that is, the values must be movable and the "
                        "iterator must be mutable.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates stride_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<stride_fn>, stride)
        }
        /// @}
    }
}

#endif
