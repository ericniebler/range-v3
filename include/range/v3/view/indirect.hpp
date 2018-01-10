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

#ifndef RANGES_V3_VIEW_INDIRECT_HPP
#define RANGES_V3_VIEW_INDIRECT_HPP

#include <iterator>
#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct indirect_view
          : view_adaptor<indirect_view<Rng>, Rng>
        {
        private:
            friend range_access;
            struct adaptor
              : adaptor_base
            {
                constexpr auto read(iterator_t<Rng> const &it) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    **it
                )
                constexpr auto iter_move(iterator_t<Rng> const &it) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::iter_move(*it)
                )
            };
            constexpr adaptor begin_adaptor() const noexcept
            {
                return {};
            }
            constexpr adaptor end_adaptor() const noexcept
            {
                return {};
            }
        public:
            indirect_view() = default;
            explicit constexpr indirect_view(Rng rng)
                noexcept(std::is_nothrow_constructible<
                    typename indirect_view::view_adaptor, Rng>::value)
              : indirect_view::view_adaptor{detail::move(rng)}
            {}
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            constexpr range_size_type_t<Rng> size() const
                noexcept(noexcept(ranges::size(std::declval<Rng const &>())))
            {
                return ranges::size(this->base());
            }
            CONCEPT_REQUIRES(!SizedRange<Rng const>() && SizedRange<Rng>())
            RANGES_CXX14_CONSTEXPR range_size_type_t<Rng> size()
                noexcept(noexcept(ranges::size(std::declval<Rng &>())))
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct indirect_fn
            {
                template<typename Rng>
                using Constraint = meta::and_<
                    InputRange<Rng>,
                    Readable<range_value_type_t<Rng>>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Constraint<Rng>())>
                constexpr auto operator()(Rng &&rng) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    indirect_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))}
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Constraint<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The argument to view::indirect must be a model of the InputRange "
                        "concept");
                    CONCEPT_ASSERT_MSG(Readable<range_value_type_t<Rng>>(),
                        "The value type of the range passed to view::indirect must be a model "
                        "of the Readable concept.");
                }
            #endif
            };

            /// \relates indirect_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<indirect_fn>, indirect)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::indirect_view)

#endif
