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

#ifndef RANGES_V3_VIEW_INDIRECT_HPP
#define RANGES_V3_VIEW_INDIRECT_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/functional.hpp>
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
                auto read(iterator_t<Rng> const &it) const ->
                    decltype(**it)
                {
                    return **it;
                }
                auto iter_move(iterator_t<Rng> const &it) const ->
                    decltype(ranges::iter_move(*it))
                {
                    return ranges::iter_move(*it);
                }
            };
            adaptor begin_adaptor() const
            {
                return {};
            }
            adaptor end_adaptor() const
            {
                return{};
            }
        public:
            indirect_view() = default;
            explicit indirect_view(Rng rng)
              : indirect_view::view_adaptor{std::move(rng)}
            {}
            CONCEPT_REQUIRES(SizedRange<Rng>())
            range_size_type_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct indirect_fn
            {
                template<typename Rng>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    Readable<range_value_type_t<Rng>>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Concept<Rng>())>
                indirect_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    CONCEPT_ASSERT(InputRange<Rng>());
                    return indirect_view<all_t<Rng>>{all(static_cast<Rng&&>(rng))};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Concept<Rng>())>
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
