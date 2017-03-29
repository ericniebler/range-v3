/// \file
// Range v3 library
//
//  Copyright Filip Matzner 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_SHUFFLE_HPP
#define RANGES_V3_ACTION_SHUFFLE_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/algorithm/shuffle.hpp>
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
            struct shuffle_fn
            {
            private:
                friend action_access;
                template<typename Gen,
                    CONCEPT_REQUIRES_(UniformRandomNumberGenerator<Gen>())>
                static auto bind(shuffle_fn shuffle, Gen && gen)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(shuffle, std::placeholders::_1, bind_forward<Gen>(gen))
                )
            public:
                struct ConceptImpl
                {
                    template<typename Rng, typename Gen,
                        typename I = iterator_t<Rng>>
                    auto requires_() -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<concepts::RandomAccessRange, Rng>(),
                            concepts::is_true(Permutable<I>()),
                            concepts::is_true(UniformRandomNumberGenerator<Gen>()),
                            concepts::is_true(ConvertibleTo<
                                concepts::UniformRandomNumberGenerator::result_t<Gen>,
                                difference_type_t<I>>())
                        ));
                };

                template<typename Rng, typename Gen>
                using Concept = concepts::models<ConceptImpl, Rng, Gen>;

                template<typename Rng, typename Gen,
                    CONCEPT_REQUIRES_(Concept<Rng, Gen>())>
                Rng operator()(Rng && rng, Gen && gen) const
                {
                    ranges::shuffle(rng, static_cast<Gen&&>(gen));
                    return static_cast<Rng&&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Gen,
                    CONCEPT_REQUIRES_(!Concept<Rng, Gen>())>
                void operator()(Rng &&, Gen &&) const
                {
                    CONCEPT_ASSERT_MSG(RandomAccessRange<Rng>(),
                        "The object on which action::shuffle operates must be a model of the "
                        "RandomAccessRange concept.");
                    using I = iterator_t<Rng>;
                    CONCEPT_ASSERT_MSG(Permutable<I>(),
                        "The iterator type of the range passed to action::shuffle must allow its "
                        "elements to be permuted; that is, the values must be movable and the "
                        "iterator must be mutable.");
                    CONCEPT_ASSERT_MSG(UniformRandomNumberGenerator<Gen>(),
                        "The generator passed to action::shuffle must fulfill the "
                        "UniformRandomNumberGenerator concept.");
                    CONCEPT_ASSERT_MSG(ConvertibleTo<
                        concepts::UniformRandomNumberGenerator::result_t<Gen>,
                        difference_type_t<I>>(),
                        "The random generator passed to action::shuffle has to have a return type "
                        "convertible to the container iterator difference type.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates shuffle_fn
            /// \sa `action`
            RANGES_INLINE_VARIABLE(action<shuffle_fn>, shuffle)
        }
        /// @}
    }
}

#endif
