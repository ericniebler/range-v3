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

#ifndef RANGES_V3_VIEW_DELIMIT_HPP
#define RANGES_V3_VIEW_DELIMIT_HPP

#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
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
        template<typename Rng, typename Val>
        struct delimit_view
          : view_adaptor<delimit_view<Rng, Val>, Rng, is_finite<Rng>::value ? finite : unknown>
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
                template<class I, class S>
                bool empty(I const &it, S const &end) const
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
            delimit_view(Rng rng, Val value)
              : delimit_view::view_adaptor{std::move(rng)}
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
                    InputRange<Rng>,
                    SemiRegular<Val>,
                    EqualityComparable<Val, range_reference_t<Rng>>>;

                template<typename Rng, typename Val,
                    CONCEPT_REQUIRES_(Concept<Rng, Val>())>
                delimit_view<all_t<Rng>, Val>
                operator()(Rng && rng, Val value) const
                {
                    return {all(static_cast<Rng&&>(rng)), std::move(value)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Val,
                    CONCEPT_REQUIRES_(!Concept<Rng, Val>())>
                void
                operator()(Rng &&, Val) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "Rng must model the InputRange concept");
                    CONCEPT_ASSERT_MSG(SemiRegular<Val>(),
                        "The delimiting value type must be SemiRegular.");
                    CONCEPT_ASSERT_MSG(EqualityComparable<Val, range_reference_t<Rng>>(),
                        "The delimiting value type must be EqualityComparable to the "
                        "range's common reference type.");
                }
            #endif
            };

            struct delimit_fn : view<delimit_impl_fn>
            {
                using view<delimit_impl_fn>::operator();

                template<typename I_, typename Val, typename I = detail::decay_t<I_>,
                    CONCEPT_REQUIRES_(!Range<I_>() && ConvertibleTo<I_, I>() &&
                        InputIterator<I>() && SemiRegular<Val>() &&
                        EqualityComparable<Val, reference_t<I>>())>
                delimit_view<iterator_range<I, unreachable>, Val>
                operator()(I_ && begin_, Val value) const
                {
                    return {{static_cast<I_ &&>(begin_), {}}, std::move(value)};
                }
            };

            /// \relates delimit_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(delimit_fn, delimit)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::delimit_view)

#endif
