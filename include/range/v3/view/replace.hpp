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

#ifndef RANGES_V3_VIEW_REPLACE_HPP
#define RANGES_V3_VIEW_REPLACE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct replace_fn
            {
            private:
                friend view_access;
                template<typename Val>
                struct replacer_fun
                {
                private:
                    friend struct replace_fn;
                    Val old_value_;
                    Val new_value_;

                    template<typename Val1, typename Val2>
                    replacer_fun(Val1 && old_value, Val2 && new_value)
                      : old_value_(std::forward<Val1>(old_value)),
                        new_value_(std::forward<Val2>(new_value))
                    {}

                public:
                    template<typename Other>
                    Other operator()(Other && other) const
                    {
                        return (other == old_value_) ? new_value_ : std::forward<Other>(other);
                    }
                };
                template<typename Val1, typename Val2,
                    CONCEPT_REQUIRES_(Same<detail::decay_t<Val1>, detail::decay_t<Val2>>())>
                static auto bind(replace_fn replace, Val1 && old_value, Val2 && new_value)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(replace, std::placeholders::_1,
                        bind_forward<Val1>(old_value), bind_forward<Val2>(new_value)))
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Val1, typename Val2,
                    CONCEPT_REQUIRES_(!Same<detail::decay_t<Val1>, detail::decay_t<Val2>>())>
                static detail::null_pipe bind(replace_fn replace, Val1 &&, Val2 &&)
                {
                    CONCEPT_ASSERT_MSG(Same<detail::decay_t<Val1>, detail::decay_t<Val2>>(),
                        "The two values passed to view::replace must have the same type.");
                    return {};
                }
            #endif
            public:
                template<typename Rng, typename Val1, typename Val2>
                using Concept = meta::and_<
                    InputIterable<Rng>,
                    Same<detail::decay_t<Val1>, detail::decay_t<Val2>>,
                    EqualityComparable<range_value_t<Rng>, detail::decay_t<Val1>>,
                    Convertible<detail::decay_t<Val1> const &, range_reference_t<Rng>>>;

                template<typename Rng, typename Val1, typename Val2,
                    CONCEPT_REQUIRES_(Concept<Rng, Val1, Val2>())>
                transform_view<Rng, replacer_fun<detail::decay_t<Val1>>>
                operator()(Rng && rng, Val1 && old_value, Val2 && new_value) const
                {
                    return {std::forward<Rng>(rng),
                            {std::forward<Val1>(old_value),
                             std::forward<Val2>(new_value)}};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Rng, typename Val1, typename Val2,
                    CONCEPT_REQUIRES_(!Concept<Rng, Val1, Val2>())>
                void operator()(Rng && rng, Val1 && old_value, Val2 && new_value) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The first argument to view::replace must be a model of the "
                        "InputIterable concept.");
                    CONCEPT_ASSERT_MSG(Same<detail::decay_t<Val1>, detail::decay_t<Val2>>(),
                        "The two values passed to view::replace must have the same type.");
                    CONCEPT_ASSERT_MSG(EqualityComparable<range_value_t<Rng>,
                        detail::decay_t<Val1>>(),
                        "The values passed to view::replace must be EqualityComparable "
                        "to the range's value type.");
                    CONCEPT_ASSERT_MSG(Convertible<detail::decay_t<Val1> const &,
                        range_reference_t<Rng>>(),
                        "The value passed to view::replace must be convertible to the "
                        "range's reference type.");
                }
            #endif
            };

            /// \sa `replace_fn`
            /// \ingroup group-views
            constexpr view<replace_fn> replace{};
        }
        /// @}
    }
}

#endif
