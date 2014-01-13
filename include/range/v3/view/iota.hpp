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
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/utility/safe_int.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace concepts
        {
            struct ForwardIota
              : refines<EqualityComparable, DefaultConstructible>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T &>(++t)
                    ));
            };

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
        using Iota = concepts::models<concepts::ForwardIota, T>;

        template<typename T>
        using iota_concept_t = concepts::most_refined_t<concepts::RandomAccessIota, T>;

        namespace detail
        {
            auto iota_category(concepts::ForwardIota)->std::forward_iterator_tag;
            auto iota_category(concepts::BidirectionalIota)->std::bidirectional_iterator_tag;
            auto iota_category(concepts::RandomAccessIota)->std::random_access_iterator_tag;

            template<typename Value>
            auto iota_difference(concepts::ForwardIota) -> safe_int<std::ptrdiff_t>;
            template<typename Value>
            auto iota_difference(concepts::RandomAccessIota) ->
                make_safe_int_t<decltype(std::declval<Value>() - std::declval<Value>())>;
        }

        template<typename Value>
        struct iota_iterable_view
        {
        private:
            Value value_;

        public:
            struct sentinel;
            using const_iterator = struct iterator
              : ranges::iterator_facade<
                    iterator
                  , Value
                  , decltype(detail::iota_category(iota_concept_t<Value>{}))
                  , Value
                  , decltype(detail::iota_difference<Value>(iota_concept_t<Value>{}))
                >
            {
                using difference_type = typename iterator::difference_type;
            private:
                friend struct iota_iterable_view;
                friend struct iterator_core_access;
                Value value_;

                explicit iterator(Value value)
                  : value_(std::move(value))
                {}
                Value dereference() const
                {
                    return value_;
                }
                bool equal(iterator const &that) const
                {
                    return value_ == that.value_;
                }
                constexpr bool equal(sentinel const &) const
                {
                    return false;
                }
                void increment()
                {
                    ++value_;
                }
                void decrement()
                {
                    ++value_;
                }
                void advance(difference_type n)
                {
                    RANGES_ASSERT(n.is_finite());
                    value_ += n.get();
                }
                difference_type distance_to(iterator const &that) const
                {
                    return that.value_ - value_;
                }
                difference_type distance_to(sentinel) const
                {
                    return difference_type::inf();
                }
            public:
                iterator()
                  : value_{}
                {}
            };
            using const_sentinel = struct sentinel
              : ranges::sentinel_facade<sentinel, iterator>
            {};

            explicit iota_iterable_view(Value value)
              : value_(std::move(value))
            {}

            iterator begin() const
            {
                return iterator{value_};
            }
            sentinel end() const
            {
                return {};
            }
        };

        namespace view
        {
            struct iota_maker : bindable<iota_maker>, pipeable<iota_maker>
            {
                template<typename Value>
                static iota_iterable_view<Value> invoke(iota_maker, Value value)
                {
                    CONCEPT_ASSERT(ranges::Iota<Value>());
                    return iota_iterable_view<Value>{std::move(value)};
                }
            };

            RANGES_CONSTEXPR iota_maker iota{};
        }
    }
}

#endif
