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

#ifndef RANGES_V3_VIEW_TAKE_WHILE_HPP
#define RANGES_V3_VIEW_TAKE_WHILE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/view/transform.hpp>
namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
        template<typename Pred, typename SinglePass>
            struct take_while_sentinel_adaptor : default_adaptor
            {
            private:
                Pred predicate_;
            public:
                using single_pass = SinglePass;
                take_while_sentinel_adaptor() = default;
                take_while_sentinel_adaptor(Pred predicate)
                  : predicate_(std::move(predicate))
                {}
                template<typename Cur, typename S>
                bool empty(Cur const &pos, S const &end) const
                {
                    return end.equal(pos) || !predicate_(pos.current());
                }
            };
        }

        template<typename Rng, typename Pred>
        struct take_whileed_view
          : range_adaptor<take_whileed_view<Rng, Pred>, Rng>
        {
        private:
            using reference_t = result_of_t<invokable_t<Pred> const(range_value_t<Rng>)>;
            friend range_core_access;
            using view_fun_t = detail::conditional_t<(bool) SemiRegular<invokable_t<Pred>>(),
                invokable_t<Pred>, detail::value_wrapper<invokable_t<Pred>>>;
            using adaptor_fun_t = detail::conditional_t<(bool) SemiRegular<invokable_t<Pred>>(),
                view_fun_t, detail::reference_wrapper<view_fun_t const>>;
            view_fun_t predicate_;

            using single_pass = detail::or_t<
                Derived<ranges::input_iterator_tag, range_category_t<Rng>>,
                detail::not_t<std::is_reference<reference_t>>>;
            using end_adaptor_t = detail::take_while_sentinel_adaptor<adaptor_fun_t, single_pass>;
            using use_sentinel_t = detail::or_t<detail::not_t<Range<Rng>>, single_pass>;

            default_adaptor begin_adaptor() const
            {
                return {};
            }

            end_adaptor_t end_adaptor() const
            {
                return {predicate_};
            }
        public:
            take_whileed_view(Rng && rng, Pred predicate)
              : range_adaptor_t<take_whileed_view>(std::forward<Rng>(rng))
              , predicate_(invokable(std::move(predicate)))
            {}
        };

        namespace view
        {
            struct take_while_fn : bindable<take_while_fn>
            {
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Iterable<Rng>())>
                static take_whileed_view<Rng, Pred>
                invoke(take_while_fn, Rng && rng, Pred predicate)
                {
                    return {std::forward<Rng>(rng), std::move(predicate)};
                }

                template<typename I, typename Pred,
                    CONCEPT_REQUIRES_(InputIterator<I>())>
                static take_whileed_view<iterator_range<I, unreachable>, Pred>
                invoke(take_while_fn, I begin, Pred predicate)
                {
                    return {{std::move(begin), {}}, std::move(predicate)};
                }

                template<typename Pred>
                static auto
                invoke(take_while_fn take_while, Pred predicate) ->
                    decltype(take_while.move_bind(std::placeholders::_1, std::move(predicate)))
                {
                    return take_while.move_bind(std::placeholders::_1, std::move(predicate));
                }
            };

            RANGES_CONSTEXPR take_while_fn take_while{};
        }
    }
}

#endif
