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

#ifndef RANGES_V3_VIEW_ADJACENT_FILTER_HPP
#define RANGES_V3_VIEW_ADJACENT_FILTER_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct adjacent_filter_view
          : view_adaptor<
                adjacent_filter_view<Rng, Pred>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            friend range_access;
            semiregular_t<function_type<Pred>> pred_;

            struct adaptor : adaptor_base
            {
            private:
                adjacent_filter_view const *rng_;
            public:
                adaptor() = default;
                adaptor(adjacent_filter_view const &rng)
                  : rng_(&rng)
                {}
                void next(range_iterator_t<Rng> &it) const
                {
                    auto const end = ranges::end(rng_->mutable_base());
                    auto &&pred = rng_->pred_;
                    RANGES_ASSERT(it != end);
                    for(auto prev = it; ++it != end; prev = it)
                        if(pred(*prev, *it))
                            break;
                }
                void prev() = delete;
                void distance_to() = delete;
            };
            adaptor begin_adaptor() const
            {
                return {*this};
            }
            adaptor end_adaptor() const
            {
                return {*this};
            }
        public:
            adjacent_filter_view() = default;
            adjacent_filter_view(Rng rng, Pred pred)
              : adjacent_filter_view::view_adaptor{std::move(rng)}
              , pred_(as_function(std::move(pred)))
            {}
       };

        namespace view
        {
            struct adjacent_filter_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(adjacent_filter_fn adjacent_filter, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(adjacent_filter, std::placeholders::_1,
                        protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename Pred>
                using Concept = meta::and_<
                    ForwardRange<Rng>,
                    IndirectCallablePredicate<Pred, range_iterator_t<Rng>,
                        range_iterator_t<Rng>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred>())>
                adjacent_filter_view<all_t<Rng>, Pred> operator()(Rng && rng, Pred pred) const
                {
                    return {all(std::forward<Rng>(rng)), std::move(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "Rng must model the ForwardRange concept");
                    CONCEPT_ASSERT_MSG(IndirectCallablePredicate<Pred, range_iterator_t<Rng>,
                        range_iterator_t<Rng>>(),
                        "Function Pred must be callable with two arguments of the range's common "
                        "reference type, and it must return something convertible to bool.");
                }
            #endif
            };

            /// \relates adjacent_filter_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<adjacent_filter_fn>, adjacent_filter)
        }
        /// @}
    }
}

#endif
