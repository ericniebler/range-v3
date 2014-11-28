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

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            struct replace_fn
            {
            private:
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
            public:
                template<typename Rng, typename Val1, typename Val2,
                    CONCEPT_REQUIRES_(Same<detail::decay_t<Val1>,
                                           detail::decay_t<Val2>>())>
                transformed_view<Rng, replacer_fun<detail::decay_t<Val1>>>
                operator()(Rng && rng, Val1 && old_value, Val2 && new_value) const
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    CONCEPT_ASSERT(EqualityComparable<range_reference_t<Rng>,
                        detail::decay_t<Val1> const &>());
                    CONCEPT_ASSERT(Convertible<detail::decay_t<Val1> const &,
                        range_reference_t<Rng>>());
                    return {std::forward<Rng>(rng),
                            {std::forward<Val1>(old_value),
                             std::forward<Val2>(new_value)}};

                }

                template<typename Val1, typename Val2,
                    CONCEPT_REQUIRES_(Same<detail::decay_t<Val1>,
                                           detail::decay_t<Val2>>())>
                auto operator()(Val1 && old_value, Val2 && new_value) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, bind_forward<Val1>(old_value),
                        bind_forward<Val2>(new_value))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, bind_forward<Val1>(old_value),
                        bind_forward<Val2>(new_value)));
                }
            };

            constexpr replace_fn replace {};
        }
    }
}

#endif
