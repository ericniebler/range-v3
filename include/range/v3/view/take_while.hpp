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

#ifndef RANGES_V3_VIEW_TAKE_WHILE_HPP
#define RANGES_V3_VIEW_TAKE_WHILE_HPP

#include <utility>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng, typename Pred>
        struct take_while_view
          : range_adaptor<take_while_view<Rng, Pred>, Rng>
        {
        private:
            friend range_access;
            using reference_t = concepts::Invokable::result_t<Pred, range_value_t<Rng>>;
            semiregular_invokable_t<Pred> pred_;

            using single_pass = meta::or_<
                SinglePass<range_iterator_t<Rng>>,
                meta::not_<std::is_reference<reference_t>>>;

            template<bool IsConst>
            struct sentinel_adaptor
              : adaptor_base
            {
            private:
                semiregular_invokable_ref_t<Pred, IsConst> pred_;
            public:
                using single_pass = take_while_view::single_pass;
                sentinel_adaptor() = default;
                sentinel_adaptor(semiregular_invokable_ref_t<Pred, IsConst> pred)
                  : pred_(std::move(pred))
                {}
                bool empty(range_iterator_t<Rng> it, range_sentinel_t<Rng> end) const
                {
                    return it == end || !pred_(*it);
                }
            };

            CONCEPT_REQUIRES(!Invokable<Pred const, range_value_t<Rng>>())
            sentinel_adaptor<false> end_adaptor()
            {
                return {pred_};
            }
            CONCEPT_REQUIRES(Invokable<Pred const, range_value_t<Rng>>())
            sentinel_adaptor<true> end_adaptor() const
            {
                return {pred_};
            }
        public:
            take_while_view() = default;
            take_while_view(Rng && rng, Pred pred)
              : range_adaptor_t<take_while_view>{std::forward<Rng>(rng)}
              , pred_(invokable(std::move(pred)))
            {}
        };

        namespace view
        {
            struct take_while_fn
            {
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Iterable<Rng>())>
                take_while_view<Rng, Pred> operator()(Rng && rng, Pred pred) const
                {
                    return {std::forward<Rng>(rng), std::move(pred)};
                }
                template<typename Pred>
                auto operator()(Pred pred) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(pred))))
                )
            };

            constexpr take_while_fn take_while{};
        }
    }
}

#endif
