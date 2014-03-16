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
            template<typename Iterable>
            using begin_adaptor_t = decltype(range_core_access::begin_adaptor(std::declval<Iterable const &>()));

            template<typename Iterable>
            using end_adaptor_t = decltype(range_core_access::end_adaptor(std::declval<Iterable const &>()));

            // Give Iterable::iterator a simple interface for passing to Derived
            template<typename IteratorOrSentinel>
            struct basic_adaptor
            {
            private:
                template<typename Other>
                friend struct basic_adaptor;
                IteratorOrSentinel it_;
            public:
                basic_adaptor() = default;
                constexpr basic_adaptor(IteratorOrSentinel it)
                  : it_(detail::move(it))
                {}
                template<typename Other,
                    CONCEPT_REQUIRES_(EqualityComparable<IteratorOrSentinel, Other>())>
                constexpr bool equal(basic_adaptor<Other> const &that) const
                {
                    return it_ == that.it_;
                }
                CONCEPT_REQUIRES(Iterator<IteratorOrSentinel>())
                void next()
                {
                    ++it_;
                }
                template<typename I = IteratorOrSentinel,
                    CONCEPT_REQUIRES_(Iterator<IteratorOrSentinel>())>
                auto current() const -> decltype(*std::declval<I const &>())
                {
                    return *it_;
                }
                CONCEPT_REQUIRES(BidirectionalIterator<IteratorOrSentinel>())
                void prev()
                {
                    --it_;
                }
                template<typename I = IteratorOrSentinel,
                    CONCEPT_REQUIRES_(RandomAccessIterator<IteratorOrSentinel>())>
                void advance(iterator_difference_t<I> n)
                {
                    it_ += n;
                }
                template<typename I = IteratorOrSentinel,
                    CONCEPT_REQUIRES_(RandomAccessIterator<IteratorOrSentinel>())>
                iterator_difference_t<I>
                distance_to(basic_adaptor const &that) const
                {
                    return that.it_ - it_;
                }
            };
        }

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
            static auto begin(Range const &rng) -> decltype(rng.base_begin())
            {
                return rng.base_begin();
            }
            template<typename Range>
            static auto end(Range const &rng) -> decltype(rng.base_end())
            {
                return rng.base_end();
            }
        };

        template<typename Derived>
        using range_adaptor_t = typename Derived::range_adaptor_;

        template<typename Derived>
        using base_cursor_t = typename Derived::base_cursor_t;

        template<typename Derived>
        using base_sentinel_t = typename Derived::base_sentinel_t;

        template<typename Derived, typename BaseIterable, bool Infinite>
        struct range_adaptor
          : range_facade<Derived, Infinite>
        {
        private:
            friend Derived;
            friend range_core_access;
            friend adaptor_defaults;
            using range_adaptor_ = range_adaptor;
            using range_facade<Derived, Infinite>::derived;
            BaseIterable rng_;

            // TODO: or, if BaseIterable was created with range_facade, base_cursor_t
            // could simply be the underlying cursor type and base_sentinel_t the underlying
            // sentinel.
            using base_cursor_t = detail::basic_adaptor<range_iterator_t<BaseIterable const>>;
            using base_sentinel_t = detail::basic_adaptor<range_sentinel_t<BaseIterable const>>;

            template<typename Adaptor, typename BaseCursorOrSentinel>
            struct cursor_or_sentinel : private Adaptor
            {
            private:
                template<typename A, typename I>
                friend struct cursor_or_sentinel;
                friend struct range_adaptor;
                BaseCursorOrSentinel base_;
                cursor_or_sentinel(Adaptor adaptor, BaseCursorOrSentinel base)
                  : Adaptor(std::move(adaptor)), base_(std::move(base))
                {}
                Adaptor &adaptor()
                {
                    return *this;
                }
                Adaptor const &adaptor() const
                {
                    return *this;
                }
            public:
                cursor_or_sentinel() = default;
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().current(base_))>
                R current() const
                {
                    return adaptor().current(base_);
                }
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().next(base_))>
                void next()
                {
                    adaptor().next(base_);
                }
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().equal(base_, base_))>
                bool equal(cursor_or_sentinel const &that) const
                {
                    return adaptor().equal(base_, that.base_);
                }
                template<typename A = Adaptor,
                         typename A2, typename I,
                         typename R = decltype(std::declval<A>().empty(std::declval<I const &>(), base_))>
                constexpr bool equal(cursor_or_sentinel<A2, I> const &that) const
                {
                    // "that" is the iterator, "this" is the sentinel
                    CONCEPT_ASSERT(Same<I, base_cursor_t>() && Same<A2, detail::begin_adaptor_t<Derived>>());
                    return adaptor().empty(that.base_, base_);
                }
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().prev(base_))>
                void prev()
                {
                    adaptor().prev(base_);
                }
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().advance(base_, 0))>
                void advance(range_difference_t<BaseIterable> n)
                {
                    adaptor().advance(base_, n);
                }
                template<typename A = Adaptor,
                         typename R = decltype(std::declval<A>().distance_to(base_, base_))>
                range_difference_t<BaseIterable>
                distance_to(cursor_or_sentinel const &that) const
                {
                    adaptor().distance_to(base_, that.base_);
                }
            };

            adaptor_defaults get_adaptor(begin_end_tag) const
            {
                return {};
            }

            base_cursor_t base_begin() const
            {
                return {ranges::begin(rng_)};
            }
            base_sentinel_t base_end() const
            {
                return {ranges::end(rng_)};
            }

            template<typename D = Derived>
            cursor_or_sentinel<detail::begin_adaptor_t<D>, base_cursor_t>
            get_begin() const
            {
                auto adaptor = range_core_access::begin_adaptor(derived());
                auto pos = adaptor.begin(derived());
                return {std::move(adaptor), std::move(pos)};
            }

            template<typename D = Derived>
            cursor_or_sentinel<detail::end_adaptor_t<D>, base_sentinel_t>
            get_end() const
            {
                auto adaptor = range_core_access::end_adaptor(derived());
                auto pos = adaptor.end(derived());
                return {std::move(adaptor), std::move(pos)};
            }
        public:
            constexpr range_adaptor(BaseIterable && rng)
              : rng_(detail::forward<BaseIterable>(rng))
            {}
        };
    }
}

#endif
