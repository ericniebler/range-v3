/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/polymorphic_cast.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            std::pair<T *, std::ptrdiff_t> get_temporary_buffer(std::ptrdiff_t count) noexcept
            {
                RANGES_EXPECT(count >= 0);
                std::size_t n = static_cast<std::size_t>(count);
                if (n > PTRDIFF_MAX / sizeof(T))
                    n = PTRDIFF_MAX / sizeof(T);

                void *ptr = nullptr;
                for (; ptr == nullptr && n > 0; n /= 2)
                {
#if RANGES_CXX_ALIGNED_NEW < RANGES_CXX_ALIGNED_NEW_17
                    static_assert(alignof(T) <= alignof(std::max_align_t),
                        "Sorry: over-aligned types are supported only with C++17.");
#else // RANGES_CXX_ALIGNED_NEW
                    if RANGES_CONSTEXPR_IF (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
                        ptr = ::operator new(sizeof(T) * n, std::align_val_t{alignof(T)}, std::nothrow);
                    else
#endif // RANGES_CXX_ALIGNED_NEW
                        ptr = ::operator new(sizeof(T) * n, std::nothrow);
                }

                return {static_cast<T *>(ptr), static_cast<std::ptrdiff_t>(n)};
            }

            struct return_temporary_buffer
            {
                template<typename T>
                void operator()(T *p) const
                {
#if RANGES_CXX_ALIGNED_NEW < RANGES_CXX_ALIGNED_NEW_17
                    static_assert(alignof(T) <= alignof(std::max_align_t),
                        "Sorry: over-aligned types are supported only with C++17.");
#else // RANGES_CXX_ALIGNED_NEW
                    if RANGES_CONSTEXPR_IF (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
                        ::operator delete(p, std::align_val_t{alignof(T)});
                    else
#endif // RANGES_CXX_ALIGNED_NEW
                        ::operator delete(p);
                }
            };

            template<typename T, typename... Args,
                CONCEPT_REQUIRES_(!std::is_array<T>::value)>
            std::unique_ptr<T> make_unique(Args &&... args)
            {
                return std::unique_ptr<T>{new T(static_cast<Args &&>(args)...)};
            }
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{
        template<typename O, typename Val>
        struct raw_storage_iterator
        {
        private:
            CONCEPT_ASSERT(OutputIterator<O, Val>());
            CONCEPT_ASSERT(std::is_lvalue_reference<reference_t<O>>());
            O out_;
        public:
            using difference_type = difference_type_t<O>;
            raw_storage_iterator() = default;
            explicit raw_storage_iterator(O out)
              : out_(std::move(out))
            {}
            raw_storage_iterator &operator*() noexcept
            {
                return *this;
            }
            CONCEPT_REQUIRES(CopyConstructible<Val>())
            raw_storage_iterator &operator=(Val const & val)
            {
                ::new((void*) std::addressof(*out_)) Val(val);
                return *this;
            }
            CONCEPT_REQUIRES(MoveConstructible<Val>())
            raw_storage_iterator &operator=(Val && val)
            {
                ::new((void*) std::addressof(*out_)) Val(std::move(val));
                return *this;
            }
            raw_storage_iterator &operator++()
            {
                ++out_;
                return *this;
            }
            CONCEPT_REQUIRES(!ForwardIterator<O>())
            void operator++(int)
            {
                ++out_;
            }
            CONCEPT_REQUIRES(ForwardIterator<O>())
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

        template<typename I>
        struct iterator_wrapper
        {
        private:
            CONCEPT_ASSERT(Iterator<I>());
            mutable I *i_ = nullptr;
        public:
            using difference_type = difference_type_t<I>;
            iterator_wrapper() = default;
            iterator_wrapper(iterator_wrapper const &that)
              : i_(that.i_)
            {
                that.i_ = nullptr;
            }
            iterator_wrapper &operator=(iterator_wrapper const &that)
            {
                i_ = that.i_;
                that.i_ = nullptr;
                return *this;
            }
            iterator_wrapper(I &i)
              : i_(std::addressof(i))
            {}
            auto operator*() const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                **i_
            )
            iterator_wrapper &operator++()
            {
                ++*i_;
                return *this;
            }
            void operator++(int)
            {
                ++*i_;
            }
            I base() const
            {
                return *i_;
            }
        };

        template<typename I,
            CONCEPT_REQUIRES_(Iterator<I>())>
        iterator_wrapper<I> iter_ref(I &i)
        {
            return i;
        }

        template<typename I>
        struct iterator_category<iterator_wrapper<I>>
          : meta::if_<
                InputIterator<I>,
                meta::id<input_iterator_tag>,
                meta::nil_>
        {};

        template<typename I>
        struct value_type<iterator_wrapper<I>>
          : meta::if_<
                InputIterator<I>,
                meta::defer<value_type_t, I>,
                meta::nil_>
        {};

        template<typename Val>
        struct raw_buffer
        {
        private:
            Val *begin_;
            raw_storage_iterator<Val *, Val> rsi_;
        public:
            explicit raw_buffer(Val *begin)
              : begin_(begin), rsi_(begin)
            {}
            raw_buffer(raw_buffer &&) = default;
            raw_buffer(raw_buffer const &) = delete;
            ~raw_buffer()
            {
                for(; begin_ != rsi_.base(); ++begin_)
                    begin_->~Val();
            }
            iterator_wrapper<raw_storage_iterator<Val *, Val>> begin()
            {
                return ranges::iter_ref(rsi_);
            }
        };

        template<typename Val>
        raw_buffer<Val> make_raw_buffer(Val *val)
        {
            return raw_buffer<Val>(val);
        }
        /// @}
    }
}

#endif
