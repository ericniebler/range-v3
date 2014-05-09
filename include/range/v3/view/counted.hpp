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
                friend struct counted_sentinel<I>;
                I it_;
                iterator_difference_t<I> n_;

            public:
                struct mixin : basic_mixin<counted_cursor>
                {
                    mixin() = default;
                    mixin(counted_cursor pos)
                      : basic_mixin<counted_cursor>{std::move(pos)}
                    {}
                    mixin(I it, iterator_difference_t<I> n)
                      : mixin(counted_cursor{it, n})
                    {}
                    I base() const
                    {
                        return this->get().base();
                    }
                    iterator_difference_t<I> count() const
                    {
                        return this->get().count();
                    }
                };
                counted_cursor() = default;
                counted_cursor(I it, iterator_difference_t<I> n)
                  : it_(std::move(it)), n_(n)
                {}
                I base() const
                {
                    return it_;
                }
                iterator_difference_t<I> count() const
                {
                    return n_;
                }
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
                    ++n_;
                }
                CONCEPT_REQUIRES(BidirectionalIterator<I>())
                void prev()
                {
                    --it_;
                    --n_;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                void advance(iterator_difference_t<I> n)
                {
                    it_ += n;
                    n_ += n;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                iterator_difference_t<I>
                distance_to(counted_view<I> const &that) const
                {
                    return that.n_ - n_;
                }
            };

            template<typename I>
            struct counted_sentinel
            {
            private:
                iterator_difference_t<I> n_;
            public:
                struct mixin : basic_mixin<counted_sentinel>
                {
                    mixin() = default;
                    mixin(counted_sentinel pos)
                      : basic_mixin<counted_sentinel>{std::move(pos)}
                    {}
                    explicit mixin(iterator_difference_t<I> n)
                      : mixin(counted_sentinel{n})
                    {}
                    iterator_difference_t<I> count() const
                    {
                        return this->get().count();
                    }
                };
                counted_sentinel() = default;
                counted_sentinel(iterator_difference_t<I> n)
                  : n_(n)
                {}
                bool equal(counted_cursor<I> const &that) const
                {
                    return n_ == that.n_;
                }
                iterator_difference_t<I> count() const
                {
                    return n_;
                }
            };
        }

        template<typename I>
        struct counted_view
          : range_facade<counted_view<I>>
        {
        private:
            friend range_core_access;
            using size_type = meta_apply<std::make_unsigned, iterator_difference_t<I>>;
            I it_;
            iterator_difference_t<I> n_;

            detail::counted_cursor<I> begin_cursor() const
            {
                return {it_, 0};
            }
            detail::counted_sentinel<I> end_cursor() const
            {
                return {n_};
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
                    CONCEPT_ASSERT(InputIterator<I>());
                    return {std::move(it), n};
                }
            };

            RANGES_CONSTEXPR counted_fn counted {};
        }
    }
}

#endif
