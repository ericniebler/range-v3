// Boost.Range library
//
//  Copyright Eric Niebler 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

// Don't include this header directly
#ifndef RANGES_V3_RANGE_CONCEPTS_HPP
#error This file should only be included from range_concepts.hpp
#endif

        namespace detail
        {
            // Not a real concept!
            struct HasSizeConcept
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<std::size_t>(t.size())
                    ));
            };

            template<typename T>
            using HasSize = concepts::models<HasSizeConcept, T>;
        }

        // Iterables with a .size() member function, like std::list
        template<typename Iterable,
            typename Size = meta_apply<std::make_unsigned, concepts::ConvertibleToIterable::difference_t<Iterable> >,
            CONCEPT_REQUIRES_(ranges::ConvertibleToIterable<Iterable>() && detail::HasSize<Iterable>())>
        Size
        range_size(Iterable &&rng)
        {
            return static_cast<Size>(rng.size());
        }

        // Random-access ranges that don't have a .size() member function
        template<typename RandomAccessRange,
            typename Size = meta_apply<std::make_unsigned, concepts::ConvertibleToIterable::difference_t<RandomAccessRange>>,
            CONCEPT_REQUIRES_(!detail::HasSize<RandomAccessRange>() &&
                              ranges::ConvertibleToRange<RandomAccessRange>() &&
                              ranges::RandomAccessIterator<concepts::ConvertibleToIterable::iterator_t<RandomAccessRange>>())>
        Size
        range_size(RandomAccessRange &&rng)
        {
            return static_cast<Size>(ranges::end(rng) - ranges::begin(rng));
        }

        // Built-in arrays
        template<typename T, std::size_t N>
        constexpr std::size_t
        range_size(T const (&)[N])
        {
            return N;
        }
