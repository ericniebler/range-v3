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
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct indirect_view
          : range_adaptor<indirect_view<Rng>, Rng>
        {
        private:
            friend range_access;
            struct adaptor
              : adaptor_base
            {
                auto current(range_iterator_t<Rng> it) const -> decltype(**it)
                {
                    return **it;
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
            explicit indirect_view(Rng && rng)
              : range_adaptor_t<indirect_view>{std::forward<Rng>(rng)}
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
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
                    InputIterable<Rng>,
                    Readable<range_value_t<Rng>>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Concept<Rng>())>
                indirect_view<Rng> operator()(Rng && rng) const
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    return indirect_view<Rng>{std::forward<Rng>(rng)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Concept<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The argument to view::indirect must be a model of the InputIterable "
                        "concept");
                    CONCEPT_ASSERT_MSG(Readable<range_value_t<Rng>>(),
                        "The value type of the range passed to view::indirect must be a model "
                        "of the Readable concept.");
                }
            #endif
            };

            /// \sa `indirect_fn`
            /// \ingroup group-views
            constexpr view<indirect_fn> indirect{};
        }
        /// @}
    }
}

#endif
