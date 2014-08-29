//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_UTILITY_FUNCTIONAL_HPP
#define RANGES_V3_UTILITY_FUNCTIONAL_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct equal_to
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(EqualityComparable<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t == (U &&) u;
            }
        };

        struct less
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(WeaklyOrdered<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t < (U &&) u;
            }
        };

        struct ordered_less
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return (T &&) t < (U &&) u;
            }
        };

        struct ident
        {
            template<typename T>
            T operator()(T && t) const
            {
                return (T &&) t;
            }
        };

        struct plus
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(Addable<T, U>())>
            auto operator()(T && t, U && u) const ->
                decltype((T &&) t + (U &&) u)
            {
                return (T &&) t + (U &&) u;
            }
        };

        struct minus
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(Subtractable<T, U>())>
            auto operator()(T && t, U && u) const ->
                decltype((T &&) t - (U &&) u)
            {
                return (T &&) t - (U &&) u;
            }
        };

        struct multiplies
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(Multiplicable<T, U>())>
            auto operator()(T && t, U && u) const ->
                decltype((T &&) t * (U &&) u)
            {
                return (T &&) t * (U &&) u;
            }
        };

        template<typename Pred>
        struct logical_negate
        {
        private:
            Pred pred_;
        public:
            logical_negate() = default;

            explicit constexpr logical_negate(Pred pred)
              : pred_((Pred &&) pred)
            {}

            template<typename T,
                CONCEPT_REQUIRES_(Predicate<Pred, T>())>
            constexpr bool operator()(T && t) const
            {
                return !pred_((T &&) t);
            }

            template<typename T, typename U,
                CONCEPT_REQUIRES_(Predicate<Pred, T, U>())>
            constexpr bool operator()(T && t, U && u) const
            {
                return !pred_((T &&) t, (U &&) u);
            }
        };

        struct not_fn
        {
            template<typename Pred>
            constexpr logical_negate<Pred> operator()(Pred pred) const
            {
                return logical_negate<Pred>{(Pred &&) pred};
            }
        };

        RANGES_CONSTEXPR not_fn not_ {};
    }
}

#endif
