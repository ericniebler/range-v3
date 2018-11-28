/// \file
// Range v3 library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_DATA_HPP
#define RANGES_V3_DATA_HPP

#include <string>
#include <type_traits>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Customization point data
        /// \cond
        namespace _data_
        {
            struct fn
            {
            private:
                template<typename R>
                using member_data_t =
                    detail::decay_t<decltype(std::declval<R>().data())>;

                template<typename R>
                static constexpr auto impl_(R &r, detail::priority_tag<2>)
                    noexcept(noexcept(r.data())) ->
                        CPP_ret(member_data_t<R &>)(
                            requires std::is_pointer<member_data_t<R &>>::value)
                {
                    return r.data();
                }
                template<typename R>
                static constexpr auto impl_(R &&r, detail::priority_tag<1>)
                    noexcept(noexcept(ranges::begin((R &&) r))) ->
                        CPP_ret(_begin_::_t<R>)(
                            requires std::is_pointer<_begin_::_t<R>>::value)
                {
                    return ranges::begin((R &&) r);
                }
                template<typename R>
                static constexpr auto impl_(R &&r, detail::priority_tag<0>)
                    noexcept(noexcept(ranges::begin((R &&) r) == ranges::end((R &&) r)
                          ? nullptr
                          : std::addressof(*ranges::begin((R &&) r)))) ->
                        CPP_ret(decltype(std::addressof(*ranges::begin((R &&) r))))(
                            requires ContiguousIterator<_begin_::_t<R>>)
                {
                    return ranges::begin((R &&) r) == ranges::end((R &&) r)
                      ? nullptr
                      : std::addressof(*ranges::begin((R &&) r));
                }
            public:
                template<typename charT, typename Traits, typename Alloc>
                constexpr charT *operator()(std::basic_string<charT, Traits, Alloc> &s) const noexcept
                {
                    // string doesn't have non-const data before C++17
                    return const_cast<charT *>(detail::as_const(s).data());
                }

                template<typename R>
                constexpr auto operator()(R &&r) const
                    noexcept(noexcept(fn::impl_((R &&) r, detail::priority_tag<2>{}))) ->
                    decltype(fn::impl_((R &&) r, detail::priority_tag<2>{}))
                {
                    return fn::impl_((R &&) r, detail::priority_tag<2>{});
                }
            };

            template<typename R>
            using _t = decltype(fn{}(std::declval<R>()));
        }
        /// \endcond

        RANGES_INLINE_VARIABLE(_data_::fn, data)

        /// \cond
        namespace _cdata_
        {
            struct fn
            {
                template<typename R>
                constexpr _data_::_t<R const &> operator()(R const &r) const
                    noexcept(noexcept(ranges::data(r)))
                {
                    return ranges::data(r);
                }
                template<typename R>
                constexpr _data_::_t<R const> operator()(R const &&r) const
                    noexcept(noexcept(ranges::data((R const &&) r)))
                {
                    return ranges::data((R const &&) r);
                }
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \param r
        /// \return The result of calling `ranges::data` with a const-qualified
        ///    (lvalue or rvalue) reference to `r`.
        RANGES_INLINE_VARIABLE(_cdata_::fn, cdata)
    }
}

#endif // RANGES_V3_DATA_HPP
