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

#ifndef RANGES_V3_VIEW_SLICE_HPP
#define RANGES_V3_VIEW_SLICE_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename ForwardIterable>
        struct sliced_view
          : range_facade<sliced_view<ForwardIterable>>
        {
        private:
            friend range_core_access;
            using size_type = range_size_t<ForwardIterable>;
            using difference_type = range_difference_t<ForwardIterable>;
            ForwardIterable rng_;
            size_type from_, to_;

            using Bidi = Same<range_category_t<ForwardIterable>, std::bidirectional_iterator_tag>;
            using Rand = Same<range_category_t<ForwardIterable>, std::random_access_iterator_tag>;
            using dirty_t = detail::conditional_t<(Bidi()), mutable_<bool>, constant<bool, false>>;

            struct cursor : private dirty_t
            {
            private:
                sliced_view const *rng_;
                range_iterator_t<ForwardIterable> it_;
                size_type n_;

                dirty_t & dirty() { return *this; }
                dirty_t const & dirty() const { return *this; }
            public:
                cursor(sliced_view const &rng, begin_tag)
                  : dirty_t{false}, rng_(&rng), n_(rng.from_), it_(ranges::next(ranges::begin(rng.rng_), rng.from_))
                {}
                cursor(sliced_view const &rng, end_tag)
                  : dirty_t{true}, rng_(&rng), n_(rng.to_), it_(ranges::begin(rng.rng_))
                {
                    if(Rand())
                        do_clean();
                }
                range_reference_t<ForwardIterable> current() const
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    return *it_;
                }
                bool equal(cursor const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return n_ == that.n_;
                }
                void next()
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    ++n_;
                    ++it_;
                }
                void prev()
                {
                    RANGES_ASSERT(rng_->from_ < n_);
                    clean();
                    --n_;
                    --it_;
                }
                void advance(difference_type d)
                {
                    RANGES_ASSERT(n_ + d >= rng_->from_ && n_ + d <= rng_->to_);
                    clean();
                    n_ += d;
                    it_ += d;
                }
                difference_type distance_to(cursor const & that) const
                {
                    return static_cast<difference_type>(that.n_) - static_cast<difference_type>(n_);
                }
                void clean()
                {
                    if(dirty())
                    {
                        // BUGBUG investigate why this gets called twice
                        //std::cout << "\ncleaning!!!\n";
                        do_clean();
                        dirty() = false;
                    }
                }
                void do_clean()
                {
                    it_ = ranges::next(ranges::begin(rng_->rng_), rng_->to_);
                }
            };
            cursor begin_cursor() const
            {
                return {*this, begin_tag{}};
            }
            cursor end_cursor() const
            {
                return {*this, end_tag{}};
            }
        public:
            sliced_view() = default;
            sliced_view(ForwardIterable && rng, size_type from, size_type to)
              : rng_(std::forward<ForwardIterable>(rng))
              , from_(from)
              , to_(to)
            {
                RANGES_ASSERT(from <= to);
            }
        };

        namespace view
        {
            struct slicer : bindable<slicer>
            {
                template<typename ForwardIterable>
                static sliced_view<ForwardIterable>
                invoke(slicer, ForwardIterable && rng, range_size_t<ForwardIterable> from,
                    range_size_t<ForwardIterable> to)
                {
                    CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                    CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                    return {std::forward<ForwardIterable>(rng), from, to};
                }
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto
                invoke(slicer slice, Int from, Int to) ->
                    decltype(slice.move_bind(std::placeholders::_1, (Int)from, (Int)to))
                {
                    return slice.move_bind(std::placeholders::_1, (Int)from, (Int)to);
                }
            };

            RANGES_CONSTEXPR slicer slice {};
        }
    }
}

#endif
