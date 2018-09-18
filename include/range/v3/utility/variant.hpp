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

#include <range/v3/detail/variant.hpp>
// BUGBUG Layering violation!!!
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/size.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/utility/iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
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
                void fill_default_(T *p, std::true_type)
                {
                    for(; p != ranges::end(data_); ++p)
                        ::new((void *) p) T{};
                }
                void fill_default_(T *p, std::false_type)
                {
                    RANGES_EXPECT(p == ranges::end(data_));
                }
            public:
                CONCEPT_REQUIRES(DefaultConstructible<T>())
                constexpr indexed_datum(meta::nil_ = {})
                  : data_{}
                {}
                CONCEPT_REQUIRES(MoveConstructible<T>())
                indexed_datum(indexed_datum &&that)
                {
                    std::uninitialized_copy_n(make_move_iterator(that.data_), N, data_);
                }
                CONCEPT_REQUIRES(CopyConstructible<T>())
                indexed_datum(indexed_datum const &that)
                {
                    std::uninitialized_copy_n(that.data_, N, data_);
                }
                // \pre Requires distance(first, last) <= N
                // \pre Requires DefaultConstructible<T>() || distance(first, last) == N
                template<typename I, typename S,
                    CONCEPT_REQUIRES_(Sentinel<S, I>() && InputIterator<I>() &&
                        Constructible<T, reference_t<I>>())>
                indexed_datum(I first, S last)
                {
                    T *p = detail::uninitialized_copy(first, last, data_);
                    this->fill_default_(p, DefaultConstructible<T>{});
                }
                // \pre Requires distance(r) <= N
                // \pre Requires DefaultConstructible<T>() || distance(r) == N
                template<typename R,
                    CONCEPT_REQUIRES_(InputRange<R>() && Constructible<T, range_reference_t<R>>())>
                explicit indexed_datum(R &&r)
                  : indexed_datum{ranges::begin(r), ranges::end(r)}
                {}
                CONCEPT_REQUIRES(Assignable<T &, T>())
                indexed_datum &operator=(indexed_datum &&that)
                {
                    ranges::move(that.data_, data_);
                    return *this;
                }
                CONCEPT_REQUIRES(Assignable<T &, T const &>())
                indexed_datum &operator=(indexed_datum const &that)
                {
                    ranges::copy(that.data_, data_);
                    return *this;
                }
                // \pre Requires ranges::distance(r) <= N
                template<typename R,
                    CONCEPT_REQUIRES_(InputRange<R>() &&
                        Assignable<T &, range_reference_t<R>>())>
                indexed_datum &operator=(R &&r)
                {
                    ranges::copy(r, data_);
                    return *this;
                }
                RANGES_CXX14_CONSTEXPR indexed_element<T[N], Index::value> ref()
                {
                    return {data_};
                }
                constexpr indexed_element<T const[N], Index::value> ref() const
                {
                    return {data_};
                }
                RANGES_CXX14_CONSTEXPR T (&get() noexcept)[N]
                {
                    return data_;
                }
                constexpr T const (&get() const noexcept)[N]
                {
                    return data_;
                }
            };
        }
    }
}

#endif
