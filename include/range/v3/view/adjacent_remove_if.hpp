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

#ifndef RANGES_V3_VIEW_ADJACENT_REMOVE_IF_HPP
#define RANGES_V3_VIEW_ADJACENT_REMOVE_IF_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
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
        template<typename Rng, typename F>
        struct adjacent_remove_if_view
          : view_adaptor<
                adjacent_remove_if_view<Rng, F>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            friend range_access;
            semiregular_t<function_type<F>> pred_;

            struct adaptor : adaptor_base
            {
            private:
                adjacent_remove_if_view const *rng_;
            public:
                adaptor() = default;
                adaptor(adjacent_remove_if_view const &rng)
                  : rng_(&rng)
                {}
                void next(range_iterator_t<Rng> &it) const
                {
                    auto const end = ranges::end(rng_->mutable_base());
                    RANGES_ASSERT(it != end);
                    it = adjacent_find(std::move(it), end, not_(std::ref(rng_->pred_)));
                    ranges::advance(it, 1, end);
                }
                void prev() = delete;
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
            adjacent_remove_if_view() = default;
            adjacent_remove_if_view(Rng rng, F pred)
              : view_adaptor_t<adjacent_remove_if_view>{std::move(rng)}
              , pred_(as_function(std::move(pred)))
            {}
        };

        namespace view
        {
            struct adjacent_remove_if_fn
            {
            private:
                friend view_access;
                template<typename F>
                static auto bind(adjacent_remove_if_fn adjacent_remove_if, F pred)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(adjacent_remove_if, std::placeholders::_1,
                        protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename F>
                using Concept = meta::and_<
                    ForwardRange<Rng>,
                    IndirectCallablePredicate<F, range_iterator_t<Rng>,
                        range_iterator_t<Rng>>>;

                template<typename Rng, typename F,
                    CONCEPT_REQUIRES_(Concept<Rng, F>())>
                adjacent_remove_if_view<all_t<Rng>, F> operator()(Rng && rng, F pred) const
                {
                    return {all(std::forward<Rng>(rng)), std::move(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename F,
                    CONCEPT_REQUIRES_(!Concept<Rng, F>())>
                void operator()(Rng &&, F) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "Rng must model the ForwardRange concept");
                    CONCEPT_ASSERT_MSG(IndirectCallablePredicate<F, range_iterator_t<Rng>,
                        range_iterator_t<Rng>>(),
                        "Function F must be callable with two arguments of the range's common "
                        "reference type, and it must return something convertible to bool.");
                }
            #endif
            };

            /// \relates adjacent_remove_if_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& adjacent_remove_if = static_const<view<adjacent_remove_if_fn>>::value;
            }
        }
        /// @}
    }
}

#endif
