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

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/find_if.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng, typename Pred>
        struct filtered_view
          : range_adaptor<filtered_view<Rng, Pred>, Rng>
        {
        private:
            friend range_access;
            semiregular_invokable_t<Pred> pred_;

            template<bool IsConst>
            struct adaptor
              : adaptor_base
            {
            private:
                using filtered_view_t = meta::apply<meta::add_const_if_c<IsConst>, filtered_view>;
                filtered_view_t *rng_;
                using adaptor_base::advance;
                void satisfy(range_iterator_t<Rng> &it) const
                {
                    it = find_if(std::move(it), ranges::end(rng_->mutable_base()), std::ref(rng_->pred_));
                }
            public:
                adaptor() = default;
                adaptor(filtered_view_t &rng)
                  : rng_(&rng)
                {}
                range_iterator_t<Rng> begin(filtered_view_t &rng) const
                {
                    auto it = ranges::begin(rng.mutable_base());
                    this->satisfy(it);
                    return it;
                }
                void next(range_iterator_t<Rng> &it) const
                {
                    this->satisfy(++it);
                }
                CONCEPT_REQUIRES(BidirectionalIterable<Rng>())
                void prev(range_iterator_t<Rng> &it) const
                {
                    auto &&pred = rng_->pred_;
                    do --it; while(!pred(*it));
                }
            };
            CONCEPT_REQUIRES(!Invokable<Pred const, range_value_t<Rng>>())
            adaptor<false> begin_adaptor()
            {
                return {*this};
            }
            CONCEPT_REQUIRES(Invokable<Pred const, range_value_t<Rng>>())
            adaptor<true> begin_adaptor() const
            {
                return {*this};
            }
            // TODO: if end is a sentinel, it holds an unnecessary pointer back to
            // this range.
            CONCEPT_REQUIRES(!Invokable<Pred const, range_value_t<Rng>>())
            adaptor<false> end_adaptor()
            {
                return {*this};
            }
            CONCEPT_REQUIRES(Invokable<Pred const, range_value_t<Rng>>())
            adaptor<true> end_adaptor() const
            {
                return {*this};
            }
        public:
            filtered_view() = default;
            filtered_view(Rng && rng, Pred pred)
              : range_adaptor_t<filtered_view>{std::forward<Rng>(rng)}
              , pred_(invokable(std::move(pred)))
            {}
        };

        namespace view
        {
            struct filter_fn
            {
                template<typename Rng, typename Pred>
                filtered_view<Rng, Pred>
                operator()(Rng && rng, Pred pred) const
                {
                    CONCEPT_ASSERT(Iterable<Rng>());
                    CONCEPT_ASSERT(InvokablePredicate<Pred, range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), std::move(pred)};
                }
                template<typename Pred>
                auto operator()(Pred pred) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(pred)))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(pred))));
                }
            };

            constexpr filter_fn filter {};
        }
    }
}

#endif
