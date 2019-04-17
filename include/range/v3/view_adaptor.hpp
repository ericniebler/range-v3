/// \file
// Range v3 library
//
// Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_ADAPTOR_HPP
#define RANGES_V3_VIEW_ADAPTOR_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/view_facade.hpp>
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
                detail::decay_t<decltype(range_access::begin_adaptor(std::declval<Derived &>()))>;

            template<typename Derived>
            using end_adaptor_t =
                detail::decay_t<decltype(range_access::end_adaptor(std::declval<Derived &>()))>;

            template<typename Derived>
            using adapted_iterator_t =
                detail::decay_t<decltype(std::declval<begin_adaptor_t<Derived>>().begin(std::declval<Derived &>()))>;

            template<typename Derived>
            using adapted_sentinel_t =
                detail::decay_t<decltype(std::declval<end_adaptor_t<Derived>>().end(std::declval<Derived &>()))>;

            struct adaptor_base_current_mem_fn
            {};

            template<typename BaseIter, typename Adapt, typename Enable = void>
            struct adaptor_value_type_2_
              : compressed_pair<BaseIter, Adapt>
            {
                using compressed_pair<BaseIter, Adapt>::compressed_pair;
            };

            template<typename BaseIter, typename Adapt>
            struct adaptor_value_type_2_<
                BaseIter,
                Adapt,
                meta::void_<decltype(Adapt::read(std::declval<BaseIter const &>(), adaptor_base_current_mem_fn{}))>>
              : compressed_pair<BaseIter, Adapt>
            {
                using compressed_pair<BaseIter, Adapt>::compressed_pair;
                using value_type = value_type_t<BaseIter>;
            };

            template<typename BaseIter, typename Adapt, typename Enable = void>
            struct adaptor_value_type_
              : adaptor_value_type_2_<BaseIter, Adapt>
            {
                using adaptor_value_type_2_<BaseIter, Adapt>::adaptor_value_type_2_;
            };

            template<typename BaseIter, typename Adapt>
            struct adaptor_value_type_<BaseIter, Adapt, meta::void_<typename Adapt::value_type>>
              : compressed_pair<BaseIter, Adapt>
            {
                using compressed_pair<BaseIter, Adapt>::compressed_pair;
#ifdef RANGES_WORKAROUND_MSVC_688606
                using value_type = value_type_t<Adapt>;
#else // ^^^ workaround ^^^ / vvv no workaround vvv
                using value_type = typename Adapt::value_type;
#endif // RANGES_WORKAROUND_MSVC_688606
            };
        }
        /// \endcond

        /// \addtogroup group-core
        /// @{
        template<typename BaseIt, typename Adapt>
        struct adaptor_cursor;

        template<typename BaseSent, typename Adapt>
        struct base_adaptor_sentinel;

        struct adaptor_base
        {
            adaptor_base() = default;
            adaptor_base(adaptor_base &&) = default;
            adaptor_base(adaptor_base const &) = default;
            adaptor_base &operator=(adaptor_base &&) = default;
            adaptor_base &operator=(adaptor_base const &) = default;

            adaptor_base(detail::any, detail::any = {}, detail::any = {})
            {}
            template<typename Rng>
            static constexpr auto begin(Rng &rng)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                ranges::begin(rng.base())
            )
            template<typename Rng>
            static constexpr auto end(Rng &rng)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                ranges::end(rng.base())
            )
            template<typename I, CONCEPT_REQUIRES_(EqualityComparable<I>())>
            static bool equal(I const &it0, I const &it1)
            {
                return it0 == it1;
            }
            template<typename I, CONCEPT_REQUIRES_(Iterator<I>())>
            static reference_t<I> read(I const &it,
                detail::adaptor_base_current_mem_fn = {})
                noexcept(noexcept(reference_t<I>(*it)))
            {
                return *it;
            }
            template<typename I, CONCEPT_REQUIRES_(Iterator<I>())>
            static void next(I &it)
            {
                ++it;
            }
            template<typename I, CONCEPT_REQUIRES_(BidirectionalIterator<I>())>
            static void prev(I &it)
            {
                --it;
            }
            template<typename I, CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            static void advance(I &it, difference_type_t<I> n)
            {
                it += n;
            }
            template<typename I, CONCEPT_REQUIRES_(SizedSentinel<I, I>())>
            static difference_type_t<I> distance_to(I const &it0, I const &it1)
            {
                return it1 - it0;
            }
            template<typename I, typename S, CONCEPT_REQUIRES_(Sentinel<S, I>())>
            static constexpr bool empty(I const &it, S const &end)
            {
                return it == end;
            }
        };

        // Build a sentinel out of a sentinel into the adapted range, and an
        // adaptor that customizes behavior.
        template<typename BaseSent, typename Adapt>
        struct base_adaptor_sentinel
          : private compressed_pair<BaseSent, Adapt>
        {
        private:
            template<typename, typename>
            friend struct adaptor_cursor;

            using compressed_pair<BaseSent, Adapt>::first;
            using compressed_pair<BaseSent, Adapt>::second;
        public:
            using compressed_pair<BaseSent, Adapt>::compressed_pair;

            // All sentinels into adapted ranges have a base() member for fetching
            // the underlying sentinel.
            BaseSent base() const
            {
                return first();
            }

        protected:
            // Adaptor accessor
            Adapt& get()
            {
                return second();
            }
            const Adapt& get() const
            {
                return second();
            }
        };

        namespace detail
        {
            template<typename BaseSent, typename Adapt>
            meta::id<base_adaptor_sentinel<BaseSent, Adapt>> base_adaptor_sentinel_2_(long);

            template<typename BaseSent, typename Adapt>
            meta::id<typename Adapt::template mixin<base_adaptor_sentinel<BaseSent, Adapt>>> base_adaptor_sentinel_2_(int);

            template<typename BaseSent, typename Adapt>
            struct base_adaptor_sentinel_
              : decltype(base_adaptor_sentinel_2_<BaseSent, Adapt>(42))
            {};

            template<typename BaseSent, typename Adapt>
            using adaptor_sentinel_ = meta::_t<base_adaptor_sentinel_<BaseSent, Adapt>>;
        }

        template<typename BaseSent, typename Adapt>
        struct adaptor_sentinel
          : detail::adaptor_sentinel_<BaseSent, Adapt>
        {
            using detail::adaptor_sentinel_<BaseSent, Adapt>::adaptor_sentinel_;
        };

        // Build a cursor out of an iterator into the adapted range, and an
        // adaptor that customizes behavior.
        template<typename BaseIter, typename Adapt>
        struct adaptor_cursor
          : private detail::adaptor_value_type_<BaseIter, Adapt>
        {
        private:
            friend range_access;
            using base_t = detail::adaptor_value_type_<BaseIter, Adapt>;
            using single_pass = meta::or_<
                range_access::single_pass_t<Adapt>,
                SinglePass<BaseIter>>;

            struct basic_adaptor_mixin
              : basic_mixin<adaptor_cursor>
            {
                basic_adaptor_mixin() = default;
                using basic_mixin<adaptor_cursor<BaseIter, Adapt>>::basic_mixin;

                // All iterators into adapted ranges have a base() member for fetching
                // the underlying iterator.
                BaseIter base() const
                {
                    return basic_adaptor_mixin::basic_mixin::get().first();
                }

            protected:
                Adapt& get()
                {
                    return basic_adaptor_mixin::basic_mixin::get().second();
                }
                const Adapt& get() const
                {
                    return basic_adaptor_mixin::basic_mixin::get().second();
                }
            };

            template<typename Adapt_>
            static meta::id<basic_adaptor_mixin> basic_adaptor_mixin_2_(long);
            template<typename Adapt_>
            static meta::id<typename Adapt_::template mixin<basic_adaptor_mixin>> basic_adaptor_mixin_2_(int);

            using mixin = meta::_t<decltype(basic_adaptor_mixin_2_<Adapt>(42))>;

            using base_t::first;
            using base_t::second;

            template<typename A = Adapt, typename R = decltype(
                std::declval<A const &>().read(std::declval<BaseIter const &>()))>
            R read() const
                noexcept(noexcept(
                    std::declval<A const &>().read(std::declval<BaseIter const &>())))
            {
                using V = range_access::cursor_value_t<adaptor_cursor>;
                static_assert(
                    CommonReference<R &&, V &>(),
                    "In your adaptor, you've specified a value type that does not "
                    "share a common reference type with the return type of read.");
                return second().read(first());
            }
            template<typename A = Adapt,
                typename = decltype(std::declval<A &>().next(std::declval<BaseIter &>()))>
            void next()
            {
                second().next(first());
            }
            template<typename A = Adapt, typename = decltype(
                std::declval<A const &>().equal(
                    std::declval<BaseIter const &>(),
                    std::declval<BaseIter const &>(),
                    std::declval<A const &>()))>
            bool equal_(adaptor_cursor const &that, int) const
            {
                return second().equal(first(), that.first(), that.second());
            }
            template<typename A = Adapt, typename = decltype(
                std::declval<A const &>().equal(
                    std::declval<BaseIter const &>(),
                    std::declval<BaseIter const &>()))>
            bool equal_(adaptor_cursor const &that, long) const
            {
                return second().equal(first(), that.first());
            }
            template<typename C = adaptor_cursor>
            auto equal(adaptor_cursor const &that) const ->
                decltype(std::declval<C const &>().equal_(that, 42))
            {
                return this->equal_(that, 42);
            }
            template<typename S, typename A, typename = decltype(
                std::declval<A const &>().empty(
                    std::declval<BaseIter const &>(),
                    std::declval<Adapt const &>(),
                    std::declval<S const &>()))>
            constexpr bool equal_(adaptor_sentinel<S, A> const &that, int) const
            {
                return that.second().empty(first(), second(), that.first());
            }
            template<typename S, typename A, typename = decltype(
                std::declval<A const &>().empty(
                    std::declval<BaseIter const &>(),
                    std::declval<S const &>()))>
            constexpr bool equal_(adaptor_sentinel<S, A> const &that, long) const
            {
                return that.second().empty(first(), that.first());
            }
            template<typename S, typename A>
            constexpr auto equal(adaptor_sentinel<S, A> const &that) const ->
                decltype(std::declval<adaptor_cursor const &>().equal_(that, 42))
            {
                return this->equal_(that, 42);
            }
            template<typename A = Adapt, typename = decltype(
                std::declval<A &>().prev(std::declval<BaseIter &>()))>
            void prev()
            {
                second().prev(first());
            }
            template<typename A = Adapt, typename = decltype(
                std::declval<A &>().advance(std::declval<BaseIter &>(), 0))>
            void advance(difference_type_t<BaseIter> n)
            {
                second().advance(first(), n);
            }
            template<typename A = Adapt, typename R = decltype(
                std::declval<A const &>().distance_to(
                    std::declval<BaseIter const &>(),
                    std::declval<BaseIter const &>(),
                    std::declval<A const &>()))>
            R distance_to_(adaptor_cursor const &that, int) const
            {
                return second().distance_to(first(), that.first(), that.second());
            }
            template<typename A = Adapt, typename R = decltype(
                std::declval<A const &>().distance_to(
                    std::declval<BaseIter const &>(),
                    std::declval<BaseIter const &>()))>
            R distance_to_(adaptor_cursor const &that, long) const
            {
                return second().distance_to(first(), that.first());
            }
            template<typename C = adaptor_cursor>
            auto distance_to(adaptor_cursor const &that) const ->
                decltype(std::declval<C const &>().distance_to_(that, 42))
            {
                return this->distance_to_(that, 42);
            }
            // If the adaptor has an iter_move function, use it.
            template<typename A = Adapt, typename X = decltype(
                std::declval<A const &>().iter_move(
                    std::declval<BaseIter const &>()))>
            X iter_move_(int) const
                noexcept(noexcept(std::declval<A const &>().iter_move(
                    std::declval<BaseIter const &>())))
            {
                using V = range_access::cursor_value_t<adaptor_cursor>;
                using R = decltype(second().read(first()));
                static_assert(
                    CommonReference<X &&, V const &>(),
                    "In your adaptor, the result of your iter_move member function does "
                    "not share a common reference with your value type.");
                static_assert(
                    CommonReference<R &&, X &&>(),
                    "In your adaptor, the result of your iter_move member function does "
                    "not share a common reference with the result of your read member "
                    "function.");
                return second().iter_move(first());
            }
            // If there is no iter_move member and the adaptor has not overridden the read
            // member function, then dispatch to the base iterator's iter_move function.
            template<typename A = Adapt,
                typename R = decltype(std::declval<A const &>().read(
                    std::declval<BaseIter const &>(), detail::adaptor_base_current_mem_fn{})),
                typename X = rvalue_reference_t<BaseIter>>
            X iter_move_(long) const
                noexcept(noexcept(X(ranges::iter_move(std::declval<BaseIter const &>()))))
            {
                return ranges::iter_move(first());
            }
            // If the adaptor does not have an iter_move function but overrides the read
            // member function, apply std::move to the result of calling read.
            template<typename A = Adapt,
                typename R = decltype(std::declval<A const &>().read(std::declval<BaseIter const &>())),
                typename X = aux::move_t<R>>
            X iter_move_(detail::any) const
                noexcept(noexcept(X(static_cast<X &&>(
                    std::declval<A const &>().read(std::declval<BaseIter const &>())))))
            {
                using V = range_access::cursor_value_t<adaptor_cursor>;
                static_assert(
                    CommonReference<X &&, V const &>(),
                    "In your adaptor, you've specified a value type that does not share a common "
                    "reference type with the result of moving the result of the read member "
                    "function. Consider defining an iter_move function in your adaptor.");
                return static_cast<X &&>(second().read(first()));
            }
            // Gives users a way to override the default iter_move function in their adaptors.
            auto move() const
                noexcept(noexcept(std::declval<const adaptor_cursor &>().iter_move_(42))) ->
                decltype(std::declval<const adaptor_cursor &>().iter_move_(42))
            {
                return iter_move_(42);
            }
        public:
            using base_t::base_t;
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
                adaptor_cursor_t<D>,
                adaptor_sentinel<detail::adapted_sentinel_t<D>, detail::end_adaptor_t<D>>>;

        template<typename Derived, typename BaseRng,
            cardinality Cardinality /*= range_cardinality<BaseRng>::value*/>
        struct view_adaptor
          : view_facade<Derived, Cardinality>
        {
        private:
            friend Derived;
            friend range_access;
            friend adaptor_base;
            using base_range_t = view::all_t<BaseRng>;
            using view_facade<Derived, Cardinality>::derived;

            base_range_t rng_;

            constexpr adaptor_base begin_adaptor() const noexcept
            {
                return {};
            }
            constexpr adaptor_base end_adaptor() const noexcept
            {
                return {};
            }

            template<typename D>
            static RANGES_CXX14_CONSTEXPR adaptor_cursor_t<D> begin_cursor_(D &d)
                noexcept(noexcept(adaptor_cursor_t<D>{
                    std::declval<detail::begin_adaptor_t<D> &>().begin(d),
                    range_access::begin_adaptor(d)}))
            {
                auto adapt = range_access::begin_adaptor(d);
                auto pos = adapt.begin(d);
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            RANGES_CXX14_CONSTEXPR auto begin_cursor()
            RANGES_DECLTYPE_NOEXCEPT(view_adaptor::begin_cursor_(std::declval<D &>()))
            {
                return view_adaptor::begin_cursor_(derived());
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && Range<base_range_t const>())>
            RANGES_CXX14_CONSTEXPR auto begin_cursor() const
            RANGES_DECLTYPE_NOEXCEPT(view_adaptor::begin_cursor_(std::declval<D const &>()))
            {
                return view_adaptor::begin_cursor_(derived());
            }

            template<typename D>
            static RANGES_CXX14_CONSTEXPR adaptor_sentinel_t<D> end_cursor_(D &d)
                noexcept(noexcept(adaptor_sentinel_t<D>{
                    std::declval<detail::end_adaptor_t<D> &>().end(d),
                    range_access::end_adaptor(d)}))
            {
                auto adapt = range_access::end_adaptor(d);
                auto pos = adapt.end(d);
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            RANGES_CXX14_CONSTEXPR auto end_cursor()
            RANGES_DECLTYPE_NOEXCEPT(view_adaptor::end_cursor_(std::declval<D &>()))
            {
                return view_adaptor::end_cursor_(derived());
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && Range<base_range_t const>())>
            RANGES_CXX14_CONSTEXPR auto end_cursor() const
            RANGES_DECLTYPE_NOEXCEPT(view_adaptor::end_cursor_(std::declval<D const &>()))
            {
                return view_adaptor::end_cursor_(derived());
            }
        protected:
            ~view_adaptor() = default;
        public:
            view_adaptor() = default;
            view_adaptor(view_adaptor &&) = default;
            view_adaptor(view_adaptor const &) = default;
            view_adaptor &operator=(view_adaptor &&) = default;
            view_adaptor &operator=(view_adaptor const &) = default;
            explicit constexpr view_adaptor(BaseRng &&rng)
              : rng_(view::all(static_cast<BaseRng &&>(rng)))
            {}
            RANGES_CXX14_CONSTEXPR base_range_t &base() noexcept
            {
                return rng_;
            }
            /// \overload
            constexpr base_range_t const &base() const noexcept
            {
                return rng_;
            }
        };

        /// @}
    }
}

#endif
