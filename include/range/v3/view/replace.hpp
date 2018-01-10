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

#ifndef RANGES_V3_VIEW_REPLACE_HPP
#define RANGES_V3_VIEW_REPLACE_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

RANGES_DISABLE_WARNINGS

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Val1, typename Val2>
            struct replacer_fn
            {
            private:
                Val1 old_value_;
                Val2 new_value_;
            public:
                replacer_fn() = default;
                replacer_fn(Val1 old_value, Val2 new_value)
                  : old_value_(std::move(old_value))
                  , new_value_(std::move(new_value))
                {}

                template<typename I>
                [[noreturn]]
                common_type_t<decay_t<unwrap_reference_t<Val2 const &>>, value_type_t<I>> &
                operator()(copy_tag, I const &) const
                {
                    RANGES_EXPECT(false);
                }

                template<typename I>
                common_reference_t<unwrap_reference_t<Val2 const &>, reference_t<I>>
                operator()(I const &i) const
                {
                    auto &&x = *i;
                    if(x == unwrap_reference(old_value_))
                        return unwrap_reference(new_value_);
                    return ((decltype(x) &&) x);
                }

                template<typename I>
                common_reference_t<unwrap_reference_t<Val2 const &>, rvalue_reference_t<I>>
                operator()(move_tag, I const &i) const
                {
                    auto &&x = iter_move(i);
                    if(x == unwrap_reference(old_value_))
                        return unwrap_reference(new_value_);
                    return ((decltype(x) &&) x);
                }
            };
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct replace_fn
            {
            private:
                friend view_access;
                template<typename Val1, typename Val2,
                    typename V1 = detail::decay_t<unwrap_reference_t<Val1>>,
                    typename V2 = detail::decay_t<unwrap_reference_t<Val2>>,
                    CONCEPT_REQUIRES_(Same<V1, V2>())>
                static auto bind(replace_fn replace, Val1 old_value, Val2 new_value)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(replace, std::placeholders::_1,
                        std::move(old_value), std::move(new_value)))
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Val1, typename Val2,
                    typename V1 = detail::decay_t<unwrap_reference_t<Val1>>,
                    typename V2 = detail::decay_t<unwrap_reference_t<Val2>>,
                    CONCEPT_REQUIRES_(!Same<V1, V2>())>
                static detail::null_pipe bind(replace_fn, Val1, Val2)
                {
                    CONCEPT_ASSERT_MSG(Same<V1, V2>(),
                        "The two values passed to view::replace must have the same type.");
                    return {};
                }
            #endif
            public:
                template<typename Rng, typename Val1, typename Val2,
                    typename V1 = detail::decay_t<unwrap_reference_t<Val1>>,
                    typename V2 = detail::decay_t<unwrap_reference_t<Val2>>>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    Same<V1, V2>,
                    EqualityComparable<V1, range_value_type_t<Rng>>,
                    Common<detail::decay_t<unwrap_reference_t<Val2 const &>>, range_value_type_t<Rng>>,
                    CommonReference<unwrap_reference_t<Val2 const &>, range_reference_t<Rng>>,
                    CommonReference<unwrap_reference_t<Val2 const &>, range_rvalue_reference_t<Rng>>>;

                template<typename Rng, typename Val1, typename Val2,
                    CONCEPT_REQUIRES_(Concept<Rng, Val1, Val2>())>
                replace_view<all_t<Rng>, detail::decay_t<Val1>, detail::decay_t<Val2>>
                operator()(Rng && rng, Val1 && old_value, Val2 && new_value) const
                {
                    return {all(static_cast<Rng&&>(rng)),
                            {static_cast<Val1&&>(old_value),
                             static_cast<Val2&&>(new_value)}};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Rng, typename Val1, typename Val2,
                    CONCEPT_REQUIRES_(!Concept<Rng, Val1, Val2>())>
                void operator()(Rng &&, Val1 &&, Val2 &&) const
                {
                    using V1 = detail::decay_t<unwrap_reference_t<Val1>>;
                    using V2 = detail::decay_t<unwrap_reference_t<Val2>>;
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument to view::replace must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(Same<V1, V2>(),
                        "The two values passed to view::replace must have the same type.");
                    CONCEPT_ASSERT_MSG(EqualityComparable<V1, range_value_type_t<Rng>>(),
                        "The values passed to view::replace must be EqualityComparable "
                        "to the range's value type.");
                    CONCEPT_ASSERT_MSG(Common<detail::decay_t<unwrap_reference_t<Val2 const &>>,
                            range_value_type_t<Rng>>(),
                        "The value passed to view::replace must share a common type with the "
                        "range's value type.");
                    CONCEPT_ASSERT_MSG(CommonReference<unwrap_reference_t<Val2 const &>,
                            range_reference_t<Rng>>(),
                        "The value passed to view::replace must share a reference with the "
                        "range's reference type.");
                    CONCEPT_ASSERT_MSG(CommonReference<unwrap_reference_t<Val2 const &>,
                            range_rvalue_reference_t<Rng>>(),
                        "The value passed to view::replace must share a reference with the "
                        "range's rvalue reference type.");
                }
            #endif
            };

            /// \relates replace_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<replace_fn>, replace)
        }
        /// @}
    }
}

RANGES_RE_ENABLE_WARNINGS

#endif
