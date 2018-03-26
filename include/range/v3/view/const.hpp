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

#ifndef RANGES_V3_VIEW_CONST_HPP
#define RANGES_V3_VIEW_CONST_HPP

#include <utility>
#include <type_traits>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct const_view
          : view_adaptor<const_view<Rng>, Rng>
        {
        private:
            friend range_access;
            using value_ =
                range_value_type_t<Rng>;
            using reference_ =
                common_reference_t<value_ const &&, range_reference_t<Rng>>;
            using rvalue_reference_ =
                common_reference_t<value_ const &&, range_rvalue_reference_t<Rng>>;
            struct adaptor
              : adaptor_base
            {
                reference_ read(iterator_t<Rng> const &it) const
                {
                    return *it;
                }
                rvalue_reference_ iter_move(iterator_t<Rng> const &it) const
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    ranges::iter_move(it)
                )
            };
            adaptor begin_adaptor() const
            {
                return {};
            }
            adaptor end_adaptor() const
            {
                return {};
            }
        public:
            const_view() = default;
            explicit const_view(Rng rng)
              : const_view::view_adaptor{std::move(rng)}
            {}
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            range_size_type_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            range_size_type_t<Rng> size()
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct const_fn
            {
                template<typename Rng>
                const_view<all_t<Rng>> operator()(Rng &&rng) const
                {
                    CONCEPT_ASSERT_MSG(Range<Rng>(),
                        "Rng must be a model of the Range concept");
                    return const_view<all_t<Rng>>{all(static_cast<Rng&&>(rng))};
                }
            };

            /// \relates const_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<const_fn>, const_)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::const_view)

#endif
