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

#ifndef RANGES_V3_VIEW_TRANSFORM_HPP
#define RANGES_V3_VIEW_TRANSFORM_HPP

#include <utility>
#include <iterator>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
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
        template<typename Rng, typename Fun>
        struct transformed_view
          : range_adaptor<transformed_view<Rng, Fun>, Rng>
        {
        private:
            friend range_access;
            using reference_t = concepts::Invokable::result_t<Fun, range_value_t<Rng>>;
            semiregular_invokable_t<Fun> fun_;
            // Forward ranges must always return references. If the result of calling the function
            // is not a reference, this range is input-only.
            using single_pass = meta::or_<
                SinglePass<range_iterator_t<Rng>>,
                meta::not_<std::is_reference<reference_t>>>;
            using use_sentinel_t = meta::or_<meta::not_<BoundedIterable<Rng>>, single_pass>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                semiregular_invokable_ref_t<Fun, IsConst> fun_;
            public:
                using single_pass = transformed_view::single_pass;
                adaptor() = default;
                adaptor(semiregular_invokable_ref_t<Fun, IsConst> fun)
                  : fun_(std::move(fun))
                {}
                auto current(range_iterator_t<Rng> it) const ->
                    decltype(fun_(*it))
                {
                    return fun_(*it);
                }
            };

            CONCEPT_REQUIRES(!Invokable<Fun const, range_value_t<Rng>>())
            adaptor<false> begin_adaptor()
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(Invokable<Fun const, range_value_t<Rng>>())
            adaptor<true> begin_adaptor() const
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(use_sentinel_t())
            adaptor_base end_adaptor() const
            {
                return {};
            }
            CONCEPT_REQUIRES(!use_sentinel_t() && !Invokable<Fun const, range_value_t<Rng>>())
            adaptor<false> end_adaptor()
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(!use_sentinel_t() && Invokable<Fun const, range_value_t<Rng>>())
            adaptor<true> end_adaptor() const
            {
                return {fun_};
            }
        public:
            transformed_view() = default;
            transformed_view(Rng && rng, Fun fun)
              : range_adaptor_t<transformed_view>{std::forward<Rng>(rng)}
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
            struct transform_fn
            {
                template<typename Rng, typename Fun>
                transformed_view<Rng, Fun> operator()(Rng && rng, Fun fun) const
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    CONCEPT_ASSERT(Invokable<Fun, range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), std::move(fun)};
                }

                template<typename Fun>
                auto operator()(Fun fun) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(fun)))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(fun))));
                }
            };

            constexpr transform_fn transform {};
        }
    }
}

#endif
