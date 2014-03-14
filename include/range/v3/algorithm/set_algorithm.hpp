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
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/copy.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator1, typename Sentenel1,
                     typename InputIterator2, typename Sentenel2,
                     typename BinaryPredicate = ranges::less>
            bool
            includes(InputIterator1 begin1, Sentenel1 end1,
                     InputIterator2 begin2, Sentenel2 end2,
                     BinaryPredicate pred = BinaryPredicate{})
            {
                for(; begin2 != end2; ++begin1)
                {
                    if(begin1 == end1 || pred(*begin2, *begin1))
                        return false;
                    if(!pred(*begin1, *begin2))
                        ++begin2;
                }
                return true;
            }

            template<typename InputIterator1, typename Sentenel1,
                     typename InputIterator2, typename Sentenel2,
                     typename OutputIterator, typename BinaryPredicate>
            OutputIterator
            set_union(InputIterator1 begin1, Sentenel1 end1,
                      InputIterator2 begin2, Sentenel2 end2,
                      OutputIterator out, BinaryPredicate pred)
            {
                for(; begin1 != end1; ++out)
                {
                    if(begin2 == end2)
                        return detail::copy(std::move(begin1), std::move(end1), std::move(out));
                    if(pred(*begin2, *begin1))
                    {
                        *out = *begin2;
                        ++begin2;
                    }
                    else
                    {
                        *out = *begin1;
                        if(!pred(*begin1, *begin2))
                            ++begin2;
                        ++begin1;
                    }
                }
                return detail::copy(begin2, end2, out);
            }

            template<typename InputIterator1, typename Sentenel1,
                     typename InputIterator2, typename Sentenel2,
                     typename OutputIterator, typename BinaryPredicate>
            OutputIterator
            set_intersection(InputIterator1 begin1, Sentenel1 end1,
                             InputIterator2 begin2, Sentenel2 end2,
                             OutputIterator out, BinaryPredicate pred)
            {
                while(begin1 != end1 && begin2 != end2)
                {
                    if(pred(*begin1, *begin2))
                        ++begin1;
                    else
                    {
                        if(!pred(*begin2, *begin1))
                        {
                            *out = *begin1;
                            ++out;
                            ++begin1;
                        }
                        ++begin2;
                    }
                }
                return out;
            }

            template<typename InputIterator1, typename Sentenel1,
                     typename InputIterator2, typename Sentenel2,
                     typename OutputIterator, typename BinaryPredicate>
            OutputIterator
            set_difference(InputIterator1 begin1, Sentenel1 end1,
                           InputIterator2 begin2, Sentenel2 end2,
                           OutputIterator out, BinaryPredicate pred)
            {
                while(begin1 != end1)
                {
                    if(begin2 == end2)
                        return detail::copy(std::move(begin1), std::move(end1), std::move(out));
                    if(pred(*begin1, *begin2))
                    {
                        *out = *begin1;
                        ++out;
                        ++begin1;
                    }
                    else
                    {
                        if(!pred(*begin2, *begin1))
                            ++begin1;
                        ++begin2;
                    }
                }
                return out;
            }

            template<typename InputIterator1, typename Sentenel1,
                     typename InputIterator2, typename Sentenel2,
                     typename OutputIterator, typename BinaryPredicate>
            OutputIterator
            set_symmetric_difference(InputIterator1 begin1, Sentenel1 end1,
                                     InputIterator2 begin2, Sentenel2 end2,
                                     OutputIterator out, BinaryPredicate pred)
            {
                while(begin1 != end1)
                {
                    if(begin2 == end2)
                        return detail::copy(std::move(begin1), std::move(end1), std::move(out));
                    if(pred(*begin1, *begin2))
                    {
                        *out = *begin1;
                        ++out;
                        ++begin1;
                    }
                    else
                    {
                        if(pred(*begin2, *begin1))
                        {
                            *out = *begin2;
                            ++out;
                        }
                        else
                            ++begin1;
                        ++begin2;
                    }
                }
                return detail::copy(std::move(begin2), std::move(end2), std::move(out));
            }
        }

        struct includer : bindable<includer>
        {
            /// \brief template function includes
            ///
            /// range-based version of the includes std algorithm
            ///
            /// \pre InputIterable1 is a model of the InputIterable concept
            /// \pre InputIterable2 is a model of the InputIterable concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputIterable1, typename InputIterable2,
                typename BinaryPredicate = ranges::less>
            static bool invoke(includer, InputIterable1 const & rng1, InputIterable2 const & rng2,
                BinaryPredicate pred = BinaryPredicate{})
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputIterable1 const>,
                                                       range_reference_t<InputIterable2 const>>());
                return detail::includes(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2),
                    ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR includer includes{};

        struct set_unioner : bindable<set_unioner>
        {
            /// \brief template function set_union
            ///
            /// range-based version of the set_union std algorithm
            ///
            /// \pre InputIterable1 is a model of the InputIterable concept
            /// \pre InputIterable2 is a model of the InputIterable concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputIterable1, typename InputIterable2, typename OutputIterator,
                     typename BinaryPredicate = ranges::less>
            static OutputIterator invoke(set_unioner, InputIterable1 const & rng1,
                InputIterable2 const & rng2, OutputIterator out,
                BinaryPredicate pred = BinaryPredicate{})
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputIterable1 const>,
                                                       range_reference_t<InputIterable2 const>>());
                return detail::set_union(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), std::move(out),
                    ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR set_unioner set_union{};

        struct set_intersecter : bindable<set_intersecter>
        {
            /// \brief template function set_union
            ///
            /// range-based version of the set_union std algorithm
            ///
            /// \pre InputIterable1 is a model of the InputIterable concept
            /// \pre InputIterable2 is a model of the InputIterable concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputIterable1, typename InputIterable2, typename OutputIterator,
                typename BinaryPredicate = ranges::less>
            static OutputIterator invoke(set_intersecter, InputIterable1 const & rng1,
                InputIterable2 const & rng2, OutputIterator out,
                BinaryPredicate pred = BinaryPredicate{})
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputIterable1 const>,
                                                       range_reference_t<InputIterable2 const>>());
                return detail::set_intersection(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), std::move(out),
                    ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR set_intersecter set_intersection{};

        struct set_differencer : bindable<set_differencer>
        {
            /// \brief template function set_union
            ///
            /// range-based version of the set_union std algorithm
            ///
            /// \pre InputIterable1 is a model of the InputIterable concept
            /// \pre InputIterable2 is a model of the InputIterable concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputIterable1, typename InputIterable2, typename OutputIterator,
                typename BinaryPredicate = ranges::less>
            static OutputIterator invoke(set_differencer, InputIterable1 const & rng1,
                InputIterable2 const & rng2, OutputIterator out,
                BinaryPredicate pred = BinaryPredicate{})
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputIterable1 const>,
                                                       range_reference_t<InputIterable2 const>>());
                return detail::set_difference(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), std::move(out),
                    ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR set_differencer set_difference{};

        struct set_symmetric_differencer : bindable<set_symmetric_differencer>
        {
            /// \brief template function set_symmetric_union
            ///
            /// range-based version of the set_symmetric_union std algorithm
            ///
            /// \pre InputIterable1 is a model of the InputIterable concept
            /// \pre InputIterable2 is a model of the InputIterable concept
            /// \pre BinaryPredicate is a model of the BinaryPredicate concept
            template<typename InputIterable1, typename InputIterable2, typename OutputIterator,
                typename BinaryPredicate = ranges::less>
            static OutputIterator invoke(set_symmetric_differencer, InputIterable1 const & rng1,
                InputIterable2 const & rng2, OutputIterator out,
                BinaryPredicate pred = BinaryPredicate{})
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable1 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::Iterable<InputIterable2 const>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable1 const>>());
                CONCEPT_ASSERT(ranges::OutputIterator<OutputIterator,
                                                      range_reference_t<InputIterable2 const>>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<InputIterable1 const>,
                                                       range_reference_t<InputIterable2 const>>());
                return detail::set_symmetric_difference(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2), std::move(out),
                    ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR set_symmetric_differencer set_symmetric_difference{};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
