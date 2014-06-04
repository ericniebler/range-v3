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
#ifndef RANGES_V3_ALGORITHM_PREDICATE_HPP
#define RANGES_V3_ALGORITHM_PREDICATE_HPP

#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Predicate adaptor for natural predicates.
        template<typename P>
        struct predicate 
        {
            explicit predicate(const P& p) : pred(p) { }
            explicit predicate(P&& p) : pred(std::move(p)) { }

            template<typename ...Args>
            bool operator()(Args&&... args) const
            {
                return pred(std::forward<Args>(args)...);
            }

            P pred;
        };

        // Predicate adaptor for member pointers.
        template<typename C, typename T>
        struct predicate<T C::*> {
            explicit predicate(T C::* p) : ptr(p) { }

            template<typename ...Args>
            bool operator()(C& c, Args... args) const
            {
                return (c.*ptr)(std::forward<Args>(args)...);
            }

            template<typename ...Args>
            bool operator()(const C& c, Args&&... args) const
            {
                return (c.*ptr)(std::forward<Args>(args)...);
            }

            T C::* ptr;
        };


        namespace concepts
        {
            struct AdaptablePredicate
            {
                template<typename P, typename... Args>
                auto requires_(P pred, Args&&... args) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate>(
                            predicate<P>(pred), std::forward<Args>(args)...)
                    ));
            };
        } // namespace concepts

        template<typename P, typename ...Args>
        using AdaptablePredicate = concepts::models<concepts::AdaptablePredicate, P, Args...>;
 
    } // inline namespace v3

} // namespace ranges

#endif // include guard
