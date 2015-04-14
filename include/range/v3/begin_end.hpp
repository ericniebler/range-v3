/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_BEGIN_END_HPP
#define RANGES_V3_BEGIN_END_HPP

#include <utility>
#include <iterator>
#include <functional>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/dangling.hpp>
#include <range/v3/utility/reverse_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace adl_begin_end_detail
        {
            using std::begin;
            using std::end;

#if (__cplusplus >= 201402L) && \
    ((defined(_LIBCXX_VERSION) && (_LIBCXX_VERSION >= 1101) && \
      defined(_LIBCPP_STD_VER) && (_LIBCPP_STD_VER > 11)) || \
    (defined(__GLIBCXX__) && (__GLIBCXX__ >= 20150119)))
            using std::rbegin;
            using std::rend;
#endif

            // A reference-wrapped Iterable is an Iterable
            template<typename T>
            RANGES_RELAXED_CONSTEXPR
            auto begin(std::reference_wrapper<T> ref) -> decltype(begin(ref.get()))
            {
                return begin(ref.get());
            }

            template<typename T>
            RANGES_RELAXED_CONSTEXPR
            auto end(std::reference_wrapper<T> ref) -> decltype(end(ref.get()))
            {
                return end(ref.get());
            }

            template<typename T, bool RValue>
            RANGES_RELAXED_CONSTEXPR
            auto begin(ranges::reference_wrapper<T, RValue> ref) -> decltype(begin(ref.get()))
            {
                return begin(ref.get());
            }

            template<typename T, bool RValue>
            RANGES_RELAXED_CONSTEXPR
            auto end(ranges::reference_wrapper<T, RValue> ref) -> decltype(end(ref.get()))
            {
                return end(ref.get());
            }

            template<typename T>
            RANGES_RELAXED_CONSTEXPR
            auto rbegin(std::reference_wrapper<T> ref) -> decltype(rbegin(ref.get()))
            {
                return rbegin(ref.get());
            }

            template<typename T>
            RANGES_RELAXED_CONSTEXPR
            auto rend(std::reference_wrapper<T> ref) -> decltype(rend(ref.get()))
            {
                return rend(ref.get());
            }

            template<typename T, bool RValue>
            RANGES_RELAXED_CONSTEXPR
            auto rbegin(ranges::reference_wrapper<T, RValue> ref) -> decltype(rbegin(ref.get()))
            {
                return rbegin(ref.get());
            }

            template<typename T, bool RValue>
            RANGES_RELAXED_CONSTEXPR
            auto rend(ranges::reference_wrapper<T, RValue> ref) -> decltype(rend(ref.get()))
            {
                return rend(ref.get());
            }

            struct begin_fn
            {
            private:
                template<typename Rng>
                static constexpr auto impl(Rng && rng, long)
                    noexcept(noexcept(begin(rng))) ->
                    decltype(begin(rng))
                {
                    return begin(rng);
                }
                template<typename Rng,
                    meta::if_c<std::is_lvalue_reference<Rng>::value, int> = 0>
                static constexpr auto impl(Rng && rng, int)
                    noexcept(noexcept(rng.begin())) ->
                    decltype(rng.begin())
                {
                    return rng.begin();
                }
            public:
                template<typename Rng>
                constexpr auto operator()(Rng && rng) const
                    noexcept(noexcept(begin_fn::impl(static_cast<Rng &&>(rng), 0))) ->
                    decltype(begin_fn::impl(static_cast<Rng &&>(rng), 0))
                {
                    return begin_fn::impl(static_cast<Rng &&>(rng), 0);
                }
            };

            struct end_fn
            {
            private:
                template<typename Rng>
                static constexpr auto impl(Rng && rng, long)
                    noexcept(noexcept(end(rng))) ->
                    decltype(end(rng))
                {
                    return end(rng);
                }
                template<typename Rng,
                    meta::if_c<std::is_lvalue_reference<Rng>::value, int> = 0>
                static constexpr auto impl(Rng && rng, int)
                    noexcept(noexcept(rng.end())) ->
                    decltype(rng.end())
                {
                    return rng.end();
                }
            public:
                template<typename Rng>
                constexpr auto operator()(Rng && rng) const
                    noexcept(noexcept(end_fn::impl(static_cast<Rng &&>(rng), 0))) ->
                    decltype(end_fn::impl(static_cast<Rng &&>(rng), 0))
                {
                    return end_fn::impl(static_cast<Rng &&>(rng), 0);
                }
            };

            struct rbegin_fn
            {
            private:
                template<typename Rng>
                static constexpr auto impl(Rng && rng, long)
                    noexcept(noexcept(rbegin(rng))) ->
                    decltype(rbegin(rng))
                {
                    return rbegin(rng);
                }
                template<typename Rng,
                    meta::if_c<std::is_lvalue_reference<Rng>::value, int> = 0>
                static constexpr auto impl(Rng && rng, int)
                    noexcept(noexcept(rng.rbegin())) ->
                    decltype(rng.rbegin())
                {
                    return rng.rbegin();
                }
                template<typename T, std::size_t N>
                static constexpr auto impl(T (&t)[N], int)
                        noexcept -> ranges::reverse_iterator<T *>
                {
                    return ranges::reverse_iterator<T *>(t + N);
                }
                template<typename T>
                static constexpr auto impl(std::initializer_list<T> il, int)
                        noexcept -> ranges::reverse_iterator<T const *>
                {
                    return ranges::reverse_iterator<T const *>(il.end());
                }
            public:
                template<typename Rng>
                constexpr auto operator()(Rng && rng) const
                     noexcept(noexcept(rbegin_fn::impl(static_cast<Rng &&>(rng), 0))) ->
                     decltype(rbegin_fn::impl(static_cast<Rng &&>(rng), 0))
                {
                    return rbegin_fn::impl(static_cast<Rng &&>(rng), 0);
                }
            };

            struct rend_fn
            {
            private:
                template<typename Rng>
                static constexpr auto impl(Rng && rng, long)
                    noexcept(noexcept(rend(rng))) ->
                    decltype(rend(rng))
                {
                    return rend(rng);
                }
                template<typename Rng,
                    meta::if_c<std::is_lvalue_reference<Rng>::value, int> = 0>
                static constexpr auto impl(Rng && rng, int)
                    noexcept(noexcept(rng.rend())) ->
                    decltype(rng.rend())
                {
                    return rng.rend();
                }
                template<typename T, std::size_t N>
                static constexpr auto impl(T (&t)[N], int)
                        noexcept -> ranges::reverse_iterator<T *>
                {
                    return ranges::reverse_iterator<T *>(t);
                }
                template<typename T>
                static constexpr auto impl(std::initializer_list<T> il, int)
                        noexcept -> ranges::reverse_iterator<T const *>
                {
                    return ranges::reverse_iterator<T const *>(il.begin());
                }
            public:
                template<typename Rng>
                constexpr auto operator()(Rng && rng) const
                    noexcept(noexcept(rend_fn::impl(static_cast<Rng &&>(rng), 0))) ->
                    decltype(rend_fn::impl(static_cast<Rng &&>(rng), 0))
                {
                    return rend_fn::impl(static_cast<Rng &&>(rng), 0);
                }
            };

            struct cbegin_fn
            {
                template<typename Rng>
                constexpr auto operator()(Rng const & rng) const
                    noexcept(noexcept(static_const<begin_fn>::value(rng))) ->
                    decltype(static_const<begin_fn>::value(rng))
                {
                    return static_const<begin_fn>::value(rng);
                }
            };

            struct cend_fn
            {
                template<typename Rng>
                constexpr auto operator()(Rng const & rng) const
                    noexcept(noexcept(static_const<end_fn>::value(rng))) ->
                    decltype(static_const<end_fn>::value(rng))
                {
                    return static_const<end_fn>::value(rng);
                }
            };

            struct crbegin_fn
            {
                template<typename Rng>
                constexpr auto operator()(Rng const & rng) const
                    noexcept(noexcept(static_const<rbegin_fn>::value(rng))) ->
                    decltype(static_const<rbegin_fn>::value(rng))
                {
                    return static_const<rbegin_fn>::value(rng);
                }
            };

            struct crend_fn
            {
                template<typename Rng>
                constexpr auto operator()(Rng const & rng) const
                    noexcept(noexcept(static_const<rend_fn>::value(rng))) ->
                    decltype(static_const<rend_fn>::value(rng))
                {
                    return static_const<rend_fn>::value(rng);
                }
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `begin` free function
        namespace
        {
            constexpr auto&& begin = static_const<begin_fn>::value;
        }

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `end` free function
        namespace
        {
            constexpr auto&& end = static_const<end_fn>::value;
        }

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `begin` free function
        /// with a const-qualified argument.
        namespace
        {
            constexpr auto&& cbegin = static_const<cbegin_fn>::value;
        }

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `end` free function
        /// with a const-qualified argument.
        namespace
        {
            constexpr auto&& cend = static_const<cend_fn>::value;
        }

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `rbegin` free function
        namespace
        {
            constexpr auto&& rbegin = static_const<rbegin_fn>::value;
        }

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `rend` free function
        namespace
        {
            constexpr auto&& rend = static_const<rend_fn>::value;
        }

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `rbegin` free function
        /// with a const-qualified argument.
        namespace
        {
            constexpr auto&& crbegin = static_const<crbegin_fn>::value;
        }

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `rend` free function
        /// with a const-qualified argument.
        namespace
        {
            constexpr auto&& crend = static_const<crend_fn>::value;
        }

        /// \ingroup group-core
        struct safe_begin_fn
        {
            /// \return `begin(rng)` if \p rng is an lvalue; otherwise, it returns `begin(rng)`
            /// wrapped in \c ranges::dangling.
            template<typename Rng, typename I = decltype(begin(std::declval<Rng>()))>
            meta::if_<std::is_lvalue_reference<Rng>, I, dangling<I>>
            constexpr operator()(Rng && rng) const
            {
                return begin(rng);
            }
        };

        /// \ingroup group-core
        struct safe_end_fn
        {
            /// \return `begin(rng)` if \p rng is an lvalue; otherwise, it returns `begin(rng)`
            /// wrapped in \c ranges::dangling.
            template<typename Rng, typename I = decltype(end(std::declval<Rng>()))>
            meta::if_<std::is_lvalue_reference<Rng>, I, dangling<I>>
            constexpr operator()(Rng && rng) const
            {
                return end(rng);
            }
        };

        /// \ingroup group-core
        /// \return `begin(rng)` if `rng` is an lvalue; otherwise, it returns `begin(rng)`
        /// wrapped in \c ranges::dangling.
        namespace
        {
            constexpr auto&& safe_begin = static_const<safe_begin_fn>::value;
        }

        /// \ingroup group-core
        /// \return `end(rng)` if `rng` is an lvalue; otherwise, it returns `end(rng)`
        /// wrapped in \c ranges::dangling.
        namespace
        {
            constexpr auto&& safe_end = static_const<safe_end_fn>::value;
        }
    }
}

#endif
