/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_DANGLING_HPP
#define RANGES_V3_UTILITY_DANGLING_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>

#ifndef RANGES_NO_STD_FORWARD_DECLARATIONS
// Non-portable forward declarations of standard containers
RANGES_BEGIN_NAMESPACE_STD
    template<typename...Ts>
    class tuple;
RANGES_END_NAMESPACE_STD
#else
#include <tuple>
#endif

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            struct is_dangling
              : meta::is<T, dangling>
            {};

            template<typename T>
            struct is_dangling<T &>
              : is_dangling<T>
            {};

            template<typename T>
            struct is_dangling<T const>
              : is_dangling<T>
            {};
        }
        /// \endcond

        struct sanitize_fn;

        /// \ingroup group-core
        /// A wrapper for an iterator or a sentinel into a range that may
        /// no longer be valid.
        template<typename I>
        struct dangling
        {
        private:
            I it_;
        public:
            dangling() = default;
            /// Implicit converting constructor
            constexpr dangling(I it)
              : it_(it)
            {}
            /// \return The iterator from which this \c dangling object was constructed.
            /// \note The returned iterator may be invalid.
            /// \pre
            /// \li Either the range from which the iterator was obtained has not been
            /// destructed yet, or else the range's iterators are permitted to outlive the
            /// range, and
            /// \li No operation that invalidates the iterator has been performed.
            constexpr I get_unsafe() const
            {
                return it_;
            }
        };

        template<>
        struct dangling<void>
        {
        private:
            friend struct sanitize_fn;
            template<typename T>
            dangling(dangling<T> const &)
            {}
        public:
            dangling() = default;
        };

        /// \ingroup group-core
        struct sanitize_fn
        {
        private:
            template<typename T>
            using result_t = decltype(std::declval<sanitize_fn const &>()(std::declval<T>()));

            template<typename Res, typename Tuple, typename ...Ts, std::size_t...Is>
            constexpr Res sanitize_tuple(Tuple &&tup,
                meta::list<Ts...>, meta::index_sequence<Is...>) const
            {
                return Res{
                    (*this)(static_cast<Ts &&>(std::get<Is>(static_cast<Tuple &&>(tup))))...};
            }
        public:
            template<typename T>
            constexpr meta::if_<detail::is_dangling<T>, dangling<>, T> operator()(T &&t) const
            {
                return static_cast<T&&>(t);
            }
            template<typename T, typename U>
            constexpr std::pair<result_t<T>, result_t<U>> operator()(std::pair<T, U> &p) const
            {
                return {(*this)(p.first), (*this)(p.second)};
            }
            template<typename T, typename U>
            constexpr std::pair<result_t<T>, result_t<U>> operator()(std::pair<T, U> const &p) const
            {
                return {(*this)(p.first), (*this)(p.second)};
            }
            template<typename T, typename U>
            constexpr std::pair<result_t<T>, result_t<U>> operator()(std::pair<T, U> &&p) const
            {
                return {(*this)(static_cast<T &&>(p.first)), (*this)(static_cast<U &&>(p.second))};
            }
            template<typename ...Ts>
            constexpr std::tuple<result_t<Ts>...> operator()(std::tuple<Ts...> &tup) const
            {
                return this->sanitize_tuple<std::tuple<result_t<Ts>...>>(tup,
                    meta::list<Ts &...>{}, meta::make_index_sequence<sizeof...(Ts)>{});
            }
            template<typename ...Ts>
            constexpr std::tuple<result_t<Ts>...> operator()(std::tuple<Ts...> const &tup) const
            {
                return this->sanitize_tuple<std::tuple<result_t<Ts>...>>(tup,
                    meta::list<Ts const &...>{}, meta::make_index_sequence<sizeof...(Ts)>{});
            }
            template<typename ...Ts>
            constexpr std::tuple<result_t<Ts>...> operator()(std::tuple<Ts...> &&tup) const
            {
                return this->sanitize_tuple<std::tuple<result_t<Ts>...>>(std::move(tup),
                    meta::list<Ts...>{}, meta::make_index_sequence<sizeof...(Ts)>{});
            }
            template<typename Base, typename... Tags>
            constexpr tagged<result_t<Base &>, Tags...> operator()(tagged<Base, Tags...> &tup) const
            {
                return (*this)(static_cast<Base &>(tup));
            }
            template<typename Base, typename... Tags>
            constexpr tagged<result_t<Base const &>, Tags...> operator()(tagged<Base, Tags...> const &tup) const
            {
                return (*this)(static_cast<Base const &>(tup));
            }
            template<typename Base, typename... Tags>
            constexpr tagged<result_t<Base &&>, Tags...> operator()(tagged<Base, Tags...> &&tup) const
            {
                return (*this)(static_cast<Base &&>(tup));
            }
        };

        /// \ingroup group-core
        struct get_unsafe_fn
        {
            /// \return \c t.get_unsafe() if \p t is an instance of `ranges::dangling`; otherwise,
            /// return \p t.
            template<typename T>
            constexpr T operator()(T && t) const
            {
                return static_cast<T&&>(t);
            }
            /// \overload
            template<typename T>
            constexpr T operator()(dangling<T> t) const
            {
                return t.get_unsafe();
            }
        };

        /// \ingroup group-core
        /// \return \c t.get_unsafe() if \p t is an instance of `ranges::dangling`; otherwise,
        /// return \p t.
        RANGES_INLINE_VARIABLE(get_unsafe_fn, get_unsafe)

        /// \ingroup group-core
        /// \return the result of replacing all \c ranges::dangling<T> objects with
        /// \c ranges::dangling<void>, introspecting \c std::pair and \c std::tuple
        /// objects recursively.
        RANGES_INLINE_VARIABLE(sanitize_fn, sanitize)
    }
}

#endif
