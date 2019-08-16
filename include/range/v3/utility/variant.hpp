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

#ifndef RANGES_V3_UTILITY_VARIANT_HPP
#define RANGES_V3_UTILITY_VARIANT_HPP

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/detail/variant.hpp>
#include <range/v3/iterator/move_iterators.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename T, std::size_t N, typename Index>
        struct indexed_datum<T[N], Index>
        {
        private:
            union
            {
                char c;
                T data_[N];
            };
            void fill_default_(T * p, std::true_type)
            {
                for(; p != ranges::end(data_); ++p)
                    ::new((void *)p) T{};
            }
            void fill_default_(T * p, std::false_type)
            {
                RANGES_EXPECT(p == ranges::end(data_));
            }

        public:
            CPP_member
            constexpr CPP_ctor(indexed_datum)(meta::nil_ = {})(
                requires default_constructible<T>)
              : data_{}
            {}
            CPP_member
            CPP_ctor(indexed_datum)(indexed_datum && that)(requires move_constructible<T>)
            {
                std::uninitialized_copy_n(make_move_iterator(that.data_), N, data_);
            }
            CPP_member
            CPP_ctor(indexed_datum)(indexed_datum const & that)(
                requires copy_constructible<T>)
            {
                std::uninitialized_copy_n(that.data_, N, data_);
            }
            // \pre Requires distance(first, last) <= N
            // \pre Requires default_constructible<T> || distance(first, last) == N
            template<typename I, typename S>
            CPP_ctor(indexed_datum)(I first, S last)( //
                requires sentinel_for<S, I> && input_iterator<I> &&
                    constructible_from<T, iter_reference_t<I>>)
            {
                T * p = detail::uninitialized_copy(first, last, data_);
                this->fill_default_(p, meta::bool_<default_constructible<T>>{});
            }
            // \pre Requires distance(r) <= N
            // \pre Requires default_constructible<T> || distance(r) == N
            template<typename R>
            explicit CPP_ctor(indexed_datum)(R && r)( //
                requires input_range<R> && constructible_from<T, range_reference_t<R>>)
              : indexed_datum{ranges::begin(r), ranges::end(r)}
            {}
            CPP_member
            auto operator=(indexed_datum && that) -> CPP_ret(indexed_datum &)( //
                requires assignable_from<T &, T>)
            {
                ranges::move(that.data_, data_);
                return *this;
            }
            CPP_member
            auto operator=(indexed_datum const & that) -> CPP_ret(indexed_datum &)( //
                requires assignable_from<T &, T const &>)
            {
                ranges::copy(that.data_, data_);
                return *this;
            }
            // \pre Requires ranges::distance(r) <= N
            template<typename R>
            auto operator=(R && r) -> CPP_ret(indexed_datum &)( //
                requires input_range<R> && assignable_from<T &, range_reference_t<R>>)
            {
                ranges::copy(r, data_);
                return *this;
            }
            constexpr indexed_element<T[N], Index::value> ref()
            {
                return {data_};
            }
            constexpr indexed_element<T const [N], Index::value> ref() const {
                return {data_};
            } constexpr T (&get() noexcept)[N]
            {
                return data_;
            }
            constexpr T const (&get() const noexcept)[N]
            {
                return data_;
            }
        };
    } // namespace detail
    /// \endcond
} // namespace ranges

#endif
