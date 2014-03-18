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
            template<typename Iterator, typename Sentinel>
            struct common_cursor
            {
            private:
                static_assert(!std::is_same<Iterator, Sentinel>::value,
                              "Error: iterator and sentinel types are the same");
                Iterator it_;
                Sentinel se_;
                bool is_sentinel_;
            public:
                using single_pass = Same<iterator_category_t<Iterator>, std::input_iterator_tag>;
                common_cursor() = default;
                common_cursor(public_t, Iterator it)
                  : it_(std::move(it)), se_{}, is_sentinel_(false)
                {}
                common_cursor(public_t, Sentinel se)
                  : it_{}, se_(std::move(se)), is_sentinel_(true)
                {}
                common_cursor(Iterator it, Sentinel se, bool is_sentinel)
                  : it_(std::move(it)), se_(std::move(se)), is_sentinel_(is_sentinel)
                {}
                template<typename OtherIterator, typename OtherSentinel,
                    CONCEPT_REQUIRES_(Convertible<Iterator, OtherIterator>() &&
                                      Convertible<Sentinel, OtherSentinel>())>
                operator common_cursor<OtherIterator, OtherSentinel>() const
                {
                    return {it_, se_, is_sentinel_};
                }
                auto current() const -> decltype(*it_)
                {
                    RANGES_ASSERT(!is_sentinel_ && it_ != se_);
                    return *it_;
                }
                template<typename OtherIterator, typename OtherSentinel,
                    CONCEPT_REQUIRES_(Common<Iterator, OtherIterator>() &&
                                      Common<Sentinel, OtherSentinel>())>
                bool equal(common_cursor<OtherIterator, OtherSentinel> const &that) const
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

        template<typename Cursor, typename Sentinel>
        struct common_type<basic_range_iterator<Cursor, Sentinel>, basic_range_sentinel<Sentinel>>
        {
            using type =
                common_range_iterator<
                    basic_range_iterator<Cursor, Sentinel>,
                    basic_range_sentinel<Sentinel>>;
        };

        template<typename Cursor, typename Sentinel>
        struct common_type<basic_range_sentinel<Sentinel>, basic_range_iterator<Cursor, Sentinel>>
        {
            using type =
                common_range_iterator<
                    basic_range_iterator<Cursor, Sentinel>,
                    basic_range_sentinel<Sentinel>>;
        };
    }
}

#endif
