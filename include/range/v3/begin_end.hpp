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

#include <iterator>
#include <functional>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace adl_begin_end_detail
        {
            using std::begin;
            using std::end;

            template<typename T>
            auto rbegin(T &t) -> decltype(t.rbegin())
            {
                return t.rbegin();
            }
            template<typename T>
            auto rbegin(T const &t) -> decltype(t.rbegin())
            {
                return t.rbegin();
            }
            template<typename T>
            auto rend(T &t) -> decltype(t.rend())
            {
                return t.rend();
            }
            template<typename T>
            auto rend(T const &t) -> decltype(t.rend())
            {
                return t.rend();
            }
            template<typename T, std::size_t N>
            std::reverse_iterator<T *> rbegin(T (&t)[N])
            {
                return std::reverse_iterator<T *>(t + N);
            }
            template<typename T, std::size_t N>
            std::reverse_iterator<T *> rend(T (&t)[N])
            {
                return std::reverse_iterator<T *>(t);
            }
            template<typename T>
            std::reverse_iterator<T const *> rbegin(std::initializer_list<T> il)
            {
                return std::reverse_iterator<T *>(il.end());
            }
            template<typename T>
            std::reverse_iterator<T const *> rend(std::initializer_list<T> il)
            {
                return std::reverse_iterator<T const *>(il.begin());
            }

            // A reference-wrapped Iterable is an Iterable
            template<typename T>
            auto begin(std::reference_wrapper<T> ref) -> decltype(begin(ref.get()))
            {
                return begin(ref.get());
            }

            template<typename T>
            auto end(std::reference_wrapper<T> ref) -> decltype(end(ref.get()))
            {
                return end(ref.get());
            }

            template<typename T, bool RValue>
            auto begin(ranges::reference_wrapper<T, RValue> ref) -> decltype(begin(ref.get()))
            {
                return begin(ref.get());
            }

            template<typename T, bool RValue>
            auto end(ranges::reference_wrapper<T, RValue> ref) -> decltype(end(ref.get()))
            {
                return end(ref.get());
            }

            template<typename T>
            auto rbegin(std::reference_wrapper<T> ref) -> decltype(rbegin(ref.get()))
            {
                return rbegin(ref.get());
            }

            template<typename T>
            auto rend(std::reference_wrapper<T> ref) -> decltype(rend(ref.get()))
            {
                return rend(ref.get());
            }

            template<typename T, bool RValue>
            auto rbegin(ranges::reference_wrapper<T, RValue> ref) -> decltype(rbegin(ref.get()))
            {
                return rbegin(ref.get());
            }

            template<typename T, bool RValue>
            auto rend(ranges::reference_wrapper<T, RValue> ref) -> decltype(rend(ref.get()))
            {
                return rend(ref.get());
            }

            struct begin_fn
            {
                template<typename Rng>
                auto operator()(Rng && rng) const ->
                    decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct end_fn
            {
                template<typename Rng>
                auto operator()(Rng && rng) const ->
                    decltype(end(rng))
                {
                    return end(rng);
                }
            };

            struct rbegin_fn
            {
                template<typename Rng>
                auto operator()(Rng && rng) const ->
                    decltype(rbegin(rng))
                {
                    return rbegin(rng);
                }
            };

            struct rend_fn
            {
                template<typename Rng>
                auto operator()(Rng && rng) const ->
                    decltype(rend(rng))
                {
                    return rend(rng);
                }
            };

            struct cbegin_fn
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const ->
                    decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct cend_fn
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const ->
                    decltype(end(rng))
                {
                    return end(rng);
                }
            };

            struct crbegin_fn
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const ->
                    decltype(rbegin(rng))
                {
                    return rbegin(rng);
                }
            };

            struct crend_fn
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const ->
                    decltype(rend(rng))
                {
                    return rend(rng);
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
        struct get_unsafe_fn
        {
            /// \return \c t.get_unsafe() if \p t is an instance of `ranges::dangling`; otherwise,
            /// return \p t.
            template<typename T>
            constexpr T operator()(T && t) const
            {
                return detail::forward<T>(t);
            }
            /// \override
            template<typename T>
            constexpr T operator()(dangling<T> t) const
            {
                return t.get_unsafe();
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

        /// \ingroup group-core
        /// \return \c t.get_unsafe() if \p t is an instance of `ranges::dangling`; otherwise,
        /// return \p t.
        namespace
        {
            constexpr auto&& get_unsafe = static_const<get_unsafe_fn>::value;
        }
    }
}

#endif
