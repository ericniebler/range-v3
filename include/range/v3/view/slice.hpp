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

#include <atomic>
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
#include <range/v3/view/all.hpp>

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
                CONCEPT_ASSERT(InputIterable<Rng>());
                CONCEPT_ASSERT(IsTakeView || ForwardIterable<Rng>());
                friend range_core_access;
                using size_type_ = range_size_t<Rng>;
                using difference_type_ = range_difference_t<Rng>;
                using from_t =
                    detail::conditional_t<
                        IsTakeView,
                        constant<size_type_, 0>,
                        mutable_<std::atomic<size_type_>>>;
                using base_range_t = view::all_t<Rng>;
                // Mutable here. Const-correctness is enforced below by only conditionally
                // allowing the const-qualified begin_cursor()/end_cursor() accessors.
                mutable base_range_t rng_;
                compressed_pair<from_t, size_type_> from_to_;

                constexpr size_type_ from() const
                {
                    return from_to_.first();
                }
                size_type_ to() const
                {
                    return from_to_.second();
                }

                using Bidi = Same<range_category_t<Rng>, ranges::bidirectional_iterator_tag>;
                using Rand = Same<range_category_t<Rng>, ranges::random_access_iterator_tag>;
                using dirty_t =
                    detail::conditional_t<
                        (Bidi()),
                        mutable_<std::atomic<bool>>,
                        constant<bool, false>>;

                struct cursor : private dirty_t
                {
                private:
                    using base_iterator_t = range_iterator_t<base_range_t>;
                    basic_sliced_view const *rng_;
                    size_type_ n_;
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
                        it_ = ranges::next(begin(rng_->rng_), rng_->to());
                    }
                public:
                    cursor() = default;
                    cursor(basic_sliced_view const &rng, begin_tag)
                      : dirty_t{false}, rng_(&rng), n_(rng.from())
                      , it_(ranges::next(begin(rng.rng_), rng.from()))
                    {}
                    cursor(basic_sliced_view const &rng, end_tag)
                      : dirty_t{true}, rng_(&rng), n_(rng.to())
                      , it_(begin(rng.rng_))
                    {
                        if(Rand())
                            do_clean();
                    }
                    range_reference_t<base_range_t> current() const
                    {
                        RANGES_ASSERT(n_ < rng_->to());
                        RANGES_ASSERT(it_ != end(rng_->rng_));
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
                        RANGES_ASSERT(it_ != end(rng_->rng_));
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
                    void advance(difference_type_ d)
                    {
                        RANGES_ASSERT(n_ + d >= rng_->from() && n_ + d <= rng_->to());
                        clean();
                        n_ += d;
                        it_ += d;
                    }
                    CONCEPT_REQUIRES(RandomAccessIterator<base_iterator_t>())
                    difference_type_ distance_to(cursor const & that) const
                    {
                        return static_cast<difference_type_>(that.n_) - static_cast<difference_type_>(n_);
                    }
                };
                cursor begin_cursor()
                {
                    return {*this, begin_tag{}};
                }
                cursor end_cursor()
                {
                    return {*this, end_tag{}};
                }
                CONCEPT_REQUIRES(Range<base_range_t const>())
                cursor begin_cursor() const
                {
                    return {*this, begin_tag{}};
                }
                CONCEPT_REQUIRES(Range<base_range_t const>())
                cursor end_cursor() const
                {
                    return {*this, end_tag{}};
                }
            public:
                basic_sliced_view() = default;
                CONCEPT_REQUIRES(IsTakeView)
                basic_sliced_view(Rng && rng, size_type_ to)
                  : rng_(view::all(std::forward<Rng>(rng))), from_to_(0, to)
                {}
                CONCEPT_REQUIRES(!IsTakeView)
                basic_sliced_view(Rng && rng, size_type_ from, size_type_ to)
                  : rng_(view::all(std::forward<Rng>(rng))), from_to_(from, to)
                {
                    RANGES_ASSERT(from <= to);
                }
                size_type_ size() const
                {
                    return to() - from();
                }
            };
        }

        template<typename Rng>
        struct sliced_view
          : detail::basic_sliced_view<Rng>
        {
            CONCEPT_ASSERT(ForwardIterable<Rng>());

            using size_type_ = range_size_t<Rng>;

            sliced_view() = default;
            sliced_view(Rng && rng, size_type_ from, size_type_ to)
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
                    CONCEPT_ASSERT(ForwardIterable<Rng>());
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
