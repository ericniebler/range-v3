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
#include <range/v3/detail/adl_get.hpp>
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
        }

        namespace _tagged_
        {
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

#if RANGES_BROKEN_CPO_LOOKUP
            template <typename> struct adl_hook {};
#endif
        }
        /// \endcond

        template<typename Base, typename...Tags>
        class tagged
          : public meta::_t<_tagged_::chain<Base, 0, Tags...>>
#if RANGES_BROKEN_CPO_LOOKUP
          , private _tagged_::adl_hook<tagged<Base, Tags...>>
#endif
        {
            CONCEPT_ASSERT(Same<Base, uncvref_t<Base>>());
            using base_t = meta::_t<_tagged_::chain<Base, 0, Tags...>>;

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
                noexcept(noexcept(std::declval<Base &>() = static_cast<U&&>(u)))
            {
                static_cast<Base &>(*this) = static_cast<U&&>(u);
                return *this;
            }
            template<typename B = Base>
            RANGES_CXX14_CONSTEXPR meta::if_c<is_swappable<B>::value>
            swap(tagged &that)
                noexcept(is_nothrow_swappable<B>::value)
            {
                ranges::swap(static_cast<Base &>(*this), static_cast<Base &>(that));
            }
#if !RANGES_BROKEN_CPO_LOOKUP
            template<typename B = Base>
            friend RANGES_CXX14_CONSTEXPR meta::if_c<is_swappable<B>::value>
            swap(tagged &x, tagged &y)
                noexcept(is_nothrow_swappable<B>::value)
            {
                x.swap(y);
            }
#endif
        };

#if RANGES_BROKEN_CPO_LOOKUP
        namespace _tagged_
        {
            template<typename Base, typename...Tags>
            RANGES_CXX14_CONSTEXPR meta::if_c<is_swappable<Base>::value>
            swap(tagged<Base, Tags...> &x, tagged<Base, Tags...> &y)
                noexcept(is_nothrow_swappable<Base>::value)
            {
                x.swap(y);
            }
        }
#endif

        template<std::size_t I, typename Base, typename... Tags>
        auto get(tagged<Base, Tags...> &t)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            ranges::detail::adl_get<I>(static_cast<Base &>(t))
        )
        template<std::size_t I, typename Base, typename... Tags>
        auto get(tagged<Base, Tags...> const &t)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            ranges::detail::adl_get<I>(static_cast<Base const &>(t))
        )
        template<std::size_t I, typename Base, typename... Tags>
        auto get(tagged<Base, Tags...> &&t)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            ranges::detail::adl_get<I>(static_cast<Base &&>(t))
        )
        template<std::size_t I, typename Base, typename... Tags>
        void get(tagged<Base, Tags...> const &&) = delete;

        template<typename T, typename Base, typename... Tags>
        auto get(tagged<Base, Tags...> &t)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            ranges::detail::adl_get<T>(static_cast<Base &>(t))
        )
        template<typename T, typename Base, typename... Tags>
        auto get(tagged<Base, Tags...> const &t)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            ranges::detail::adl_get<T>(static_cast<Base const &>(t))
        )
        template<typename T, typename Base, typename... Tags>
        auto get(tagged<Base, Tags...> &&t)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            ranges::detail::adl_get<T>(static_cast<Base &&>(t))
        )
        template<typename T, typename Base, typename... Tags>
        void get(tagged<Base, Tags...> const &&) = delete;

        template<typename F, typename S>
        using tagged_pair =
            tagged<std::pair<detail::tag_elem<F>, detail::tag_elem<S>>,
                   detail::tag_spec<F>, detail::tag_spec<S>>;

        template<typename Tag1, typename Tag2, typename T1, typename T2,
            typename R = tagged_pair<Tag1(bind_element_t<T1>), Tag2(bind_element_t<T2>)>>
        constexpr R make_tagged_pair(T1 && t1, T2 && t2)
            noexcept(std::is_nothrow_constructible<R, T1, T2>::value)
        {
            return {static_cast<T1&&>(t1), static_cast<T2&&>(t2)};
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
