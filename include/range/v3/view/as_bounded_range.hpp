//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_VIEW_AS_BOUNDED_RANGE_HPP
#define RANGES_V3_VIEW_AS_BOUNDED_RANGE_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct as_bounded_range_view
          : range_facade<as_bounded_range_view<Rng>, is_infinite<Rng>::value>
        {
        private:
            friend range_core_access;
            detail::base_range_holder<Rng> rng_;

            struct cursor
            {
            private:
                using base_iterator_t = range_iterator_t<Rng>;
                using base_sentinel_t = range_sentinel_t<Rng>;

                base_iterator_t it_;
                base_sentinel_t se_;
                bool is_sentinel_;

                void clean()
                {
                    if(is_sentinel_)
                    {
                        while(it_ != se_)
                            ++it_;
                        is_sentinel_ = false;
                    }
                }
            public:
                using single_pass = Derived<ranges::input_iterator_tag, range_category_t<Rng>>;
                using difference_type = range_difference_t<Rng>;
                cursor() = default;
                cursor(base_iterator_t it, base_sentinel_t se, bool is_sentinel)
                  : it_(std::move(it)), se_(std::move(se)), is_sentinel_(is_sentinel)
                {}
                auto current() const -> decltype(*it_)
                {
                    RANGES_ASSERT(!is_sentinel_ && it_ != se_);
                    return *it_;
                }
                bool equal(cursor const &that) const
                {
                    return is_sentinel_ ?
                        that.is_sentinel_ || that.it_ == se_ :
                        that.is_sentinel_ ?
                            it_ == that.se_ :
                            it_ == that.it_;
                }
                void next()
                {
                    RANGES_ASSERT(!is_sentinel_ && it_ != se_);
                    ++it_;
                }
                CONCEPT_REQUIRES(BidirectionalIterator<base_iterator_t>())
                void prev()
                {
                    clean();
                    --it_;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<base_iterator_t>())
                void advance(range_difference_t<Rng> n)
                {
                    clean();
                    it_ += n;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<base_iterator_t>())
                range_difference_t<Rng> distance_to(cursor const &that) const
                {
                    clean();
                    that.clean();
                    return that.it_ - it_;
                }
            };
            cursor begin_cursor()
            {
                return {begin(rng_.get()), end(rng_.get()), false};
            }
            cursor end_cursor()
            {
                return {begin(rng_.get()), end(rng_.get()), true};
            }
            CONCEPT_REQUIRES(Range<Rng const>())
            cursor begin_cursor() const
            {
                return {begin(rng_.get()), end(rng_.get()), false};
            }
            CONCEPT_REQUIRES(Range<Rng const>())
            cursor end_cursor() const
            {
                return {begin(rng_.get()), end(rng_.get()), true};
            }
        public:
            as_bounded_range_view() = default;
            explicit as_bounded_range_view(Rng && rng)
              : rng_(std::forward<Rng>(rng))
            {}
            CONCEPT_REQUIRES(SizedRange<Rng>())
            range_size_t<Rng> size() const
            {
                return ranges::size(rng_.get());
            }
        };

        namespace view
        {
            struct as_bounded_range_fn : bindable<as_bounded_range_fn>, pipeable<as_bounded_range_fn>
            {
                template<typename Rng>
                static as_bounded_range_view<Rng>
                invoke(as_bounded_range_fn, Rng && rng)
                {
                    CONCEPT_ASSERT(InputRange<Rng>());
                    CONCEPT_ASSERT(!BoundedRange<Rng>());
                    return as_bounded_range_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            RANGES_CONSTEXPR as_bounded_range_fn as_bounded_range{};
        }
    }
}

#endif
