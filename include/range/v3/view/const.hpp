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

#ifndef RANGES_V3_VIEW_CONST_HPP
#define RANGES_V3_VIEW_CONST_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            struct cref_t { using type = T; };

            template<typename T>
            struct cref_t<T &> { using type = T const &; };

            template<typename T>
            struct cref_t<T &&> { using type = T const &&; };
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct const_view
          : range_adaptor<const_view<Rng>, Rng>
        {
        private:
            friend range_access;
            using reference_ = meta::eval<detail::cref_t<range_reference_t<Rng>>>;
            struct adaptor
              : adaptor_base
            {
                reference_ current(range_iterator_t<Rng> it) const
                {
                    return *it;
                }
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
            explicit const_view(Rng && rng)
              : range_adaptor_t<const_view>{std::forward<Rng>(rng)}
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct const_fn
            {
                template<typename Rng>
                const_view<Rng> operator()(Rng && rng) const
                {
                    CONCEPT_ASSERT_MSG(Iterable<Rng>(),
                        "Rng must be a model of the Iterable concept");
                    return const_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            /// \sa `const_fn`
            /// \ingroup group-views
            constexpr view<const_fn> const_{};
        }
        /// @}
    }
}

#endif
