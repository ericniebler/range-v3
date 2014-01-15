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

#ifndef RANGES_V3_ISTREAM_RANGE_HPP
#define RANGES_V3_ISTREAM_RANGE_HPP

#include <istream>
#include <iterator>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        template< typename T >
        struct istream_range
        {
        private:
            std::istream & sin_;
            mutable T obj_;

            bool next() const
            {
                return sin_ >> obj_;
            }
        public:
            // Define const_iterator and iterator together:
            using const_iterator = struct iterator
              : ranges::iterator_facade<
                    iterator,
                    T const,
                    std::input_iterator_tag
                >
            {
                constexpr iterator()
                  : rng_{}
                {}
            private:
                friend struct istream_range;
                friend struct ranges::iterator_core_access;

                explicit iterator(istream_range const & rng)
                  : rng_(rng ? &rng : nullptr)
                {}
                void increment()
                {
                    // Don't advance a singular iterator
                    RANGES_ASSERT(rng_);
                    // Fetch the next element, null out the
                    // iterator if it fails
                    if(!rng_->next())
                        rng_ = nullptr;
                }
                bool equal(iterator that) const
                {
                    return rng_ == that.rng_;
                }
                T const & dereference() const
                {
                    // Don't deref a singular iterator
                    RANGES_ASSERT(rng_);
                    return rng_->obj_;
                }
                istream_range const *rng_;
            };

            explicit istream_range(std::istream & sin)
              : sin_(sin), obj_{}
            {
                next(); // prime the pump
            }
            iterator begin() const
            {
                return iterator{*this};
            }
            iterator end() const
            {
                return {};
            }
            explicit operator bool() const // any objects left?
            {
                return sin_;
            }
            bool operator!() const
            {
                return !sin_;
            }
        };

        template<typename T>
        istream_range<T> istream(std::istream & sin)
        {
            return istream_range<T>{sin};
        }
    }
}

#endif
