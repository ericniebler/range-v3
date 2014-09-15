// Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_RANGE_ADAPTOR_HPP
#define RANGES_V3_RANGE_ADAPTOR_HPP

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
        namespace detail
        {
            template<typename I, bool IsIterator = WeakInputIterator<I>()>
            struct basic_cursor_associated_types
            {};

            template<typename I>
            struct basic_cursor_associated_types<I, true>
            {
                using single_pass = SinglePass<I>;
                using difference_type = iterator_difference_t<I>;
            };

            // Turns an Iterator (or a sentinel) into a Cursor
            template<typename I, bool IsCursor>
            struct basic_cursor : basic_cursor_associated_types<I>
            {
            private:
                template<typename OtherIter, bool OtherIsCursor>
                friend struct basic_cursor;
                I it_;
            public:
                basic_cursor() = default;
                constexpr basic_cursor(I it)
                  : it_(detail::move(it))
                {}
                template<typename Other,
                    CONCEPT_REQUIRES_(EqualityComparable<I, Other>())>
                constexpr bool equal(basic_cursor<Other, false> const &that) const
                {
                    return it_ == that.it_;
                }
                CONCEPT_REQUIRES(WeakInputIterator<I>())
                void next()
                {
                    ++it_;
                }
                template<typename I2 = I,
                    CONCEPT_REQUIRES_(WeakInputIterator<I>())>
                auto current() const -> decltype(*std::declval<I2 const &>())
                {
                    return *it_;
                }
                CONCEPT_REQUIRES(BidirectionalIterator<I>())
                void prev()
                {
                    --it_;
                }
                template<typename I2 = I,
                    CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
                void advance(iterator_difference_t<I2> n)
                {
                    it_ += n;
                }
                template<typename I2 = I,
                    CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
                iterator_difference_t<I2>
                distance_to(basic_cursor const &that) const
                {
                    return that.it_ - it_;
                }
            };

            // Thinly wrap a cursor to make sure its interface is public.
            template<typename Cur>
            struct basic_cursor<Cur, true>
            {
            private:
                template<typename Other, bool OtherIsCursor>
                friend struct basic_cursor;
                Cur pos_;
            public:
                using difference_type = range_core_access::cursor_difference_t<Cur>;
                using single_pass = range_core_access::single_pass_t<Cur>;
                basic_cursor() = default;
                constexpr basic_cursor(Cur pos)
                  : pos_(detail::move(pos))
                {}
                template<typename Other, enable_if_t<!std::is_same<Other, Cur>::value> = 0>
                constexpr bool equal(basic_cursor<Other, true> const &that) const
                {
                    // BUGBUG which parameter order is correct?
                    return range_core_access::empty(that.pos_, pos_);
                }
                template<typename C = Cur,
                    CONCEPT_REQUIRES_(InputCursor<Cur>())>
                auto equal(basic_cursor const &that) const ->
                decltype(range_core_access::equal(std::declval<C const &>(), std::declval<C const &>()))
                {
                    return range_core_access::equal(that.pos_, pos_);
                }
                CONCEPT_REQUIRES(InputCursor<Cur>())
                bool equal(any_<basic_cursor>) const
                {
                    return true;
                }
                CONCEPT_REQUIRES(InputCursor<Cur>())
                void next()
                {
                    range_core_access::next(pos_);
                }
                template<typename C = Cur,
                    CONCEPT_REQUIRES_(InputCursor<Cur>())>
                auto current() const ->
                    decltype(range_core_access::current(std::declval<C const &>()))
                {
                    return range_core_access::current(pos_);
                }
                CONCEPT_REQUIRES(BidirectionalCursor<Cur>())
                void prev()
                {
                    range_core_access::prev(pos_);
                }
                template<typename C = Cur,
                    CONCEPT_REQUIRES_(RandomAccessCursor<Cur>())>
                void advance(difference_type n)
                {
                    range_core_access::advance(pos_, n);
                }
                template<typename C = Cur,
                    CONCEPT_REQUIRES_(RandomAccessCursor<Cur>())>
                difference_type
                distance_to(basic_cursor const &that) const
                {
                    return range_core_access::distance_to(pos_, that.pos_);
                }
            };

            // BUGBUG This leads to some strange errors because in some contexts, Adapt
            // is not yet complete when the user may want it to be. Rethink this.
            template<typename BaseCursor, typename Adapt>
            struct basic_cursor_and_adaptor : BaseCursor
            {
            private:
                // As a base class would be preferable, but it leads to compile errors.
                Adapt adapt_;
            public:
                basic_cursor_and_adaptor() = default;
                basic_cursor_and_adaptor(BaseCursor base, Adapt adapt)
                  : BaseCursor(std::move(base)), adapt_(std::move(adapt))
                {}
                Adapt &adaptor()
                {
                    return adapt_;
                }
                Adapt const &adaptor() const
                {
                    return adapt_;
                }
            };

            // A cursor with adapted behaviors
            // BUGBUG The use of decltype to SFINAE-out functions here is a hack. Use concepts.
            template<typename BaseCursor, typename Adapt>
            struct basic_adapted_cursor
            {
            private:
                template<typename A, typename I>
                friend struct basic_adapted_cursor;
                basic_cursor_and_adaptor<BaseCursor, Adapt> base_;
            public:
                using single_pass = or_t<
                    range_core_access::single_pass_t<Adapt>,
                    range_core_access::single_pass_t<BaseCursor>>;
                basic_adapted_cursor() = default;
                basic_adapted_cursor(BaseCursor base, Adapt adapt)
                  : base_{std::move(base), std::move(adapt)}
                {}
                template<typename A = Adapt,
                         typename R = decltype(std::declval<A>().current(base_))>
                R current() const
                {
                    return base_.adaptor().current(base_);
                }
                template<typename A = Adapt,
                         typename R = decltype(std::declval<A>().next(base_))>
                void next()
                {
                    base_.adaptor().next(base_);
                }
                template<typename C2, typename A2,
                         typename A = enable_if_t<
                            std::is_same<basic_adapted_cursor<C2, A2>, basic_adapted_cursor>::value,
                                Adapt>,
                         typename R = decltype(std::declval<A>().equal(base_, base_))>
                enable_if_t<std::is_same<basic_adapted_cursor<C2, A2>, basic_adapted_cursor>::value, bool>
                equal(basic_adapted_cursor<C2, A2> const &that) const
                {
                    return base_.adaptor().equal(base_, that.base_);
                }
                template<typename C2, typename A2,
                         typename A = enable_if_t<
                            !std::is_same<basic_adapted_cursor<C2, A2>, basic_adapted_cursor>::value,
                                Adapt>,
                         typename R = decltype(std::declval<A>().empty(std::declval<C2 const &>(), base_))>
                constexpr
                enable_if_t<!std::is_same<basic_adapted_cursor<C2, A2>, basic_adapted_cursor>::value, bool>
                equal(basic_adapted_cursor<C2, A2> const &that) const
                {
                    // "that" is the iterator, "this" is the sentinel
                    return base_.adaptor().empty(that.base_, base_);
                }
                template<typename A = Adapt,
                         typename R = decltype(std::declval<A>().prev(base_))>
                void prev()
                {
                    base_.adaptor().prev(base_);
                }
                template<typename A = Adapt,
                         typename R = decltype(std::declval<A>().advance(base_, 0))>
                void advance(range_core_access::cursor_difference_t<BaseCursor> n)
                {
                    base_.adaptor().advance(base_, n);
                }
                template<typename A = Adapt,
                         typename R = decltype(std::declval<A>().distance_to(base_, base_))>
                range_core_access::cursor_difference_t<BaseCursor>
                distance_to(basic_adapted_cursor const &that) const
                {
                    return base_.adaptor().distance_to(base_, that.base_);
                }
            };

            template<typename Derived>
            using cursor_adaptor_t =
                decltype(range_core_access::begin_adaptor(std::declval<Derived &>()));

            template<typename Derived>
            using sentinel_adaptor_t =
                decltype(range_core_access::end_adaptor(std::declval<Derived &>()));

            template<typename Derived>
            using derived_cursor_t =
                decltype(std::declval<cursor_adaptor_t<Derived>>().begin(std::declval<Derived &>()));

            template<typename Derived>
            using derived_sentinel_t =
                decltype(std::declval<sentinel_adaptor_t<Derived>>().end(std::declval<Derived &>()));

            // Define a trait to tell whether a particular range type was implemented in terms
            // of range_facade, and hence has cursors.
            template<typename Derived, bool Inf>
            std::true_type has_cursor_(range_facade<Derived, Inf> const &);
            std::false_type has_cursor_(any);

            template<typename T>
            struct has_cursor
              : decltype(detail::has_cursor_(std::declval<T>()))
            {};

            // If the base Range was implemented in terms of range_facade, then
            // it already has a cursor and a sentinel. Just use them. Otherwise, fake them up
            // by wrapping the base Range's iterators.
            template<typename BaseRng, bool HasCur = has_cursor<BaseRng>::value>
            struct base_cursor_helper
            {
                using cursor_t = basic_cursor<facade_cursor_t<BaseRng>, true>;
                static cursor_t begin_cursor(BaseRng &rng)
                {
                    return range_core_access::begin_cursor(rng);
                }

                using sentinel_t = basic_cursor<facade_sentinel2_t<BaseRng>, true>;
                static sentinel_t end_cursor(BaseRng &rng)
                {
                    return range_core_access::end_cursor(rng);
                }
            };

            template<typename BaseRng>
            struct base_cursor_helper<BaseRng, false>
            {
                using cursor_t = basic_cursor<range_iterator_t<BaseRng>, false>;
                static cursor_t begin_cursor(BaseRng &rng)
                {
                    return {begin(rng)};
                }

                using sentinel_t = basic_cursor<range_sentinel_t<BaseRng>, false>;
                static sentinel_t end_cursor(BaseRng &rng)
                {
                    return {end(rng)};
                }
            };
        }

        template<typename Derived>
        using base_range_t = typename range_core_access::base_range<Derived>::type;

        template<typename Derived>
        using base_cursor_t =
            typename detail::base_cursor_helper<base_range_t<Derived>>::cursor_t;

        template<typename Derived>
        using base_sentinel_t =
            typename detail::base_cursor_helper<base_range_t<Derived>>::sentinel_t;

        template<typename Cur, typename Adapt>
        using derived_cursor_t =
            detail::basic_cursor_and_adaptor<Cur, Adapt>;

        template<typename S, typename Adapt>
        using derived_sentinel_t =
            detail::basic_cursor_and_adaptor<S, Adapt>;

        struct default_adaptor : private range_core_access
        {
            using range_core_access::equal;
            using range_core_access::empty;
            using range_core_access::current;
            using range_core_access::next;
            using range_core_access::prev;
            using range_core_access::advance;
            using range_core_access::distance_to;
            template<typename Rng>
            static base_cursor_t<Rng> begin(Rng &rng)
            {
                return rng.base_begin();
            }
            template<typename Rng>
            static base_sentinel_t<Rng> end(Rng &rng)
            {
                return rng.base_end();
            }
        };

        template<typename Derived, typename BaseRng, bool Inf>
        struct range_adaptor
          : range_facade<Derived, Inf>
        {
        private:
            friend Derived;
            friend range_core_access;
            friend default_adaptor;
            using range_adaptor_t = range_adaptor;
            using base_range_t = view::all_t<BaseRng>;
            using range_facade<Derived, Inf>::derived;
            // Mutable here. Const-correctness is enforced below by disabling
            // begin_cursor/end_cursor if "BaseRng const" does not model
            // the Range concept.
            mutable base_range_t rng_;

            default_adaptor begin_adaptor() const
            {
                return {};
            }
            default_adaptor end_adaptor() const
            {
                return {};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            base_cursor_t<D> base_begin() const
            {
                return detail::base_cursor_helper<base_range_t>::begin_cursor(rng_);
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            base_sentinel_t<D> base_end() const
            {
                return detail::base_cursor_helper<base_range_t>::end_cursor(rng_);
            }
            range_difference_t<base_range_t> base_distance() const
            {
                return distance(rng_);
            }
            CONCEPT_REQUIRES(SizedRange<base_range_t>())
            range_size_t<base_range_t> base_size() const
            {
                return size(rng_);
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::basic_adapted_cursor<detail::derived_cursor_t<D>, detail::cursor_adaptor_t<D>>
            begin_cursor()
            {
                auto adapt = range_core_access::begin_adaptor(derived());
                auto pos = adapt.begin(derived());
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::basic_adapted_cursor<detail::derived_sentinel_t<D>, detail::sentinel_adaptor_t<D>>
            end_cursor()
            {
                auto adapt = range_core_access::end_adaptor(derived());
                auto pos = adapt.end(derived());
                return {std::move(pos), std::move(adapt)};
            }
            // Const-correctness is enforced here by only allowing these if the base range
            // has const begin/end accessors. That disables the const begin()/end() accessors
            // in range_facade, meaning the derived range type only has mutable iterators.
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && Range<base_range_t const>())>
            detail::basic_adapted_cursor<detail::derived_cursor_t<D const>, detail::cursor_adaptor_t<D const>>
            begin_cursor() const
            {
                auto adapt = range_core_access::begin_adaptor(derived());
                auto pos = adapt.begin(derived());
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && Range<base_range_t const>())>
            detail::basic_adapted_cursor<detail::derived_sentinel_t<D const>, detail::sentinel_adaptor_t<D const>>
            end_cursor() const
            {
                auto adapt = range_core_access::end_adaptor(derived());
                auto pos = adapt.end(derived());
                return {std::move(pos), std::move(adapt)};
            }
        public:
            range_adaptor() = default;
            constexpr range_adaptor(BaseRng && rng)
              : rng_(view::all(detail::forward<BaseRng>(rng)))
            {}
        };

        template<typename RangeAdaptor>
        using range_adaptor_t = meta_apply<range_core_access::range_adaptor, RangeAdaptor>;
    }
}

#endif
