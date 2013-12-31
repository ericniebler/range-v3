// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
#define RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP

#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // iterator_traits_impl
            template<typename T, typename Enable = void>
            struct iterator_traits_impl
            {};

            template<typename T>
            struct iterator_traits_impl<
                T,
                detail::always_t<
                    void,
                    typename T::iterator_category,
                    typename T::value_type,
                    typename T::difference_type,
                    typename T::reference,
                    typename T::pointer>>
            {
                using iterator_category = typename T::iterator_category;
                using value_type = typename T::value_type;
                using difference_type = typename T::difference_type;
                using reference = typename T::reference;
                using pointer = typename T::pointer;
            };

            template<typename T>
            struct iterator_traits_impl<T *>
            {
                using iterator_category = std::random_access_iterator_tag;
                using value_type = typename std::remove_const<T>::type;
                using difference_type = std::ptrdiff_t;
                using reference = T &;
                using pointer = T *;
            };
        }

        namespace concepts
        {
            struct Iterator
              : refines<CopyConstructible, CopyAssignable, Destructible>
            {
                // Users should specialize this to hook the traits mechanism.
                template<typename T>
                struct traits
                  : detail::iterator_traits_impl<T>
                {};

                // Associated types
                template<typename T>
                using category_t = typename traits<T>::iterator_category;

                template<typename T>
                using value_t = typename traits<T>::value_type;

                template<typename T>
                using difference_t = typename traits<T>::difference_type;

                template<typename T>
                using reference_t = typename traits<T>::reference;

                template<typename T>
                using pointer_t = typename traits<T>::pointer;

                // Valid expressions
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        *t,
                        concepts::has_type<T &>(++t)
                    ));
            };

            struct OutputIterator
              : refines<Iterator(_1)> // OutputIterator<T,U> refines Iterator<T>
            {
                template<typename T, typename O>
                auto requires(T && t, O && o) -> decltype(
                    concepts::valid_expr(
                        t++,
                        *t = o,
                        *t++ = o
                    ));
            };

            struct InputIterator
              : refines<Iterator, EqualityComparable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        t++,
                        concepts::has_common_type(*t, *t++)
                    ));
            };

            struct ForwardIterator
              : refines<InputIterator>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(*t, *t++)
                    ));
            };

            struct BidirectionalIterator
              : refines<ForwardIterator>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T &>(--t),
                        concepts::same_type(*t, *t--)
                    ));
            };

            struct RandomAccessIterator
              : refines<BidirectionalIterator>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<SignedIntegral>(t-t),
                        t = t + (t-t),
                        t = (t-t) + t,
                        t = t - (t-t),
                        t += (t-t),
                        t -= (t-t),
                        concepts::same_type(*t, t[t-t]),
                        concepts::model_of<Orderable>(t)
                    ));
            };
        }

        template<typename T>
        using Iterator = concepts::models<concepts::Iterator, T>;

        template<typename T, typename O>
        using OutputIterator = concepts::models<concepts::OutputIterator, T, O>;

        template<typename T>
        using InputIterator = concepts::models<concepts::InputIterator, T>;

        template<typename T>
        using ForwardIterator = concepts::models<concepts::ForwardIterator, T>;

        template<typename T>
        using BidirectionalIterator = concepts::models<concepts::BidirectionalIterator, T>;

        template<typename T>
        using RandomAccessIterator = concepts::models<concepts::RandomAccessIterator, T>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // iterator_concept
        template<typename T>
        using iterator_concept_t =
            concepts::most_refined_t<concepts::RandomAccessIterator, T>;

        template<typename T>
        struct iterator_concept
        {
            using type = iterator_concept_t<T>;
        };
    }
}

#endif // RANGES_V3_UTILITY_ITERATOR_CONCEPTS_HPP
