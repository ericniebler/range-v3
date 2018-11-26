/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_EMPTY_HPP
#define RANGES_V3_EMPTY_HPP

#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/size.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace _empty_
        {
            struct fn
            {
            private:
                // Prefer member if it is valid.
                template<typename R>
                static constexpr auto impl_(R &&r, detail::priority_tag<2>)
                    noexcept(noexcept(bool(((R &&) r).empty()))) ->
                        decltype(bool(((R &&) r).empty()))
                {
                    return bool(((R &&) r).empty());
                }

                // Fall back to size == 0.
                template<typename R>
                static constexpr auto impl_(R &&r, detail::priority_tag<1>)
                    noexcept(noexcept(bool(ranges::size((R &&) r) == 0))) ->
                        decltype(bool(ranges::size((R &&) r) == 0))
                {
                    return bool(ranges::size((R &&) r) == 0);
                }

                // Fall further back to begin == end.
                template<typename R>
                static constexpr auto impl_(R &&r, detail::priority_tag<0>)
                    noexcept(noexcept(bool(ranges::begin((R &&) r) == ranges::end((R &&) r)))) ->
                        CPP_ret(bool)(
                            requires ForwardRange<R>)
                {
                    return bool(ranges::begin((R &&) r) == ranges::end((R &&) r));
                }

            public:
                template<typename R>
                constexpr auto operator()(R &&r) const
                    noexcept(noexcept(fn::impl_((R &&) r, detail::priority_tag<2>{}))) ->
                    decltype(fn::impl_((R &&) r, detail::priority_tag<2>{}))
                {
                    return fn::impl_((R &&) r, detail::priority_tag<2>{});
                }

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto operator()(std::reference_wrapper<T> ref) const
                    noexcept(noexcept(Fn{}(ref.get()))) ->
                    decltype(Fn{}(ref.get()))
                {
                    return Fn{}(ref.get());
                }

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                    noexcept(noexcept(Fn{}(ref.get()))) ->
                    decltype(Fn{}(ref.get()))
                {
                    return Fn{}(ref.get());
                }
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \return true if and only if range contains no elements.
        RANGES_INLINE_VARIABLE(_empty_::fn, empty)
    }
}

#endif
