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

#ifndef BOOST_RANGE_V3_ISTREAM_RANGE_HPP
#define BOOST_RANGE_V3_ISTREAM_RANGE_HPP

#include <istream>
#include <iterator>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/v3/range_fwd.hpp>

namespace boost
{
    namespace range
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
                  : boost::iterator_facade<
                        iterator,
                        T const,
                        std::input_iterator_tag
                    >
                {
                    iterator() : rng_{} {}
                private:
                    friend class istream_range;
                    friend class boost::iterator_core_access;
    
                    explicit iterator(istream_range const & rng)
                      : rng_(rng ? &rng : nullptr)
                    {}
                    void increment()
                    {
                        // Don't advance a singular iterator
                        BOOST_ASSERT(rng_);
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
                        BOOST_ASSERT(rng_);
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
        }
    }
}

#endif
