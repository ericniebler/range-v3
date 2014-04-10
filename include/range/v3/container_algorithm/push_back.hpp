//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_CONTAINER_ALGORITHM_PUSH_BACK_HPP
#define RANGES_V3_CONTAINER_ALGORITHM_PUSH_BACK_HPP

#include <range/v3/algorithm/copy.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Container, typename InputIterable>
            Container& push_back(Container& container, InputIterable&& rng)
            {
              ranges::copy(std::forward<InputIterable>(rng),
                           std::back_inserter(container));
              return container;
            }
        }

        struct push_backer : bindable<push_backer>
        {
            /// \brief template function push_backer::operator()
            ///
            /// range-based version of the \c push_back Boost.Range algorithm
            ///
            /// \pre \c Container is a model of the Container concept
            /// \pre \c InputIterable is a model of the InputIterable concept
            template<typename Container, typename InputIterable>
            static Container& invoke(push_backer, Container& container,
                                     InputIterable&& rng)
            {
              CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
              // CONCEPT_ASSERT(ranges::Container<Container>()); /// \todo
              return detail::push_back(container,
                                       std::forward<InputIterable>(rng));
            }

            /// \overload
            /// for container | push_back(rng)
            template<typename InputIterable>
            static auto invoke(push_backer push_back, InputIterable&& rng) ->
                decltype(push_back.move_bind(std::placeholders::_1,
                                             std::forward<InputIterable>(rng)))
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
                return push_back.move_bind(std::placeholders::_1,
                                           std::forward<InputIterable>(rng));
            }
        };

        RANGES_CONSTEXPR push_backer push_back {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
