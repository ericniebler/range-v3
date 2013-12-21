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
#include <range/v3/next_prev.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct begin_tag {};
            struct end_tag {};
        }

        template<typename InputRange>
        struct slice_range_view
        {
        private:
            InputRange rng_;
            std::size_t from_;
            std::size_t to_;
            // The following iterator essentially ammounts to an internal pointer
            // if rng_ is not a reference, so care must be taken to implement copy
            // and move correctly.
            range_iterator_t<InputRange> begin_;

            static constexpr bool use_other_iterator_on_copy_and_move()
            {
                return std::is_reference<InputRange>::value ||
                       std::is_same<range_category_t<InputRange>, std::input_iterator_tag>::value;
            }

            template<bool Const>
            struct basic_iterator;

            template<bool Const>
            using iterator_facade_ =
                ranges::iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<detail::add_const_if_t<InputRange, Const>>
                  , range_category_t<detail::add_const_if_t<InputRange, Const>>
                  , range_reference_t<detail::add_const_if_t<InputRange, Const>>
                  , range_difference_t<detail::add_const_if_t<InputRange, Const>>
                >;

            // Implementation for InputIterator and ForwardIterator
            template<bool Const, typename Category = range_category_t<InputRange>>
            struct basic_iterator_impl
              : iterator_facade_<Const>
            {
            private:
                friend struct ranges::iterator_core_access;
                friend struct basic_iterator<Const>;
                friend struct basic_iterator_impl<Const, std::bidirectional_iterator_tag>;
                using base_range = detail::add_const_if_t<InputRange, Const>;
                using slice_range_view_ = detail::add_const_if_t<slice_range_view, Const>;

                slice_range_view_ *rng_;
                std::size_t n_;
                range_iterator_t<base_range> it_;

                basic_iterator_impl()
                  : rng_{}, n_{}, it_{}
                {}
                basic_iterator_impl(slice_range_view_ &rng, detail::begin_tag)
                  : rng_(&rng), n_(rng_->from_), it_(rng_->begin_)
                {}
                basic_iterator_impl(slice_range_view_ &rng, detail::end_tag)
                  : rng_(&rng), n_(rng_->to_), it_(rng_->begin_)
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst,
                         typename std::enable_if<!OtherConst, int>::type = 0>
                basic_iterator_impl(basic_iterator_impl<OtherConst> that)
                  : rng_(that.rng_), n_(that.n_), it_(std::move(that).it_)
                {}
                void increment()
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    ++it_;
                    ++n_;
                }
                bool equal(basic_iterator_impl const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return n_ == that.n_;
                }
                range_reference_t<base_range> dereference() const
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    return *it_;
                }
            };

            // Implementation for BidirectionalIterator
            template<bool Const>
            struct basic_iterator_impl<Const, std::bidirectional_iterator_tag>
              : basic_iterator_impl<Const, std::input_iterator_tag>
            {
            private:
                bool dirty_ = false;
                friend struct ranges::iterator_core_access;
                friend struct basic_iterator<Const>;
                using input_iterator_impl = basic_iterator_impl<Const, std::input_iterator_tag>;
                using typename input_iterator_impl::base_range;
                using typename input_iterator_impl::slice_range_view_;

                using input_iterator_impl::input_iterator_impl;
                basic_iterator_impl(slice_range_view_ &rng, detail::end_tag et)
                  : input_iterator_impl(rng, et), dirty_(true)
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst,
                         typename std::enable_if<!OtherConst, int>::type = 0>
                basic_iterator_impl(basic_iterator_impl<OtherConst,
                                                        std::bidirectional_iterator_tag> that)
                  : input_iterator_impl(std::move(that)), dirty_{that.dirty_}
                {}
                void decrement()
                {
                    RANGES_ASSERT(this->rng_->from_ < this->n_);
                    clean();
                    --this->n_;
                    --this->it_;
                }
                void clean()
                {
                    if(dirty_)
                    {
                        // BUGBUG investigate why this gets called twice
                        //std::cout << "\ncleaning!!!\n";
                        this->it_ = ranges::next(this->rng_->begin_,
                                                 this->rng_->to_ - this->rng_->from_);
                        dirty_ = false;
                    }
                }
            };

            // Implementation for RandomAccessIterator
            template<bool Const>
            struct basic_iterator_impl<Const, std::random_access_iterator_tag>
              : iterator_facade_<Const>
            {
            private:
                friend struct ranges::iterator_core_access;
                friend struct basic_iterator<Const>;
                using base_range = detail::add_const_if_t<InputRange, Const>;
                using slice_range_view_ = detail::add_const_if_t<slice_range_view, Const>;

                slice_range_view_ *rng_;
                range_iterator_t<base_range> it_;

                basic_iterator_impl(slice_range_view_ &rng, detail::begin_tag)
                  : rng_(&rng), it_(rng_->begin_)
                {}
                basic_iterator_impl(slice_range_view_ &rng, detail::end_tag)
                  : rng_(&rng), it_(rng_->begin_ + (rng_->to_ - rng_->from_))
                {}
                void increment()
                {
                    RANGES_ASSERT(it_ < rng_->begin_ + (rng_->to_ - rng_->from_));
                    ++it_;
                }
                bool equal(basic_iterator_impl const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                range_reference_t<base_range> dereference() const
                {
                    RANGES_ASSERT(it_ < rng_->begin_ + (rng_->to_ - rng_->from_));
                    RANGES_ASSERT(rng_->begin_ <= it_);
                    return *it_;
                }
                void decrement()
                {
                    RANGES_ASSERT(rng_->begin_ < it_);
                    --it_;
                }
                void advance(range_difference_t<base_range> n)
                {
                    RANGES_ASSERT(it_ + n <= rng_->begin_ + (rng_->to_ - rng_->from_));
                    RANGES_ASSERT(rng_->begin_ <= it_ + n);
                    it_ += n;
                }
                range_difference_t<base_range> distance_to(basic_iterator_impl const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return that.it_ - it_;
                }
            };

            template<bool Const>
            struct basic_iterator
              : basic_iterator_impl<Const>
            {
            private:
                friend struct slice_range_view;
                using basic_iterator_impl<Const>::basic_iterator_impl;
            public:
                basic_iterator()
                  : basic_iterator_impl<Const>{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst,
                         typename std::enable_if<!OtherConst, int>::type = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : basic_iterator_impl<Const>(std::move(that))
                {}
            };
        public:
            using iterator       = basic_iterator<false>;
            using const_iterator = basic_iterator<true>;

            slice_range_view(InputRange && rng, std::size_t from, std::size_t to)
              : rng_(std::forward<InputRange>(rng))
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
            slice_range_view &operator=(slice_range_view const &that)
            {
                rng_   = that.rng_;
                from_  = that.from_;
                to_    = that.to_;
                begin_ = use_other_iterator_on_copy_and_move() ?
                             that.begin_ :
                             ranges::next(ranges::begin(rng_), from_);
                return *this;
            }
            slice_range_view &operator=(slice_range_view &&that)
            {
                rng_   = std::move(that).rng_;
                from_  = that.from_;
                to_    = that.to_;
                begin_ = use_other_iterator_on_copy_and_move() ?
                             std::move(that).begin_ :
                             ranges::next(ranges::begin(rng_), from_);
                return *this;
            }
            iterator begin()
            {
                return {*this, detail::begin_tag{}};
            }
            iterator end()
            {
                return {*this, detail::end_tag{}};
            }
            const_iterator begin() const
            {
                return {*this, detail::begin_tag{}};
            }
            const_iterator end() const
            {
                return {*this, detail::end_tag{}};
            }
            bool operator!() const
            {
                return to_ == from_;
            }
            explicit operator bool() const
            {
                return to_ != from_;
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
