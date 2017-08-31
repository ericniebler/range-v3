/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_FILTER_HPP
#define RANGES_V3_VIEW_FILTER_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/experimental/utility/async_generator.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct filter_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(filter_fn filter, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(filter, std::placeholders::_1, protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename Pred>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    IndirectPredicate<Pred, iterator_t<Rng>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred>())>
                remove_if_view<all_t<Rng>, logical_negate<Pred>>
                operator()(Rng && rng, Pred pred) const
                {
                    return {all(static_cast<Rng&&>(rng)), not_fn(std::move(pred))};
                }

                // Asynchronously filter an async range:
                template<typename Rng, typename Pred>
                using AsyncConcept = meta::and_<
                    AsyncView<Rng>,
                    IndirectPredicate<Pred, co_iterator_t<Rng>>>;

            #if RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(AsyncConcept<Rng, Pred>())>
                experimental::async_generator<reference_t<co_iterator_t<Rng>>>
                operator()(Rng rng, Pred pred) const
                {
                    using reference_t = reference_t<co_iterator_t<Rng>>;
                    for co_await(reference_t e : rng)
                    {
                        if(pred(e))
                            co_yield static_cast<reference_t &&>(e);
                    }
                }
            #endif

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred>() && !AsyncConcept<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>() || AsyncView<Rng>(),
                        "The first argument to view::filter must be a model of either the "
                        "InputRange or the AsyncView concept.");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, xsync_iterator_t<Rng>>(),
                        "The second argument to view::filter must be callable with "
                        "a value of the range, and the return type must be convertible "
                        "to bool.");
                }
            #endif
            };

            /// \relates filter_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<filter_fn>, filter)
        }
        /// @}
    }
}

#endif
