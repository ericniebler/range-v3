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
            struct basic_adaptor_sentinel;
        }

        // Give Iterable::iterator a simple interface for passing to Derived
        template<typename Iterable>
        struct basic_adaptor_impl
        {
        private:
            template<typename OtherIterable>
            friend struct detail::basic_adaptor_sentinel;
            template<typename OtherIterable>
            friend struct basic_adaptor_impl;
            // TODO remove the const here:
            using base_range_iterator = range_iterator_t<Iterable const>;
            base_range_iterator it_;
        public:
            constexpr basic_adaptor_impl(base_range_iterator it)
              : it_(detail::move(it))
            {}
            constexpr basic_adaptor_impl()
              : it_{}
            {}
            template<typename I, CONCEPT_REQUIRES_(Same<I, Iterable>())>
            basic_adaptor_impl(basic_adaptor_impl<I> that)
              : it_(std::move(that).it_)
            {}
            basic_adaptor_impl &base()
            {
                return *this;
            }
            basic_adaptor_impl const &base() const
            {
                return *this;
            }
            void next()
            {
                ++it_;
            }
            auto current() const -> decltype(*it_)
            {
                return *it_;
            }
            bool equal(basic_adaptor_impl<Iterable> const &that) const
            {
                return it_ == that.it_;
            }
            bool equal(detail::basic_adaptor_sentinel<Iterable> const &that) const
            {
                return that.equal(*this);
            }
            CONCEPT_REQUIRES(BidirectionalIterator<range_iterator_t<Iterable>>())
            void prev()
            {
                --it_;
            }
            CONCEPT_REQUIRES(RandomAccessIterator<range_iterator_t<Iterable>>())
            void advance(range_difference_t<Iterable> n)
            {
                it_ += n;
            }
            CONCEPT_REQUIRES(RandomAccessIterator<range_iterator_t<Iterable>>())
            range_difference_t<Iterable>
            distance_to(basic_adaptor_impl<Iterable> const &that) const
            {
                return that.it_ - it_;
            }
        };

        namespace detail
        {
            template<typename Iterable>
            struct basic_adaptor_sentinel
            {
            private:
                template<typename OtherIterable>
                friend struct basic_adaptor_sentinel;
                // TODO remove the const here
                using base_range_sentinel = range_sentinel_t<Iterable const>;
                base_range_sentinel end_;
            public:
                constexpr basic_adaptor_sentinel(base_range_sentinel end)
                  : end_(detail::move(end))
                {}
                constexpr basic_adaptor_sentinel()
                  : end_{}
                {}
                template<typename I, CONCEPT_REQUIRES_(Same<I, Iterable>())>
                basic_adaptor_sentinel(basic_adaptor_sentinel<I> that)
                  : end_(std::move(that).end_)
                {}
                basic_adaptor_sentinel &base()
                {
                    return *this;
                }
                basic_adaptor_sentinel const &base() const
                {
                    return *this;
                }
                constexpr bool equal(basic_adaptor_impl<Iterable> const &that) const
                {
                    return that.it_ == end_;
                }
            };
        }

        template<typename Iterable>
        using basic_adaptor_sentinel =
            detail::conditional_t<
                (Range<Iterable>())
              , basic_adaptor_impl<Iterable>
              , detail::basic_adaptor_sentinel<Iterable>>;

        template<typename Derived>
        using range_adaptor_t = typename Derived::range_adaptor_;

        template<typename Derived, typename BaseIterable, bool Infinite>
        struct range_adaptor
          : range_facade<Derived, Infinite>
        {
        protected:
            using range_adaptor_ = range_adaptor;
            using range_facade<Derived, Infinite>::derived;

            range_adaptor &adaptor()
            {
                return *this;
            }
            range_adaptor const &adaptor() const
            {
                return *this;
            }
        private:
            BaseIterable rng_;
            friend range_core_access;
            friend Derived;

            basic_adaptor_impl<BaseIterable> begin_impl() const
            {
                return {ranges::begin(rng_)};
            }
            basic_adaptor_sentinel<BaseIterable> end_impl() const
            {
                return {ranges::end(rng_)};
            }
        public:
            constexpr range_adaptor(BaseIterable && rng)
              : rng_(detail::forward<BaseIterable>(rng))
            {}
            BaseIterable & base()
            {
                return rng_;
            }
            BaseIterable const & base() const
            {
                return rng_;
            }
        };
    }
}

#endif
