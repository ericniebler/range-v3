/// \file
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

#ifndef RANGES_V3_VIEW_IOTA_HPP
#define RANGES_V3_VIEW_IOTA_HPP

#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/delimit.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace concepts
        {
            struct BidirectionalIncrementable
              : refines<Incrementable>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T &>(--t),
                        concepts::has_type<T>(t--)
                    ));
            };

            struct RandomAccessIncrementable
              : refines<BidirectionalIncrementable>
            {
                template<typename T>
                auto requires_(T&& t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Integral>(t - t),
                        concepts::has_type<T &>(t += (t - t)),
                        concepts::has_type<T &>(t -= (t - t)),
                        concepts::convertible_to<T>(t - (t - t)),
                        concepts::convertible_to<T>(t + (t - t)),
                        concepts::convertible_to<T>((t - t) + t)
                    ));
            };
        }

        template<typename T>
        using BidirectionalIncrementable = concepts::models<concepts::BidirectionalIncrementable, T>;

        template<typename T>
        using RandomAccessIncrementable = concepts::models<concepts::RandomAccessIncrementable, T>;

        template<typename T>
        using incrementable_concept =
            concepts::most_refined<
                meta::list<
                    concepts::RandomAccessIncrementable,
                    concepts::BidirectionalIncrementable,
                    concepts::Incrementable,
                    concepts::WeaklyIncrementable>, T>;

        template<typename T>
        using incrementable_concept_t = meta::eval<incrementable_concept<T>>;

        /// \cond
        namespace detail
        {
            template<typename Val, typename Iota = incrementable_concept_t<Val>,
                bool IsIntegral = std::is_integral<Val>::value>
            struct iota_difference_
              : ranges::difference_type<Val>
            {};

            template<typename Val>
            struct iota_difference_<Val, concepts::RandomAccessIncrementable, true>
            {
            private:
                using difference_t = decltype(std::declval<Val>() - std::declval<Val>());
                static constexpr std::size_t bits = sizeof(difference_t) * CHAR_BIT;
            public:
                using type =
                    meta::if_<
                        meta::not_<std::is_same<Val, difference_t>>,
                        meta::eval<std::make_signed<difference_t>>,
                        meta::if_c<
                            (bits < 8),
                            std::int_fast8_t,
                            meta::if_c<
                                (bits < 16),
                                std::int_fast16_t,
                                meta::if_c<
                                    (bits < 32),
                                    std::int_fast32_t,
                                    std::int_fast64_t> > > >;
            };

            template<typename Val>
            struct iota_difference
              : iota_difference_<Val>
            {};

            template<typename Val>
            using iota_difference_t = meta::eval<iota_difference<Val>>;

            template<typename Val, CONCEPT_REQUIRES_(!Integral<Val>())>
            constexpr iota_difference_t<Val> iota_minus(Val const &v0, Val const &v1)
            {
                return v0 - v1;
            }

            template<typename Val, CONCEPT_REQUIRES_(SignedIntegral<Val>())>
            constexpr iota_difference_t<Val> iota_minus(Val const &v0, Val const &v1)
            {
                using D = iota_difference_t<Val>;
                return (D) v0 - (D) v1;
            }

            template<typename Val, CONCEPT_REQUIRES_(UnsignedIntegral<Val>())>
            constexpr iota_difference_t<Val> iota_minus(Val const &v0, Val const &v1)
            {
                using D = iota_difference_t<Val>;
                return (D) (v0 - v1);
            }
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{

        /// An iota view in a closed range with non-random access iota value type
        template<typename Val, typename Val2 /* = void */>
        struct iota_view
          : range_facade<iota_view<Val, Val2>, true>
        {
        private:
            using incrementable_concept_t = ranges::incrementable_concept<Val>;
            friend range_access;
            using difference_type_ = detail::iota_difference_t<Val>;

            Val from_;
            Val2 to_;
            bool done_ = false;

            constexpr Val current() const
            {
                return from_;
            }
            RANGES_RELAXED_CONSTEXPR void next()
            {
                if(from_ == to_)
                    done_ = true;
                else
                    ++from_;
            }
            constexpr bool done() const
            {
                return done_;
            }
            CONCEPT_REQUIRES(Incrementable<Val>())
            constexpr bool equal(iota_view const &that) const
            {
                return that.from_ == from_;
            }
            CONCEPT_REQUIRES(BidirectionalIncrementable<Val>())
            RANGES_RELAXED_CONSTEXPR void prev()
            {
                --from_;
            }
            CONCEPT_REQUIRES(RandomAccessIncrementable<Val>())
            RANGES_RELAXED_CONSTEXPR void advance(difference_type_ n)
            {
                RANGES_ASSERT(detail::iota_minus(to_, from_) >= n);
                from_ += n;
            }
            CONCEPT_REQUIRES(RandomAccessIncrementable<Val>())
            RANGES_RELAXED_CONSTEXPR
            difference_type_ distance_to(iota_view const &that) const
            {
                return detail::iota_minus(that.from_, from_);
            }
        public:
            constexpr iota_view() = default;
            constexpr iota_view(Val from, Val2 to)
              : from_(std::move(from)), to_(std::move(to))
            {}
        };

        template<typename Val>
        struct iota_view<Val, void>
          : range_facade<iota_view<Val, void>, true>
        {
        private:
            using incrementable_concept_t = ranges::incrementable_concept<Val>;
            friend range_access;
            using difference_type_ = detail::iota_difference_t<Val>;

            Val value_;

            RANGES_RELAXED_CONSTEXPR Val current() const
            {
                return value_;
            }
            RANGES_RELAXED_CONSTEXPR void next()
            {
                ++value_;
            }
            constexpr bool done() const
            {
                return false;
            }
            CONCEPT_REQUIRES(Incrementable<Val>())
            RANGES_RELAXED_CONSTEXPR
            bool equal(iota_view const &that) const
            {
                return that.value_ == value_;
            }
            CONCEPT_REQUIRES(BidirectionalIncrementable<Val>())
            RANGES_RELAXED_CONSTEXPR
            void prev()
            {
                --value_;
            }
            CONCEPT_REQUIRES(RandomAccessIncrementable<Val>())
            RANGES_RELAXED_CONSTEXPR
            void advance(difference_type_ n)
            {
                value_ += n;
            }
            CONCEPT_REQUIRES(RandomAccessIncrementable<Val>())
            RANGES_RELAXED_CONSTEXPR
            difference_type_ distance_to(iota_view const &that) const
            {
                return detail::iota_minus(that.value_, value_);
            }
        public:
            constexpr iota_view() = default;
            constexpr explicit iota_view(Val value)
              : value_(detail::move(value))
            {}
        };

        namespace view
        {
            struct iota_fn
            {
            private:
                template<typename Val>
                static RANGES_RELAXED_CONSTEXPR
                take_exactly_view<iota_view<Val>>
                impl(Val from, Val to, concepts::RandomAccessIncrementable *)
                {
                    return {iota_view<Val>{std::move(from)}, detail::iota_minus(to, from) + 1};
                }
                template<typename Val, typename Val2>
                static RANGES_RELAXED_CONSTEXPR iota_view<Val, Val2>
                impl(Val from, Val2 to, concepts::WeaklyIncrementable *)
                {
                    return {std::move(from), std::move(to)};
                }
            public:
                template<typename Val,
                    CONCEPT_REQUIRES_(WeaklyIncrementable<Val>())>
                RANGES_RELAXED_CONSTEXPR
                iota_view<Val> operator()(Val value) const
                {
                    CONCEPT_ASSERT(WeaklyIncrementable<Val>());
                    return iota_view<Val>{std::move(value)};
                }
                template<typename Val, typename Val2,
                    CONCEPT_REQUIRES_(WeaklyIncrementable<Val>() && EqualityComparable<Val, Val2>())>
                RANGES_RELAXED_CONSTEXPR
                meta::if_<
                    meta::and_<RandomAccessIncrementable<Val>, Same<Val, Val2>>,
                    take_exactly_view<iota_view<Val>>,
                    iota_view<Val, Val2>>
                operator()(Val from, Val2 to) const
                {
                    CONCEPT_ASSERT(EqualityComparable<Val, Val2>());
                    return iota_fn::impl(std::move(from), std::move(to), incrementable_concept<Val>{});
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Val,
                    CONCEPT_REQUIRES_(!WeaklyIncrementable<Val>())>
                RANGES_RELAXED_CONSTEXPR
                void operator()(Val) const
                {
                    CONCEPT_ASSERT_MSG(WeaklyIncrementable<Val>(),
                        "The object passed to view::iota must model the WeaklyIncrementable concept; "
                        "that is, it must have pre- and post-increment operators and it must have a "
                        " difference_type");
                }
                template<typename Val, typename Val2,
                    CONCEPT_REQUIRES_(!(WeaklyIncrementable<Val>() && EqualityComparable<Val, Val2>()))>
                RANGES_RELAXED_CONSTEXPR
                void operator()(Val, Val2) const
                {
                    CONCEPT_ASSERT_MSG(WeaklyIncrementable<Val>(),
                        "The object passed to view::iota must model the WeaklyIncrementable concept; "
                        "that is, it must have pre- and post-increment operators and it must have a "
                        " difference_type");
                    CONCEPT_ASSERT_MSG(EqualityComparable<Val, Val2>(),
                        "The two arguments passed to view::iota must be EqualityComparable with == and !=");
                }
            #endif
            };

            /// \relates iota_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& iota = static_const<iota_fn>::value;
            }

            struct ints_fn
              : iota_view<int>
            {
                using iota_view<int>::iota_view;

                template<typename Val,
                    CONCEPT_REQUIRES_(Integral<Val>())>
                RANGES_RELAXED_CONSTEXPR
                iota_view<Val> operator()(Val value) const
                {
                    return iota_view<Val>{value};
                }
                template<typename Val,
                    CONCEPT_REQUIRES_(Integral<Val>())>
                RANGES_RELAXED_CONSTEXPR
                take_exactly_view<iota_view<Val>> operator()(Val from, Val to) const
                {
                    return {iota_view<Val>{from}, detail::iota_minus(to, from) + 1};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Val,
                    CONCEPT_REQUIRES_(!Integral<Val>())>
                RANGES_RELAXED_CONSTEXPR
                void operator()(Val) const
                {
                    CONCEPT_ASSERT_MSG(Integral<Val>(),
                        "The object passed to view::ints must be Integral");
                }
                template<typename Val,
                    CONCEPT_REQUIRES_(!Integral<Val>())>
                RANGES_RELAXED_CONSTEXPR
                void operator()(Val, Val) const
                {
                    CONCEPT_ASSERT_MSG(Integral<Val>(),
                        "The object passed to view::ints must be Integral");
                }
            #endif
            };

            /// \relates ints_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& ints = static_const<ints_fn>::value;
            }
        }
        /// @}
    }
}

#endif
