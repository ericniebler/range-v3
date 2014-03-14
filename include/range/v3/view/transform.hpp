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
            using impl_base_t = basic_adaptor_impl<InputIterable>;
            using sentinel_base_t = basic_adaptor_sentinel<InputIterable>;

            invokable_t<UnaryFunction> fun_;

            struct impl : impl_base_t
            {
                transform_iterable_view const *rng_;
                impl() = default;
                using impl_base_t::impl_base_t;
                impl(impl_base_t base, transform_iterable_view const &rng)
                  : impl_base_t(std::move(base)), rng_(&rng)
                {}
                auto current() const -> decltype(rng_->fun_(this->base().current()))
                {
                    return rng_->fun_(this->base().current());
                }
            };

            struct sentinel : sentinel_base_t
            {
                sentinel() = default;
                using sentinel_base_t::sentinel_base_t;
                sentinel(sentinel_base_t base, transform_iterable_view const &)
                  : sentinel_base_t(std::move(base))
                {}
            };

            using sentinel_t = detail::conditional_t<(Range<InputIterable>()), impl, sentinel>;

            impl begin_impl() const
            {
                return {this->adaptor().begin_impl(), *this};
            }
            sentinel_t end_impl() const
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
                    CONCEPT_ASSERT(ranges::Range<InputRange1>());
                    CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputRange1>>());
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
