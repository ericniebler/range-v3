// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008. 
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGE_V3_ADAPTOR_REVERSE_HPP
#define RANGE_V3_ADAPTOR_REVERSE_HPP

#include <cassert>
#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct reverse_range
        {
        private:
            Rng rng_;

            template<typename RevRng>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<RevRng>
                  , range_value_t<Rng>
                  , range_category_t<Rng>
                  , decltype(*ranges::begin(std::declval<RevRng &>().rng_))
                  , range_difference_t<Rng>
                >
            {
            private:
                friend struct reverse_range;
                friend struct ranges::iterator_core_access;
                using base_range_iterator = decltype(ranges::begin(std::declval<RevRng &>().rng_));

                RevRng *rng_;
                base_range_iterator it_;

                basic_iterator(RevRng &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
                {}
                void increment()
                {
                    assert(it_ != ranges::begin(rng_->rng_));
                    --it_;
                }
                void decrement()
                {
                    assert(it_ != ranges::end(rng_->rng_));
                    ++it_;
                }
                void advance(typename basic_iterator::difference_type n)
                {
                    it_ -= n;
                }
                typename basic_iterator::difference_type distance_to(basic_iterator const &that) const
                {
                    assert(rng_ == that.rng_);
                    return it_ - that.it_;
                }
                bool equal(basic_iterator const &that) const
                {
                    assert(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                auto dereference() const -> decltype(*it_)
                {
                    assert(it_ != ranges::begin(rng_->rng_));
                    return *std::prev(it_);
                }
            public:
                basic_iterator()
                  : rng_{}, it_{}
                {}
                // For iterator -> const_iterator conversion
                template<typename OtherRevRng,
                         typename = typename std::enable_if<
                                        !std::is_const<OtherRevRng>::value>::type>
                basic_iterator(basic_iterator<OtherRevRng> that)
                  : rng_(that.rng_), it_(std::move(that).it_)
                {}
            };

        public:
            using iterator       = basic_iterator<reverse_range>;
            using const_iterator = basic_iterator<reverse_range const>;

            explicit reverse_range(Rng && rng)
              : rng_(std::forward<Rng>(rng))
            {}
            iterator begin()
            {
                return {*this, ranges::end(rng_)};
            }
            iterator end()
            {
                return {*this, ranges::begin(rng_)};
            }
            const_iterator begin() const
            {
                return {*this, ranges::end(rng_)};
            }
            const_iterator end() const
            {
                return {*this, ranges::begin(rng_)};
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            Rng & base()
            {
                return rng_;
            }
            Rng const & base() const
            {
                return rng_;
            }
        };

        struct reverser
        {
            template<typename Rng>
            reverse_range<Rng> operator()(Rng && rng) const
            {
                return reverse_range<Rng>{std::forward<Rng>(rng)};
            }
            template<typename Rng>
            friend reverse_range<Rng> operator|(Rng && rng, reverser const &)
            {
                return reverse_range<Rng>{std::forward<Rng>(rng)};
            }
        };
        
        constexpr bindable<reverser> reverse {};
    }
}

#endif
