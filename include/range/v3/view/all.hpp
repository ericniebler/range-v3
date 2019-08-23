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
#ifndef RANGES_V3_VIEW_ALL_HPP
#define RANGES_V3_VIEW_ALL_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/ref.hpp>
#include <range/v3/view/subrange.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    namespace views
    {
        struct all_fn : pipeable_base
        {
        private:
            /// If it's a view already, pass it though.
            template<typename T>
            static constexpr auto from_range_(T && t, std::true_type, detail::ignore_t,
                                              detail::ignore_t)
            {
                return static_cast<T &&>(t);
            }

            /// If it is container-like, turn it into a view, being careful
            /// to preserve the Sized-ness of the range.
            template<typename T>
            static constexpr auto from_range_(T && t, std::false_type, std::true_type,
                                              detail::ignore_t)
            {
                return ranges::views::ref(t);
            }

            /// Not a view and not an lvalue? If it's a forwarding_range_, then
            /// return a subrange holding the range's begin/end.
            template<typename T>
            static constexpr auto from_range_(T && t, std::false_type, std::false_type,
                                              std::true_type)
            {
                return make_subrange(static_cast<T &&>(t));
            }

        public:
            template<typename T>
            constexpr auto CPP_fun(operator())(T && t)(const requires viewable_range<T>)
            {
                return all_fn::from_range_(static_cast<T &&>(t),
                                           meta::bool_<view_<uncvref_t<T>>>{},
                                           std::is_lvalue_reference<T>{},
                                           meta::bool_<forwarding_range_<T>>{});
            }

            template<typename T>
            RANGES_DEPRECATED("Passing a reference_wrapper to views::all is deprecated.")
            constexpr auto operator()(std::reference_wrapper<T> r) const
                -> CPP_ret(ref_view<T>)( //
                    requires range<T &>)
            {
                return ranges::views::ref(r.get());
            }
        };

        /// \relates all_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(all_fn, all)

        template<typename Rng>
        using all_t = decltype(all(std::declval<Rng>()));
    } // namespace views

    template<typename Rng>
    struct identity_adaptor : Rng
    {
        CPP_assert(view_<Rng>);

        identity_adaptor() = default;
        constexpr explicit identity_adaptor(Rng const & rng)
          : Rng(rng)
        {}
        constexpr explicit identity_adaptor(Rng && rng)
          : Rng(detail::move(rng))
        {}
    };

    namespace cpp20
    {
        namespace views
        {
            using ranges::views::all;
        }
        CPP_template(typename Rng)(       //
            requires viewable_range<Rng>) //
            using all_view = ranges::views::all_t<Rng>;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif
