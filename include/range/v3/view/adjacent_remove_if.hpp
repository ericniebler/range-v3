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
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct adjacent_remove_if_view
          : view_adaptor<
                adjacent_remove_if_view<Rng, Pred>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            friend range_access;
            semiregular_t<Pred> pred_;
            detail::non_propagating_cache<iterator_t<Rng>> begin_;

            struct adaptor : adaptor_base
            {
            private:
                adjacent_remove_if_view *rng_;
                void satisfy(iterator_t<Rng> &it) const
                {
                    auto const end = ranges::end(rng_->mutable_base());
                    auto &pred = rng_->pred_;
                    if(it == end)
                        return;
                    auto next = it;
                    for(; ++next != end; it = next)
                        if(!invoke(pred, *it, *next))
                            return;
                }
            public:
                adaptor() = default;
                adaptor(adjacent_remove_if_view &rng)
                  : rng_(&rng)
                {}
                iterator_t<Rng> begin(adjacent_remove_if_view &) const
                {
                    auto &beg = rng_->begin_;
                    if(!beg)
                    {
                        beg = ranges::begin(rng_->mutable_base());
                        this->satisfy(*beg);
                    }
                    return *beg;
                }
                void next(iterator_t<Rng> &it) const
                {
                    RANGES_ASSERT(it != ranges::end(rng_->mutable_base()));
                    this->satisfy(++it);
                }
                void prev() = delete;
                void distance_to() = delete;
            };
            adaptor begin_adaptor()
            {
                return {*this};
            }
            adaptor end_adaptor()
            {
                return {*this};
            }
        public:
            adjacent_remove_if_view() = default;
            adjacent_remove_if_view(Rng rng, Pred pred)
              : adjacent_remove_if_view::view_adaptor{std::move(rng)}
              , pred_(std::move(pred))
            {}
       };

        namespace view
        {
            struct adjacent_remove_if_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(adjacent_remove_if_fn adjacent_remove_if, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(adjacent_remove_if, std::placeholders::_1,
                        protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename Pred>
                using Concept = meta::and_<
                    ForwardRange<Rng>,
                    IndirectPredicate<Pred, iterator_t<Rng>,
                        iterator_t<Rng>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred>())>
                adjacent_remove_if_view<all_t<Rng>, Pred> operator()(Rng && rng, Pred pred) const
                {
                    return {all(static_cast<Rng&&>(rng)), std::move(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "Rng must model the ForwardRange concept");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, iterator_t<Rng>,
                        iterator_t<Rng>>(),
                        "Pred must be callable with two arguments of the range's common "
                        "reference type, and it must return something convertible to bool.");
                }
            #endif
            };

            /// \relates adjacent_remove_if_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<adjacent_remove_if_fn>, adjacent_remove_if)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::adjacent_remove_if_view)

#endif
