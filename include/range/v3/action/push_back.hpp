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

#ifndef RANGES_V3_ACTION_PUSH_BACK_HPP
#define RANGES_V3_ACTION_PUSH_BACK_HPP

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
        namespace adl_push_back_detail
        {
            template<typename Cont, typename T,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Constructible<range_value_type_t<Cont>, T>())>
            auto push_back(Cont && cont, T && t) ->
                decltype((void)unwrap_reference(cont).push_back(static_cast<T&&>(t)))
            {
                unwrap_reference(cont).push_back(static_cast<T&&>(t));
            }

            template<typename Cont, typename Rng,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Range<Rng>())>
            auto push_back(Cont && cont, Rng && rng) ->
                decltype((void)ranges::insert(unwrap_reference(cont), end(cont), static_cast<Rng&&>(rng)))
            {
                ranges::insert(unwrap_reference(cont), end(cont), static_cast<Rng&&>(rng));
            }

            struct push_back_fn
            {
            private:
                friend action::action_access;
                template<typename T>
                static auto bind(push_back_fn push_back, T && val)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::bind(push_back, std::placeholders::_1, bind_forward<T>(val))
                )
            public:
                struct ConceptImpl
                {
                    template<typename Rng, typename T>
                    auto requires_(Rng &&rng, T &&t) -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<concepts::InputRange, Rng>(),
                            concepts::is_true(meta::or_<
                                Constructible<range_value_type_t<Rng>, T>,
                                Range<T>>()),
                            ((void)push_back(rng, (T &&) t), 42)
                        ));
                };

                template<typename Rng, typename Fun>
                using Concept = concepts::models<ConceptImpl, Rng, Fun>;

                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(Concept<Rng, T>())>
                Rng operator()(Rng && rng, T && t) const
                {
                    push_back(rng, static_cast<T&&>(t));
                    return static_cast<Rng&&>(rng);
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!Concept<Rng, T>())>
                void operator()(Rng &&rng, T &&t) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which action::push_back operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(meta::or_<
                        Constructible<range_value_type_t<Rng>, T>,
                        Range<T>>(),
                        "The object to be inserted with action::push_back must either be "
                        "convertible to the range's value type, or else it must be a range "
                        "of elements that are convertible to the range's value type.");
                    push_back(rng, (T &&) t);
                }
            #endif
            };
        }
        /// \endcond

        namespace action
        {
            /// \ingroup group-actions
            /// \sa with_braced_init_args
            RANGES_INLINE_VARIABLE(with_braced_init_args<action<adl_push_back_detail::push_back_fn>>,
                                   push_back)
        }

        using action::push_back;
    }
}

#endif
