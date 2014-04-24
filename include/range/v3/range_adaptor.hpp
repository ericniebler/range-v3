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

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Iterator, bool IsIterator = ranges::WeakInputIterator<Iterator>()>
            struct basic_cursor_associated_types
            {};

            template<typename Iterator>
            struct basic_cursor_associated_types<Iterator, true>
            {
                using single_pass = Derived<ranges::input_iterator_tag, iterator_category_t<Iterator>>;
                using difference_type = iterator_difference_t<Iterator>;
            };

            // Turns an Iterator (or a sentinel) into a Cursor
            template<typename Iterator, bool IsCursor>
            struct basic_cursor : basic_cursor_associated_types<Iterator>
            {
            private:
                template<typename OtherIter, bool OtherIsCursor>
                friend struct basic_cursor;
                Iterator it_;
            public:
                basic_cursor() = default;
                constexpr basic_cursor(Iterator it)
                  : it_(detail::move(it))
                {}
                template<typename Other,
                    CONCEPT_REQUIRES_(ranges::EqualityComparable<Iterator, Other>())>
                constexpr bool equal(basic_cursor<Other, false> const &that) const
                {
                    return it_ == that.it_;
                }
                CONCEPT_REQUIRES(ranges::WeakInputIterator<Iterator>())
                void next()
                {
                    ++it_;
                }
                template<typename I = Iterator,
                    CONCEPT_REQUIRES_(ranges::WeakInputIterator<Iterator>())>
                auto current() const -> decltype(*std::declval<I const &>())
                {
                    return *it_;
                }
                CONCEPT_REQUIRES(ranges::BidirectionalIterator<Iterator>())
                void prev()
                {
                    --it_;
                }
                template<typename I = Iterator,
                    CONCEPT_REQUIRES_(ranges::RandomAccessIterator<Iterator>())>
                void advance(iterator_difference_t<I> n)
                {
                    it_ += n;
                }
                template<typename I = Iterator,
                    CONCEPT_REQUIRES_(ranges::RandomAccessIterator<Iterator>())>
                iterator_difference_t<I>
                distance_to(basic_cursor const &that) const
                {
                    return that.it_ - it_;
                }
            };

            // Thinly wrap a cursor to make sure its interface is public.
            template<typename Cursor>
            struct basic_cursor<Cursor, true>
            {
            private:
                template<typename Other, bool OtherIsCursor>
                friend struct basic_cursor;
                Cursor pos_;
            public:
                using difference_type = range_core_access::cursor_difference_t<Cursor>;
                using single_pass = range_core_access::single_pass_t<Cursor>;
                basic_cursor() = default;
                constexpr basic_cursor(Cursor pos)
                  : pos_(detail::move(pos))
                {}
                template<typename Other, enable_if_t<!std::is_same<Other, Cursor>::value> = 0>
                constexpr bool equal(basic_cursor<Other, true> const &that) const
                {
                    // BUGBUG which parameter order is correct?
                    return range_core_access::empty(that.pos_, pos_);
                }
                template<typename C = Cursor,
                    CONCEPT_REQUIRES_(InputCursor<Cursor>())>
                auto equal(basic_cursor const &that) const ->
                decltype(range_core_access::equal(std::declval<C const &>(), std::declval<C const &>()))
                {
                    return range_core_access::equal(that.pos_, pos_);
                }
                CONCEPT_REQUIRES(InputCursor<Cursor>())
                bool equal(any_<basic_cursor>) const
                {
                    return true;
                }
                CONCEPT_REQUIRES(InputCursor<Cursor>())
                void next()
                {
                    range_core_access::next(pos_);
                }
                template<typename C = Cursor,
                    CONCEPT_REQUIRES_(InputCursor<Cursor>())>
                auto current() const ->
                    decltype(range_core_access::current(std::declval<C const &>()))
                {
                    return range_core_access::current(pos_);
                }
                CONCEPT_REQUIRES(BidirectionalCursor<Cursor>())
                void prev()
                {
                    range_core_access::prev(pos_);
                }
                template<typename C = Cursor,
                    CONCEPT_REQUIRES_(RandomAccessCursor<Cursor>())>
                void advance(difference_type n)
                {
                    range_core_access::advance(pos_, n);
                }
                template<typename C = Cursor,
                    CONCEPT_REQUIRES_(RandomAccessCursor<Cursor>())>
                difference_type
                distance_to(basic_cursor const &that) const
                {
                    return range_core_access::distance_to(pos_, that.pos_);
                }
            };

            // This leads to some strange errors because in some contexts, Adaptor
            // is not yet complete when the user may want it to be. Rethink this.
            template<typename BaseCursor, typename Adaptor>
            struct basic_cursor_and_adaptor : BaseCursor
            {
            private:
                // As a base class would be preferable, but it leads to compile errors.
                Adaptor adapt_;
            public:
                basic_cursor_and_adaptor() = default;
                basic_cursor_and_adaptor(BaseCursor base, Adaptor adapt)
                  : BaseCursor(std::move(base)), adapt_(std::move(adapt))
                {}
                Adaptor &adaptor()
                {
                    return adapt_;
                }
                Adaptor const &adaptor() const
                {
                    return adapt_;
                }
            };

            // A cursor with adapted behaviors
            // The use of decltype to SFINAE-out functions here is a hack. Use concepts.
            template<typename BaseCursor, typename Adaptor>
            struct basic_adapted_cursor
            {
            private:
                template<typename A, typename I>
                friend struct basic_adapted_cursor;
                basic_cursor_and_adaptor<BaseCursor, Adaptor> base_;
            public:
                using single_pass = or_t<
                    range_core_access::single_pass_t<Adaptor>,
                    range_core_access::single_pass_t<BaseCursor>>;
                basic_adapted_cursor() = default;
                basic_adapted_cursor(BaseCursor base, Adaptor adapt)
                  : base_{std::move(base), std::move(adapt)}
                {}
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().current(base_))>
                R current() const
                {
                    return base_.adaptor().current(base_);
                }
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().next(base_))>
                void next()
                {
                    base_.adaptor().next(base_);
                }
                template<typename C2, typename A2,
                         typename A = enable_if_t<
                            std::is_same<basic_adapted_cursor<C2, A2>, basic_adapted_cursor>::value,
                                Adaptor>,
                         typename R = decltype(std::declval<A>().equal(base_, base_))>
                enable_if_t<std::is_same<basic_adapted_cursor<C2, A2>, basic_adapted_cursor>::value, bool>
                equal(basic_adapted_cursor<C2, A2> const &that) const
                {
                    return base_.adaptor().equal(base_, that.base_);
                }
                template<typename C2, typename A2,
                         typename A = enable_if_t<
                            !std::is_same<basic_adapted_cursor<C2, A2>, basic_adapted_cursor>::value,
                                Adaptor>,
                         typename R = decltype(std::declval<A>().empty(std::declval<C2 const &>(), base_))>
                constexpr
                enable_if_t<!std::is_same<basic_adapted_cursor<C2, A2>, basic_adapted_cursor>::value, bool>
                equal(basic_adapted_cursor<C2, A2> const &that) const
                {
                    // "that" is the iterator, "this" is the sentinel
                    return base_.adaptor().empty(that.base_, base_);
                }
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().prev(base_))>
                void prev()
                {
                    base_.adaptor().prev(base_);
                }
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().advance(base_, 0))>
                void advance(range_core_access::cursor_difference_t<BaseCursor> n)
                {
                    base_.adaptor().advance(base_, n);
                }
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().distance_to(base_, base_))>
                range_core_access::cursor_difference_t<BaseCursor>
                distance_to(basic_adapted_cursor const &that) const
                {
                    return base_.adaptor().distance_to(base_, that.base_);
                }
            };

            template<typename Derived>
            using cursor_adaptor_t =
                decltype(range_core_access::begin_adaptor(std::declval<Derived const &>()));

            template<typename Derived>
            using sentinel_adaptor_t =
                decltype(range_core_access::end_adaptor(std::declval<Derived const &>()));

            template<typename Derived>
            using derived_cursor_t =
                decltype(std::declval<cursor_adaptor_t<Derived>>().begin(std::declval<Derived const &>()));

            template<typename Derived>
            using derived_sentinel_t =
                decltype(std::declval<sentinel_adaptor_t<Derived>>().end(std::declval<Derived const &>()));

            // Define a trait to tell whether a particular range type was implemented in terms
            // of range_facade, and hence has cursors.
            template<typename Derived, bool Infinite>
            std::true_type has_cursor_(range_facade<Derived, Infinite> const &);
            std::false_type has_cursor_(any);

            template<typename T>
            struct has_cursor
              : decltype(detail::has_cursor_(std::declval<T>()))
            {};

            // If the base Iterable was implemented in terms of range_facade, then
            // it already has a cursor and a sentinel. Just use them. Otherwise, fake them up
            // by wrapping the base Iterable's iterators.
            template<typename BaseIterable, bool HasCuror = has_cursor<BaseIterable>::value>
            struct base_cursor_helper
            {
                using cursor_t = basic_cursor<facade_cursor_t<BaseIterable>, true>;
                static cursor_t begin_cursor(BaseIterable const &rng)
                {
                    return range_core_access::begin_cursor(rng);
                }

                using sentinel_t = basic_cursor<facade_sentinel2_t<BaseIterable>, true>;
                static sentinel_t end_cursor(BaseIterable const &rng)
                {
                    return range_core_access::end_cursor(rng);
                }
            };

            template<typename BaseIterable>
            struct base_cursor_helper<BaseIterable, false>
            {
                using cursor_t = basic_cursor<range_iterator_t<BaseIterable const>, false>;
                static cursor_t begin_cursor(BaseIterable const &rng)
                {
                    return {ranges::begin(rng)};
                }

                using sentinel_t = basic_cursor<range_sentinel_t<BaseIterable const>, false>;
                static sentinel_t end_cursor(BaseIterable const &rng)
                {
                    return {ranges::end(rng)};
                }
            };
        }

        template<typename Derived>
        using base_iterable_t = typename range_core_access::base_iterable<Derived>::type;

        template<typename Derived>
        using base_cursor_t =
            typename detail::base_cursor_helper<base_iterable_t<Derived>>::cursor_t;

        template<typename Derived>
        using base_sentinel_t =
            typename detail::base_cursor_helper<base_iterable_t<Derived>>::sentinel_t;

        template<typename Cursor, typename Adaptor>
        using derived_cursor_t =
            detail::basic_cursor_and_adaptor<Cursor, Adaptor>;

        template<typename Sentinel, typename Adaptor>
        using derived_sentinel_t =
            detail::basic_cursor_and_adaptor<Sentinel, Adaptor>;

        struct default_adaptor : private range_core_access
        {
            using range_core_access::equal;
            using range_core_access::empty;
            using range_core_access::current;
            using range_core_access::next;
            using range_core_access::prev;
            using range_core_access::advance;
            using range_core_access::distance_to;
            template<typename Range>
            static base_cursor_t<Range> begin(Range const &rng)
            {
                return rng.base_begin();
            }
            template<typename Range>
            static base_sentinel_t<Range> end(Range const &rng)
            {
                return rng.base_end();
            }
        };

        template<typename Derived, typename BaseIterable, bool Infinite>
        struct range_adaptor
          : range_facade<Derived, Infinite>
        {
        private:
            friend Derived;
            friend range_core_access;
            friend default_adaptor;
            using range_adaptor_t = range_adaptor;
            using base_iterable_t = BaseIterable;
            using range_facade<Derived, Infinite>::derived;
            detail::base_iterable_holder<BaseIterable> rng_;

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
                return detail::base_cursor_helper<BaseIterable>::begin_cursor(rng_.get());
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            base_sentinel_t<D> base_end() const
            {
                return detail::base_cursor_helper<BaseIterable>::end_cursor(rng_.get());
            }
            range_difference_t<BaseIterable> base_distance() const
            {
                return ranges::distance(rng_.get());
            }
            CONCEPT_REQUIRES(SizedIterable<BaseIterable>())
            range_size_t<BaseIterable> base_size() const
            {
                return ranges::size(rng_.get());
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::basic_adapted_cursor<detail::derived_cursor_t<D>, detail::cursor_adaptor_t<D>>
            begin_cursor() const
            {
                auto adapt = range_core_access::begin_adaptor(derived());
                auto pos = adapt.begin(derived());
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::basic_adapted_cursor<detail::derived_sentinel_t<D>, detail::sentinel_adaptor_t<D>>
            end_cursor() const
            {
                auto adapt = range_core_access::end_adaptor(derived());
                auto pos = adapt.end(derived());
                return {std::move(pos), std::move(adapt)};
            }
        public:
            range_adaptor() = default;
            constexpr range_adaptor(BaseIterable && rng)
              : rng_(detail::forward<BaseIterable>(rng))
            {}
        };

        template<typename RangeAdaptor>
        using range_adaptor_t = meta_apply<range_core_access::range_adaptor, RangeAdaptor>;
    }
}

#endif
