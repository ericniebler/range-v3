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
#ifndef RANGES_V3_UTILITY_COMMON_ITERATOR_HPP
#define RANGES_V3_UTILITY_COMMON_ITERATOR_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/basic_iterator.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/detail/variant.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename I, bool IsReadable = (bool) Readable<I>()>
            struct common_cursor_types
            {};

            template<typename I>
            struct common_cursor_types<I, true>
            {
                using single_pass = SinglePass<I>;
                using value_type = iterator_value_t<I>;
            };

            template<typename I, typename S>
            struct common_cursor
              : private common_cursor_types<I>
            {
            private:
                friend range_access;
                static_assert(!std::is_same<I, S>::value,
                              "Error: iterator and sentinel types are the same");
                using difference_type = iterator_difference_t<I>;
                struct mixin
                  : basic_mixin<common_cursor>
                {
                    mixin() = default;
                    using basic_mixin<common_cursor>::basic_mixin;
                    explicit mixin(I it)
                      : mixin(common_cursor{std::move(it)})
                    {}
                    explicit mixin(S se)
                      : mixin(common_cursor{std::move(se)})
                    {}
                };

                variant<I, S> data_;

                explicit common_cursor(I it)
                  : data_(emplaced_index<0>, std::move(it))
                {}
                explicit common_cursor(S se)
                  : data_(emplaced_index<1>, std::move(se))
                {}
                bool is_sentinel() const
                {
                    RANGES_ASSERT(data_.valid());
                    return data_.index() == 1u;
                }
                I & it()
                {
                    RANGES_ASSERT(!is_sentinel());
                    return ranges::get<0>(data_);
                }
                I const & it() const
                {
                    RANGES_ASSERT(!is_sentinel());
                    return ranges::get<0>(data_);
                }
                S const & se() const
                {
                    RANGES_ASSERT(is_sentinel());
                    return ranges::get<1>(data_);
                }
                CONCEPT_REQUIRES((bool)SizedSentinel<S, I>() && (bool)SizedSentinel<I, I>())
                friend iterator_difference_t<I>
                operator-(common_iterator<I, S> const &end, common_iterator<I, S> const &begin)
                {
                    common_cursor const &this_ = get_cursor(begin), &that = get_cursor(end);
                    return that.is_sentinel() ?
                        (this_.is_sentinel() ? 0 : that.se() - this_.it()) :
                        (this_.is_sentinel() ?
                             that.it() - this_.se() :
                             that.it() - this_.it());
                }
                CONCEPT_REQUIRES(Readable<I>())
                iterator_rvalue_reference_t<I> move() const
                    noexcept(noexcept(iter_move(std::declval<I const &>())))
                {
                    RANGES_ASSERT(!is_sentinel());
                    return iter_move(it());
                }
                CONCEPT_REQUIRES(Readable<I>())
                iterator_reference_t<I> get() const
                {
                    return *it();
                }
                template<typename T,
                    CONCEPT_REQUIRES_(ExclusivelyWritable_<I, T &&>())>
                void set(T &&t) const
                {
                    *it() = (T &&) t;
                }
                template<typename I2, typename S2,
                    CONCEPT_REQUIRES_(Sentinel<S2, I>() && Sentinel<S, I2>() &&
                        !EqualityComparable<I, I2>())>
                bool equal(common_cursor<I2, S2> const &that) const
                {
                    return is_sentinel() ?
                        (that.is_sentinel() || that.it() == se()) :
                        (!that.is_sentinel() || it() == that.se());
                }
                template<typename I2, typename S2,
                    CONCEPT_REQUIRES_(Sentinel<S2, I>() && Sentinel<S, I2>() &&
                        EqualityComparable<I, I2>())>
                bool equal(common_cursor<I2, S2> const &that) const
                {
                    return is_sentinel() ?
                        (that.is_sentinel() || that.it() == se()) :
                        (that.is_sentinel() ?
                            it() == that.se() :
                            it() == that.it());
                }
                void next()
                {
                    ++it();
                }
                // BUGBUG TODO what about advance??
            public:
                common_cursor() = default;
                template<typename I2, typename S2,
                    CONCEPT_REQUIRES_(ExplicitlyConvertibleTo<I, I2>() &&
                                      ExplicitlyConvertibleTo<S, S2>())>
                operator common_cursor<I2, S2>() const
                {
                    return is_sentinel() ?
                        common_cursor<I2, S2>{S2{se()}} :
                        common_cursor<I2, S2>{I2{it()}};
                }
            };
        }
        /// \endcond
    }
}

#endif
