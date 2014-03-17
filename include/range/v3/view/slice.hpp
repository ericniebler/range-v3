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
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/compressed_tuple.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputIterable>
        struct slice_range_view
          : range_facade<slice_range_view<InputIterable>>
        {
        private:
            friend range_core_access;
            InputIterable rng_;
            range_difference_t<InputIterable> from_;
            range_difference_t<InputIterable> to_;
            // The following iterator essentially amounts to an internal pointer
            // if rng_ is not a reference, so care must be taken to implement copy
            // and move correctly.
            range_iterator_t<InputIterable> begin_;

            static constexpr bool is_bidi()
            {
                return std::is_same<range_category_t<InputIterable>,
                    std::bidirectional_iterator_tag>::value;
            }

            static constexpr bool is_rand()
            {
                return std::is_same<range_category_t<InputIterable>,
                    std::random_access_iterator_tag>::value;
            }

            using is_dirty_t = detail::conditional_t<is_bidi(), bool, constant<bool, false>>;

            struct cursor
            {
                slice_range_view const *rng_;
                range_difference_t<InputIterable> n_;
                compressed_tuple<range_iterator_t<InputIterable>, is_dirty_t> it_dirt_;

                cursor(slice_range_view const &rng, begin_tag)
                  : rng_(&rng), n_(rng_->from_), it_dirt_{rng_->begin_, false}
                {}
                cursor(slice_range_view const &rng, end_tag)
                  : rng_(&rng), n_(rng_->to_), it_dirt_{rng_->begin_, true}
                {
                    if(is_rand())
                        do_clean();
                }
                range_reference_t<InputIterable> current() const
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it() != ranges::end(rng_->rng_));
                    return *it();
                }
                bool equal(cursor const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return n_ == that.n_;
                }
                void next()
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it() != ranges::end(rng_->rng_));
                    ++n_;
                    ++it();
                }
                void prev()
                {
                    RANGES_ASSERT(rng_->from_ < n_);
                    clean();
                    --n_;
                    --it();
                }
                void advance(range_difference_t<InputIterable> d)
                {
                    RANGES_ASSERT(n_ + d >= rng_->from_ && n_ + d <= rng_->to_);
                    clean();
                    n_ += d;
                    it() += d;
                }
                range_difference_t<InputIterable>
                distance_to(cursor const & that) const
                {
                    return that.n_ - n_;
                }
                range_iterator_t<InputIterable> & it()
                {
                    return ranges::get<0>(it_dirt_);
                }
                range_iterator_t<InputIterable> const & it() const
                {
                    return ranges::get<0>(it_dirt_);
                }
                void clean()
                {
                    if(is_dirty())
                    {
                        // BUGBUG investigate why this gets called twice
                        //std::cout << "\ncleaning!!!\n";
                        do_clean();
                        set_dirty(false);
                    }
                }
                void do_clean()
                {
                    it() = ranges::next(rng_->begin_, rng_->to_ - rng_->from_);
                }
                bool is_dirty() const
                {
                    return ranges::get<1>(it_dirt_);
                }
                void set_dirty(bool b)
                {
                    ranges::get<1>(it_dirt_) = b;
                }
            };
            cursor get_begin() const
            {
                return {*this, begin_tag{}};
            }
            cursor get_end() const
            {
                return {*this, end_tag{}};
            }
        public:
            slice_range_view(InputIterable && rng,
                             range_difference_t<InputIterable> from,
                             range_difference_t<InputIterable> to)
              : rng_(std::forward<InputIterable>(rng))
              , from_(from)
              , to_(to)
              , begin_(ranges::next(ranges::begin(rng_), from_))
            {
                RANGES_ASSERT(from <= to);
            }
            slice_range_view(slice_range_view const &that)
              : rng_(that.rng_)
              , from_(that.from_)
              , to_(that.to_)
              , begin_(ranges::next(ranges::begin(rng_), from_))
            {}
            slice_range_view(slice_range_view &&that)
              : rng_(std::move(that).rng_)
              , from_(that.from_)
              , to_(that.to_)
              , begin_(ranges::next(ranges::begin(rng_), from_))
            {}
            // BUGBUG
            slice_range_view &operator=(slice_range_view const &that) = delete;
            slice_range_view &operator=(slice_range_view &&that) = delete;
        };

        namespace view
        {
            struct slicer : bindable<slicer>
            {
                template<typename InputIterable>
                static slice_range_view<InputIterable>
                invoke(slicer, InputIterable && rng, range_difference_t<InputIterable> from,
                    range_difference_t<InputIterable> to)
                {
                    CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
                    CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                    return {std::forward<InputIterable>(rng), from, to};
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
