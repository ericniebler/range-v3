//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_UTILITY_COMMON_RANGE_ITERATOR_HPP
#define RANGES_V3_UTILITY_COMMON_RANGE_ITERATOR_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename I, typename S>
            struct common_cursor
            {
            private:
                static_assert(!std::is_same<I, S>::value,
                              "Error: iterator and sentinel types are the same");
                I it_;
                S se_;
                bool is_sentinel_;

            public:
                using single_pass = Derived<ranges::input_iterator_tag, iterator_category_t<I>>;
                struct mixin
                  : basic_mixin<common_cursor>
                {
                    mixin() = default;
                    mixin(common_cursor pos)
                      : basic_mixin<common_cursor>{std::move(pos)}
                    {}
                    explicit mixin(I it)
                      : mixin(common_cursor{std::move(it)})
                    {}
                    explicit mixin(S se)
                      : mixin(common_cursor{std::move(se)})
                    {}
                };
                common_cursor() = default;
                explicit common_cursor(I it)
                  : it_(std::move(it)), se_{}, is_sentinel_(false)
                {}
                explicit common_cursor(S se)
                  : it_{}, se_(std::move(se)), is_sentinel_(true)
                {}
                common_cursor(I it, S se, bool is_sentinel)
                  : it_(std::move(it)), se_(std::move(se)), is_sentinel_(is_sentinel)
                {}
                template<typename I2, typename S2,
                    CONCEPT_REQUIRES_(Convertible<I, I2>() &&
                                      Convertible<S, S2>())>
                operator common_cursor<I2, S2>() const
                {
                    return {it_, se_, is_sentinel_};
                }
                auto current() const -> decltype(*it_)
                {
                    RANGES_ASSERT(!is_sentinel_ && it_ != se_);
                    return *it_;
                }
                template<typename I2, typename S2,
                    CONCEPT_REQUIRES_(Common<I, I2>() &&
                                      Common<S, S2>())>
                bool equal(common_cursor<I2, S2> const &that) const
                {
                    return is_sentinel_ ?
                        that.is_sentinel_ || that.it_ == se_ :
                        that.is_sentinel_ ?
                            it_ == that.se_ :
                            it_ == that.it_;
                }
                void next()
                {
                    RANGES_ASSERT(!is_sentinel_ && it_ != se_);
                    ++it_;
                }
            };
        }

        template<typename Cur, typename S>
        struct common_type<basic_range_iterator<Cur, S>, basic_range_sentinel<S>>
        {
            using type =
                common_range_iterator<
                    basic_range_iterator<Cur, S>,
                    basic_range_sentinel<S>>;
        };

        template<typename Cur, typename S>
        struct common_type<basic_range_sentinel<S>, basic_range_iterator<Cur, S>>
        {
            using type =
                common_range_iterator<
                    basic_range_iterator<Cur, S>,
                    basic_range_sentinel<S>>;
        };
    }
}

#endif
