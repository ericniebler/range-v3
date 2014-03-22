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
#include <range/v3/range_facade.hpp>
#include <range/v3/range_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Iterator, bool IsIterator = ranges::Iterator<Iterator>()>
            struct basic_cursor_associated_types
            {};

            template<typename Iterator>
            struct basic_cursor_associated_types<Iterator, true>
            {
                using single_pass = Same<iterator_category_t<Iterator>, std::input_iterator_tag>;
                using difference_type = iterator_difference_t<Iterator>;
            };

            // Turns an Iterator (or a sentinel) into a Cursor
            template<typename Iterator>
            struct basic_cursor : basic_cursor_associated_types<Iterator>
            {
            private:
                template<typename Other>
                friend struct basic_cursor;
                Iterator it_;
            public:
                basic_cursor() = default;
                constexpr basic_cursor(Iterator it)
                  : it_(detail::move(it))
                {}
                template<typename Other,
                    CONCEPT_REQUIRES_(ranges::EqualityComparable<Iterator, Other>())>
                constexpr bool equal(basic_cursor<Other> const &that) const
                {
                    return it_ == that.it_;
                }
                CONCEPT_REQUIRES(ranges::Iterator<Iterator>())
                void next()
                {
                    ++it_;
                }
                template<typename I = Iterator,
                    CONCEPT_REQUIRES_(ranges::Iterator<Iterator>())>
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
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().equal(base_, base_))>
                bool equal(basic_adapted_cursor const &that) const
                {
                    return base_.adaptor().equal(base_, that.base_);
                }
                template<typename A = Adaptor,
                         typename C2, typename A2,
                         typename R = decltype(std::declval<A>().empty(std::declval<C2 const &>(), base_))>
                constexpr bool equal(basic_adapted_cursor<C2, A2> const &that) const
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
        }

        template<typename Derived>
        using range_adaptor_t = typename Derived::range_adaptor_t;

        // TODO: or, if BaseIterable was created with range_facade, base_cursor_t
        // could simply be the underlying cursor type and base_sentinel_t the underlying
        // sentinel.
        template<typename Derived>
        using base_iterable_t = typename range_core_access::base_iterable<Derived>::type;

        template<typename Derived>
        using base_cursor_t =
            detail::basic_cursor<range_iterator_t<base_iterable_t<Derived> const>>;

        template<typename Derived>
        using base_sentinel_t =
            detail::basic_cursor<range_sentinel_t<base_iterable_t<Derived> const>>;

        template<typename Derived>
        using derived_cursor_t =
            detail::basic_cursor_and_adaptor<detail::derived_cursor_t<Derived>, detail::cursor_adaptor_t<Derived>>;

        template<typename Derived>
        using derived_sentinel_t =
            detail::basic_cursor_and_adaptor<detail::derived_sentinel_t<Derived>, detail::sentinel_adaptor_t<Derived>>;

        struct adaptor_defaults : private range_core_access
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
            friend adaptor_defaults;
            using range_adaptor_t = range_adaptor;
            using base_iterable_t = BaseIterable;
            using range_facade<Derived, Infinite>::derived;
            BaseIterable rng_;

            adaptor_defaults get_adaptor(begin_end_tag) const
            {
                return {};
            }
            BaseIterable &base()
            {
                return rng_;
            }
            BaseIterable const &base() const
            {
                return rng_;
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            base_cursor_t<D> base_begin() const
            {
                return {ranges::begin(rng_)};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            base_sentinel_t<D> base_end() const
            {
                return {ranges::end(rng_)};
            }
            CONCEPT_REQUIRES(SizedIterable<BaseIterable>())
            range_size_t<BaseIterable> base_size() const
            {
                return ranges::size(rng_);
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::basic_adapted_cursor<detail::derived_cursor_t<D>, detail::cursor_adaptor_t<D>>
            get_begin() const
            {
                auto adapt = range_core_access::begin_adaptor(derived());
                auto pos = adapt.begin(derived());
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::basic_adapted_cursor<detail::derived_sentinel_t<D>, detail::sentinel_adaptor_t<D>>
            get_end() const
            {
                auto adapt = range_core_access::end_adaptor(derived());
                auto pos = adapt.end(derived());
                return {std::move(pos), std::move(adapt)};
            }
        public:
            constexpr range_adaptor(BaseIterable && rng)
              : rng_(detail::forward<BaseIterable>(rng))
            {}
        };
    }
}

#endif
