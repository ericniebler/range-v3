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

#ifndef RANGES_V3_ACTION_SPLIT_HPP
#define RANGES_V3_ACTION_SPLIT_HPP

#include <vector>
#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct split_fn
            {
            private:
                template<typename Rng>
                using split_value_t =
                    meta::if_c<
                        (bool) ranges::Container<Rng>(),
                        uncvref_t<Rng>,
                        std::vector<range_value_type_t<Rng>>>;
            public:
                // BUGBUG something is not right with the actions. It should be possible
                // to move a container into a split and have elements moved into the result.
                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(view::split_fn::FunctionConcept<Rng, Fun>())>
                std::vector<split_value_t<Rng>> operator()(Rng && rng, Fun fun) const
                {
                    return view::split(rng, std::move(fun))
                         | view::transform(to_<split_value_t<Rng>>()) | to_vector;
                }
                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(view::split_fn::PredicateConcept<Rng, Fun>())>
                std::vector<split_value_t<Rng>> operator()(Rng && rng, Fun fun) const
                {
                    return view::split(rng, std::move(fun))
                         | view::transform(to_<split_value_t<Rng>>()) | to_vector;
                }
                template<typename Rng,
                    CONCEPT_REQUIRES_(view::split_fn::ElementConcept<Rng>())>
                std::vector<split_value_t<Rng>> operator()(Rng && rng, range_value_type_t<Rng> val) const
                {
                    return view::split(rng, std::move(val))
                         | view::transform(to_<split_value_t<Rng>>()) | to_vector;
                }
                template<typename Rng, typename Sub,
                    CONCEPT_REQUIRES_(view::split_fn::SubRangeConcept<Rng, Sub>())>
                std::vector<split_value_t<Rng>> operator()(Rng && rng, Sub && sub) const
                {
                    return view::split(rng, static_cast<Sub&&>(sub))
                         | view::transform(to_<split_value_t<Rng>>()) | to_vector;
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!ConvertibleTo<T, range_value_type_t<Rng>>())>
                void operator()(Rng &&, T &&) const volatile
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "The object on which action::split operates must be a model of the "
                        "ForwardRange concept.");
                    CONCEPT_ASSERT_MSG(ConvertibleTo<T, range_value_type_t<Rng>>(),
                        "The delimiter argument to action::split must be one of the following: "
                        "(1) A single element of the range's value type, where the value type is a "
                        "model of the Regular concept, "
                        "(2) A ForwardRange whose value type is EqualityComparable to the input "
                        "range's value type, "
                        "(3) A Predicate that is callable with one argument of the range's reference "
                        "type, or "
                        "(4) An Invocable that accepts two arguments, the range's iterator "
                        "and sentinel, and that returns a std::pair<bool, I> where I is the "
                        "input range's difference_type.");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates split_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<split_fn>, split)
        }
        /// @}
    }
}

#endif
