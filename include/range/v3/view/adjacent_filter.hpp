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
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng, typename F>
        struct adjacent_filtered_view
          : range_adaptor<adjacent_filtered_view<Rng, F>, Rng>
        {
        private:
            friend range_access;
            invokable_t<F> pred_;

            struct adaptor : adaptor_base
            {
            private:
                adjacent_filtered_view const *rng_;
                using adaptor_base::prev;
            public:
                adaptor() = default;
                adaptor(adjacent_filtered_view const &rng)
                  : rng_(&rng)
                {}
                void next(range_iterator_t<Rng> &it) const
                {
                    auto const end = ranges::end(rng_->mutable_base());
                    RANGES_ASSERT(it != end);
                    it = adjacent_find(std::move(it), end, std::ref(rng_->pred_));
                    advance_bounded(it, 1, end);
                }
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
            adjacent_filtered_view() = default;
            adjacent_filtered_view(Rng && rng, F pred)
              : range_adaptor_t<adjacent_filtered_view>{std::forward<Rng>(rng)}
              , pred_(invokable(std::move(pred)))
            {}
        };

        namespace view
        {
            struct adjacent_filter_fn
            {
                template<typename Rng, typename F>
                adjacent_filtered_view<Rng, F>
                operator()(Rng && rng, F pred) const
                {
                    CONCEPT_ASSERT(ForwardIterable<Rng>());
                    CONCEPT_ASSERT(InvokablePredicate<F, range_value_t<Rng>,
                                                         range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), std::move(pred)};
                }
                template<typename F>
                auto operator()(F pred) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(pred)))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(pred))));
                }
            };

            constexpr adjacent_filter_fn adjacent_filter {};
        }
    }
}

#endif
