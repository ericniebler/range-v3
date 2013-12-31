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
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_facade.hpp>

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
                       std::is_same<range_category_t<InputIterable>, std::input_iterator_tag>::value;
            }

            static constexpr bool is_bidi()
            {
                return std::is_same<range_category_t<InputIterable>,
                    std::bidirectional_iterator_tag>::value;
            }

            using is_dirty_t = box<
                    detail::conditional_t<is_bidi(), bool, constant<bool, false>>
                  , detail::dirty_tag
                >;

            // Implementation for InputIterator, ForwardIterator and BidirectionalIterator
            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<detail::add_const_if_t<InputIterable, Const>>
                  , range_category_t<InputIterable>
                  , range_reference_t<detail::add_const_if_t<InputIterable, Const>>
                  , range_difference_t<InputIterable>
                >
              , private is_dirty_t
            {
            private:
                friend struct slice_range_view;
                friend struct ranges::iterator_core_access;
                using base_range = detail::add_const_if_t<InputIterable, Const>;
                using slice_range_view_ = detail::add_const_if_t<slice_range_view, Const>;

                slice_range_view_ *rng_;
                std::size_t n_;
                range_iterator_t<base_range> it_;

                basic_iterator(slice_range_view_ &rng, detail::begin_tag)
                  : is_dirty_t{false}, rng_(&rng), n_(rng_->from_), it_(rng_->begin_)
                {}
                basic_iterator(slice_range_view_ &rng, detail::end_tag)
                  : is_dirty_t{true}, rng_(&rng), n_(rng_->to_), it_(rng_->begin_)
                {}
                void increment()
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    this->check_end(range_concept_t<InputIterable>{});
                    ++it_;
                    ++n_;
                }
                bool equal(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return n_ == that.n_;
                }
                range_reference_t<base_range> dereference() const
                {
                    RANGES_ASSERT(n_ < rng_->to_);
                    this->check_end(range_concept_t<InputIterable>{});
                    return *it_;
                }
                void decrement()
                {
                    RANGES_ASSERT(rng_->from_ < n_);
                    clean();
                    --n_;
                    --it_;
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
                    it_ = ranges::next(rng_->begin_, rng_->to_ - rng_->from_);
                }
                bool is_dirty() const
                {
                    return ranges::get<detail::dirty_tag>(*this);
                }
                void set_dirty(bool b)
                {
                    ranges::get<detail::dirty_tag>(*this) = b;
                }
                void check_end(concepts::Iterable) const
                {
                    // no-op, iterables don't have ends to check
                }
                void check_end(concepts::Range) const
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                }
            public:
                basic_iterator()
                  : is_dirty_t{}, rng_{}, n_{}, it_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : is_dirty_t{that.is_dirty()}
                  , rng_(that.rng_), n_(that.n_), it_(std::move(that).it_)
                {}
            };

            template<bool Const, typename Category = range_category_t<InputIterable>>
            struct iterator_factory
            {
                using iterator = basic_iterator<Const>;
                using slice_range_view_ = detail::add_const_if_t<slice_range_view, Const>;
                template<typename Tag>
                static iterator make_iterator(slice_range_view_ &rng, Tag tag)
                {
                    return {rng, tag};
                }
            };

            // For random-access ranges, the view's iterator type can simply be
            // the adapted range's iterator type.
            template<bool Const>
            struct iterator_factory<Const, std::random_access_iterator_tag>
            {
                using base_range_iterator =
                    range_iterator_t<detail::add_const_if_t<InputIterable, Const>>;
                using slice_range_view_ =
                    detail::add_const_if_t<slice_range_view, Const>;
                using iterator =
                    RANGES_DEBUG_ITERATOR(slice_range_view_, base_range_iterator);

                static iterator make_iterator(slice_range_view_ &rng, detail::begin_tag tag)
                {
                    return RANGES_MAKE_DEBUG_ITERATOR(rng,
                        base_range_iterator{rng.begin_});
                }
                static iterator make_iterator(slice_range_view_ &rng, detail::end_tag tag)
                {
                    return RANGES_MAKE_DEBUG_ITERATOR(rng,
                        base_range_iterator{rng.begin_ + (rng.to_ - rng.from_)});
                }
            };

            template<bool Const>
            using basic_iterator_t = typename iterator_factory<Const>::iterator;
        public:
            using iterator       = basic_iterator_t<false>;
            using const_iterator = basic_iterator_t<true>;

            slice_range_view(InputIterable && rng, std::size_t from, std::size_t to)
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
                return iterator_factory<false>::make_iterator(*this, detail::begin_tag{});
            }
            iterator end()
            {
                return iterator_factory<false>::make_iterator(*this, detail::end_tag{});
            }
            const_iterator begin() const
            {
                return iterator_factory<true>::make_iterator(*this, detail::begin_tag{});
            }
            const_iterator end() const
            {
                return iterator_factory<true>::make_iterator(*this, detail::end_tag{});
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
                invoke(Slicer slice, std::size_t from, std::size_t to)
                    -> decltype(slice.move_bind(std::placeholders::_1, (std::size_t)from, (std::size_t)to))
                {
                    return slice.move_bind(std::placeholders::_1, (std::size_t)from, (std::size_t)to);
                }
            };

            RANGES_CONSTEXPR slicer slice {};
        }
    }
}

#endif
