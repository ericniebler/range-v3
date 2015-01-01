/// \file
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

#ifndef RANGES_V3_ACTION_STRIDE_HPP
#define RANGES_V3_ACTION_STRIDE_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
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
            struct stride_fn
            {
            private:
                friend action_access;
                template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
                static auto bind(stride_fn stride, D step)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(stride, std::placeholders::_1, step)
                )
            public:
                struct ConceptImpl
                {
                    template<typename Rng, typename T,
                        typename I = range_iterator_t<Rng>,
                        typename S = range_sentinel_t<Rng>,
                        typename D = range_difference_t<Rng>>
                    auto requires_(Rng rng, T) -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<concepts::ForwardIterable, Rng>(),
                            concepts::model_of<concepts::EraseableIterable, Rng, I, S>(),
                            concepts::model_of<concepts::Convertible, T, D>(),
                            concepts::is_true(Permutable<I>())
                        ));
                };

                template<typename Rng, typename T>
                using Concept = concepts::models<ConceptImpl, Rng, T>;

                template<typename Rng, typename D = range_difference_t<Rng>,
                    CONCEPT_REQUIRES_(Concept<Rng, D>())>
                Rng operator()(Rng && rng, range_difference_t<Rng> const step) const
                {
                    using I = range_iterator_t<Rng>;
                    using S = range_sentinel_t<Rng>;
                    RANGES_ASSERT(0 < step);
                    if(1 < step)
                    {
                        I begin = ranges::begin(rng);
                        S const end = ranges::end(rng);
                        if(begin != end)
                        {
                            for(I i = next_bounded(++begin, step-1, end); i != end;
                                advance_bounded(i, step, end), ++begin)
                            {
                                *begin = iter_move(i);
                            }
                        }
                        ranges::action::erase(rng, begin, end);
                    }
                    return std::forward<Rng>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!Concept<Rng, T>())>
                void operator()(Rng &&, T &&) const
                {
                    CONCEPT_ASSERT_MSG(ForwardIterable<Rng>(),
                        "The object on which action::stride operates must be a model of the "
                        "ForwardIterable concept.");
                    using I = range_iterator_t<Rng>;
                    using S = range_sentinel_t<Rng>;
                    CONCEPT_ASSERT_MSG(EraseableIterable<Rng, I, S>(),
                        "The object on which action::stride operates must allow element removal.");
                    CONCEPT_ASSERT_MSG(Convertible<T, range_difference_t<Rng>>(),
                        "The stride argument to action::stride must be convertible to the range's "
                        "difference type.");
                    CONCEPT_ASSERT_MSG(Permutable<I>(),
                        "The iterator type of the range passed to action::stride must allow its "
                        "elements to be permutaed; that is, the values must be movable and the "
                        "iterator must be mutable.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates stride_fn
            /// \sa action
            constexpr action<stride_fn> stride{};
        }
        /// @}
    }
}

#endif
