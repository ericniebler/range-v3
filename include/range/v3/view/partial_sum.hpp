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

#ifndef RANGES_V3_VIEW_PARTIAL_SUM_HPP
#define RANGES_V3_VIEW_PARTIAL_SUM_HPP

#include <utility>
#include <iterator>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/front.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Fun>
        struct partial_sum_view
          : range_adaptor<partial_sum_view<Rng, Fun>, Rng>
        {
        private:
            friend range_access;
            semiregular_invokable_t<Fun> fun_;
            using single_pass = SinglePass<range_iterator_t<Rng>>;
            using use_sentinel_t = meta::or_<meta::not_<BoundedIterable<Rng>>, single_pass>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                using partial_sum_view_t = meta::apply<meta::add_const_if_c<IsConst>, partial_sum_view>;
                optional<range_value_t<Rng>> sum_;
                partial_sum_view_t *rng_;
                using adaptor_base::prev;
            public:
                using single_pass = partial_sum_view::single_pass;
                adaptor() = default;
                adaptor(partial_sum_view_t &rng)
                  : sum_{}, rng_(&rng)
                {}
                adaptor(partial_sum_view_t &rng, range_value_t<Rng> sum)
                  : sum_(std::move(sum)), rng_(&rng)
                {}
                range_value_t<Rng> current(range_iterator_t<Rng> it) const
                {
                    return *sum_;
                }
                void next(range_iterator_t<Rng> &it)
                {
                    if(++it != ranges::end(rng_->mutable_base()))
                    {
                        sum_ = rng_->fun_(*sum_, *it);
                    }
                }
            };

            CONCEPT_REQUIRES(!Invokable<Fun const, range_value_t<Rng>>())
            adaptor<false> begin_adaptor()
            {
                return empty(this->base()) ? adaptor<false>{*this} : adaptor<false>{*this, front(this->base())};
            }
            CONCEPT_REQUIRES(Invokable<Fun const, range_value_t<Rng>>())
            adaptor<true> begin_adaptor() const
            {
                return empty(this->base()) ? adaptor<true>{*this} : adaptor<true>{*this, front(this->base())};
            }
            CONCEPT_REQUIRES(use_sentinel_t())
            adaptor_base end_adaptor() const
            {
                return {};
            }
            CONCEPT_REQUIRES(!use_sentinel_t() && !Invokable<Fun const, range_value_t<Rng>>())
            adaptor<false> end_adaptor()
            {
                return empty(this->base()) ? adaptor<false>{*this} : adaptor<false>{*this, front(this->base())};
            }
            CONCEPT_REQUIRES(!use_sentinel_t() && Invokable<Fun const, range_value_t<Rng>>())
            adaptor<true> end_adaptor() const
            {
                return empty(this->base()) ? adaptor<true>{*this} : adaptor<true>{*this, front(this->base())};
            }
        public:
            partial_sum_view() = default;
            partial_sum_view(Rng && rng, Fun fun)
              : range_adaptor_t<partial_sum_view>{std::forward<Rng>(rng)}
              , fun_(invokable(std::move(fun)))
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct partial_sum_fn
            {
                template<typename Rng, typename Fun>
                partial_sum_view<Rng, Fun> operator()(Rng && rng, Fun fun) const
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    CONCEPT_ASSERT(Invokable<Fun, range_value_t<Rng>, range_value_t<Rng>>());
                    CONCEPT_ASSERT(Convertible<
                        concepts::Invokable::result_t<Fun, range_value_t<Rng>, range_value_t<Rng>>,
                        range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), std::move(fun)};
                }

                template<typename Fun>
                auto operator()(Fun fun) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(fun)))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(fun))));
                }
            };

            /// \sa `partial_sum_fn`
            /// \ingroup group-views
            constexpr partial_sum_fn partial_sum{};
        }
        /// @}
    }
}

#endif
