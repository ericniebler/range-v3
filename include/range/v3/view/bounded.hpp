//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_VIEW_BOUNDED_HPP
#define RANGES_V3_VIEW_BOUNDED_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct bounded_view
          : range_facade<bounded_view<Rng>, is_infinite<Rng>::value>
        {
        private:
            friend range_core_access;
            using base_range_t = view::all_t<Rng>;
            base_range_t rng_;

            struct cursor
            {
            private:
                using base_iterator_t = range_iterator_t<base_range_t>;
                using base_sentinel_t = range_sentinel_t<base_range_t>;

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
                range_difference_t<base_range_t> distance_to(cursor const &that) const
                {
                    clean();
                    that.clean();
                    return that.it_ - it_;
                }
            };
            cursor begin_cursor()
            {
                return {begin(rng_), end(rng_), false};
            }
            cursor end_cursor()
            {
                return {begin(rng_), end(rng_), true};
            }
            CONCEPT_REQUIRES(Range<base_range_t const>())
            cursor begin_cursor() const
            {
                return {begin(rng_), end(rng_), false};
            }
            CONCEPT_REQUIRES(Range<base_range_t const>())
            cursor end_cursor() const
            {
                return {begin(rng_), end(rng_), true};
            }
        public:
            bounded_view() = default;
            explicit bounded_view(Rng && rng)
              : rng_(view::all(std::forward<Rng>(rng)))
            {}
            CONCEPT_REQUIRES(SizedRange<base_range_t>())
            range_size_t<base_range_t> size() const
            {
                return ranges::size(rng_);
            }
        };

        namespace view
        {
            struct bounded_fn : bindable<bounded_fn>, pipeable<bounded_fn>
            {
                template<typename Rng>
                static bounded_view<Rng>
                invoke(bounded_fn, Rng && rng)
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    CONCEPT_ASSERT(!BoundedIterable<Rng>());
                    return bounded_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            RANGES_CONSTEXPR bounded_fn bounded{};
        }
    }
}

#endif
