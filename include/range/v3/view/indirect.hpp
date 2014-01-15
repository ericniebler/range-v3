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
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/debug_iterator.hpp>
#include <range/v3/utility/iterator_adaptor.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputRange>
        struct indirect_range_view
        {
        private:
            InputRange rng_;
            using reference = decltype(*std::declval<range_reference_t<InputRange const>>());
            using base_range_iterator = range_iterator_t<InputRange const>;

            struct basic_iterator
              : ranges::iterator_adaptor<
                    basic_iterator
                  , base_range_iterator
                  , typename std::remove_reference<reference>::type
                  , use_default
                  , reference
                >
            {
            private:
                friend struct indirect_range_view;
                friend struct ranges::iterator_core_access;
                using iterator_adaptor_ = typename basic_iterator::iterator_adaptor_;

                explicit basic_iterator(base_range_iterator it)
                  : iterator_adaptor_(std::move(it))
                {}
                reference dereference() const
                {
                    return **this->base();
                }
            public:
                constexpr basic_iterator()
                  : iterator_adaptor_{}
                {}
            };
        public:
            using iterator = RANGES_DEBUG_ITERATOR(indirect_range_view const, basic_iterator);
            using const_iterator = iterator;

            explicit indirect_range_view(InputRange && rng)
              : rng_(std::forward<InputRange>(rng))
            {}
            iterator begin() const
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this, basic_iterator{ranges::begin(rng_)});
            }
            iterator end() const
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this, basic_iterator{ranges::end(rng_)});
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
