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

#ifndef RANGES_V3_VIEW_REPLACE_IF_HPP
#define RANGES_V3_VIEW_REPLACE_IF_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
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
            template<typename Pred, typename Val>
            struct replacer_if_fn
              : compressed_pair<semiregular_t<Pred>, Val>
            {
            private:
                using base_t = compressed_pair<semiregular_t<Pred>, Val>;
                using base_t::first;
                using base_t::second;

            public:
                replacer_if_fn() = default;
                replacer_if_fn(Pred pred, Val new_value)
                  : base_t{std::move(pred), std::move(new_value)}
                {}

                template<typename I>
                [[noreturn]]
                common_type_t<decay_t<unwrap_reference_t<Val const &>>, value_type_t<I>> &
                operator()(copy_tag, I const &) const
                {
                    RANGES_EXPECT(false);
                }

                template<typename I,
                    CONCEPT_REQUIRES_(!Invocable<Pred const&, reference_t<I>>())>
                common_reference_t<unwrap_reference_t<Val const &>, reference_t<I>>
                operator()(I const &i)
                {
                    auto &&x = *i;
                    if(invoke(first(), (decltype(x) &&) x))
                        return unwrap_reference(second());
                    return (decltype(x) &&) x;
                }
                template<typename I,
                    CONCEPT_REQUIRES_(Invocable<Pred const&, reference_t<I>>())>
                common_reference_t<unwrap_reference_t<Val const &>, reference_t<I>>
                operator()(I const &i) const
                {
                    auto &&x = *i;
                    if(invoke(first(), (decltype(x) &&) x))
                        return unwrap_reference(second());
                    return (decltype(x) &&) x;
                }

                template<typename I,
                    CONCEPT_REQUIRES_(!Invocable<Pred const&, rvalue_reference_t<I>>())>
                common_reference_t<unwrap_reference_t<Val const &>, rvalue_reference_t<I>>
                operator()(move_tag, I const &i)
                {
                    auto &&x = iter_move(i);
                    if(invoke(first(), (decltype(x) &&) x))
                        return unwrap_reference(second());
                    return (decltype(x) &&) x;
                }
                template<typename I,
                    CONCEPT_REQUIRES_(Invocable<Pred const&, rvalue_reference_t<I>>())>
                common_reference_t<unwrap_reference_t<Val const &>, rvalue_reference_t<I>>
                operator()(move_tag, I const &i) const
                {
                    auto &&x = iter_move(i);
                    if(invoke(first(), (decltype(x) &&) x))
                        return unwrap_reference(second());
                    return (decltype(x) &&) x;
                }
            };
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct replace_if_fn
            {
            private:
                friend view_access;
                template<typename Pred, typename Val>
                static auto bind(replace_if_fn replace_if, Pred pred, Val new_value)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(replace_if, std::placeholders::_1,
                        protect(std::move(pred)), std::move(new_value)))
                )
            public:
                template<typename Rng, typename Pred, typename Val>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    IndirectPredicate<Pred, iterator_t<Rng>>,
                    Common<detail::decay_t<unwrap_reference_t<Val const &>>, range_value_type_t<Rng>>,
                    CommonReference<unwrap_reference_t<Val const &>, range_reference_t<Rng>>,
                    CommonReference<unwrap_reference_t<Val const &>, range_rvalue_reference_t<Rng>>>;

                template<typename Rng, typename Pred, typename Val,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred, Val>())>
                replace_if_view<all_t<Rng>, Pred, Val>
                operator()(Rng && rng, Pred pred, Val new_value) const
                {
                    return {all(static_cast<Rng&&>(rng)), {std::move(pred), std::move(new_value)}};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Rng, typename Pred, typename Val,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred, Val>())>
                void operator()(Rng &&, Pred, Val) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::replace_if operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, iterator_t<Rng>>(),
                        "The function passed to view::replace_if must be callable with "
                        "objects of the range's common reference type, and the result must be "
                        "convertible to bool.");
                    CONCEPT_ASSERT_MSG(Common<detail::decay_t<unwrap_reference_t<Val const &>>,
                            range_value_type_t<Rng>>(),
                        "The value passed to view::replace must share a common type with the "
                        "range's value type.");
                    CONCEPT_ASSERT_MSG(CommonReference<unwrap_reference_t<Val const &>,
                            range_reference_t<Rng>>(),
                        "The value passed to view::replace must share a reference with the "
                        "range's reference type.");
                    CONCEPT_ASSERT_MSG(CommonReference<unwrap_reference_t<Val const &>,
                            range_rvalue_reference_t<Rng>>(),
                        "The value passed to view::replace must share a reference with the "
                        "range's rvalue reference type.");
                }
            #endif
            };

            /// \relates replace_if_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<replace_if_fn>, replace_if)
        }
        /// @}
    }
}

RANGES_RE_ENABLE_WARNINGS

#endif
