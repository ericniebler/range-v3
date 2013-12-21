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
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputRange>
        struct slice_range_view
        {
        private:
            InputRange rng_;
            std::size_t from_;
            std::size_t to_;
            range_iterator_t<InputRange> begin_;

            static constexpr bool skip_iterator_advance()
            {
                return std::is_reference<InputRange>::value ||
                       std::is_same<range_category_t<InputRange>, std::input_iterator_tag>::value;
            }

            // SliceRange is either filter_range or filter_range const.
            template<typename SliceRange>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<SliceRange>
                  , range_value_t<InputRange>
                  , range_category_t<InputRange>
                  , range_reference_t<InputRange>
                  , range_difference_t<InputRange>
                >
            {
            private:
                friend struct slice_range_view;
                friend struct ranges::iterator_core_access;
                using base_range_iterator = range_iterator_t<InputRange>;

                SliceRange *rng_;
                std::size_t n_;
                base_range_iterator it_;

                basic_iterator(SliceRange &rng, std::size_t n, base_range_iterator it)
                  : rng_(&rng), n_(n), it_(std::move(it))
                {}
                void increment()
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    ++it_;
                    ++n_;
                }
                void decrement()
                {
                    RANGES_ASSERT(rng_->from_ < n_);
                    if(n_ == rng_->to_ && it_ == ranges::end(rng_->rng_))
                        it_ = std::next(rng_->begin_, rng_->to_ - rng_->from_);
                    --n_;
                    --it_;
                }
                bool equal(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return n_ == that.n_;
                }
                range_reference_t<InputRange> dereference() const
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    return *it_;
                }
                void advance(range_difference_t<InputRange> n)
                {
                    if(n_ == rng_->to_)
                        it_ = rng_->begin_ + (rng_->to_ - rng_->from_);
                    n_ += n;
                    it_ += n;
                }
                range_difference_t<InputRange> distance_to(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return that.n_ - n_;
                }
            public:
                basic_iterator()
                  : rng_{}, n_{}, it_{}
                {}
                // For iterator -> const_iterator conversion
                template<typename OtherSliceRange,
                         typename = typename std::enable_if<
                                        !std::is_const<OtherSliceRange>::value>::type>
                basic_iterator(basic_iterator<OtherSliceRange> that)
                  : rng_(that.rng_), n_(that.n_), it_(std::move(that).it_)
                {}
            };
        public:
            using iterator       = basic_iterator<slice_range_view>;
            using const_iterator = basic_iterator<slice_range_view const>;

            slice_range_view(InputRange && rng, std::size_t from, std::size_t to)
              : rng_(std::forward<InputRange>(rng))
              , from_(from)
              , to_(to)
              , begin_(std::next(ranges::begin(rng_), from_))
            {}
            slice_range_view(slice_range_view const &that)
              : rng_(that.rng_)
              , from_(that.from_)
              , to_(that.to_)
              , begin_(skip_iterator_advance() ?
                       that.begin_ :
                       std::next(ranges::begin(rng_), from_))
            {}
            slice_range_view(slice_range_view &&that)
              : rng_(std::move(that).rng_)
              , from_(that.from_)
              , to_(that.to_)
              , begin_(skip_iterator_advance() ?
                       std::move(that).begin_ :
                       std::next(ranges::begin(rng_), from_))
            {}
            slice_range_view &operator=(slice_range_view const &that)
            {
                rng_   = that.rng_;
                from_  = that.from_;
                to_    = that.to_;
                begin_ = skip_iterator_advance() ?
                         that.begin_ :
                         std::next(ranges::begin(rng_), from_);
                return *this;
            }
            slice_range_view &operator=(slice_range_view &&that)
            {
                rng_   = std::move(that).rng_;
                from_  = that.from_;
                to_    = that.to_;
                begin_ = skip_iterator_advance() ?
                         std::move(that).begin_ :
                         std::next(ranges::begin(rng_), from_);
                return *this;
            }
            iterator begin()
            {
                return {*this, from_, begin_};
            }
            iterator end()
            {
                return {*this, to_, ranges::end(rng_)};
            }
            const_iterator begin() const
            {
                return {*this, from_, begin_};
            }
            const_iterator end() const
            {
                return {*this, to_, ranges::end(rng_)};
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            InputRange & base()
            {
                return rng_;
            }
            InputRange const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct slicer : bindable<slicer>
            {
                template<typename InputRange>
                static slice_range_view<InputRange>
                invoke(slicer, InputRange && rng, std::size_t from, std::size_t to)
                {
                    return {std::forward<InputRange>(rng), from, to};
                }
                template<typename Slicer = slicer>
                static auto
                invoke(Slicer slice, std::size_t from, std::size_t to)
                    -> decltype(slice(std::placeholders::_1, (std::size_t)from, (std::size_t)to))
                {
                    return slice(std::placeholders::_1, (std::size_t)from, (std::size_t)to);
                }
            };

            RANGES_CONSTEXPR slicer slice {};
        }
    }
}

#endif
