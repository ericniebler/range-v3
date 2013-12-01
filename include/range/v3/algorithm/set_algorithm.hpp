//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_SET_ALGORITHM_HPP
#define RANGES_V3_ALGORITHM_SET_ALGORITHM_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct includer : bindable<includer>
        {
            /// \brief template function includes
            ///
            /// range-based version of the includes std algorithm
            ///
            /// \pre InputRange1 is a model of the InputRange concept
            /// \pre InputRange2 is a model of the InputRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename InputRange2>
            static bool invoke(includer, InputRange1 const & rng1, InputRange2 const & rng2)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::includes(ranges::begin(rng1), ranges::end(rng1),
                                     ranges::begin(rng2), ranges::end(rng2));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename BinaryPredicate>
            static bool invoke(includer, InputRange1 const & rng1, InputRange2 const & rng2,
                               BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::includes(ranges::begin(rng1), ranges::end(rng1),
                                     ranges::begin(rng2), ranges::end(rng2), detail::move(pred));
            }
        };

        RANGES_CONSTEXPR includer includes{};

        struct set_unioner : bindable<set_unioner>
        {
            /// \brief template function set_union
            ///
            /// range-based version of the set_union std algorithm
            ///
            /// \pre InputRange1 is a model of the InputRange concept
            /// \pre InputRange2 is a model of the InputRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename InputRange2, typename OutputIterator>
            static OutputIterator invoke(set_unioner, InputRange1 const & rng1,
                InputRange2 const & rng2, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::set_union(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), detail::move(out));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename OutputIterator,
                     typename BinaryPredicate>
            static OutputIterator invoke(set_unioner, InputRange1 const & rng1,
                InputRange2 const & rng2, OutputIterator out, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::set_union(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), detail::move(out), detail::move(pred));
            }
        };

        RANGES_CONSTEXPR set_unioner set_union{};

        struct set_intersecter : bindable<set_intersecter>
        {
            /// \brief template function set_union
            ///
            /// range-based version of the set_union std algorithm
            ///
            /// \pre InputRange1 is a model of the InputRange concept
            /// \pre InputRange2 is a model of the InputRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename InputRange2, typename OutputIterator>
            static OutputIterator invoke(set_intersecter, InputRange1 const & rng1,
                InputRange2 const & rng2, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::set_intersection(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), detail::move(out));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename OutputIterator,
                typename BinaryPredicate>
            static OutputIterator invoke(set_intersecter, InputRange1 const & rng1,
                InputRange2 const & rng2, OutputIterator out, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::set_intersection(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), detail::move(out), detail::move(pred));
            }
        };

        RANGES_CONSTEXPR set_intersecter set_intersection{};

        struct set_differencer : bindable<set_differencer>
        {
            /// \brief template function set_union
            ///
            /// range-based version of the set_union std algorithm
            ///
            /// \pre InputRange1 is a model of the InputRange concept
            /// \pre InputRange2 is a model of the InputRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename InputRange2, typename OutputIterator>
            static OutputIterator invoke(set_differencer, InputRange1 const & rng1,
                InputRange2 const & rng2, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::set_difference(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), detail::move(out));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename OutputIterator,
                typename BinaryPredicate>
            static OutputIterator invoke(set_differencer, InputRange1 const & rng1,
                InputRange2 const & rng2, OutputIterator out, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::set_difference(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), detail::move(out), detail::move(pred));
            }
        };

        RANGES_CONSTEXPR set_differencer set_difference{};

        struct set_symmetric_differencer : bindable<set_symmetric_differencer>
        {
            /// \brief template function set_symmetric_union
            ///
            /// range-based version of the set_symmetric_union std algorithm
            ///
            /// \pre InputRange1 is a model of the InputRange concept
            /// \pre InputRange2 is a model of the InputRange concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputRange1, typename InputRange2, typename OutputIterator>
            static OutputIterator invoke(set_symmetric_differencer, InputRange1 const & rng1,
                InputRange2 const & rng2, OutputIterator out)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::set_symmetric_difference(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), detail::move(out));
            }

            /// \overload
            template<typename InputRange1, typename InputRange2, typename OutputIterator,
                typename BinaryPredicate>
            static OutputIterator invoke(set_symmetric_differencer, InputRange1 const & rng1,
                InputRange2 const & rng2, OutputIterator out, BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::InputRange<InputRange1 const>());
                CONCEPT_ASSERT(ranges::InputRange<InputRange2 const>());
                return std::set_symmetric_difference(ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), detail::move(out), detail::move(pred));
            }
        };

        RANGES_CONSTEXPR set_symmetric_differencer set_symmetric_difference{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
