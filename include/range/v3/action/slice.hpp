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

#ifndef RANGES_V3_ACTION_SLICE_HPP
#define RANGES_V3_ACTION_SLICE_HPP

#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            CONCEPT_def
            (
                template(typename Rng, typename T, typename U)
                concept SliceActionConcept,
                    ForwardRange<Rng> &&
                    ErasableRange<Rng &, iterator_t<Rng>, iterator_t<Rng>> &&
                    ConvertibleTo<T, range_difference_type_t<Rng>> &&
                    ConvertibleTo<U, range_difference_type_t<Rng>>
            );

        struct slice_fn
            {
            private:
                friend action_access;
                CONCEPT_template(typename D)(
                    requires Integral<D>)
                (static auto) bind(slice_fn slice, D from, D to)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(slice, std::placeholders::_1, from, to)
                )
            public:
                // TODO support slice from end.
                CONCEPT_template(typename Rng,
                    typename I = iterator_t<Rng>,
                    typename D = range_difference_type_t<Rng>)(
                    requires SliceActionConcept<Rng, D, D>)
                (Rng) operator()(Rng &&rng, range_difference_type_t<Rng> from,
                    range_difference_type_t<Rng> to) const
                {
                    RANGES_EXPECT(from <= to);
                    ranges::action::erase(rng, next(begin(rng), to), end(rng));
                    ranges::action::erase(rng, begin(rng), next(begin(rng), from));
                    return static_cast<Rng &&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                CONCEPT_template(typename Rng, typename T, typename U)(
                    requires not SliceActionConcept<Rng, T, U>)
                (void) operator()(Rng &&, T &&, U &&) const
                {
                    CONCEPT_assert_msg(ForwardRange<Rng>,
                        "The object on which action::slice operates must be a model of the "
                        "ForwardRange concept.");
                    using I = iterator_t<Rng>;
                    CONCEPT_assert_msg(ErasableRange<Rng &, I, I>,
                        "The object on which action::slice operates must allow element "
                        "removal.");
                    CONCEPT_assert_msg(And<ConvertibleTo<T, range_difference_type_t<Rng>>,
                            ConvertibleTo<U, range_difference_type_t<Rng>>>,
                        "The bounds passed to action::slice must be convertible to the range's "
                        "difference type. TODO slicing from the end with 'end-2' syntax is not "
                        "supported yet, sorry!");
                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates slice_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<slice_fn>, slice)
        }
        /// @}
    }
}

#endif
