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
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_HPP

#include <utility>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator, typename Sentinel, typename UnaryFunction>
            UnaryFunction for_each(InputIterator begin, Sentinel end, UnaryFunction fun)
            {
                for(; begin != end; ++begin)
                    fun(*begin);
                return fun;
            }
        }

        struct for_eacher : bindable<for_eacher>
        {
            /// \brief template function \c for_eacher::operator()
            ///
            /// range-based version of the \c for_each std algorithm
            ///
            /// \pre \c InputIterable is a model of the InputIterable concept
            /// \pre \c UnaryFunction is a model of the UnaryFunction concept
            template<typename InputIterable, typename UnaryFunction>
            static UnaryFunction invoke(for_eacher, InputIterable && rng, UnaryFunction fun)
            {
                CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
                CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                CONCEPT_ASSERT(ranges::Callable<invokable_t<UnaryFunction>,
                                                range_reference_t<InputIterable>>());
                return detail::for_each(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(fun)));
            }

            /// \overload
            /// for rng | for_each(fun)
            template<typename UnaryFunction>
            static auto invoke(for_eacher for_each, UnaryFunction fun) ->
                decltype(for_each.move_bind(std::placeholders::_1, std::move(fun)))
            {
                return for_each.move_bind(std::placeholders::_1, std::move(fun));
            }
        };

        RANGES_CONSTEXPR for_eacher for_each {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
