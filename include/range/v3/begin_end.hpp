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

            template<typename T>
            auto begin(ranges::reference_wrapper<T> ref) -> decltype(begin(ref.get()))
            {
                return begin(ref.get());
            }

            template<typename T>
            auto end(ranges::reference_wrapper<T> ref) -> decltype(end(ref.get()))
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

            template<typename T>
            auto rbegin(ranges::reference_wrapper<T> ref) -> decltype(rbegin(ref.get()))
            {
                return rbegin(ref.get());
            }

            template<typename T>
            auto rend(ranges::reference_wrapper<T> ref) -> decltype(rend(ref.get()))
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
        constexpr begin_fn begin {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `end` free function
        constexpr end_fn end {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `begin` free function
        /// with a const-qualified argument.
        constexpr cbegin_fn cbegin {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `end` free function
        /// with a const-qualified argument.
        constexpr cend_fn cend {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `rbegin` free function
        constexpr rbegin_fn rbegin {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `rend` free function
        constexpr rend_fn rend {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `rbegin` free function
        /// with a const-qualified argument.
        constexpr crbegin_fn crbegin {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `rend` free function
        /// with a const-qualified argument.
        constexpr crend_fn crend {};
    }
}

#endif
