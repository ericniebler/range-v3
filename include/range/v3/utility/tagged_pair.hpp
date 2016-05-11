/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2015
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
#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Base, typename...Tags>
        struct tagged;

        /// \cond
        namespace detail
        {
            struct getters
            {
            private:
                template<typename, typename...> friend struct ranges::tagged;
                template<typename Type, typename Indices, typename...Tags>
                struct collect_;
                template<typename Type, std::size_t...Is, typename...Tags>
                struct collect_<Type, meta::index_sequence<Is...>, Tags...>
                  : Tags::template getter<Type, meta::_t<std::tuple_element<Is, Type>>, Is>...
                {
                    collect_() = default;
                    collect_(const collect_&) = default;
                    collect_ &operator=(const collect_&) = default;
                private:
                    template<typename, typename...> friend struct ranges::tagged;
                    ~collect_() = default;
                };
                template<typename Type, typename...Tags>
                using collect = collect_<Type, meta::make_index_sequence<sizeof...(Tags)>, Tags...>;
            };

            template<typename T>
            using tag_spec = meta::front<meta::as_list<T>>;

            template<typename T>
            using tag_elem = meta::back<meta::as_list<T>>;

        #if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 6)
            template<typename Base, typename...Tags>
            meta::if_c<is_swappable<Base &>::value>
            swap(tagged<Base, Tags...> &x, tagged<Base, Tags...> &y)
                noexcept(is_nothrow_swappable<Base &>::value)
            {
                x.swap(y);
            }
        #endif
        }
        /// \endcond

        template<typename Base, typename...Tags>
        struct tagged
          : Base
          , detail::getters::collect<tagged<Base, Tags...>, Tags...>
        {
            tagged() = default;
            tagged(tagged &&) = default;
            tagged(tagged const &) = default;
            using Base::Base;
            CONCEPT_REQUIRES(MoveConstructible<Base>())
            tagged(Base && that)
              : Base(std::move(that))
            {}
            CONCEPT_REQUIRES(CopyConstructible<Base>())
            tagged(Base const & that)
              : Base(that)
            {}
            tagged &operator=(tagged &&) = default;
            tagged &operator=(tagged const &) = default;
            template<typename Other, typename = meta::if_<std::is_convertible<Other, Base>>>
            tagged(tagged<Other, Tags...> &&that)
                noexcept(noexcept(Base(static_cast<Other &&>(that))))
              : Base(static_cast<Other &&>(that))
            {}
            template<typename Other, typename = meta::if_<std::is_convertible<Other, Base>>>
            tagged(tagged<Other, Tags...> const &that)
              : Base(static_cast<Other const &>(that))
            {}
            template<typename Other, typename = meta::if_<std::is_convertible<Other, Base>>>
            tagged &operator=(tagged<Other, Tags...> &&that)
                noexcept(noexcept(std::declval<Base &>() = static_cast<Other &&>(that)))
            {
                static_cast<Base &>(*this) = static_cast<Other &&>(that);
                return *this;
            }
            template<typename Other, typename = meta::if_<std::is_convertible<Other, Base>>>
            tagged &operator=(tagged<Other, Tags...> const &that)
            {
                static_cast<Base &>(*this) = static_cast<Other const &>(that);
                return *this;
            }
            template<typename U,
                typename = meta::if_c<!std::is_same<tagged, detail::decay_t<U>>::value>,
                typename = decltype(std::declval<Base &>() = std::declval<U>())>
            tagged &operator=(U && u)
                noexcept(noexcept(std::declval<Base &>() = std::forward<U>(u)))
            {
                static_cast<Base &>(*this) = std::forward<U>(u);
                return *this;
            }
            template<int tagged_dummy_ = 42>
            meta::if_c<tagged_dummy_ == 43 || is_swappable<Base &>::value>
            swap(tagged &that)
                noexcept(is_nothrow_swappable<Base &>::value)
            {
                ranges::swap(static_cast<Base &>(*this), static_cast<Base &>(that));
            }
            // Workaround for GCC PR66957 https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66957
        #if !defined(__GNUC__) || defined(__clang__) || !(__GNUC__ < 6)
            template<int tagged_dummy_ = 42>
            friend meta::if_c<tagged_dummy_ == 43 || is_swappable<Base &>::value>
            swap(tagged &x, tagged &y)
                noexcept(is_nothrow_swappable<Base &>::value)
            {
                x.swap(y);
            }
        #endif
        };

        template<typename F, typename S>
        using tagged_pair =
            tagged<std::pair<detail::tag_elem<F>, detail::tag_elem<S>>,
                   detail::tag_spec<F>, detail::tag_spec<S>>;

        template<typename Tag1, typename Tag2, typename T1, typename T2>
        constexpr tagged_pair<Tag1(bind_element_t<T1>), Tag2(bind_element_t<T2>)>
        make_tagged_pair(T1 && t1, T2 && t2)
        {
            return {detail::forward<T1>(t1), detail::forward<T2>(t2)};
        }
    }
}

#define RANGES_DEFINE_TAG_SPECIFIER(NAME)                                       \
    namespace tag                                                               \
    {                                                                           \
        struct NAME                                                             \
        {                                                                       \
        private:                                                                \
            friend struct ranges::detail::getters;                              \
            template<typename Derived, typename Type, std::size_t I>            \
            struct getter                                                       \
            {                                                                   \
                getter() = default;                                             \
                getter(getter const &) = default;                               \
                getter &operator=(getter const &) = default;                    \
                RANGES_CXX14_CONSTEXPR Type &NAME() &                           \
                {                                                               \
                    return std::get<I>(static_cast<Derived &>(*this));          \
                }                                                               \
                RANGES_CXX14_CONSTEXPR Type &&NAME() &&                         \
                {                                                               \
                    return std::get<I>(static_cast<Derived &&>(*this));         \
                }                                                               \
                constexpr Type const &NAME() const &                            \
                {                                                               \
                    return std::get<I>(static_cast<Derived const &>(*this));    \
                }                                                               \
            private:                                                            \
                friend struct ranges::detail::getters;                          \
                ~getter() = default;                                            \
            };                                                                  \
        };                                                                      \
    }                                                                           \
    /**/

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_PRAGMAS
RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

namespace std
{
    template<typename Base, typename...Tags>
    struct tuple_size<ranges::tagged<Base, Tags...>>
      : tuple_size<Base>
    {};

    template<size_t N, typename Base, typename...Tags>
    struct tuple_element<N, ranges::tagged<Base, Tags...>>
      : tuple_element<N, Base>
    {};
}

RANGES_DIAGNOSTIC_POP

#endif
