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

#ifndef RANGES_V3_VIEW_REPLACE_IF_HPP
#define RANGES_V3_VIEW_REPLACE_IF_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct replace_if_fn
            {
            private:
                friend view_access;
                template<typename Pred, typename Val>
                struct replacer_if_fun
                {
                private:
                    friend struct replace_if_fn;
                    compressed_pair<semiregular_invokable_t<Pred>, Val> fun_and_new_value_;

                    template<typename Val2>
                    replacer_if_fun(Pred pred, Val2 new_value)
                      : fun_and_new_value_{invokable(std::move(pred)), std::move(new_value)}
                    {}
                public:
                    template<typename Other,
                        CONCEPT_REQUIRES_(!Invokable<Pred const, Other &>())>
                    uncvref_t<Other> operator()(Other && other)
                    {
                        return (fun_and_new_value_.first(other)) ?
                            fun_and_new_value_.second : std::forward<Other>(other);
                    }
                    template<typename Other,
                        CONCEPT_REQUIRES_(Invokable<Pred const, Other &>())>
                    uncvref_t<Other> operator()(Other && other) const
                    {
                        return (fun_and_new_value_.first(other)) ?
                            fun_and_new_value_.second : std::forward<Other>(other);
                    }
                };
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
                    InputIterable<Rng>,
                    InvokablePredicate<Pred, range_value_t<Rng>>,
                    EqualityComparable<
                        range_value_t<Rng>,
                        concepts::InvokablePredicate::result_t<Pred, range_value_t<Rng>>>,
                    Convertible<Val, range_value_t<Rng>>>;

                template<typename Rng, typename Pred, typename Val,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred, Val>())>
                transform_view<Rng, replacer_if_fun<Pred, detail::decay_t<Val>>>
                operator()(Rng && rng, Pred pred, Val new_value) const
                {
                    return {std::forward<Rng>(rng), {std::move(pred), std::move(new_value)}};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Rng, typename Pred, typename Val,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred, Val>())>
                void operator()(Rng && rng, Pred pred, Val new_value) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object on which view::replace_if operates must be a model of the "
                        "InputIterable concept.");
                    CONCEPT_ASSERT_MSG(InvokablePredicate<Pred, range_value_t<Rng>>(),
                        "The function passed to view::replace_if must be callable with "
                        "values from the range.");
                    CONCEPT_ASSERT_MSG(EqualityComparable<range_value_t<Rng>,
                        concepts::InvokablePredicate::result_t<Pred, range_value_t<Rng>>>(),
                        "The result of the function passed to view::replace_if must be "
                        "EqualityComparable with the range's value type.");
                    CONCEPT_ASSERT_MSG(Convertible<Val, range_value_t<Rng>>(),
                        "The value passed to view::replace_if must be convertible to the "
                        "range's value type.");
                }
            #endif
            };

            /// \sa `replace_if_fn`
            /// \ingroup group-views
            constexpr view<replace_if_fn> replace_if{};
        }
        /// @}
    }
}

#endif
