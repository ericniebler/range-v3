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

#ifndef RANGES_V3_ACTION_PUSH_FRONT_HPP
#define RANGES_V3_ACTION_PUSH_FRONT_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace adl_push_front_detail
        {
            CONCEPT_template(typename Cont, typename T)(
                requires LvalueContainerLike<Cont> &&
                    Constructible<range_value_type_t<Cont>, T>)
            (decltype(static_cast<void>(unwrap_reference(std::declval<Cont &>()).
                push_front(std::declval<T>()))))
            push_front(Cont &&cont, T &&t)
            {
                unwrap_reference(cont).push_front(static_cast<T &&>(t));
            }

            CONCEPT_template(typename Cont, typename Rng)(
                requires LvalueContainerLike<Cont> && Range<Rng>)
            (decltype(static_cast<void>(ranges::insert(
                std::declval<Cont &>(),
                std::declval<iterator_t<Cont>>(),
                std::declval<Rng>()))))
            push_front(Cont &&cont, Rng &&rng)
            {
                ranges::insert(cont, begin(cont), static_cast<Rng &&>(rng));
            }

            CONCEPT_def
            (
                template(typename Rng, typename T)
                concept PushFrontActionConcept,
                    requires (Rng &&rng, T &&t)
                    (
                        push_front(rng, (T &&) t)
                    ) &&
                    InputRange<Rng> &&
                        (Constructible<range_value_type_t<Rng>, T> ||
                        Range<T>)
            );

            struct push_front_fn
            {
            private:
                friend action::action_access;
                template<typename T>
                static auto bind(push_front_fn push_front, T &&val)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(push_front, std::placeholders::_1, bind_forward<T>(val))
                )
            public:
                CONCEPT_template(typename Rng, typename T)(
                    requires PushFrontActionConcept<Rng, T>)
                (Rng) operator()(Rng &&rng, T &&t) const
                {
                    push_front(rng, static_cast<T &&>(t));
                    return static_cast<Rng &&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                CONCEPT_template(typename Rng, typename T)(
                    requires not PushFrontActionConcept<Rng, T>)
                (void) operator()(Rng &&rng, T &&t) const
                {
                    CONCEPT_assert_msg(InputRange<Rng>,
                        "The object on which action::push_front operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_assert_msg(Or<
                        Constructible<range_value_type_t<Rng>, T>,
                        Range<T>>,
                        "The object to be inserted with action::push_front must either be "
                        "convertible to the range's value type, or else it must be a range "
                        "of elements that are convertible to the range's value type.");
                    push_front(rng, (T &&) t);
                }
            #endif
            };
        }
        /// \endcond

        namespace action
        {
            /// \ingroup group-actions
            /// \sa with_braced_init_args
            RANGES_INLINE_VARIABLE(with_braced_init_args<action<adl_push_front_detail::push_front_fn>>,
                                   push_front)
        }

        using action::push_front;
    }
}

#endif
