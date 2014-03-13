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
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_facade.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Value>
        struct istream_iterable
          : range_facade<istream_iterable<Value>>
        {
        private:
            friend range_core_access;
            std::istream *sin_;
            mutable Value obj_;
            struct impl
            {
                istream_iterable const *rng_;
                void next()
                {
                    *rng_->sin_ >> rng_->obj_;
                }
                Value const &current() const
                {
                    return rng_->obj_;
                }
                bool done() const
                {
                    return !*rng_->sin_;
                }
            };
            impl begin_impl() const
            {
                return {this};
            }
        public:
            istream_iterable(std::istream &sin)
              : sin_(&sin), obj_{}
            {
                *sin_ >> obj_; // prime the pump
            }
        };

        template<typename Value>
        istream_iterable<Value > istream(std::istream & sin)
        {
            return istream_iterable<Value>{sin};
        }
    }
}

#endif
