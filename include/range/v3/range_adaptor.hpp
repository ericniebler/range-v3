/// \file
// Range v3 library
//
// Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_RANGE_ADAPTOR_HPP
#define RANGES_V3_RANGE_ADAPTOR_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Derived>
            using begin_adaptor_t =
                decltype(range_access::begin_adaptor(std::declval<Derived &>(), 42));

            template<typename Derived>
            using end_adaptor_t =
                decltype(range_access::end_adaptor(std::declval<Derived &>(), 42));

            template<typename Derived>
            using adapted_iterator_t =
                decltype(std::declval<begin_adaptor_t<Derived>>().begin(std::declval<Derived &>()));

            template<typename Derived>
            using adapted_sentinel_t =
                decltype(std::declval<end_adaptor_t<Derived>>().end(std::declval<Derived &>()));

            struct adaptor_base_current_mem_fn
            {};

            template<typename BaseIter, typename Adapt, typename Enable = void>
            struct adaptor_value_type2
            {};

            template<typename BaseIter, typename Adapt>
            struct adaptor_value_type2<
                BaseIter,
                Adapt,
                meta::void_<decltype(Adapt::current(BaseIter{}, adaptor_base_current_mem_fn{}))>>
            {
                using value_type = iterator_value_t<BaseIter>;
            };

            template<typename BaseIter, typename Adapt, typename Enable = void>
            struct adaptor_value_type
              : adaptor_value_type2<BaseIter, Adapt>
            {};

            template<typename BaseIter, typename Adapt>
            struct adaptor_value_type<BaseIter, Adapt, meta::void_<typename Adapt::value_type>>
            {
                using value_type = typename Adapt::value_type;
            };
        }
        /// \endcond

        /// \addtogroup group-core
        /// @{
        template<typename Derived>
        using base_range_t = meta::eval<range_access::base_range<Derived>>;

        template<typename Derived>
        using range_adaptor_t = meta::eval<range_access::range_adaptor<Derived>>;

        template<typename BaseIt, typename Adapt>
        struct adaptor_cursor;

        template<typename BaseSent, typename Adapt>
        struct adaptor_sentinel;

        struct adaptor_base
        {
            RANGES_RELAXED_CONSTEXPR adaptor_base() = default;
            RANGES_RELAXED_CONSTEXPR adaptor_base(adaptor_base &&) = default;
            RANGES_RELAXED_CONSTEXPR adaptor_base(adaptor_base const &) = default;
            RANGES_RELAXED_CONSTEXPR adaptor_base &operator=(adaptor_base &&) = default;
            RANGES_RELAXED_CONSTEXPR adaptor_base &operator=(adaptor_base const &) = default;

            RANGES_RELAXED_CONSTEXPR adaptor_base(detail::any, detail::any = {}, detail::any = {})
            {}
            template<typename Rng>
            RANGES_RELAXED_CONSTEXPR range_iterator_t<base_range_t<Rng>> begin(Rng &rng) const
            {
                return ranges::begin(rng.base());
            }
            template<typename Rng>
            RANGES_RELAXED_CONSTEXPR range_sentinel_t<base_range_t<Rng>> end(Rng &rng) const
            {
                return ranges::end(rng.base());
            }
            template<typename I, CONCEPT_REQUIRES_(Iterator<I>())>
            static RANGES_RELAXED_CONSTEXPR bool equal(I const &it0, I const &it1)
            {
                return it0 == it1;
            }
            template<typename I, CONCEPT_REQUIRES_(WeakIterator<I>())>
            static RANGES_RELAXED_CONSTEXPR iterator_reference_t<I> current(I const &it, detail::adaptor_base_current_mem_fn = {})
                noexcept(noexcept(iterator_reference_t<I>(*it)))
            {
                return *it;
            }
            template<typename I, CONCEPT_REQUIRES_(WeakIterator<I>())>
            static RANGES_RELAXED_CONSTEXPR void next(I &it)
            {
                ++it;
            }
            template<typename I, CONCEPT_REQUIRES_(BidirectionalIterator<I>())>
            static RANGES_RELAXED_CONSTEXPR void prev(I &it)
            {
                --it;
            }
            template<typename I, CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            static RANGES_RELAXED_CONSTEXPR void advance(I &it, iterator_difference_t<I> n)
            {
                it += n;
            }
            template<typename I, CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            static RANGES_RELAXED_CONSTEXPR iterator_difference_t<I> distance_to(I const &it0, I const &it1)
            {
                return it1 - it0;
            }
            template<typename I, typename S, CONCEPT_REQUIRES_(IteratorRange<I, S>())>
            static constexpr bool empty(I const &it, S const &end)
            {
                return it == end;
            }
        };

        // Build a cursor out of an iterator into the adapted range, and an
        // adaptor that customizes behavior.
        template<typename BaseIter, typename Adapt>
        struct adaptor_cursor
          : private compressed_pair<BaseIter, Adapt>
          , detail::adaptor_value_type<BaseIter, Adapt>
        {
            using single_pass = meta::or_<
                range_access::single_pass_t<Adapt>,
                SinglePass<BaseIter>>;
            using compressed_pair<BaseIter, Adapt>::compressed_pair;
            struct mixin : basic_mixin<adaptor_cursor>
            {
                using basic_mixin<adaptor_cursor>::basic_mixin;
                // All iterators into adapted ranges have a base() member for fetching
                // the underlying iterator.
                RANGES_RELAXED_CONSTEXPR BaseIter base() const
                {
                    return this->get().first;
                }
            };
        private:
            template<typename BaseSent, typename SentAdapt>
            friend struct adaptor_sentinel;
            using compressed_pair<BaseIter, Adapt>::first;
            using compressed_pair<BaseIter, Adapt>::second;
            template<typename A = Adapt,
                typename R = decltype(std::declval<A>().equal(first, first, second))>
            RANGES_RELAXED_CONSTEXPR bool equal_(adaptor_cursor const &that, int) const
            {
                return second.equal(first, that.first, that.second);
            }
            template<typename A = Adapt,
                typename R = decltype(std::declval<A>().equal(first, first))>
            RANGES_RELAXED_CONSTEXPR bool equal_(adaptor_cursor const &that, long) const
            {
                return second.equal(first, that.first);
            }
            template<typename A = Adapt,
                typename R = decltype(std::declval<A>().distance_to(first, first, second))>
            RANGES_RELAXED_CONSTEXPR R distance_to_(adaptor_cursor const &that, int) const
            {
                return second.distance_to(first, that.first, that.second);
            }
            template<typename A = Adapt,
                typename R = decltype(std::declval<A>().distance_to(first, first))>
            RANGES_RELAXED_CONSTEXPR R distance_to_(adaptor_cursor const &that, long) const
            {
                return second.distance_to(first, that.first);
            }
            // If the adaptor has an indirect_move function, use it.
            template<typename A = Adapt,
                typename X = decltype(std::declval<A>().indirect_move(first))>
            X indirect_move_(int) const
                noexcept(noexcept(std::declval<A>().indirect_move(first)))
            {
                using V = range_access::cursor_value_t<adaptor_cursor>;
                using R = decltype(std::declval<A>().current(first));
                static_assert(
                    CommonReference<X &&, V const &>(),
                    "In your adaptor, the result of your indirect_move member function does "
                    "not share a common reference with your value type.");
                static_assert(
                    CommonReference<R &&, X &&>(),
                    "In your adaptor, the result of your indirect_move member function does "
                    "not share a common reference with the result of your current member "
                    "function.");
                return second.indirect_move(first);
            }
            // If there is no indirect_move member and the adaptor has not overridden the current
            // member function, then dispatch to the base iterator's indirect_move function.
            template<typename A = Adapt,
                typename R = decltype(std::declval<A>().current(first, detail::adaptor_base_current_mem_fn{})),
                typename X = iterator_rvalue_reference_t<BaseIter>>
            RANGES_RELAXED_CONSTEXPR X indirect_move_(long) const
                noexcept(noexcept(X(ranges::indirect_move(first))))
            {
                return ranges::indirect_move(first);
            }
            // If the adaptor does not have an indirect_move function but overrides the current
            // member function, apply std::move to the result of calling current.
            template<typename A = Adapt,
                typename R = decltype(std::declval<A>().current(first)),
                typename X =
                    meta::if_<std::is_reference<R>, meta::eval<std::remove_reference<R>> &&, R>>
            X indirect_move_(detail::any) const
                noexcept(noexcept(X(static_cast<X &&>(std::declval<R>()))))
            {
                using V = range_access::cursor_value_t<adaptor_cursor>;
                static_assert(
                    CommonReference<X &&, V const &>(),
                    "In your adaptor, you've specified a value type that does not "
                    "share a common reference type with the result of moving the value "
                    "returned by current. Consider defining an indirect_move function "
                    "in your adaptor.");
                return static_cast<X &&>(second.current(first));
            }
            // Gives users a way to override the default indirect_move function in their adaptors.
            template<typename Sent, typename M = mixin>
            RANGES_RELAXED_CONSTEXPR
            friend auto indirect_move(basic_iterator<adaptor_cursor, Sent> const &it)
                noexcept(noexcept(std::declval<M const &>().get().indirect_move_(42))) ->
                decltype(std::declval<M const &>().get().indirect_move_(42))
            {
                return get_cursor(it).indirect_move_(42);
            }
        public:

            // TODO: [constexpr] this is a workaround around the eager template
            // instantiations in constexpr functions: basically as long as the
            // code does only type computations you put it in a non
            // constexpr function and then "lazily" evaluated in a decltype
            // of a default template parameter of the original function
            template<typename A = Adapt,
                     typename R = decltype(std::declval<A>().current(first))>
            static auto not_fail()  -> void {
            using V = range_access::cursor_value_t<adaptor_cursor>;
            static_assert(
            CommonReference<R &&, V &>(),
                "In your adaptor, you've specified a value type that does not "
                    "share a common reference type with the return type of current.");
            }

            template<typename A = Adapt,
                     typename R = decltype(std::declval<A>().current(first)),
                     typename W = decltype(not_fail())>
            // TODO: [constexpr] eagerly instantiating this function breaks everything
            RANGES_RELAXED_CONSTEXPR R current() const
                noexcept(noexcept(R(std::declval<A>().current(first))))
            {
                return second.current(first);
            }
            template<typename A = Adapt,
                typename R = decltype(std::declval<A>().next(first))>
            RANGES_RELAXED_CONSTEXPR void next()
            {
                second.next(first);
            }
            template<typename C = adaptor_cursor>
            RANGES_RELAXED_CONSTEXPR auto equal(adaptor_cursor const &that) const ->
                decltype(std::declval<C const &>().equal_(that, 42))
            {
                return this->equal_(that, 42);
            }
            template<typename A = Adapt,
                typename R = decltype(std::declval<A>().prev(first))>
            RANGES_RELAXED_CONSTEXPR void prev()
            {
                second.prev(first);
            }
            template<typename A = Adapt,
                typename R = decltype(std::declval<A>().advance(first, 0))>
            RANGES_RELAXED_CONSTEXPR
            void advance(iterator_difference_t<BaseIter> n)
            {
                second.advance(first, n);
            }
            template<typename C = adaptor_cursor>
            RANGES_RELAXED_CONSTEXPR
            auto distance_to(adaptor_cursor const &that) const ->
                decltype(std::declval<C const &>().distance_to_(that, 42))
            {
                return this->distance_to_(that, 42);
            }
        };

        // Build a sentinel out of a sentinel into the adapted range, and an
        // adaptor that customizes behavior.
        template<typename BaseSent, typename Adapt>
        struct adaptor_sentinel
          : private compressed_pair<BaseSent, Adapt>
        {
            using single_pass = range_access::single_pass_t<Adapt>;
            using compressed_pair<BaseSent, Adapt>::compressed_pair;
        private:
            using compressed_pair<BaseSent, Adapt>::first;
            using compressed_pair<BaseSent, Adapt>::second;
            template<typename I, typename IA, typename A = Adapt,
                typename R = decltype(std::declval<A>().empty(std::declval<I>(), std::declval<IA>(), first))>
            constexpr bool equal_(adaptor_cursor<I, IA> const &that, int) const
            {
                return second.empty(that.first, that.second, first);
            }
            template<typename I, typename IA, typename A = Adapt,
                typename R = decltype(std::declval<A>().empty(std::declval<I>(), first))>
            constexpr bool equal_(adaptor_cursor<I, IA> const &that, long) const
            {
                return second.empty(that.first, first);
            }
        public:
            struct mixin
              : basic_mixin<adaptor_sentinel>
            {
                using basic_mixin<adaptor_sentinel>::basic_mixin;
                // All iterators into adapted ranges have a base() member for fetching
                // the underlying iterator.
                RANGES_RELAXED_CONSTEXPR BaseSent base() const
                {
                    return this->get().first;
                }
            };
            template<typename I, typename IA, typename S = adaptor_sentinel>
            constexpr auto equal(adaptor_cursor<I, IA> const &that) const ->
                decltype(std::declval<S const &>().equal_(that, 42))
            {
                return this->equal_(that, 42);
            }
        };

        template<typename D>
        using adaptor_cursor_t =
            adaptor_cursor<detail::adapted_iterator_t<D>, detail::begin_adaptor_t<D>>;

        template<typename D>
        using adaptor_sentinel_t =
            meta::if_<
                meta::and_<
                    Same<detail::adapted_iterator_t<D>, detail::adapted_sentinel_t<D>>,
                    Same<detail::begin_adaptor_t<D>, detail::end_adaptor_t<D>>>,
                adaptor_cursor<detail::adapted_iterator_t<D>, detail::begin_adaptor_t<D>>,
                adaptor_sentinel<detail::adapted_sentinel_t<D>, detail::end_adaptor_t<D>>>;

        template<typename Derived, typename BaseRng, bool Inf /*= is_infinite<BaseRng>::value*/>
        struct range_adaptor
          : range_facade<Derived, Inf>
        {
        private:
            friend Derived;
            friend range_access;
            friend adaptor_base;
            using range_adaptor_t = range_adaptor;
            using base_range_t = view::all_t<BaseRng>;
            using range_facade<Derived, Inf>::derived;
            // TODO: [constexpr] rng_ is not mutable anymore!
            // Mutable here. Const-correctness is enforced below by disabling
            // begin_cursor/end_cursor if "BaseRng const" does not model
            // the Range concept.
            /*mutable*/ base_range_t rng_;

            RANGES_RELAXED_CONSTEXPR
            range_adaptor& mut_this() const { return *const_cast<range_adaptor*>(this);}

            RANGES_RELAXED_CONSTEXPR
            base_range_t & mutable_base() const
            {
                return mut_this().rng_;
            }
            RANGES_RELAXED_CONSTEXPR
            adaptor_base begin_adaptor() const
            {
                return {};
            }
            RANGES_RELAXED_CONSTEXPR
            adaptor_base end_adaptor() const
            {
                return {};
            }

            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            RANGES_RELAXED_CONSTEXPR
            adaptor_cursor_t<D> begin_cursor()
            {
                auto adapt = range_access::begin_adaptor(derived(), 42);
                auto pos = adapt.begin(derived());
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            RANGES_RELAXED_CONSTEXPR
            adaptor_sentinel_t<D> end_cursor()
            {
                auto adapt = range_access::end_adaptor(derived(), 42);
                auto pos = adapt.end(derived());
                return {std::move(pos), std::move(adapt)};
            }
            // Const-correctness is enforced here by only allowing these if the base range
            // has const begin/end accessors. That disables the const begin()/end() accessors
            // in range_facade, meaning the derived range type only has mutable iterators.
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && Range<base_range_t const>())>
            RANGES_RELAXED_CONSTEXPR
            adaptor_cursor_t<D const> begin_cursor() const
            {
                auto adapt = range_access::begin_adaptor(derived(), 42);
                auto pos = adapt.begin(derived());
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && Range<base_range_t const>())>
            RANGES_RELAXED_CONSTEXPR
            adaptor_sentinel_t<D const> end_cursor() const
            {
                auto adapt = range_access::end_adaptor(derived(), 42);
                auto pos = adapt.end(derived());
                return {std::move(pos), std::move(adapt)};
            }
        public:
            RANGES_RELAXED_CONSTEXPR range_adaptor() = default;
            constexpr range_adaptor(BaseRng && rng)
              : rng_(view::all(detail::forward<BaseRng>(rng)))
            {}
            RANGES_RELAXED_CONSTEXPR
            base_range_t & base()
            {
                return rng_;
            }
            /// \overload
            RANGES_RELAXED_CONSTEXPR base_range_t const & base() const
            {
                return rng_;
            }
        };

        /// @}
    }
}

#endif
