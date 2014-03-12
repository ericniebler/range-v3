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

#ifndef RANGES_V3_VIEW_TRANSFORM_HPP
#define RANGES_V3_VIEW_TRANSFORM_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputIterable, typename UnaryFunction>
        struct transform_iterable_view
          : range_adaptor<
                transform_iterable_view<InputIterable, UnaryFunction>,
                InputIterable>
        {
        private:
            friend range_core_access;
            using base_t = range_adaptor_t<transform_iterable_view>;
            template<bool Const>
            using impl_base_t = basic_adaptor_impl<InputIterable, Const>;
            template<bool Const>
            using sentinel_base_t = basic_adaptor_sentinel<InputIterable, Const>;

            invokable_t<UnaryFunction> fun_;

            template<bool Const>
            struct basic_impl : impl_base_t<Const>
            {
                using transform_iterable_view_ = detail::add_const_if_t<transform_iterable_view, Const>;
                transform_iterable_view_ *rng_;
                using impl_base_t<Const>::impl_base_t;
                basic_impl(impl_base_t<Const> base, transform_iterable_view_ &rng)
                  : impl_base_t<Const>(std::move(base)), rng_(&rng)
                {}
                auto current() const -> decltype(rng_->fun_(this->base().current()))
                {
                    return rng_->fun_(this->base().current());
                }
            };

            template<bool Const>
            struct basic_sentinel : sentinel_base_t<Const>
            {
                using sentinel_base_t<Const>::sentinel_base_t;
                basic_sentinel(sentinel_base_t<Const> base, transform_iterable_view const &)
                  : sentinel_base_t<Const>(std::move(base))
                {}
            };

            template<bool Const>
            using basic_sentinel_t =
                detail::conditional_t<
                    (Range<InputIterable>()), basic_impl<Const>, basic_sentinel<Const>>;

            basic_impl<false> begin_impl()
            {
                return {this->adaptor().begin_impl(), *this};
            }
            basic_impl<true> begin_impl() const
            {
                return {this->adaptor().begin_impl(), *this};
            }
            basic_sentinel_t<false> end_impl()
            {
                return {this->adaptor().end_impl(), *this};
            }
            basic_sentinel_t<false> end_impl() const
            {
                return {this->adaptor().end_impl(), *this};
            }
        public:
            transform_iterable_view(InputIterable && rng, UnaryFunction fun)
              : base_t(std::forward<InputIterable>(rng))
              , fun_(ranges::make_invokable(std::move(fun)))
            {}
        };

        namespace view
        {
            struct transformer : bindable<transformer>
            {
            private:
                template<typename UnaryFunction>
                struct transformer1 : pipeable<transformer1<UnaryFunction>>
                {
                private:
                    UnaryFunction fun_;
                public:
                    transformer1(UnaryFunction fun)
                      : fun_(std::move(fun))
                    {}
                    template<typename InputRange, typename This>
                    static transform_iterable_view<InputRange, UnaryFunction>
                    pipe(InputRange && rng, This && this_)
                    {
                        return {std::forward<InputRange>(rng), std::forward<This>(this_).fun_};
                    }
                };
            public:
                ///
                template<typename InputRange1, typename UnaryFunction>
                static transform_iterable_view<InputRange1, UnaryFunction>
                invoke(transformer, InputRange1 && rng, UnaryFunction fun)
                {
                    CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                    CONCEPT_ASSERT(ranges::Callable<invokable_t<UnaryFunction>,
                                                    range_reference_t<InputRange1>>());
                    return {std::forward<InputRange1>(rng), std::move(fun)};
                }

                /// \overload
                template<typename UnaryFunction>
                static transformer1<UnaryFunction> invoke(transformer, UnaryFunction fun)
                {
                    return {std::move(fun)};
                }
            };

            RANGES_CONSTEXPR transformer transform {};
        }
    }
}

#endif
