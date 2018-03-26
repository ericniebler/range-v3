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
            struct take_fn
            {
            private:
                friend action_access;
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(take_fn take, Int n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(take, std::placeholders::_1, n)
                )
            public:
                struct ConceptImpl
                {
                    template<typename Rng, typename T,
                        typename I = iterator_t<Rng>,
                        typename S = sentinel_t<Rng>,
                        typename D = range_difference_type_t<Rng>>
                    auto requires_() -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<concepts::ForwardRange, Rng>(),
                            concepts::model_of<concepts::ErasableRange, Rng, I, S>(),
                            concepts::model_of<concepts::ConvertibleTo, T, D>()
                        ));
                };

                template<typename Rng, typename T>
                using Concept = concepts::models<ConceptImpl, Rng, T>;

                template<typename Rng, typename D = range_difference_type_t<Rng>,
                    CONCEPT_REQUIRES_(Concept<Rng, D>())>
                Rng operator()(Rng && rng, range_difference_type_t<Rng> n) const
                {
                    RANGES_EXPECT(n >= 0);
                    ranges::action::erase(rng, ranges::next(begin(rng), n, end(rng)), end(rng));
                    return static_cast<Rng&&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!Concept<Rng, T>())>
                void operator()(Rng &&, T &&) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "The object on which action::take operates must be a model of the "
                        "ForwardRange concept.");
                    using I = iterator_t<Rng>;
                    using S = sentinel_t<Rng>;
                    CONCEPT_ASSERT_MSG(ErasableRange<Rng, I, S>(),
                        "The object on which action::take operates must allow element removal.");
                    CONCEPT_ASSERT_MSG(ConvertibleTo<T, range_difference_type_t<Rng>>(),
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
