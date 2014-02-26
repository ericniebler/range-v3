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

#ifndef RANGES_V3_VIEW_IOTA_HPP
#define RANGES_V3_VIEW_IOTA_HPP

#include <limits>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace concepts
        {
            struct InputIota
              : refines<DefaultConstructible>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T &>(++t)
                    ));
            };

            struct ForwardIota
              : refines<InputIota, EqualityComparable>
            {};

            struct BidirectionalIota
              : refines<ForwardIota>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T &>(--t)
                    ));
            };

            struct RandomAccessIota
              : refines<BidirectionalIota>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T &>(t += (t - t))
                    ));
            };
        }

        template<typename T>
        using InputIota = concepts::models<concepts::InputIota, T>;
        template<typename T>
        using ForwardIota = concepts::models<concepts::ForwardIota, T>;
        template<typename T>
        using BidirectionalIota = concepts::models<concepts::BidirectionalIota, T>;
        template<typename T>
        using RandomAccessIota = concepts::models<concepts::RandomAccessIota, T>;

        template<typename T>
        using iota_concept_t = concepts::most_refined_t<concepts::RandomAccessIota, T>;

        namespace detail
        {
            template<typename Value>
            auto iota_difference(concepts::InputIota) -> std::ptrdiff_t;

            template<typename Value>
            auto iota_difference(concepts::RandomAccessIota) ->
                typename std::make_signed<
                    decltype(std::declval<Value>() - std::declval<Value>())
                >::type;
        }

        template<typename Value>
        struct iota_iterable_view
          : range_facade<iota_iterable_view<Value>, true>
        {
        private:
            using iota_concept_t = ranges::iota_concept_t<Value>;
            friend range_core_access;
        public:
            using difference_type = decltype(detail::iota_difference<Value>(iota_concept_t{}));
        private:
            Value value_;
            Value current() const
            {
                return value_;
            }
            void next()
            {
                ++value_;
            }
            constexpr bool done() const
            {
                return false;
            }
            CONCEPT_REQUIRES(ForwardIota<Value>())
            bool equal(iota_iterable_view const &that) const
            {
                return that.value_ == value_;
            }
            CONCEPT_REQUIRES(BidirectionalIota<Value>())
            void prev()
            {
                --value_;
            }
            CONCEPT_REQUIRES(RandomAccessIota<Value>())
            void advance(difference_type n)
            {
                value_ += n;
            }
            CONCEPT_REQUIRES(RandomAccessIota<Value>())
            difference_type distance_to(iota_iterable_view const &that) const
            {
                return that.value_ - value_;
            }
        public:
            constexpr iota_iterable_view()
              : value_{}
            {}
            constexpr explicit iota_iterable_view(Value value)
              : value_(std::move(value))
            {}
        };

        namespace view
        {
            struct iota_maker : bindable<iota_maker>, pipeable<iota_maker>
            {
                template<typename Value>
                static iota_iterable_view<Value> invoke(iota_maker, Value value)
                {
                    CONCEPT_ASSERT(ranges::InputIota<Value>());
                    return iota_iterable_view<Value>{std::move(value)};
                }
            };

            RANGES_CONSTEXPR iota_maker iota{};
        }
    }
}

#endif
