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
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator>
            struct counted_cursor
            {
            private:
                friend struct counted_sentinel<InputIterator>;
                InputIterator it_;
                iterator_difference_t<InputIterator> n_;
            public:
                counted_cursor(public_t, InputIterator it, iterator_difference_t<InputIterator> n)
                  : it_(std::move(it)), n_(n)
                {}
                InputIterator base() const
                {
                    return it_;
                }
                iterator_difference_t<InputIterator> count() const
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
                CONCEPT_REQUIRES(BidirectionalIterator<InputIterator>())
                void prev()
                {
                    --it_;
                    --n_;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<InputIterator>())
                void advance(iterator_difference_t<InputIterator> n)
                {
                    it_ += n;
                    n_ += n;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<InputIterator>())
                iterator_difference_t<InputIterator>
                distance_to(counted_iterable_view<InputIterator> const &that) const
                {
                    return that.n_ - n_;
                }
            };

            template<typename InputIterator>
            struct counted_sentinel
            {
            private:
                iterator_difference_t<InputIterator> n_;
            public:
                counted_sentinel() = default;
                counted_sentinel(iterator_difference_t<InputIterator> n)
                  : n_(n)
                {}
                bool equal(counted_cursor<InputIterator> const &that) const
                {
                    return n_ == that.n_;
                }
                iterator_difference_t<InputIterator> count() const
                {
                    return n_;
                }
            };
        }

        template<typename InputIterator>
        struct counted_iterable_view
          : range_facade<counted_iterable_view<InputIterator>>
        {
        private:
            friend range_core_access;
            InputIterator it_;
            iterator_difference_t<InputIterator> n_;
            struct cursor
            {
            };
            detail::counted_cursor<InputIterator> get_begin() const
            {
                return {{}, it_, 0};
            }
            detail::counted_sentinel<InputIterator> get_end() const
            {
                return {n_};
            }
            using size_type =
                typename std::make_unsigned<iterator_difference_t<InputIterator>>::type;
        public:
            counted_iterable_view(InputIterator it, iterator_difference_t<InputIterator> n)
              : it_(it), n_(n)
            {}
            size_type size() const
            {
                return static_cast<size_type>(n_);
            }
        };

        namespace view
        {
            struct counted_maker : bindable<counted_maker>
            {
                template<typename InputIterator>
                static counted_iterable_view<InputIterator>
                invoke(counted_maker, InputIterator it, iterator_difference_t<InputIterator> n)
                {
                    CONCEPT_ASSERT(ranges::InputIterator<InputIterator>());
                    return {std::move(it), n};
                }
            };

            RANGES_CONSTEXPR counted_maker counted {};
        }

        template<typename InputIterator>
        iterator_difference_t<InputIterator>
        range_size(counted_iterable_view<InputIterator> const &rng)
        {
            return rng.end().count();
        }
    }
}

#endif
