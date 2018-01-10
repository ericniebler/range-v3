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

#ifndef RANGES_V3_VIEW_TAKE_WHILE_HPP
#define RANGES_V3_VIEW_TAKE_WHILE_HPP

#include <utility>
#include <functional>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct iter_take_while_view
          : view_adaptor<
                iter_take_while_view<Rng, Pred>,
                Rng,
                is_finite<Rng>::value ? finite : unknown>
        {
        private:
            friend range_access;
            semiregular_t<Pred> pred_;

            template<bool IsConst>
            struct sentinel_adaptor
              : adaptor_base
            {
            private:
                semiregular_ref_or_val_t<Pred, IsConst> pred_;
            public:
                sentinel_adaptor() = default;
                sentinel_adaptor(semiregular_ref_or_val_t<Pred, IsConst> pred)
                  : pred_(std::move(pred))
                {}
                bool empty(iterator_t<Rng> it, sentinel_t<Rng> end) const
                {
                    return it == end || !invoke(pred_, it);
                }
            };
            sentinel_adaptor<false> end_adaptor()
            {
                return {pred_};
            }
            CONCEPT_REQUIRES(Invocable<Pred const&, iterator_t<Rng>>())
            sentinel_adaptor<true> end_adaptor() const
            {
                return {pred_};
            }
        public:
            iter_take_while_view() = default;
            iter_take_while_view(Rng rng, Pred pred)
              : iter_take_while_view::view_adaptor{std::move(rng)}
              , pred_(std::move(pred))
            {}
        };

        template<typename Rng, typename Pred>
        struct take_while_view
          : iter_take_while_view<Rng, indirected<Pred>>
        {
            take_while_view() = default;
            take_while_view(Rng rng, Pred pred)
              : iter_take_while_view<Rng, indirected<Pred>>{std::move(rng),
                    indirect(std::move(pred))}
            {}
        };

        namespace view
        {
            struct iter_take_while_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(iter_take_while_fn iter_take_while, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(iter_take_while, std::placeholders::_1,
                        protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename Pred>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    Predicate<Pred&, iterator_t<Rng>>,
                    CopyConstructible<Pred>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred>())>
                iter_take_while_view<all_t<Rng>, Pred> operator()(Rng && rng, Pred pred) const
                {
                    return {all(static_cast<Rng&&>(rng)), std::move(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::take_while operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(Predicate<Pred&, iterator_t<Rng>>(),
                        "The function passed to view::take_while must be callable with objects of "
                        "the range's iterator type, and its result type must be convertible to "
                        "bool.");
                    CONCEPT_ASSERT_MSG(CopyConstructible<Pred>(),
                        "The function object passed to view::take_while must be CopyConstructible.");
                }
            #endif
            };

            struct take_while_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(take_while_fn take_while, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(take_while, std::placeholders::_1,
                        protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename Pred>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    IndirectPredicate<Pred, iterator_t<Rng>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred>())>
                take_while_view<all_t<Rng>, Pred> operator()(Rng && rng, Pred pred) const
                {
                    return {all(static_cast<Rng&&>(rng)), std::move(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::take_while operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, iterator_t<Rng>>(),
                        "The function passed to view::take_while must be callable with objects of "
                        "the range's common reference type, and its result type must be "
                        "convertible to bool.");
                }
            #endif
            };

            /// \relates iter_take_while_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<iter_take_while_fn>, iter_take_while)

            /// \relates take_while_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<take_while_fn>, take_while)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::iter_take_while_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::v3::take_while_view)

#endif
