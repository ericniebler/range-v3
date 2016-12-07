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

#ifndef RANGES_V3_DETAIL_OPTIONAL_HPP
#define RANGES_V3_DETAIL_OPTIONAL_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/detail/variant.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-utility
        struct in_place_t {};
        RANGES_INLINE_VARIABLE(in_place_t, in_place)

        template<typename T>
        struct optional
        {
        private:
            variant<meta::nil_, T> data_;
        public:
            optional() = default;
            optional(T t)
              : data_(emplaced_index<1>, std::move(t))
            {}
            template <typename...Args, CONCEPT_REQUIRES_(Constructible<T, Args &&...>())>
            explicit optional(in_place_t, Args &&...args)
              : data_(emplaced_index<1>, std::forward<Args>(args)...)
            {}
            explicit operator bool() const
            {
                return data_.index() != 0;
            }
            T & operator*()
            {
                RANGES_EXPECT(*this);
                return ranges::get<1>(data_);
            }
            T const & operator*() const
            {
                RANGES_EXPECT(*this);
                return ranges::get<1>(data_);
            }
            optional &operator=(T const &t)
            {
                ranges::emplace<1>(data_, t);
                return *this;
            }
            optional &operator=(T &&t)
            {
                ranges::emplace<1>(data_, std::move(t));
                return *this;
            }
            void reset()
            {
                ranges::emplace<0>(data_);
            }
        };

        namespace detail
        {
            template<typename T, typename Tag = void, bool Enable = true>
            struct non_propagating_cache
              : optional<T>
            {
                using optional<T>::optional;
                using optional<T>::operator=;

                non_propagating_cache() = default;
                constexpr
                non_propagating_cache(non_propagating_cache const &) noexcept
                  : optional<T>{}
                {}
                RANGES_CXX14_CONSTEXPR
                non_propagating_cache(non_propagating_cache && that) noexcept
                  : optional<T>{}
                {
                    that.optional<T>::reset();
                }
                RANGES_CXX14_CONSTEXPR
                non_propagating_cache& operator=(non_propagating_cache const &) noexcept
                {
                    this->optional<T>::reset();
                    return *this;
                }
                RANGES_CXX14_CONSTEXPR
                non_propagating_cache& operator=(non_propagating_cache && that) noexcept
                {
                    that.optional<T>::reset();
                    this->optional<T>::reset();
                    return *this;
                }
            };

            template<typename T, typename Tag>
            struct non_propagating_cache<T, Tag, false>
            {};
        }
    }
}

#endif
