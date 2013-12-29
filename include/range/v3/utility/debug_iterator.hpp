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

#ifndef RANGES_V3_UTILITY_DEBUG_ITERATOR_HPP
#define RANGES_V3_UTILITY_DEBUG_ITERATOR_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_adaptor.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Range, typename Iterator>
        struct debug_iterator
          : iterator_adaptor<debug_iterator<Range, Iterator>, Iterator>
        {
        private:
            static_assert(std::is_same<range_iterator_t<Range>, debug_iterator>::value,
                          "Range type does not use debug iterators");
            friend struct iterator_core_access;
            using iterator_adaptor_ = typename debug_iterator::iterator_adaptor_;

            Range * rng_;

            typename iterator_adaptor_::reference dereference() const
            {
                RANGES_ASSERT(*this != ranges::end(*rng_));
                return *this->base();
            }
            bool equal(debug_iterator const & that) const
            {
                RANGES_ASSERT(rng_ == that.rng_);
                return this->base() == that.base();
            }
            void increment()
            {
                RANGES_ASSERT(*this != ranges::end(*rng_));
                ++this->base_reference();
            }
            void decrement() 
            {
                RANGES_ASSERT(*this != ranges::begin(*rng_));
                --this->base_reference();
            }
            void advance(typename iterator_adaptor_::difference_type n)
            {
                RANGES_ASSERT(0 < n ? (ranges::end(*rng_) - *this) >= n
                                    : (ranges::begin(*rng_) - *this) <= n);
                this->base_reference() += n;
            }
            typename iterator_adaptor_::difference_type distance_to(debug_iterator const& that) const
            {
                RANGES_ASSERT(rng_ == that.rng_);
                return that.base() - this->base();
            }
        public:
            debug_iterator()
              : iterator_adaptor_{}, rng_{}
            {}
            debug_iterator(Range & rng, Iterator it)
              : iterator_adaptor_{std::move(it)}, rng_(&rng)
            {}
        };

        struct debug_iterator_maker : bindable<debug_iterator_maker>
        {
            template<typename Range, typename Iterator>
            static debug_iterator<Range, Iterator>
            invoke(debug_iterator_maker, Range & rng, Iterator it)
            {
                return debug_iterator<Range, Iterator>{rng, std::move(it)};
            }
        };

        RANGES_CONSTEXPR debug_iterator_maker make_debug_iterator {};
    }
}

#ifndef NDEBUG
# define RANGES_DEBUG_ITERATOR(...) ranges::debug_iterator<__VA_ARGS__>
# define RANGES_MAKE_DEBUG_ITERATOR(RNG, ...) ranges::make_debug_iterator(RNG, __VA_ARGS__)
#else
# define RANGES_DEBUG_ITERATOR(...) __VA_ARGS__
# define RANGES_MAKE_DEBUG_ITERATOR(RNG, ...) __VA_ARGS__
#endif

#endif
