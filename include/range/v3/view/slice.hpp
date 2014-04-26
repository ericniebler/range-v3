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
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Rng, bool IsTakeView = false>
            struct basic_sliced_view
              : range_facade<basic_sliced_view<Rng, IsTakeView>>
            {
            private:
                CONCEPT_ASSERT(Iterable<Rng>());
                CONCEPT_ASSERT(InputIterator<range_iterator_t<Rng>>());
                CONCEPT_ASSERT(IsTakeView || ForwardIterator<range_iterator_t<Rng>>());
                friend range_core_access;
                using size_type = range_size_t<Rng>;
                using difference_type = range_difference_t<Rng>;
                using from_t = detail::conditional_t<IsTakeView, constant<size_type, 0>, mutable_<size_type>>;
                detail::base_iterable_holder<Rng> rng_;
                compressed_pair<from_t, size_type> from_to_;

                constexpr size_type from() const
                {
                    return from_to_.first();
                }
                size_type to() const
                {
                    return from_to_.second();
                }

                using Bidi = Same<range_category_t<Rng>, ranges::bidirectional_iterator_tag>;
                using Rand = Same<range_category_t<Rng>, ranges::random_access_iterator_tag>;
                using dirty_t = detail::conditional_t<(Bidi()), mutable_<bool>, constant<bool, false>>;

                struct cursor : private dirty_t
                {
                private:
                    using base_iterator_t = range_iterator_t<Rng>;
                    basic_sliced_view const *rng_;
                    size_type n_;
                    base_iterator_t it_;

                    dirty_t & dirty() { return *this; }
                    dirty_t const & dirty() const { return *this; }

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
                        it_ = ranges::next(begin(rng_->rng_.get()), rng_->to());
                    }
                public:
                    cursor() = default;
                    cursor(basic_sliced_view const &rng, begin_tag)
                      : dirty_t{false}, rng_(&rng), n_(rng.from())
                      , it_(ranges::next(begin(rng.rng_.get()), rng.from()))
                    {}
                    cursor(basic_sliced_view const &rng, end_tag)
                      : dirty_t{true}, rng_(&rng), n_(rng.to())
                      , it_(begin(rng.rng_.get()))
                    {
                        if(Rand())
                            do_clean();
                    }
                    range_reference_t<Rng> current() const
                    {
                        RANGES_ASSERT(n_ < rng_->to());
                        RANGES_ASSERT(it_ != end(rng_->rng_.get()));
                        return *it_;
                    }
                    bool equal(cursor const &that) const
                    {
                        RANGES_ASSERT(rng_ == that.rng_);
                        return n_ == that.n_;
                    }
                    void next()
                    {
                        RANGES_ASSERT(n_ < rng_->to());
                        RANGES_ASSERT(it_ != end(rng_->rng_.get()));
                        ++n_;
                        ++it_;
                    }
                    CONCEPT_REQUIRES(BidirectionalIterator<base_iterator_t>())
                    void prev()
                    {
                        RANGES_ASSERT(rng_->from() < n_);
                        clean();
                        --n_;
                        --it_;
                    }
                    CONCEPT_REQUIRES(RandomAccessIterator<base_iterator_t>())
                    void advance(difference_type d)
                    {
                        RANGES_ASSERT(n_ + d >= rng_->from() && n_ + d <= rng_->to());
                        clean();
                        n_ += d;
                        it_ += d;
                    }
                    CONCEPT_REQUIRES(RandomAccessIterator<base_iterator_t>())
                    difference_type distance_to(cursor const & that) const
                    {
                        return static_cast<difference_type>(that.n_) - static_cast<difference_type>(n_);
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
                basic_sliced_view() = default;
                CONCEPT_REQUIRES(IsTakeView)
                basic_sliced_view(Rng && rng, size_type to)
                  : rng_(std::forward<Rng>(rng)), from_to_(0, to)
                {}
                CONCEPT_REQUIRES(!IsTakeView)
                basic_sliced_view(Rng && rng, size_type from, size_type to)
                  : rng_(std::forward<Rng>(rng)), from_to_(from, to)
                {
                    RANGES_ASSERT(from <= to);
                }
                size_type size() const
                {
                    return to() - from();
                }
            };
        }

        template<typename Rng>
        struct sliced_view
          : detail::basic_sliced_view<Rng>
        {
            CONCEPT_ASSERT(Iterable<Rng>());
            CONCEPT_ASSERT(ForwardIterator<range_iterator_t<Rng>>());

            using size_type = range_size_t<Rng>;

            sliced_view() = default;
            sliced_view(Rng && rng, size_type from, size_type to)
              : detail::basic_sliced_view<Rng>{std::forward<Rng>(rng), from, to}
            {}
        };

        namespace view
        {
            struct slice_fn : bindable<slice_fn>
            {
                template<typename Rng>
                static sliced_view<Rng>
                invoke(slice_fn, Rng && rng, range_size_t<Rng> from, range_size_t<Rng> to)
                {
                    CONCEPT_ASSERT(Iterable<Rng>());
                    CONCEPT_ASSERT(ForwardIterator<range_iterator_t<Rng>>());
                    return {std::forward<Rng>(rng), from, to};
                }
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto
                invoke(slice_fn slice, Int from, Int to) ->
                    decltype(slice.move_bind(std::placeholders::_1, (Int)from, (Int)to))
                {
                    return slice.move_bind(std::placeholders::_1, (Int)from, (Int)to);
                }
            };

            RANGES_CONSTEXPR slice_fn slice {};
        }
    }
}

#endif
