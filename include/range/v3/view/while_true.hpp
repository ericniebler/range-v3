// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_WHILE_TRUE_HPP
#define RANGES_V3_VIEW_WHILE_TRUE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Predicate>
            struct while_true_sentinel_adaptor : default_adaptor
            {
            private:
                Predicate predicate_;
            public:
                while_true_sentinel_adaptor() = default;
                while_true_sentinel_adaptor(Predicate predicate)
                  : predicate_(std::move(predicate))
                {}
                template<typename Cur, typename S>
                bool empty(Cur const &pos, S const &end) const
                {
                    return end.equal(pos) || !predicate_(pos.current());
                }
            };
        }

        template<typename Rng, typename Predicate>
        struct while_trueed_view
          : range_adaptor<while_trueed_view<Rng, Predicate>, Rng>
        {
        private:
            friend range_core_access;
            Predicate predicate_;

            default_adaptor begin_adaptor() const
            {
                return {};
            }
            detail::while_true_sentinel_adaptor<Predicate> end_adaptor() const
            {
                return {predicate_};
            }
        public:
            while_trueed_view(Rng && rng, Predicate predicate)
              : range_adaptor_t<while_trueed_view>(std::forward<Rng>(rng))
              , predicate_(std::move(predicate))
            {}
        };

        namespace view
        {
            struct while_true_fn : bindable<while_true_fn>
            {
                template<typename Rng, typename Predicate,
                    CONCEPT_REQUIRES_(Iterable<Rng>())>
                static while_trueed_view<Rng, Predicate>
                invoke(while_true_fn, Rng && rng, Predicate predicate)
                {
                    return {std::forward<Rng>(rng), std::move(predicate)};
                }

                template<typename I, typename Predicate,
                    CONCEPT_REQUIRES_(InputIterator<I>())>
                static while_trueed_view<iterator_range<I, unreachable>, Predicate>
                invoke(while_true_fn, I begin, Predicate predicate)
                {
                    return {{std::move(begin), {}}, std::move(predicate)};
                }

                template<typename Predicate>
                static auto
                invoke(while_true_fn while_true, Predicate predicate) ->
                    decltype(while_true.move_bind(std::placeholders::_1, std::move(predicate)))
                {
                    return while_true.move_bind(std::placeholders::_1, std::move(predicate));
                }
            };

            RANGES_CONSTEXPR while_true_fn while_true{};
        }
    }
}

#endif
