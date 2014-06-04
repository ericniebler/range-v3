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
                auto requires_(T && t) -> decltype(
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
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T &>(--t)
                    ));
            };

            struct RandomAccessIota
              : refines<BidirectionalIota>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
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
        using iota_concept =
            concepts::most_refined<
                typelist<
                    concepts::RandomAccessIota,
                    concepts::BidirectionalIota,
                    concepts::ForwardIota,
                    concepts::InputIota>, T>;

        namespace detail
        {
            template<typename Val>
            auto iota_difference(concepts::InputIota*) -> std::ptrdiff_t;

            template<typename Val>
            auto iota_difference(concepts::RandomAccessIota*) ->
                typename std::make_signed<
                    decltype(std::declval<Val>() - std::declval<Val>())
                >::type;
        }

        template<typename Val>
        struct iota_view
          : range_facade<iota_view<Val>, true>
        {
        private:
            using iota_concept = ranges::iota_concept<Val>;
            friend range_core_access;
        public:
            using difference_type = decltype(detail::iota_difference<Val>(iota_concept()));
        private:
            Val value_;
            Val current() const
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
            CONCEPT_REQUIRES(ForwardIota<Val>())
            bool equal(iota_view const &that) const
            {
                return that.value_ == value_;
            }
            CONCEPT_REQUIRES(BidirectionalIota<Val>())
            void prev()
            {
                --value_;
            }
            CONCEPT_REQUIRES(RandomAccessIota<Val>())
            void advance(difference_type n)
            {
                value_ += n;
            }
            CONCEPT_REQUIRES(RandomAccessIota<Val>())
            difference_type distance_to(iota_view const &that) const
            {
                return that.value_ - value_;
            }
        public:
            constexpr iota_view()
              : value_{}
            {}
            constexpr explicit iota_view(Val value)
              : value_(std::move(value))
            {}
        };

        namespace view
        {
            struct iota_fn : bindable<iota_fn>, pipeable<iota_fn>
            {
                template<typename Val>
                static iota_view<Val> invoke(iota_fn, Val value)
                {
                    CONCEPT_ASSERT(InputIota<Val>());
                    return iota_view<Val>{std::move(value)};
                }
            };

            RANGES_CONSTEXPR iota_fn iota{};
        }
    }
}

#endif
