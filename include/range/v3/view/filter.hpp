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
            template<bool Const>
            using impl_base_t = basic_adaptor_impl<InputIterable, Const>;
            template<bool Const>
            using sentinel_base_t = basic_adaptor_sentinel<InputIterable, Const>;

            invokable_t<UnaryPredicate> pred_;

            template<bool Const>
            struct basic_impl : impl_base_t<Const>
            {
                using filter_iterable_view_ = detail::add_const_if_t<filter_iterable_view, Const>;
                filter_iterable_view_ *rng_;
                using impl_base_t<Const>::impl_base_t;
                basic_impl() = default;
                basic_impl(impl_base_t<Const> base, filter_iterable_view_ &rng)
                  : impl_base_t<Const>(std::move(base)), rng_(&rng)
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

            template<bool Const>
            struct basic_sentinel : sentinel_base_t<Const>
            {
                using sentinel_base_t<Const>::sentinel_base_t;
                basic_sentinel() = default;
                basic_sentinel(sentinel_base_t<Const> base, filter_iterable_view const &)
                  : sentinel_base_t<Const>(std::move(base))
                {}
            };

            template<bool Const>
            using basic_sentinel_t =
                detail::conditional_t<
                    (Range<InputIterable>()), basic_impl<Const>, basic_sentinel<Const>>;

            basic_impl<false> begin_impl()
            {
                basic_impl<false> impl{this->adaptor().begin_impl(), *this};
                impl.satisfy();
                return impl;
            }
            basic_impl<true> begin_impl() const
            {
                basic_impl<true> impl{this->adaptor().begin_impl(), *this};
                impl.satisfy();
                return impl;
            }
            basic_sentinel_t<false> end_impl()
            {
                return {this->adaptor().end_impl(), *this};
            }
            basic_sentinel_t<true> end_impl() const
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
