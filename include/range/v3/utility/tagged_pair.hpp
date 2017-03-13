/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2015
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#ifndef RANGES_V3_UTILITY_TAGGED_PAIR_HPP
#define RANGES_V3_UTILITY_TAGGED_PAIR_HPP

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wundef"
#endif

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            using tag_spec = meta::front<meta::as_list<T>>;

            template<typename T>
            using tag_elem = meta::back<meta::as_list<T>>;

            namespace adl_get_detail
            {
                using std::get;

                template<std::size_t I, typename T>
                constexpr auto adl_get(T && t)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    get<I>((T &&) t)
                )
            }
            using adl_get_detail::adl_get;
        }
        /// \endcond

        namespace tagged_detail
        {
            /// \cond
            template<typename Base, std::size_t, typename...>
            struct chain
            {
                using type = Base;
            };
            template<typename Base, std::size_t I, typename First, typename... Rest>
            struct chain<Base, I, First, Rest...>
            {
                using type = typename First::template getter<
                    Base, I, meta::_t<chain<Base, I + 1, Rest...>>>;
            };
            /// \endcond

            template<typename Base, typename...Tags>
            class tagged
              : public meta::_t<chain<Base, 0, Tags...>>
            {
                CONCEPT_ASSERT(Same<Base, uncvref_t<Base>>());
                using base_t = meta::_t<chain<Base, 0, Tags...>>;

                template<typename Other>
                using can_convert =
                    meta::bool_<!std::is_same<Other, Base>::value &&
                        std::is_convertible<Other, Base>::value>;
            public:
                tagged() = default;
                using base_t::base_t;
                CONCEPT_REQUIRES(MoveConstructible<Base>())
                constexpr tagged(Base && that)
                    noexcept(std::is_nothrow_move_constructible<Base>::value)
                  : base_t(detail::move(that))
                {}
                CONCEPT_REQUIRES(CopyConstructible<Base>())
                constexpr tagged(Base const &that)
                    noexcept(std::is_nothrow_copy_constructible<Base>::value)
                  : base_t(that)
                {}
                template<typename Other, typename = meta::if_<can_convert<Other>>>
                constexpr tagged(tagged<Other, Tags...> && that)
                    noexcept(std::is_nothrow_constructible<Base, Other>::value)
                  : base_t(static_cast<Other &&>(that))
                {}
                template<typename Other, typename = meta::if_<can_convert<Other>>>
                constexpr tagged(tagged<Other, Tags...> const &that)
                    noexcept(std::is_nothrow_constructible<Base, Other const &>::value)
                  : base_t(static_cast<Other const &>(that))
                {}
                template<typename Other, typename = meta::if_<can_convert<Other>>>
                RANGES_CXX14_CONSTEXPR tagged &operator=(tagged<Other, Tags...> && that)
                    noexcept(noexcept(std::declval<Base &>() = static_cast<Other &&>(that)))
                {
                    static_cast<Base &>(*this) = static_cast<Other &&>(that);
                    return *this;
                }
                template<typename Other, typename = meta::if_<can_convert<Other>>>
                RANGES_CXX14_CONSTEXPR tagged &operator=(tagged<Other, Tags...> const &that)
                    noexcept(noexcept(std::declval<Base &>() = static_cast<Other const &>(that)))
                {
                    static_cast<Base &>(*this) = static_cast<Other const &>(that);
                    return *this;
                }
                template<typename U,
                    typename = meta::if_c<!std::is_same<tagged, detail::decay_t<U>>::value>,
                    typename = decltype(std::declval<Base &>() = std::declval<U>())>
                RANGES_CXX14_CONSTEXPR tagged &operator=(U && u)
                    noexcept(noexcept(std::declval<Base &>() = std::forward<U>(u)))
                {
                    static_cast<Base &>(*this) = std::forward<U>(u);
                    return *this;
                }
                template<int dummy_ = 42>
                RANGES_CXX14_CONSTEXPR meta::if_c<dummy_ == 43 || is_swappable<Base &>::value>
                swap(tagged &that)
                    noexcept(is_nothrow_swappable<Base &>::value)
                {
                    ranges::swap(static_cast<Base &>(*this), static_cast<Base &>(that));
                }
                template<int dummy_ = 42>
                friend RANGES_CXX14_CONSTEXPR meta::if_c<dummy_ == 43 || is_swappable<Base &>::value>
                swap(tagged &x, tagged &y)
                    noexcept(is_nothrow_swappable<Base &>::value)
                {
                    x.swap(y);
                }
            };
        }
        using tagged_detail::tagged;

        template<typename F, typename S>
        using tagged_pair =
            tagged<std::pair<detail::tag_elem<F>, detail::tag_elem<S>>,
                   detail::tag_spec<F>, detail::tag_spec<S>>;

        template<typename Tag1, typename Tag2, typename T1, typename T2,
            typename R = tagged_pair<Tag1(bind_element_t<T1>), Tag2(bind_element_t<T2>)>>
        constexpr R make_tagged_pair(T1 && t1, T2 && t2)
            noexcept(std::is_nothrow_constructible<R, T1, T2>::value)
        {
            return {detail::forward<T1>(t1), detail::forward<T2>(t2)};
        }
    }
}

#define RANGES_DEFINE_TAG_SPECIFIER(NAME)                                            \
    namespace tag                                                                    \
    {                                                                                \
        struct NAME                                                                  \
        {                                                                            \
            template<typename Untagged, std::size_t I, typename Next>                \
            class getter : public Next                                               \
            {                                                                        \
            protected:                                                               \
                ~getter() = default;                                                 \
            public:                                                                  \
                getter() = default;                                                  \
                getter(getter &&) = default;                                         \
                getter(getter const &) = default;                                    \
                using Next::Next;                                                    \
                CONCEPT_REQUIRES(MoveConstructible<Untagged>())                      \
                constexpr getter(Untagged && that)                                   \
                    noexcept(std::is_nothrow_move_constructible<Untagged>::value)    \
                  : Next(detail::move(that))                                         \
                {}                                                                   \
                CONCEPT_REQUIRES(CopyConstructible<Untagged>())                      \
                constexpr getter(Untagged const &that)                               \
                    noexcept(std::is_nothrow_copy_constructible<Untagged>::value)    \
                  : Next(that)                                                       \
                {}                                                                   \
                getter &operator=(getter &&) = default;                              \
                getter &operator=(getter const &) = default;                         \
                RANGES_CXX14_CONSTEXPR                                               \
                meta::_t<std::tuple_element<I, Untagged>> &NAME() &                  \
                    noexcept(noexcept(                                               \
                        detail::adl_get<I>(std::declval<Untagged &>())))             \
                {                                                                    \
                    return detail::adl_get<I>(static_cast<Untagged &>(*this));       \
                }                                                                    \
                RANGES_CXX14_CONSTEXPR                                               \
                meta::_t<std::tuple_element<I, Untagged>> &&NAME() &&                \
                    noexcept(noexcept(                                               \
                        detail::adl_get<I>(std::declval<Untagged>())))               \
                {                                                                    \
                    return detail::adl_get<I>(static_cast<Untagged &&>(*this));      \
                }                                                                    \
                constexpr                                                            \
                meta::_t<std::tuple_element<I, Untagged>> const &NAME() const &      \
                    noexcept(noexcept(                                               \
                        detail::adl_get<I>(std::declval<Untagged const &>())))       \
                {                                                                    \
                    return detail::adl_get<I>(static_cast<Untagged const &>(*this)); \
                }                                                                    \
            };                                                                       \
        };                                                                           \
    }                                                                                \
    /**/

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_PRAGMAS
RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

namespace std
{
    template<typename Untagged, typename...Tags>
    struct tuple_size< ::ranges::v3::tagged<Untagged, Tags...>>
      : tuple_size<Untagged>
    {};

    template<size_t N, typename Untagged, typename...Tags>
    struct tuple_element<N, ::ranges::v3::tagged<Untagged, Tags...>>
      : tuple_element<N, Untagged>
    {};
}

RANGES_DIAGNOSTIC_POP

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
