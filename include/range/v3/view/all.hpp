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
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/ref.hpp>
#include <range/v3/view/subrange.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    namespace view
    {
        struct all_fn
          : pipeable<all_fn>
        {
        private:
            /// If it's a view already, pass it though.
            template<typename T>
            static auto from_range_(T &&t, std::true_type, detail::ignore_t, detail::ignore_t)
            {
                return static_cast<T &&>(t);
            }

            /// If it is container-like, turn it into a view, being careful
            /// to preserve the Sized-ness of the range.
            template<typename T>
            static auto from_range_(T &&t, std::false_type, std::true_type, detail::ignore_t)
            {
                return ranges::view::ref(t);
            }

            /// Not a view and not an lvalue? If it's a ForwardingRange_, then
            /// return a subrange holding the range's begin/end.
            template<typename T>
            static auto from_range_(T &&t, std::false_type, std::false_type, std::true_type)
            {
                return make_subrange(static_cast<T &&>(t));
            }

        public:
            template<typename T>
            auto CPP_fun(operator())(T &&t) (const
                requires ViewableRange<T>)
            {
                return all_fn::from_range_(
                    static_cast<T &&>(t),
                    meta::bool_<View<uncvref_t<T>>>{},
                    std::is_lvalue_reference<T>{},
                    meta::bool_<ForwardingRange_<T>>{});
            }

            template<typename T>
            RANGES_DEPRECATED("Passing a reference_wrapper to view::all is deprecated.")
            auto operator()(std::reference_wrapper<T> ref) const ->
                CPP_ret(ref_view<T>)(
                    requires Range<T &>)
            {
                return ranges::view::ref(ref.get());
            }
        };

        /// \relates all_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(all_fn, all)

        template<typename Rng>
        using all_t = decltype(all(std::declval<Rng>()));
    }

    template<typename Rng>
    struct identity_adaptor
      : Rng
    {
        CPP_assert(View<Rng>);

        identity_adaptor() = default;
        constexpr explicit identity_adaptor(Rng const &rng)
          : Rng(rng)
        {}
        constexpr explicit identity_adaptor(Rng &&rng)
          : Rng(detail::move(rng))
        {}
    };

    namespace cpp20
    {
        namespace view
        {
            using ranges::view::all;
        }
        CPP_template(typename Rng)(
            requires ViewableRange<Rng>)
        using all_view = ranges::view::all_t<Rng>;
    }
    /// @}
}

#endif
