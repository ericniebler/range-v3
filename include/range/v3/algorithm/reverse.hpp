//  Copyright Neil Groves 2009. 
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_REVERSE_HPP
#define RANGES_V3_ALGORITHM_REVERSE_HPP

#include <utility>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/concepts.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_facade.hpp>

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
                  : rng_(&rng), it_(detail::move(it))
                {}
                void increment()
                {
                    RANGES_ASSERT(it_ != ranges::begin(rng_->rng_));
                    --it_;
                }
                void decrement()
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    ++it_;
                }
                void advance(typename basic_iterator::difference_type n)
                {
                    it_ -= n;
                }
                typename basic_iterator::difference_type distance_to(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ - that.it_;
                }
                bool equal(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                auto dereference() const -> decltype(*it_)
                {
                    RANGES_ASSERT(it_ != ranges::begin(rng_->rng_));
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
                  : rng_(that.rng_), it_(detail::move(that).it_)
                {}
            };

        public:
            using iterator       = basic_iterator<reverse_range>;
            using const_iterator = basic_iterator<reverse_range const>;

            explicit reverse_range(Rng && rng)
              : rng_(detail::forward<Rng>(rng))
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
            /// \brief template function \c reverser::operator()
            ///
            /// range-based version of the \c reverse std algorithm
            ///
            /// \pre \c BidirectionalRange is a model of the BidirectionalRange concept
            template<typename BidirectionalRange,
                CONCEPT_REQUIRES(ranges::BidirectionalRange<BidirectionalRange>())>
            BidirectionalRange operator()(BidirectionalRange && rng) const
            {
                std::reverse(ranges::begin(rng), ranges::end(rng));
                return detail::forward<BidirectionalRange>(rng);
            }

            template<typename BindExpression,
                CONCEPT_REQUIRES(True<is_bind_expression<BindExpression>>())>
            auto operator()(BindExpression && expr) const
                -> decltype(detail::bind(std::declval<reverser>(),
                                         detail::unwrap_bind(detail::forward<BindExpression>(expr))))
            {
                return detail::bind(reverser{},
                                    detail::unwrap_bind(detail::forward<BindExpression>(expr)));
            }

            /// \overload
            template<typename BidirectionalRange,
                CONCEPT_REQUIRES(ranges::BidirectionalRange<BidirectionalRange>())>
            friend reverse_range<BidirectionalRange>
                operator|(BidirectionalRange && rng, reverser)
            {
                return reverse_range<BidirectionalRange>{detail::forward<BidirectionalRange>(rng)};
            }
        };

        constexpr reverser reverse {};

    } // inline namespace v3
} // namespace ranges

#endif // include guard
