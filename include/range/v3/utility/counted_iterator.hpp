/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_UTILITY_COUNTED_ITERATOR_HPP
#define RANGES_V3_UTILITY_COUNTED_ITERATOR_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/basic_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename A, typename B>
            using UnambiguouslyConvertible =
                meta::or_c<
                    (bool)Same<A, B>(),
                    ConvertibleTo<A, B>() == !ConvertibleTo<B, A>()>;

            template<typename A, typename B>
            using UnambiguouslyConvertibleType =
                meta::_t<
                    meta::if_c<
                        (bool)Same<A, B>(),
                        meta::id<A>,
                        meta::if_c<
                            ConvertibleTo<A, B>() && !ConvertibleTo<B, A>(),
                            meta::id<A>,
                            meta::if_c<
                                ConvertibleTo<B, A>() && !ConvertibleTo<A, B>(),
                                meta::id<B>,
                                meta::nil_>>>>;

            template<typename I, bool IsReadable = (bool) Readable<I>()>
            struct counted_cursor_types
            {};

            template<typename I>
            struct counted_cursor_types<I, true>
            {
                using single_pass = SinglePass<I>;
                using value_type = iterator_value_t<I>;
            };

            template<typename I, typename D /* = iterator_difference_t<I>*/>
            struct counted_cursor
              : private counted_cursor_types<I>
            {
            private:
                friend range_access;
                template<typename OtherI, typename OtherD>
                friend struct counted_cursor;
                using difference_type = iterator_difference_t<I>;
                struct mixin
                  : basic_mixin<counted_cursor>
                {
                    mixin() = default;
                    using basic_mixin<counted_cursor>::basic_mixin;
                    mixin(I it, D n)
                      : mixin(counted_cursor{it, n})
                    {}
                    I base() const
                    {
                        return this->get().base();
                    }
                    D count() const
                    {
                        return this->get().count();
                    }
                };

                I it_;
                D n_;

                // Overload the advance algorithm for counted_iterators.
                // This is much faster. This gets found by ADL because
                // counted_cursor is an associated type of counted_iterator.
                friend void advance(counted_iterator<I, D> &it, iterator_difference_t<I> n)
                {
                    counted_cursor &cur = get_cursor(it);
                    cur.n_ -= n;
                    ranges::advance(cur.it_, n);
                }
                // Overload uncounted and recounted for packing and unpacking
                // counted iterators
                friend I uncounted(counted_iterator<I, D> i)
                {
                    return i.base();
                }
                friend counted_iterator<I, D>
                recounted(counted_iterator<I, D> const &j, I i, iterator_difference_t<I> n)
                {
                    RANGES_ASSERT(!ForwardIterator<I>() || ranges::next(j.base(), n) == i);
                    return {i, j.count() - n};
                }
                CONCEPT_REQUIRES(Readable<I>())
                iterator_rvalue_reference_t<I> move() const
                    noexcept(noexcept(iter_move(std::declval<I const &>())))
                {
                    return iter_move(it_);
                }
                CONCEPT_REQUIRES(Readable<I>())
                auto get() const -> decltype(*it_)
                {
                    return *it_;
                }
                template<typename T,
                    CONCEPT_REQUIRES_(ExclusivelyWritable_<I, T &&>())>
                void set(T &&t) const
                {
                    *it_ = (T &&) t;
                }
                bool done() const
                {
                    return 0 == n_;
                }
                void next()
                {
                    ++it_;
                    --n_;
                }
                CONCEPT_REQUIRES(EqualityComparable<D>())
                bool equal(counted_cursor const &that) const
                {
                    return n_ == that.n_;
                }
                CONCEPT_REQUIRES(BidirectionalIterator<I>())
                void prev()
                {
                    --it_;
                    ++n_;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                void advance(iterator_difference_t<I> n)
                {
                    it_ += n;
                    n_ -= n;
                }
                D distance_to(counted_cursor<I> const &that) const
                {
                    return n_ - that.n_;
                }
                D distance_remaining() const
                {
                    return n_;
                }
            public:
                counted_cursor()
                  : it_{}, n_{}
                {}
                counted_cursor(I it, D n)
                  : it_(std::move(it)), n_(n)
                {}
                template<typename OtherI, typename OtherD,
                    CONCEPT_REQUIRES_(ConvertibleTo<OtherI, I>() && ConvertibleTo<OtherD, D>())>
                counted_cursor(counted_cursor<OtherI, OtherD> that)
                  : it_(std::move(that.it_)), n_(std::move(that.n_))
                {}
                I base() const
                {
                    return it_;
                }
                D count() const
                {
                    return n_;
                }
            };
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{

        template<typename I, CONCEPT_REQUIRES_(Iterator<I>())>
        counted_iterator<I> make_counted_iterator(I i, iterator_difference_t<I> n)
        {
            return counted_iterator<I>{std::move(i), n};
        }
        /// @}
    }
}

#endif
