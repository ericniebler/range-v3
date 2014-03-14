// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_FILTER_HPP
#define RANGES_V3_VIEW_FILTER_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputIterable, typename UnaryPredicate>
        struct filter_iterable_view
          : range_adaptor<
                filter_iterable_view<InputIterable, UnaryPredicate>,
                InputIterable>
        {
        private:
            friend range_core_access;
            using base_t = range_adaptor_t<filter_iterable_view>;
            using impl_base_t = basic_adaptor_impl<InputIterable>;
            using sentinel_base_t = basic_adaptor_sentinel<InputIterable>;

            invokable_t<UnaryPredicate> pred_;

            struct impl : impl_base_t
            {
                filter_iterable_view const *rng_;
                using impl_base_t::impl_base_t;
                impl() = default;
                impl(impl_base_t base, filter_iterable_view const &rng)
                  : impl_base_t(std::move(base)), rng_(&rng)
                {}
                void next()
                {
                    this->base().next();
                    satisfy();
                }
                CONCEPT_REQUIRES(BidirectionalIterator<range_iterator_t<InputIterable>>())
                void prev()
                {
                    do
                    {
                        this->base().prev();
                    } while (!rng_->pred_(this->current()));
                }
                void satisfy()
                {
                    auto const e = rng_->end_impl();
                    while(!e.equal(*this) && !rng_->pred_(this->current()))
                        this->next();
                }
            };

            struct sentinel : sentinel_base_t
            {
                using sentinel_base_t::sentinel_base_t;
                sentinel() = default;
                sentinel(sentinel_base_t base, filter_iterable_view const &)
                  : sentinel_base_t(std::move(base))
                {}
            };

            using sentinel_t = detail::conditional_t<(Range<InputIterable>()), impl, sentinel>;

            impl begin_impl() const
            {
                impl begin{this->adaptor().begin_impl(), *this};
                begin.satisfy();
                return begin;
            }
            sentinel_t end_impl() const
            {
                return {this->adaptor().end_impl(), *this};
            }
        public:
            filter_iterable_view(InputIterable && rng, UnaryPredicate pred)
              : base_t(std::forward<InputIterable>(rng))
              , pred_(ranges::make_invokable(std::move(pred)))
            {}
        };

        namespace view
        {
            struct filterer : bindable<filterer>
            {
            private:
                template<typename UnaryPredicate>
                struct filterer1 : pipeable<filterer1<UnaryPredicate>>
                {
                private:
                    UnaryPredicate pred_;
                public:
                    filterer1(UnaryPredicate pred)
                      : pred_(std::move(pred))
                    {}
                    template<typename InputIterable, typename This>
                    static filter_iterable_view<InputIterable, UnaryPredicate> pipe(InputIterable && rng, This && this_)
                    {
                        return {std::forward<InputIterable>(rng), std::forward<This>(this_).pred_};
                    }
                };
            public:
                template<typename InputIterable, typename UnaryPredicate>
                static filter_iterable_view<InputIterable, UnaryPredicate>
                invoke(filterer, InputIterable && rng, UnaryPredicate pred)
                {
                    return {std::forward<InputIterable>(rng), std::move(pred)};
                }
                template<typename UnaryPredicate>
                static filterer1<UnaryPredicate> invoke(filterer, UnaryPredicate pred)
                {
                    return {std::move(pred)};
                }
            };

            RANGES_CONSTEXPR filterer filter {};
        }
    }
}

#endif
