/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_ADDRESSOF_HPP
#define RANGES_V3_VIEW_ADDRESSOF_HPP

#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/transform.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct addressof_fn
            {
            private:
                struct pred
                {
                    template<class V>
                    constexpr V *operator()(V &value) const noexcept
                    {
                        return std::addressof(value);
                    }
                };

            public:
                template<typename Rng>
                using Constraint = meta::and_<
                    InputRange<Rng>,
                    std::is_lvalue_reference<range_reference_t<Rng>>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Constraint<Rng>())>
                RANGES_CXX14_CONSTEXPR
                auto operator()(Rng &&rng) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    transform(all(static_cast<Rng &&>(rng)), pred{})
                )
            };

            /// \relates remove_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<addressof_fn>, addressof)
        }
        /// @}
    }
}

#endif //RANGES_V3_VIEW_ADDRESSOF_HPP
