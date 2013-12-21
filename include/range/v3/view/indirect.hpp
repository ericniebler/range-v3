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

#ifndef RANGES_V3_VIEW_INDIRECT_HPP
#define RANGES_V3_VIEW_INDIRECT_HPP

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
        template<typename InputRange>
        struct indirect_range_view
        {
        private:
            InputRange rng_;
            using reference_t = decltype(*std::declval<range_reference_t<InputRange const>>());

        public:
            using const_iterator = struct iterator
              : ranges::iterator_facade<
                    iterator
                  , typename std::remove_reference<reference_t>::type
                  , range_category_t<InputRange>
                  , reference_t
                  , range_difference_t<InputRange>
                >
            {
            private:
                friend struct indirect_range_view;
                friend struct ranges::iterator_core_access;
                using base_range_iterator = range_iterator_t<InputRange const>;

                base_range_iterator it_;

                explicit iterator(base_range_iterator it)
                  : it_(std::move(it))
                {}
                void increment()
                {
                    ++it_;
                }
                bool equal(iterator const &that) const
                {
                    return it_ == that.it_;
                }
                void decrement()
                {
                    --it_;
                }
                reference_t dereference() const
                {
                    return **it_;
                }
                void advance(range_difference_t<InputRange> n)
                {
                    it_ += n;
                }
                range_difference_t<InputRange> distance_to(iterator that) const
                {
                    return that.it_ - it_;
                }
            public:
                iterator()
                  : it_{}
                {}
            };

            explicit indirect_range_view(InputRange && rng)
              : rng_(std::forward<InputRange>(rng))
            {}
            iterator begin()
            {
                return iterator{rng_.begin()};
            }
            iterator end()
            {
                return iterator{rng_.end()};
            }
            const_iterator begin() const
            {
                return const_iterator{rng_.begin()};
            }
            const_iterator end() const
            {
                return const_iterator{rng_.end()};
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
            struct indirecter : bindable<indirecter>, pipeable<indirecter>
            {
                template<typename InputRange>
                static indirect_range_view<InputRange>
                invoke(indirecter, InputRange && rng)
                {
                    CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                    return indirect_range_view<InputRange>{std::forward<InputRange>(rng)};
                }
            };

            RANGES_CONSTEXPR indirecter indirect{};
        }
    }
}

#endif
