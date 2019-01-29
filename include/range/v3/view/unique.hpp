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

#ifndef RANGES_V3_VIEW_UNIQUE_HPP
#define RANGES_V3_VIEW_UNIQUE_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adjacent_filter.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct unique_fn
            {
            private:
                friend view_access;
                template<typename C,
                         CONCEPT_REQUIRES_(!Range<C>())>
                static auto bind(unique_fn unique, C pred)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(unique, std::placeholders::_1, protect(std::move(pred)))
                )

            public:
                template<typename Rng, typename C = equal_to>
                using Concept = meta::and_<
                    ForwardRange<Rng>,
                    IndirectRelation<C, iterator_t<Rng>>>;

                template<typename Rng, typename C = equal_to,
                         CONCEPT_REQUIRES_(Concept<Rng, C>())>
                auto operator()(Rng && rng, C pred = {}) const ->
                adjacent_filter_view<all_t<Rng>, logical_negate<C>>
                {
                    return {all(static_cast<Rng &&>(rng)), not_fn(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename C = equal_to,
                         CONCEPT_REQUIRES_(!Concept<Rng, C>())>
                void operator()(Rng &&, C = {}) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "The object on which view::unique operates must be a model the "
                        "ForwardRange concept.");
                    CONCEPT_ASSERT_MSG(IndirectRelation<C, iterator_t<Rng>>(),
                        "The value type of the range passed to view::unique must be "
                        "EqualityComparable or provide a function that can be callable with two arguments "
                        "of the range's common reference type, and the return type must be "
                        "convertible to bool.");
                }
            #endif
            };

            /// \relates unique_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<unique_fn>, unique)
        }
        /// @}
    }
}

#endif
