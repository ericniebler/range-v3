/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ACTION_REMOVE_HPP
#define RANGES_V3_ACTION_REMOVE_HPP

#include <meta/meta.hpp>
#include <range/v3/core.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/remove_if.hpp>


namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct remove_fn
            {
            private:
                friend action_access;
                template<typename V, typename P = ident, CONCEPT_REQUIRES_(!Range<V>())>
                static auto bind(remove_fn remove, const V& value, P proj = P{})
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(remove,
                        std::placeholders::_1,
                        std::cref(value),
                        protect(std::move(proj))
                    )
                )

                template<class Value>
                struct pred
                {
                    const Value& value;

                    template<typename T,
                        CONCEPT_REQUIRES_(EqualityComparable<const T&, const Value&>())>
                    bool operator()(const T& other_value) const
                    {
                        return other_value == value;
                    }
                };
            public:

                template<typename Rng, typename Value, typename P = ident,
                    CONCEPT_REQUIRES_(remove_if_fn::Concept<Rng, pred<Value>, P>())>
                Rng operator()(Rng&& rng, const Value& value, P proj = P{}) const
                {
                    return remove_if_fn{}(
                        static_cast<Rng&&>(rng),
                        pred<Value>{value},
                        std::move(proj)
                    );
                }
            };


            /// \ingroup group-actions
            /// \sa action
            /// \sa with_braced_init_args
            RANGES_INLINE_VARIABLE(action<remove_fn>, remove)
        }
        /// @}
    }
}

#endif // include guard
