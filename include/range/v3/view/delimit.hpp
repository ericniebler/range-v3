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

#ifndef RANGES_V3_VIEW_DELIMIT_HPP
#define RANGES_V3_VIEW_DELIMIT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/range.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Val>
        struct delimit_view
          : range_adaptor<delimit_view<Rng, Val>, Rng>
        {
        private:
            friend range_access;
            Val value_;

            struct sentinel_adaptor : adaptor_base
            {
                sentinel_adaptor() = default;
                sentinel_adaptor(Val value)
                  : value_(std::move(value))
                {}
                bool empty(range_iterator_t<Rng> it, range_sentinel_t<Rng> end) const
                {
                    return it == end || *it == value_;
                }
                Val value_;
            };

            sentinel_adaptor end_adaptor() const
            {
                return {value_};
            }
        public:
            delimit_view() = default;
            delimit_view(Rng && rng, Val value)
              : range_adaptor_t<delimit_view>{std::forward<Rng>(rng)}
              , value_(std::move(value))
            {}
        };

        namespace view
        {
            struct delimit_impl_fn
            {
            private:
                friend view_access;
                template<typename Val>
                static auto bind(delimit_impl_fn delimit, Val value)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(delimit, std::placeholders::_1, std::move(value)))
                )
            public:
                template<typename Rng, typename Val>
                using Concept = meta::and_<
                    Iterable<Rng>,
                    EqualityComparable<Val, range_value_t<Rng>>>;

                template<typename Rng, typename Val,
                    CONCEPT_REQUIRES_(Concept<Rng, Val>())>
                delimit_view<Rng, Val>
                operator()(Rng && rng, Val value) const
                {
                    return {std::forward<Rng>(rng), std::move(value)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Val,
                    CONCEPT_REQUIRES_(!Concept<Rng, Val>())>
                void
                operator()(Rng &&, Val) const
                {
                    CONCEPT_ASSERT_MSG(Iterable<Rng>(),
                        "Rng must model the Iterable concept");
                    CONCEPT_ASSERT_MSG(EqualityComparable<Val, range_value_t<Rng>>(),
                        "The delimiting value type must be EqualityComparable to the "
                        "range's value type.");
                }
            #endif
            };

            struct delimit_fn : view<delimit_impl_fn>
            {
                using view<delimit_impl_fn>::operator();

                template<typename I, typename Val,
                    CONCEPT_REQUIRES_(InputIterator<I>())>
                delimit_view<range<I, unreachable>, Val>
                operator()(I begin, Val value) const
                {
                    return {{std::move(begin), {}}, std::move(value)};
                }
            };

            /// \sa `delimit_fn`
            /// \ingroup group-views
            constexpr delimit_fn delimit{};
        }
        /// @}
    }
}

#endif
