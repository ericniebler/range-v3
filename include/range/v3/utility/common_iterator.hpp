/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2016
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
#include <range/v3/utility/basic_iterator.hpp>
#include <range/v3/utility/common_tuple.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/detail/variant.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 6 && __GNUC_MINOR__ == 2
            // Semi-workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=77537
            template<typename T, typename... Args>
            struct pair_construct_hack
              : Constructible<T, Args...>
            {};

            template<typename First, typename Second, typename Arg1, typename Arg2>
            struct pair_construct_hack<std::pair<First, Second>, std::pair<Arg1, Arg2>>
              : meta::and_<
                    std::is_constructible<First, Arg1>,
                    std::is_constructible<Second, Arg2>,
                    meta::defer<Constructible, std::pair<First, Second>, std::pair<Arg1, Arg2>>>
            {};

            template<typename First, typename Second, typename Arg1, typename Arg2>
            struct pair_construct_hack<std::pair<First, Second>, common_pair<Arg1, Arg2>>
              : meta::and_<
                    std::is_constructible<First, Arg1>,
                    std::is_constructible<Second, Arg2>,
                    meta::defer<Constructible, std::pair<First, Second>, std::pair<Arg1, Arg2>>>
            {};

            template<typename T, typename... Args>
            using is_nothrow_constructible_hack = meta::and_<
                pair_construct_hack<T, Args...>,
                meta::defer<std::is_nothrow_constructible, T, Args...>>;
#else
            template<typename T, typename... Args>
            using pair_construct_hack = Constructible<T, Args &&...>;

            template<typename T, typename... Args>
            using is_nothrow_constructible_hack = std::is_nothrow_constructible<T, Args...>;
#endif

            template<typename T>
            class operator_arrow_proxy
            {
                T value_;
            public:
                template<typename U, CONCEPT_REQUIRES_(pair_construct_hack<T, U>())>
                constexpr explicit operator_arrow_proxy(U && u)
                    noexcept(is_nothrow_constructible_hack<T, U>::value)
                  : value_(std::forward<U>(u))
                {}
                T const *operator->() const noexcept
                {
                    return std::addressof(value_);
                }
            };

            template <class, class = void>
            struct HasMemberOperatorArrow : std::false_type {};
            template <class T>
            struct HasMemberOperatorArrow<T, meta::void_<
                decltype(std::declval<T>().operator->())>>
              : std::true_type
            {};

            template<typename I,
                CONCEPT_REQUIRES_(std::is_pointer<I>::value ||
                    HasMemberOperatorArrow<I>())>
            constexpr I operator_arrow_(I const &i, int)
            noexcept(std::is_nothrow_copy_constructible<I>::value)
            {
                return i;
            }

            template<typename I,
                CONCEPT_REQUIRES_(std::is_reference<iterator_reference_t<I>>())>
            auto operator_arrow_(I const &i, long)
                noexcept(noexcept(*i)) ->
                decltype(std::addressof(std::declval<iterator_reference_t<I> &>()))
            {
                auto && tmp = *i;
                return std::addressof(tmp);
            }

            template<typename I, typename R = iterator_reference_t<I>,
                typename V = iterator_value_t<I>,
                CONCEPT_REQUIRES_(!std::is_reference<R>::value &&
                    Constructible<V, R>())>
            operator_arrow_proxy<V> operator_arrow_(I const &i, ...)
                noexcept(
                    std::is_nothrow_move_constructible<
                        operator_arrow_proxy<V>>::value &&
                    std::is_nothrow_constructible<
                        operator_arrow_proxy<V>, R>::value)
            {
                return operator_arrow_proxy<V>{*i};
            }

            template<typename I, CONCEPT_REQUIRES_(InputIterator<I>())>
            auto operator_arrow(I const &i)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                detail::operator_arrow_(i, 42)
            )

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
                    RANGES_EXPECT(data_.valid());
                    return data_.index() == 1u;
                }
                I & it()
                {
                    RANGES_EXPECT(!is_sentinel());
                    return ranges::get<0>(data_);
                }
                I const & it() const
                {
                    RANGES_EXPECT(!is_sentinel());
                    return ranges::get<0>(data_);
                }
                S const & se() const
                {
                    RANGES_EXPECT(is_sentinel());
                    return ranges::get<1>(data_);
                }
                template<typename OI, typename OS,
                    CONCEPT_REQUIRES_(
                        (bool)SizedSentinel<OS, I>() &&
                        (bool)SizedSentinel<OI, I>() &&
                        (bool)SizedSentinel<S, OI>())>
                iterator_difference_t<I>
                distance_to(common_cursor<OI, OS> const &that) const
                {
                    return that.is_sentinel() ?
                        (this->is_sentinel() ? 0 : that.se() - this->it()) :
                        (this->is_sentinel() ?
                             that.it() - this->se() :
                             that.it() - this->it());
                }
                CONCEPT_REQUIRES(Readable<I>())
                iterator_rvalue_reference_t<I> move() const
                    noexcept(noexcept(iter_move(std::declval<I const &>())))
                {
                    return iter_move(it());
                }
                CONCEPT_REQUIRES(Readable<I>())
                iterator_reference_t<I> read() const
                {
                    return *it();
                }
                template<typename T,
                    CONCEPT_REQUIRES_(ExclusivelyWritable_<I, T &&>())>
                void write(T && t) const
                {
                    *it() = (T &&) t;
                }
                template<typename II = I, CONCEPT_REQUIRES_(Readable<II>())>
                auto arrow() const
                    noexcept(noexcept(
                        detail::operator_arrow(std::declval<II const &>()))) ->
                    decltype(detail::operator_arrow(std::declval<II const &>()))
                {
                    return detail::operator_arrow(it());
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
