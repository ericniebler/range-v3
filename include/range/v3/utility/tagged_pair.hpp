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
                  : Tags::template getter<Type, meta::eval<std::tuple_element<Is, Type>>, Is>...
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
            struct tag_spec
            {};

            template<typename Spec, typename Arg>
            struct tag_spec<Spec(Arg)>
            {
                using type = Spec;
            };

            template<typename T>
            struct tag_elem
            {};

            template<typename Spec, typename Arg>
            struct tag_elem<Spec(Arg)>
            {
                using type = Arg;
            };

            template<typename U, typename V = decay_t<U>>
            struct tuplelike_elem
            {
                using type = V;
            };

            template<typename U, typename V>
            struct tuplelike_elem<U, std::reference_wrapper<V>>
            {
                using type = V &;
            };

            template<typename U, typename V, bool RValue>
            struct tuplelike_elem<U, ranges::reference_wrapper<V, RValue>>
            {
                using type = meta::if_c<RValue, V &&, V &>;
            };
        }
        /// \endcond

        template<typename Base, typename...Tags>
        struct tagged
          : Base
          , detail::getters::collect<tagged<Base, Tags...>, Tags...>
        {
            using Base::Base;
            tagged() = default;
            tagged(tagged &&) = default;
            tagged(tagged const &) = default;
            tagged &operator=(tagged &&) = default;
            tagged &operator=(tagged const &) = default;
            template <typename Other, typename = meta::if_<std::is_convertible<Other, Base>>>
            tagged(tagged<Other, Tags...> &&that)
              : Base(static_cast<Other &&>(that))
            {}
            template <typename Other, typename = meta::if_<std::is_convertible<Other, Base>>>
            tagged(tagged<Other, Tags...> const &that)
              : Base(static_cast<Other const &>(that))
            {}
            template <typename Other, typename = meta::if_<std::is_convertible<Other, Base>>>
            tagged &operator=(tagged<Other, Tags...> &&that)
            {
                static_cast<Base &>(*this) = static_cast<Other &&>(that);
                return *this;
            }
            template <typename Other, typename = meta::if_<std::is_convertible<Other, Base>>>
            tagged &operator=(tagged<Other, Tags...> const &that)
            {
                static_cast<Base &>(*this) = static_cast<Other const &>(that);
                return *this;
            }
            template <typename U,
                typename = meta::if_c<!std::is_same<tagged, detail::decay_t<U>>::value>,
                typename = decltype(std::declval<Base&>() = std::declval<U>())>
            tagged &operator=(U && u)
            {
                static_cast<Base&>(*this) = std::forward<U>(u);
                return *this;
            }
        };

        template<typename F, typename S>
        using tagged_pair =
            tagged<std::pair<meta::eval<detail::tag_elem<F>>, meta::eval<detail::tag_elem<S>>>,
                   meta::eval<detail::tag_spec<F>>, meta::eval<detail::tag_spec<S>>>;

        template<typename Tag1, typename Tag2, typename T1, typename T2>
        constexpr
        tagged_pair<Tag1(meta::eval<detail::tuplelike_elem<T1>>),
                    Tag2(meta::eval<detail::tuplelike_elem<T2>>)>
        make_tagged_pair(T1 && t1, T2 && t2)
        {
            return {std::forward<T1>(t1), std::forward<T2>(t2)};
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wmismatched-tags"

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

#pragma GCC diagnostic pop

#endif
