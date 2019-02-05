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

#ifndef RANGES_V3_VIEW_REMOVE_HPP
#define RANGES_V3_VIEW_REMOVE_HPP

#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/utility/invoke.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct remove_fn
            {
            private:
                friend view_access;

                template<typename Value, typename Proj>
                static auto bind(remove_fn remove, Value value, Proj proj)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    make_pipeable(
                        std::bind(remove,
                            std::placeholders::_1,
                            std::move(value),
                            protect(std::move(proj))
                        )
                    )
                )

                template<typename Value>
                static auto bind(remove_fn remove, Value value)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    make_pipeable(
                        std::bind(remove,
                            std::placeholders::_1, std::move(value)
                        )
                    )
                )

                template<class Value>
                struct pred
                {
                    Value value;

                    template<typename T,
                        CONCEPT_REQUIRES_(EqualityComparable<T, const Value&>())>
                    bool operator()(T&& other_value) const
                    {
                        return static_cast<T&&>(other_value) == value;
                    }
                };

            public:
                template<typename Rng, typename Value, typename Proj>
                using Constraint = meta::and_<
                    MoveConstructible<Value>,
                    remove_if_fn::Constraint<Rng, pred<Value>, Proj>>;

                template<typename Rng, typename Value, typename Proj = ident,
                    CONCEPT_REQUIRES_(Constraint<Rng, Value, Proj>())>
                RANGES_CXX14_CONSTEXPR
                auto operator()(Rng&& rng, Value value, Proj proj = Proj{}) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    remove_if(all(static_cast<Rng&&>(rng)),
                        pred<Value>{std::move(value)},
                        std::move(proj))
                )
            };

            /// \relates remove_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<remove_fn>, remove)
        }
        /// @}
    }
}

#endif //RANGES_V3_VIEW_REMOVE_HPP
