/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
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
                constexpr auto adl_get(T &&t)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    get<I>((T &&) t)
                )
            }
            using adl_get_detail::adl_get;

            template<typename Base, typename, typename...>
            struct chain
            {
                using type = Base;
            };
            template<typename Base, typename I, typename First, typename... Rest>
            struct chain<Base, I, First, Rest...>
            {
                using type =
                    meta::invoke<First, Base, I, meta::_t<chain<Base, meta::inc<I>, Rest...>>>;
            };
        }
        /// \endcond

        template<typename Base, typename...Tags>
        class tagged
          : public meta::_t<detail::chain<Base, meta::size_t<0>, Tags...>>
          , detail::member_swap<tagged<Base, Tags...>>
        {
            CONCEPT_assert(Same<Base, uncvref_t<Base>>());
            using base_t = meta::_t<detail::chain<Base, meta::size_t<0>, Tags...>>;

            CONCEPT_def
            (
                template(typename B, typename... Ts)
                (concept CanConvert)(B, Ts...),
                    requires {} &&
                    !Same<tagged<B, Ts...>, tagged>() &&
                    ConvertibleTo<B, Base>()
            );
        public:
            tagged() = default;
            using base_t::base_t;
            CONCEPT_requires(MoveConstructible<Base>())
            (constexpr) tagged(Base &&that)
                noexcept(std::is_nothrow_move_constructible<Base>::value)
              : base_t(detail::move(that))
            {}
            CONCEPT_requires(CopyConstructible<Base>())
            (constexpr) tagged(Base const &that)
                noexcept(std::is_nothrow_copy_constructible<Base>::value)
              : base_t(that)
            {}
            CONCEPT_template(typename B, typename... Ts)
                (requires CanConvert<B, Ts...>())
            (constexpr) tagged(tagged<B, Ts...> &&that)
                noexcept(std::is_nothrow_constructible<Base, B>::value)
              : base_t(static_cast<B &&>(that))
            {}
            CONCEPT_template(typename B, typename... Ts)
                (requires CanConvert<B, Ts...>())
            (constexpr) tagged(tagged<B, Ts...> const &that)
                noexcept(std::is_nothrow_constructible<Base, B const &>::value)
              : base_t(static_cast<B const &>(that))
            {}
            CONCEPT_template(typename B, typename... Ts)
                (requires CanConvert<B, Ts...>())
            (RANGES_CXX14_CONSTEXPR tagged &)operator=(tagged<B, Ts...> && that)
                noexcept(std::is_nothrow_assignable<Base &, B>::value)
            {
                static_cast<Base &>(*this) = static_cast<B &&>(that);
                return *this;
            }
            CONCEPT_template(typename B, typename... Ts)
                (requires CanConvert<B, Ts...>())
            (RANGES_CXX14_CONSTEXPR tagged &)operator=(tagged<B, Ts...> const &that)
                noexcept(std::is_nothrow_assignable<Base &, B const &>::value)
            {
                static_cast<Base &>(*this) = static_cast<B const &>(that);
                return *this;
            }
            CONCEPT_template(typename U)
                (requires !Same<tagged, detail::decay_t<U>>() && Assignable<Base &, U>())
            (RANGES_CXX14_CONSTEXPR tagged &)operator=(U &&u)
                noexcept(std::is_nothrow_assignable<Base &, U>::value)
            {
                static_cast<Base &>(*this) = static_cast<U &&>(u);
                return *this;
            }
            CONCEPT_template(typename B = Base)
                (requires True(is_swappable<B>()))
            (RANGES_CXX14_CONSTEXPR void) swap(tagged &that)
                noexcept(is_nothrow_swappable<B>::value)
            {
                ranges::swap(static_cast<Base &>(*this), static_cast<Base &>(that));
            }
        };

        template<typename F, typename S>
        using tagged_pair =
            tagged<std::pair<detail::tag_elem<F>, detail::tag_elem<S>>,
                   detail::tag_spec<F>, detail::tag_spec<S>>;

        template<typename Tag1, typename Tag2, typename T1, typename T2,
            typename R = tagged_pair<Tag1(bind_element_t<T1>), Tag2(bind_element_t<T2>)>>
        constexpr R make_tagged_pair(T1 &&t1, T2 &&t2)
            noexcept(std::is_nothrow_constructible<R, T1, T2>::value)
        {
            return {static_cast<T1 &&>(t1), static_cast<T2 &&>(t2)};
        }
    }
}

#if 0
#define RANGES_TAG_CONSTRUCT_FROM_UNTAGGED()                                                        \
    CONCEPT_template(typename U)(                                                                   \
        requires Same<::ranges::detail::decay_t<U>, Untagged>() &&                                  \
            Constructible<Untagged, U>())                                                           \
    (constexpr) invoke(U &&that)                                                                    \
        noexcept(std::is_nothrow_constructible<Untagged, U>::value)                                 \
      : Next(static_cast<U &&>(that))                                                               \
    {}                                                                                              \
    /**/
#else
#define RANGES_TAG_CONSTRUCT_FROM_UNTAGGED()                                                        \
    CONCEPT_requires(MoveConstructible<Untagged>())                                                 \
    (constexpr) invoke(Untagged && that)                                                            \
        noexcept(std::is_nothrow_move_constructible<Untagged>::value)                               \
      : Next(detail::move(that))                                                                    \
    {}                                                                                              \
    CONCEPT_requires(CopyConstructible<Untagged>())                                                 \
    (constexpr) invoke(Untagged const &that)                                                        \
        noexcept(std::is_nothrow_copy_constructible<Untagged>::value)                               \
      : Next(that)                                                                                  \
    {}                                                                                              \
    /**/
#endif

#define RANGES_DEFINE_TAG_SPECIFIER(NAME)                                                           \
    namespace tag                                                                                   \
    {                                                                                               \
        struct NAME                                                                                 \
        {                                                                                           \
            template<typename Untagged, typename I, typename Next>                                  \
            class invoke : public Next                                                              \
            {                                                                                       \
            protected:                                                                              \
                ~invoke() = default;                                                                \
            public:                                                                                 \
                invoke() = default;                                                                 \
                invoke(invoke &&) = default;                                                        \
                invoke(invoke const &) = default;                                                   \
                using Next::Next;                                                                   \
                RANGES_TAG_CONSTRUCT_FROM_UNTAGGED()                                                \
                invoke &operator=(invoke &&) = default;                                             \
                invoke &operator=(invoke const &) = default;                                        \
                RANGES_CXX14_CONSTEXPR                                                              \
                meta::_t<std::tuple_element<I::value, Untagged>> &NAME() &                          \
                    noexcept(noexcept(                                                              \
                        ::ranges::detail::adl_get<I::value>(std::declval<Untagged &>())))           \
                {                                                                                   \
                    return ::ranges::detail::adl_get<I::value>(static_cast<Untagged &>(*this));     \
                }                                                                                   \
                RANGES_CXX14_CONSTEXPR                                                              \
                meta::_t<std::tuple_element<I::value, Untagged>> &&NAME() &&                        \
                    noexcept(noexcept(                                                              \
                        ::ranges::detail::adl_get<I::value>(std::declval<Untagged>())))             \
                {                                                                                   \
                    return ::ranges::detail::adl_get<I::value>(static_cast<Untagged &&>(*this));    \
                }                                                                                   \
                constexpr                                                                           \
                meta::_t<std::tuple_element<I::value, Untagged>> const &NAME() const &              \
                    noexcept(noexcept(                                                              \
                        ::ranges::detail::adl_get<I::value>(std::declval<Untagged const &>())))     \
                {                                                                                   \
                    return ::ranges::detail::adl_get<I::value>(                                     \
                        static_cast<Untagged const &>(*this));                                      \
                }                                                                                   \
            };                                                                                      \
        };                                                                                          \
    }                                                                                               \
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

#endif
