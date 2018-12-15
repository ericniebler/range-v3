/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_SUBRANGE_HPP
#define RANGES_V3_VIEW_SUBRANGE_HPP

#include <tuple>
#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/utility/unreachable.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        enum class subrange_kind : bool { unsized, sized };

        /// \cond
        namespace detail
        {
            template<std::size_t N, typename T>
            using tuple_element_fun_t = void(*)(meta::_t<std::tuple_element<N, T>> const &);

            CPP_def
            (
                template(typename A, typename B)
                concept NotSameAs_,
                    not Same<uncvref_t<A>, uncvref_t<B>>
            );
            CPP_def
            (
                template(class From, class To)
                concept ConvertibleToNotSlicing_,
                    ConvertibleTo<From, To> &&
                    // A conversion is a slicing conversion if the source and the destination
                    // are both pointers, and if the pointed-to types differ after removing
                    // cv qualifiers.
                    not (std::is_pointer<decay_t<From>>::value &&
                        std::is_pointer<decay_t<To>>::value &&
                        NotSameAs_<meta::_t<std::remove_pointer<decay_t<From>>>,
                                   meta::_t<std::remove_pointer<decay_t<To>>>>)
            );
            CPP_def
            (
                template(typename T)
                concept PairLikeGCCBugs2_,
                    requires(T t, tuple_element_fun_t<0, T> p0, tuple_element_fun_t<1, T> p1)
                    (
                        p0( get<0>(t) ),
                        p1( get<1>(t) )
                    )
            );
            CPP_def
            (
                template(typename T)
                concept PairLikeGCCBugs_,
                    DerivedFrom<std::tuple_size<T>, meta::size_t<2>> &&
                    PairLikeGCCBugs2_<T>
            );
            CPP_def
            (
                template(typename T)
                concept GetFirstAndSecond_,
                    requires(T &t)
                    (
                        get<0>(t),
                        get<1>(t)
                    )
            );
            CPP_def
            (
                template(typename T)
                concept PairLike_,
                    not std::is_reference<T>::value &&
                    (defer::GetFirstAndSecond_<T> &&
                     defer::PairLikeGCCBugs_<T>)
            );
            // Short-circuit the PairLike concept for things known to be pairs:
            template<typename T>
            struct pair_like
              : meta::bool_<PairLike_<T>>
            {};
            template<typename F, typename S>
            struct pair_like<std::pair<F, S>>
              : meta::bool_<true>
            {};
            template<typename... Ts>
            struct pair_like<std::tuple<Ts...>>
              : meta::bool_<sizeof...(Ts) == 2u>
            {};
            CPP_def
            (
                template(class T, class U, class V)
                concept PairLikeConvertibleFromGCCBugs_,
                    ConvertibleToNotSlicing_<U, meta::_t<std::tuple_element<0, T>>> &&
                    ConvertibleTo<V, meta::_t<std::tuple_element<1, T>>>
            );
            CPP_def
            (
                template(class T, class U, class V)
                concept PairLikeConvertibleFrom_,
                    not Range<T> &&
                    Constructible<T, U, V> &&
                    (ranges::defer::True<pair_like<uncvref_t<T>>::value> &&
                     defer::PairLikeConvertibleFromGCCBugs_<T, U, V>)
            );

            template<typename S, typename I>
            constexpr auto is_sized_sentinel_() noexcept
                -> CPP_ret(bool)(
                    requires Sentinel<S, I>)
            {
                return (bool) SizedSentinel<S, I>;
            }

            template<subrange_kind K, typename S, typename I>
            constexpr bool store_size_() noexcept
            {
                return K == subrange_kind::sized && !(bool) SizedSentinel<S, I>;
            }
        }
        /// \endcond

        template<typename I, typename S = I,
            subrange_kind K = static_cast<subrange_kind>(detail::is_sized_sentinel_<S, I>())>
        struct subrange;

        /// \cond
        namespace _subrange_
        {
            struct adl_hook
            {};

            // A temporary subrange can be safely passed to ranges::begin and ranges::end.
            template<class I, class S, subrange_kind K>
            constexpr I begin(subrange<I, S, K> &&r)
            noexcept(std::is_nothrow_copy_constructible<I>::value)
            {
                return r.begin();
            }
            template<class I, class S, subrange_kind K>
            constexpr I begin(subrange<I, S, K> const &&r)
            noexcept(std::is_nothrow_copy_constructible<I>::value)
            {
                return r.begin();
            }
            template<class I, class S, subrange_kind K>
            constexpr S end(subrange<I, S, K> &&r)
            noexcept(std::is_nothrow_copy_constructible<S>::value)
            {
                return r.end();
            }
            template<class I, class S, subrange_kind K>
            constexpr S end(subrange<I, S, K> const &&r)
            noexcept(std::is_nothrow_copy_constructible<S>::value)
            {
                return r.end();
            }

            template<std::size_t N, class I, class S, subrange_kind K>
            constexpr auto get(subrange<I, S, K> const &r) ->
                CPP_ret(I)(
                    requires N == 0)
            {
                return r.begin();
            }
            template<std::size_t N, class I, class S, subrange_kind K>
            constexpr auto get(subrange<I, S, K> const &r) ->
                CPP_ret(S)(
                    requires N == 1)
            {
                return r.end();
            }
        }
        /// \endcond

        template<typename I, typename S, subrange_kind K>
        struct subrange
          : view_interface<
                subrange<I, S, K>,
                Same<S, unreachable> ? infinite : K == subrange_kind::sized ? finite : unknown>
          , private _subrange_::adl_hook
        {
        private:
            CPP_assert(Iterator<I>);
            CPP_assert(Sentinel<S, I>);
            CPP_assert(K == subrange_kind::sized || not SizedSentinel<S, I>);
            CPP_assert(K != subrange_kind::sized || not Same<S, unreachable>);

            detail::if_then_t<detail::store_size_<K, S, I>(),
                std::tuple<I, S, iter_difference_t<I>>,
                std::tuple<I, S>> data_;

            constexpr I& first_() noexcept
            {
                return std::get<0>(data_);
            }
            constexpr const I& first_() const noexcept
            {
                return std::get<0>(data_);
            }
            constexpr S& last_() noexcept
            {
                return std::get<1>(data_);
            }
            constexpr const S& last_() const noexcept
            {
                return std::get<1>(data_);
            }
            CPP_member
            constexpr auto get_size_() const ->
                CPP_ret(iter_difference_t<I>)(
                    requires K == subrange_kind::sized && not detail::store_size_<K, S, I>())
            {
                return last_() - first_();
            }
            CPP_member
            constexpr auto get_size_() const noexcept ->
                CPP_ret(iter_difference_t<I>)(
                    requires detail::store_size_<K, S, I>())
            {
                return std::get<2>(data_);
            }
            static constexpr void set_size_(...) noexcept
            {}
            CPP_member
            constexpr auto set_size_(iter_difference_t<I> n) noexcept ->
                CPP_ret(void)(
                    requires detail::store_size_<K, S, I>())
            {
                std::get<2>(data_) = n;
            }
        public:
            using iterator = I;
            using sentinel = S;

            subrange() = default;

            template <typename I2>
            constexpr CPP_ctor(subrange)(I2&& i, S s)(
                requires detail::ConvertibleToNotSlicing_<I2, I> &&
                    (!detail::store_size_<K, S, I>()))
              : data_{static_cast<I2&&>(i), std::move(s)}
            {}

            template <typename I2>
            constexpr CPP_ctor(subrange)(I2&& i, S s, iter_difference_t<I> n)(
                requires detail::ConvertibleToNotSlicing_<I2, I> &&
                    detail::store_size_<K, S, I>())
              : data_{static_cast<I2&&>(i), std::move(s), n}
            {
                if RANGES_CONSTEXPR_IF ((bool) RandomAccessIterator<I>)
                {
                    RANGES_EXPECT(ranges::next(first_(), n) == last_());
                }
            }
            template <typename I2>
            constexpr CPP_ctor(subrange)(I2&& i, S s, iter_difference_t<I> n)(
                requires detail::ConvertibleToNotSlicing_<I2, I> &&
                    SizedSentinel<S, I>)
              : data_{static_cast<I2&&>(i), std::move(s)}
            {
                RANGES_EXPECT(last_() - first_() == n);
            }

            template<typename R>
            constexpr CPP_ctor(subrange)(R&& r) (
                requires detail::defer::NotSameAs_<R, subrange> &&
                    defer::ForwardingRange_<R> &&
                    detail::defer::ConvertibleToNotSlicing_<iterator_t<R>, I> &&
                    defer::ConvertibleTo<sentinel_t<R>, S> &&
                    defer::True<!detail::store_size_<K, S, I>()>)
              : subrange{ranges::begin(r), ranges::end(r)}
            {}

            template<typename R>
            constexpr CPP_ctor(subrange)(R&& r) (
                requires detail::defer::NotSameAs_<R, subrange> &&
                    defer::ForwardingRange_<R> &&
                    detail::defer::ConvertibleToNotSlicing_<iterator_t<R>, I> &&
                    defer::ConvertibleTo<sentinel_t<R>, S> &&
                    defer::True<detail::store_size_<K, S, I>()> &&
                    defer::SizedRange<R>)
              : subrange{ranges::begin(r), ranges::end(r), ranges::distance(r)}
            {}

            template<typename R>
            constexpr CPP_ctor(subrange)(R&& r, iter_difference_t<I> n) (
                requires defer::ForwardingRange_<R> &&
                    detail::defer::ConvertibleToNotSlicing_<iterator_t<R>, I> &&
                    defer::ConvertibleTo<sentinel_t<R>, S> &&
                    defer::True<K == subrange_kind::sized>)
              : subrange{ranges::begin(r), ranges::end(r), n}
            {
                if RANGES_CONSTEXPR_IF ((bool) SizedRange<R>)
                {
                    RANGES_EXPECT(n == ranges::distance(r));
                }
            }

            /// Implicit conversion to something that looks like a container.
            CPP_template(typename Container)(
                requires detail::ConvertibleToContainer<subrange const, Container>)
            constexpr operator Container () const
            {
                return ranges::to_<Container>(*this);
            }

            CPP_template(typename PairLike)(
                requires detail::defer::NotSameAs_<PairLike, subrange> &&
                    detail::defer::PairLikeConvertibleFrom_<PairLike, const I&, const S&>)
            constexpr operator PairLike() const
            {
                return PairLike(first_(), last_());
            }

            constexpr I begin() const noexcept(std::is_nothrow_copy_constructible<I>::value)
            {
                return first_();
            }
            constexpr S end() const noexcept(std::is_nothrow_copy_constructible<S>::value)
            {
                return last_();
            }
            constexpr bool empty() const
            {
                return first_() == last_();
            }

            CPP_member
            constexpr auto size() const ->
                CPP_ret(iter_difference_t<I>)(
                    requires K == subrange_kind::sized)
            {
                return get_size_();
            }

            RANGES_NODISCARD
            constexpr subrange next(iter_difference_t<I> n = 1) const
            {
                auto tmp = *this;
                tmp.advance(n);
                return tmp;
            }

            CPP_member
            RANGES_NODISCARD
            constexpr auto prev(iter_difference_t<I> n = 1) const ->
                CPP_ret(subrange)(
                    requires BidirectionalIterator<I>)
            {
                auto tmp = *this;
                tmp.advance(-n);
                return tmp;
            }

            constexpr subrange& advance(iter_difference_t<I> n)
            {
                set_size_(get_size_() - (n - ranges::advance(first_(), n, last_())));
                return *this;
            }
        };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        template<class I, class S>
        subrange(I, S) -> subrange<I, S>;

        CPP_template(class I, class S)(
            requires Iterator<I> && Sentinel<S, I>)
        subrange(I, S, iter_difference_t<I>) -> subrange<I, S, subrange_kind::sized>;

        CPP_template(class R)(
            requires ForwardingRange_<R>)
        subrange(R &&) -> subrange<iterator_t<R>, sentinel_t<R>,
            ((bool) SizedRange<R> || (bool) SizedSentinel<sentinel_t<R>, iterator_t<R>>)
                ? subrange_kind::sized : subrange_kind::unsized>;

        CPP_template(class R)(
            requires ForwardingRange_<R>)
        subrange(R &&, iter_difference_t<iterator_t<R>>) ->
            subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>;
#endif

        // in leu of deduction guides, use make_subrange
        struct make_subrange_fn
        {
            template<class I, class S>
            constexpr auto operator()(I i, S s) const ->
                subrange<I, S>
            {
                return {i, s};
            }
            template<class I, class S>
            constexpr auto operator()(I i, S s, iter_difference_t<I> n) const ->
                CPP_ret(subrange<I, S, subrange_kind::sized>)(
                    requires Iterator<I> && Sentinel<S, I>)
            {
                return {i, s, n};
            }
            template<class R>
            constexpr auto operator()(R &&r) const ->
                CPP_ret(subrange<iterator_t<R>, sentinel_t<R>,
                    ((bool) SizedRange<R> || (bool) SizedSentinel<sentinel_t<R>, iterator_t<R>>)
                        ? subrange_kind::sized : subrange_kind::unsized>)(
                    requires ForwardingRange_<R>)
            {
                return {(R &&) r};
            }
            template<class R>
            constexpr auto operator()(R &&r, iter_difference_t<iterator_t<R>> n) const ->
                CPP_ret(subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>)(
                    requires ForwardingRange_<R>)
            {
                return {(R &&) r, n};
            }
        };

        /// \relates make_subrange_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(make_subrange_fn, make_subrange)

        template<class R>
        using safe_subrange_t =
            detail::maybe_dangling_<R, subrange<iterator_t<R>>>;
        /// @}
    }
}

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

namespace std
{
	template<class I, class S, ::ranges::subrange_kind K>
	struct tuple_size<::ranges::subrange<I, S, K>>
	  : std::integral_constant<size_t, 2>
    {};
	template<class I, class S, ::ranges::subrange_kind K>
	struct tuple_element<0, ::ranges::subrange<I, S, K>>
	{
        using type = I;
    };
	template<class I, class S, ::ranges::subrange_kind K>
	struct tuple_element<1, ::ranges::subrange<I, S, K>>
	{
        using type = S;
    };
}

RANGES_DIAGNOSTIC_POP

#endif
