//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_VIEW_COUNTED_HPP
#define RANGES_V3_VIEW_COUNTED_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename I>
            struct counted_cursor
            {
            private:
                friend struct counted_sentinel;
                I it_;
                iterator_difference_t<I> n_;

                // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60799
                #ifdef __GNUC__
             public:
                #endif
                void advance_(iterator_difference_t<I> n)
                {
                    n_ -= n;
                    ranges::advance(it_, n);
                }
            public:
                using single_pass = SinglePass<I>;
                struct mixin
                  : basic_mixin<counted_cursor>
                {
                    mixin() = default;
                    mixin(counted_cursor pos)
                      : basic_mixin<counted_cursor>{std::move(pos)}
                    {}
                    mixin(I it, iterator_difference_t<I> n)
                      : mixin(counted_cursor{it, n})
                    {}
                    I &base_reference()
                    {
                        return this->get().it_;
                    }
                    I const &base_reference() const
                    {
                        return this->get().it_;
                    }
                    I base() const
                    {
                        return this->get().it_;
                    }
                    iterator_difference_t<I> count() const
                    {
                        return this->get().n_;
                    }
                    // Overload the advance algorithm for counted_iterators.
                    // This is much faster. This gets found by ADL because
                    // counted_iterator inherits from counted_cursor::mixin.
                    friend void advance(counted_iterator<I> &it, iterator_difference_t<I> n)
                    {
                        it.counted_cursor::mixin::get().advance_(n);
                    }
                    // Overload uncounted and recounted for packing and unpacking
                    // counted iterators
                    friend I uncounted(counted_iterator<I> i)
                    {
                        return i.base();
                    }
                    friend counted_iterator<I>
                    recounted(counted_iterator<I> const &j, I i, iterator_difference_t<I> n)
                    {
                        RANGES_ASSERT(ranges::next(j.base(), n) == i);
                        return {i, j.count() - n};
                    }
                    CONCEPT_REQUIRES(RandomAccessIterator<I>())
                    friend counted_iterator<I> recounted(counted_iterator<I> const &j, I i)
                    {
                        return {i, j.count() - (i - j.base_reference())};
                    }
                };
                counted_cursor() = default;
                counted_cursor(I it, iterator_difference_t<I> n)
                  : it_(std::move(it)), n_(n)
                {}
                auto current() const -> decltype(*it_)
                {
                    return *it_;
                }
                bool equal(counted_cursor const &that) const
                {
                    return n_ == that.n_;
                }
                void next()
                {
                    ++it_;
                    --n_;
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
                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                iterator_difference_t<I>
                distance_to(counted_cursor<I> const &that) const
                {
                    return n_ - that.n_;
                }
            };

            struct counted_sentinel
            {
                template<typename I>
                bool equal(counted_cursor<I> const &that) const
                {
                    return that.n_ == 0;
                }
            };
        }

        // For RandomAccessIterator, operator- will be defined by basic_range_iterator
        template<typename I, CONCEPT_REQUIRES_(!RandomAccessIterator<I>())>
        iterator_difference_t<I> operator-(counted_iterator<I> const &end, counted_iterator<I> const &begin)
        {
            return begin.count() - end.count();
        }

        template<typename I>
        iterator_difference_t<I> operator-(counted_sentinel const &end, counted_iterator<I> const &begin)
        {
            return begin.count();
        }

        template<typename I>
        iterator_difference_t<I> operator-(counted_iterator<I> const &begin, counted_sentinel const &end)
        {
            return -begin.count();
        }

        template<typename I>
        iterator_difference_t<I> operator-(counted_sentinel const &, counted_sentinel const &)
        {
            return 0;
        }

        template<typename I>
        struct counted_view
          : range_facade<counted_view<I>>
        {
        private:
            friend range_core_access;
            using size_type = iterator_size_t<I>;
            I it_;
            iterator_difference_t<I> n_;

            detail::counted_cursor<I> begin_cursor() const
            {
                return {it_, n_};
            }
            detail::counted_sentinel end_cursor() const
            {
                return {};
            }
        public:
            counted_view() = default;
            counted_view(I it, iterator_difference_t<I> n)
              : it_(it), n_(n)
            {
                RANGES_ASSERT(0 <= n_);
            }
            size_type size() const
            {
                return static_cast<size_type>(n_);
            }
        };

        namespace view
        {
            struct counted_fn : bindable<counted_fn>
            {
                template<typename I>
                static counted_view<I> invoke(counted_fn, I it, iterator_difference_t<I> n)
                {
                    // Nothing wrong with a weak counted output iterator!
                    CONCEPT_ASSERT(WeakIterator<I>());
                    return {std::move(it), n};
                }
            };

            RANGES_CONSTEXPR counted_fn counted {};
        }
    }
}

#endif
