/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2017
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_SUBRANGE_HPP
#define RANGES_V3_SUBRANGE_HPP

#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/dangling.hpp>

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_CXX17_EXTENSIONS

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct PairLike1
            {
                template<typename T>
                auto requires_() -> decltype(concepts::valid_expr(
                    concepts::model_of<concepts::Integral, decltype(std::tuple_size<T>::value)>(),
                    concepts::is_true(meta::bool_<std::tuple_size<T>::value == 2>())
                ));
            };

            struct PairLike2
            {
                template<typename T>
                auto requires_() -> decltype(concepts::valid_expr(
                    concepts::is_true(meta::is_trait<std::tuple_element<0, T>>()),
                    concepts::is_true(meta::is_trait<std::tuple_element<1, T>>())
                ));
            };

            struct PairLike3
            {
                template<typename T>
                auto requires_(T &&t) -> decltype(concepts::valid_expr(
                    concepts::convertible_to<meta::_t<std::tuple_element<0, T>> const &>(std::get<0>(t)),
                    concepts::convertible_to<meta::_t<std::tuple_element<1, T>> const &>(std::get<1>(t))
                ));
            };

            template<typename T>
            using PairLike = meta::and_<
                Same<T, uncvref_t<T>>,
                concepts::models<PairLike1, T>,
                concepts::models<PairLike2, T>,
                concepts::models<PairLike3, T>>;

            struct PairLikeConvertibleTo1
            {
                template<typename T, typename U, typename V>
                auto requires_(T &&t) -> decltype(concepts::valid_expr(
                    concepts::convertible_to<U>(std::get<0>((T &&) t)),
                    concepts::convertible_to<V>(std::get<1>((T &&) t))
                ));
            };

            template<typename T, typename U, typename V>
            using PairLikeConvertibleTo = meta::and_<
                meta::not_<Range<T>>,
                PairLike<uncvref_t<T>>,
                concepts::models<PairLikeConvertibleTo1, T, U, V>>;

            template<typename T, typename U, typename V>
            using PairLikeConvertibleFrom = meta::and_<
                meta::not_<Range<T>>,
                PairLike<T>,
                Constructible<uncvref_t<T>, U, V>>;

            template<typename T>
            using IteratorSentinelPair1 = Sentinel<
                meta::_t<std::tuple_element<1, T>>,
                meta::_t<std::tuple_element<0, T>>>;

            template<typename T>
            using IteratorSentinelPair = meta::and_<
                meta::not_<Range<T>>,
                PairLike<T>,
                meta::defer<IteratorSentinelPair1, T>>;
        } // namespace detail

        enum class subrange_kind : bool { unsized, sized };

        template<typename I, typename S = I,
            subrange_kind K = static_cast<subrange_kind>((bool) SizedSentinel<S, I>()),
            meta::if_c<Sentinel<S, I>() &&
                (K == subrange_kind::sized || !SizedSentinel<S, I>())>* = nullptr>
        struct subrange
          : view_interface<subrange<I, S, K>>
        {
        private:
            static constexpr bool StoreSize =
                K == subrange_kind::sized && !SizedSentinel<S, I>();

            compressed_tuple<I, S,
                meta::if_c<StoreSize, difference_type_t<I>, meta::nil_>
            > data_{};

            RANGES_CXX14_CONSTEXPR I &first_() noexcept { return get<0>(data_); }
            constexpr I const &first_() const noexcept { return get<0>(data_); }

            RANGES_CXX14_CONSTEXPR S &last_() noexcept { return get<1>(data_); }
            constexpr S const &last_() const noexcept { return get<1>(data_); }

            RANGES_CXX14_CONSTEXPR difference_type_t<I> &size_() noexcept
            {
                CONCEPT_ASSERT(StoreSize);
                return get<2>(data_);
            }
            constexpr difference_type_t<I> const &size_() const noexcept
            {
                CONCEPT_ASSERT(StoreSize);
                return get<2>(data_);
            }
            constexpr difference_type_t<I> size_helper(std::true_type) const
            {
                CONCEPT_ASSERT(StoreSize);
                return size_();
            }
            constexpr difference_type_t<I> size_helper(std::false_type) const
            {
                CONCEPT_ASSERT(SizedSentinel<S, I>());
                return last_() - first_();
            }
            RANGES_CXX14_CONSTEXPR
            void modify_size(difference_type_t<I> delta, std::true_type) noexcept
            {
                CONCEPT_ASSERT(StoreSize);
                size_() += delta;
            }
            RANGES_CXX14_CONSTEXPR
            void modify_size(difference_type_t<I>, std::false_type) noexcept
            {
                CONCEPT_ASSERT(!StoreSize);
            }
        public:
            using iterator = I;
            using sentinel = S;

            subrange() = default;

            CONCEPT_REQUIRES(!StoreSize)
            constexpr subrange(I i, S s)
              : data_{detail::move(i), detail::move(s), meta::nil_{}}
            {}

            CONCEPT_REQUIRES(StoreSize)
            constexpr subrange(I i, S s, difference_type_t<I> n)
              : data_{(RANGES_ASSERT(!RandomAccessIterator<I>() ||
                    ranges::next(i, n) == s), detail::move(i)),
                    detail::move(s), n}
            {}

            CONCEPT_REQUIRES(SizedSentinel<S, I>())
            constexpr subrange(I i, S s, difference_type_t<I> n)
              : data_{(RANGES_ASSERT(s - i == n), detail::move(i)),
                    detail::move(s), ((void)n, meta::nil_{})}
            {}

            template<typename R,
                CONCEPT_REQUIRES_(!StoreSize &&
                    ReferenceableRange<R>() &&
                    ConvertibleTo<iterator_t<R>, I>() &&
                    ConvertibleTo<sentinel_t<R>, S>())>
            constexpr subrange(R &&r)
              : subrange{ranges::begin(r), ranges::end(r)}
            {}

            template<typename R,
                CONCEPT_REQUIRES_(StoreSize &&
                    ReferenceableRange<R>() &&
                    SizedRange<R>() &&
                    ConvertibleTo<iterator_t<R>, I>() &&
                    ConvertibleTo<sentinel_t<R>, S>())>
            constexpr subrange(R &&r)
              : subrange{ranges::begin(r), ranges::end(r), ranges::distance(r)}
            {}

            template<typename R,
                CONCEPT_REQUIRES_(K == subrange_kind::sized &&
                    ReferenceableRange<R>() &&
                    ConvertibleTo<iterator_t<R>, I>() &&
                    ConvertibleTo<sentinel_t<R>, S>())>
            constexpr subrange(R &&r, difference_type_t<I> n)
              : subrange{ranges::begin(r), ranges::end(r),
                    (RANGES_ASSERT(!SizedRange<R>() || n == ranges::distance(r)), n)}
            {}

            template<class PairLike,
                CONCEPT_REQUIRES_(!StoreSize &&
                    detail::PairLikeConvertibleTo<PairLike, I, S>())>
            constexpr subrange(PairLike &&r)
            : subrange{std::get<0>(static_cast<PairLike &&>(r)),
                std::get<1>(static_cast<PairLike &&>(r))}
            {}

            template<class PairLike,
                CONCEPT_REQUIRES_(K == subrange_kind::sized &&
                    detail::PairLikeConvertibleTo<PairLike, I, S>())>
            constexpr subrange(PairLike &&r, difference_type_t<I> n)
            : subrange{std::get<0>(static_cast<PairLike &&>(r)),
                std::get<1>(static_cast<PairLike &&>(r)), n}
            {}

            template<typename PairLike,
                CONCEPT_REQUIRES_(!Range<PairLike>() &&
                    detail::PairLikeConvertibleFrom<PairLike, I const &, S const &>())>
            constexpr operator PairLike() const
            {
                return PairLike(first_(), last_());
            }

            constexpr I begin() const
                noexcept(std::is_nothrow_copy_constructible<I>::value)
            {
                return first_();
            }
            constexpr S end() const
                noexcept(std::is_nothrow_copy_constructible<S>::value)
            {
                return last_();
            }
            constexpr bool empty() const
            {
                return first_() == last_();
            }

            CONCEPT_REQUIRES(K == subrange_kind::sized)
            RANGES_CXX14_CONSTEXPR difference_type_t<I> size() const
            {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
                if constexpr (StoreSize)
                    return size_();
                else
                    return last_() - first_();
#else
                return size_helper(meta::bool_<StoreSize>());
#endif
            }

            RANGES_NODISCARD RANGES_CXX14_CONSTEXPR
            subrange next(difference_type_t<I> n = 1) const
            {
                auto tmp = *this;
                tmp.advance(n);
                return tmp;
            }
            CONCEPT_REQUIRES(BidirectionalIterator<I>())
            RANGES_NODISCARD RANGES_CXX14_CONSTEXPR
            subrange prev(difference_type_t<I> n = 1) const
            {
                auto tmp = *this;
                tmp.advance(-n);
                return tmp;
            }
            RANGES_CXX14_CONSTEXPR subrange &advance(difference_type_t<I> n)
            {
                difference_type_t<I> remainder = ranges::advance(first_(), n, last_());
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
                (void)remainder;
                if constexpr (StoreSize)
                    size_() -= n - remainder;
#else
                modify_size(remainder - n, meta::bool_<StoreSize>{});
#endif
                return *this;
            }
        };

        template<typename I, typename S, subrange_kind K>
        struct is_referenceable_range<subrange<I, S, K>> : std::true_type {};

        template<typename I, typename S = I, CONCEPT_REQUIRES_(Sentinel<S, I>())>
        using sized_subrange = subrange<I, S, subrange_kind::sized>;

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        template<typename I, typename S, CONCEPT_REQUIRES_(Sentinel<S, I>())>
        subrange(I, S, difference_type_t<I>) -> subrange<I, S, subrange_kind::sized>;

        template<typename P, CONCEPT_REQUIRES_(detail::IteratorSentinelPair<P>())>
        subrange(P) -> subrange<
            meta::_t<std::tuple_element<0, P>>,
            meta::_t<std::tuple_element<1, P>>>;

        template<typename P, CONCEPT_REQUIRES_(detail::IteratorSentinelPair<P>())>
        subrange(P, difference_type_t<meta::_t<std::tuple_element<0, P>>>) ->
            subrange<meta::_t<std::tuple_element<0, P>>,
                meta::_t<std::tuple_element<1, P>>, subrange_kind::sized>;

        template<typename R, CONCEPT_REQUIRES_(ReferenceableRange<R>())>
        subrange(R &&) -> subrange<iterator_t<R>, sentinel_t<R>,
            static_cast<subrange_kind>(SizedRange<R>() ||
                SizedSentinel<sentinel_t<R>, iterator_t<R>>())>;

        template<typename R, CONCEPT_REQUIRES_(ReferenceableRange<R>())>
        subrange(R &&, range_difference_type_t<R>) ->
            subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>;
#endif // RANGES_CXX_DEDUCTION_GUIDES

        template<typename R, CONCEPT_REQUIRES_(Range<R>())>
        using safe_subrange_t = meta::if_<
            ReferenceableRange<R>,
            subrange<iterator_t<R>>,
            dangling<subrange<iterator_t<R>>>>;

#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17 && \
    RANGES_CXX_RETURN_TYPE_DEDUCTION >= RANGES_CXX_RETURN_TYPE_DEDUCTION_14 && \
    RANGES_CXX_CONSTEXPR >= RANGES_CXX_CONSTEXPR_14
        template<std::size_t N, typename I, typename S, subrange_kind K,
            CONCEPT_REQUIRES_(N < 2)>
        constexpr auto get(subrange<I, S, K> const &r)
        {
            if constexpr (N == 0)
                return r.begin();
            else
                return r.end();
        }
#else
        template<std::size_t N, typename I, typename S, subrange_kind K,
            CONCEPT_REQUIRES_(N == 0)>
        constexpr I get(subrange<I, S, K> const &r)
        {
            return r.begin();
        }

        template<std::size_t N, typename I, typename S, subrange_kind K,
            CONCEPT_REQUIRES_(N == 1)>
        constexpr S get(subrange<I, S, K> const &r)
        {
            return r.end();
        }
#endif

        template<typename I, typename S, CONCEPT_REQUIRES_(Sentinel<S, I>())>
        constexpr auto make_subrange(I i, S s)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            subrange<I, S>{detail::move(i), detail::move(s)}
        )
        template<typename I, typename S, CONCEPT_REQUIRES_(Sentinel<S, I>())>
        constexpr auto make_subrange(I i, S s, difference_type_t<I> n)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            subrange<I, S, subrange_kind::sized>{
                detail::move(i), detail::move(s), n
            }
        )

        template<typename P, CONCEPT_REQUIRES_(detail::IteratorSentinelPair<P>())>
        constexpr auto make_subrange(P p)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            subrange<meta::_t<std::tuple_element<0, P>>,
                meta::_t<std::tuple_element<1, P>>>{
                    detail::move(p)
                }
        )

        template<typename P, CONCEPT_REQUIRES_(detail::IteratorSentinelPair<P>())>
        constexpr auto make_subrange(
            P p, difference_type_t<meta::_t<std::tuple_element<0, P>>> n)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            subrange<meta::_t<std::tuple_element<0, P>>,
                meta::_t<std::tuple_element<1, P>>, subrange_kind::sized>{
                    detail::move(p), n
                }
        )

        template<typename R, CONCEPT_REQUIRES_(ReferenceableRange<R>())>
        constexpr auto make_subrange(R &&r)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            subrange<iterator_t<R>, sentinel_t<R>,
                static_cast<subrange_kind>(SizedRange<R>() ||
                    SizedSentinel<sentinel_t<R>, iterator_t<R>>())>{
                        static_cast<R &&>(r)
                    }
        )
        template<typename R, CONCEPT_REQUIRES_(ReferenceableRange<R>())>
        constexpr auto make_subrange(R &&r, range_difference_type_t<R> n)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>{
                static_cast<R &&>(r), n
            }
        )
    }
}

RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

namespace std {
    template<typename I, typename S, ::ranges::v3::subrange_kind K>
    struct tuple_size<::ranges::v3::subrange<I, S, K>>
      : std::integral_constant<size_t, 2> {};
    template<typename I, typename S, ::ranges::v3::subrange_kind K>
    struct tuple_element<0, ::ranges::v3::subrange<I, S, K>>
    { using type = I; };
    template<typename I, typename S, ::ranges::v3::subrange_kind K>
    struct tuple_element<1, ::ranges::v3::subrange<I, S, K>>
    { using type = S; };
}

RANGES_DIAGNOSTIC_POP

#endif
