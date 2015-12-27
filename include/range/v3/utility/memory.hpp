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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef RANGES_V3_UTILITY_MEMORY_HPP
#define RANGES_V3_UTILITY_MEMORY_HPP

#include <memory>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/counted_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            // Can be used with counted_cursor to maintain an external count.
            // Can only be used to create a WeakIterator!
            template<typename I>
            struct external_count
            {
            private:
                I *pi_;
            public:
                external_count() = default;
                external_count(I &i)
                  : pi_(&i)
                {}
                external_count &operator--()
                {
                    ++*pi_;
                    return *this;
                }
                void operator--(int)
                {
                    ++*pi_;
                }
            };

            template<typename T, bool B = std::is_trivially_destructible<T>::value>
            struct destroy_n
            {
                void operator()(T const *) const noexcept
                {}
                destroy_n &operator++()
                {
                    return *this;
                }
                void operator++(int)
                {}
            };

            template<typename T>
            struct destroy_n<T, false>
            {
            private:
                std::ptrdiff_t n_;
            public:
                destroy_n() = default;
                void operator()(T const *p) noexcept
                {
                    for(std::ptrdiff_t n = 0; n < n_; ++n, ++p)
                        p->~T();
                }
                destroy_n &operator++()
                {
                    ++n_;
                    return *this;
                }
                void operator++(int)
                {
                    ++n_;
                }
            };

            struct return_temporary_buffer
            {
                template<typename T>
                void operator()(T *p) const
                {
                    if(p)
                        std::return_temporary_buffer(p);
                }
            };
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{
        template<typename O, typename Val>
        struct raw_storage_iterator
        {
        private:
            O out_;
        public:
            using difference_type = iterator_difference_t<O>;
            raw_storage_iterator() = default;
            explicit raw_storage_iterator(O out)
              : out_(std::move(out))
            {}
            raw_storage_iterator const &operator*() const
            {
                return *this;
            }
            CONCEPT_REQUIRES(CopyConstructible<Val>())
            raw_storage_iterator const &operator=(Val const & val) const
            {
                ::new((void*) std::addressof(*out_)) Val(val);
                return *this;
            }
            CONCEPT_REQUIRES(MoveConstructible<Val>())
            raw_storage_iterator const &operator=(Val && val) const
            {
                ::new((void*) std::addressof(*out_)) Val(std::move(val));
                return *this;
            }
            raw_storage_iterator &operator++()
            {
                ++out_;
                return *this;
            }
            raw_storage_iterator operator++(int)
            {
                auto tmp = *this;
                ++out_;
                return tmp;
            }
            O base() const
            {
                return out_;
            }
        };

        // Constructs objects in raw storage, keeps an external count of the
        // number of objects created so they can be destroyed later.
        template<typename V, typename I, typename D>
        counted_iterator<raw_storage_iterator<I, V>, detail::external_count<D>>
        make_counted_raw_storage_iterator(I i, D &d)
        {
            return counted_iterator<raw_storage_iterator<I, V>, detail::external_count<D>>{
                raw_storage_iterator<I, V>{std::move(i)}, d};
        }

        template<typename I, typename D, typename V = iterator_value_t<I>>
        counted_iterator<raw_storage_iterator<I, V>, detail::external_count<D>>
        make_counted_raw_storage_iterator(I i, D &d)
        {
            return counted_iterator<raw_storage_iterator<I, V>, detail::external_count<D>>{
                raw_storage_iterator<I, V>{std::move(i)}, d};
        }
        /// @}
    }
}

#endif
