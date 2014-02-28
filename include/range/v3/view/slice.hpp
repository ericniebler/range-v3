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

#include <limits>
#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/utility/compressed_tuple.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputIterable>
        struct slice_range_view
        {
        private:
            InputIterable rng_;
            std::size_t from_;
            std::size_t to_;
            // The following iterator essentially amounts to an internal pointer
            // if rng_ is not a reference, so care must be taken to implement copy
            // and move correctly.
            range_iterator_t<InputIterable> begin_;

            static constexpr bool use_other_iterator_on_copy_and_move()
            {
                return std::is_reference<InputIterable>::value ||
                       std::is_same<range_category_t<InputIterable>,
                           std::input_iterator_tag>::value;
            }

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

            // Implementation for InputIterator, ForwardIterator and BidirectionalIterator
            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<InputIterable>
                  , range_category_t<InputIterable>
                  , range_reference_t<detail::add_const_if_t<InputIterable, Const>>
                  , std::ptrdiff_t
                >
            {
            private:
                template<bool OtherConst>
                friend struct basic_iterator;
                friend struct slice_range_view;
                friend struct ranges::iterator_core_access;
                using base_range = detail::add_const_if_t<InputIterable, Const>;
                using slice_range_view_ = detail::add_const_if_t<slice_range_view, Const>;

                slice_range_view_ *rng_;
                std::size_t n_;
                compressed_tuple<range_iterator_t<base_range>, is_dirty_t> it_dirt_;

                basic_iterator(slice_range_view_ &rng, begin_tag)
                  : rng_(&rng), n_(rng_->from_), it_dirt_{rng_->begin_, false}
                {}
                basic_iterator(slice_range_view_ &rng, end_tag)
                  : rng_(&rng), n_(rng_->to_), it_dirt_{rng_->begin_, true}
                {
                    if(is_rand())
                        do_clean();
                }
                void increment()
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it() != ranges::end(rng_->rng_));
                    ++n_;
                    ++it();
                }
                template<bool OtherConst>
                bool equal(basic_iterator<OtherConst> const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return n_ == that.n_;
                }
                range_reference_t<base_range> dereference() const
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it() != ranges::end(rng_->rng_));
                    return *it();
                }
                void decrement()
                {
                    RANGES_ASSERT(rng_->from_ < n_);
                    clean();
                    --n_;
                    --it();
                }
                void advance(std::ptrdiff_t d)
                {
                    RANGES_ASSERT(n_ + d >= rng_->from_ && n_ + d <= rng_->to_);
                    clean();
                    n_ += d;
                    it() += d;
                }
                template<bool OtherConst>
                std::ptrdiff_t distance_to(basic_iterator<OtherConst> const & that) const
                {
                    return n_ >= that.n_ ? -static_cast<std::ptrdiff_t>(n_ - that.n_)
                                         :  static_cast<std::ptrdiff_t>(that.n_ - n_);
                }
                range_iterator_t<base_range> & it()
                {
                    return ranges::get<0>(it_dirt_);
                }
                range_iterator_t<base_range> const & it() const
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
            public:
                constexpr basic_iterator()
                  : rng_{}, n_{}, it_dirt_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, enable_if_t<!OtherConst> = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : rng_(that.rng_), n_(that.n_), it_dirt_(std::move(that).it_dirt_)
                {}
            };

        public:
            using iterator       = basic_iterator<false>;
            using const_iterator = basic_iterator<true>;

            slice_range_view(InputIterable && rng, std::size_t from, std::size_t to)
              : rng_(std::forward<InputIterable>(rng))
              , from_(from)
              , to_(to)
              , begin_(ranges::next(ranges::begin(rng_), from_))
            {
                RANGES_ASSERT(from <= to);
                // Must be able to represent iterator difference:
                RANGES_ASSERT(to - from <= static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()));
            }
            slice_range_view(slice_range_view const &that)
              : rng_(that.rng_)
              , from_(that.from_)
              , to_(that.to_)
              , begin_(use_other_iterator_on_copy_and_move() ?
                           that.begin_ :
                           ranges::next(ranges::begin(rng_), from_))
            {}
            slice_range_view(slice_range_view &&that)
              : rng_(std::move(that).rng_)
              , from_(that.from_)
              , to_(that.to_)
              , begin_(use_other_iterator_on_copy_and_move() ?
                           std::move(that).begin_ :
                           ranges::next(ranges::begin(rng_), from_))
            {}
            slice_range_view &operator=(slice_range_view const &that) = delete;
            slice_range_view &operator=(slice_range_view &&that) = delete;

            iterator begin()
            {
                return {*this, begin_tag{}};
            }
            const_iterator begin() const
            {
                return {*this, begin_tag{}};
            }
            iterator end()
            {
                return {*this, end_tag{}};
            }
            const_iterator end() const
            {
                return {*this, end_tag{}};
            }
            bool operator!() const
            {
                return to_ == from_;
            }
            explicit operator bool() const
            {
                return to_ != from_;
            }
            InputIterable & base()
            {
                return rng_;
            }
            InputIterable const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct slicer : bindable<slicer>
            {
                template<typename InputIterable>
                static slice_range_view<InputIterable>
                invoke(slicer, InputIterable && rng, std::size_t from, std::size_t to)
                {
                    CONCEPT_ASSERT(ranges::InputIterable<InputIterable>());
                    return {std::forward<InputIterable>(rng), from, to};
                }
                template<typename Slicer = slicer>
                static auto
                invoke(Slicer slice, std::size_t from, std::size_t to) ->
                    decltype(slice.move_bind(std::placeholders::_1, (std::size_t)from, (std::size_t)to))
                {
                    return slice.move_bind(std::placeholders::_1, (std::size_t)from, (std::size_t)to);
                }
            };

            RANGES_CONSTEXPR slicer slice {};
        }
    }
}

#endif
