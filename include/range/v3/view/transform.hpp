/// \file
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

#ifndef RANGES_V3_VIEW_TRANSFORM_HPP
#define RANGES_V3_VIEW_TRANSFORM_HPP

#include <utility>
#include <iterator>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/zip_with.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Fun>
        struct iter_transform_view
          : range_adaptor<iter_transform_view<Rng, Fun>, Rng>
        {
        private:
            friend range_access;
            semiregular_t<invokable_t<Fun>> fun_;
            using use_sentinel_t =
                meta::or_<meta::not_<BoundedIterable<Rng>>, SinglePass<range_iterator_t<Rng>>>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                using fun_ref_ = semiregular_ref_or_val_t<invokable_t<Fun>, IsConst>;
                fun_ref_ fun_;
            public:
                using value_type =
                    detail::decay_t<decltype(fun_(copy_tag{}, range_iterator_t<Rng>{}))>;
                adaptor() = default;
                adaptor(fun_ref_ fun)
                  : fun_(std::move(fun))
                {}
                auto current(range_iterator_t<Rng> it) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fun_(it)
                )
                auto indirect_move(range_iterator_t<Rng> it) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fun_(move_tag{}, it)
                )
            };

            adaptor<false> begin_adaptor()
            {
                return {fun_};
            }
            meta::if_<use_sentinel_t, adaptor_base, adaptor<false>> end_adaptor()
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(Invokable<Fun const, range_iterator_t<Rng>>())
            adaptor<true> begin_adaptor() const
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(Invokable<Fun const, range_iterator_t<Rng>>())
            meta::if_<use_sentinel_t, adaptor_base, adaptor<true>> end_adaptor() const
            {
                return {fun_};
            }
        public:
            iter_transform_view() = default;
            iter_transform_view(Rng rng, Fun fun)
              : range_adaptor_t<iter_transform_view>{std::move(rng)}
              , fun_(invokable(std::move(fun)))
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
        };

        template<typename Rng, typename Fun>
        struct transform_view
          : iter_transform_view<Rng, detail::indirect_fn_<Fun>>
        {
            transform_view() = default;
            transform_view(Rng rng, Fun fun)
              : iter_transform_view<Rng, detail::indirect_fn_<Fun>>{std::move(rng),
                    {std::move(fun)}}
            {}
        };

        namespace view
        {
            struct iter_transform_fn
            {
            private:
                friend view_access;
                template<typename Fun>
                static auto bind(iter_transform_fn iter_transform, Fun fun)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(iter_transform, std::placeholders::_1,
                        protect(std::move(fun))))
                )
            public:
                template<typename Rng, typename Fun>
                using Concept = meta::and_<
                    InputIterable<Rng>,
                    Invokable<Fun, range_iterator_t<Rng>>,
                    Invokable<Fun, copy_tag, range_iterator_t<Rng>>,
                    Invokable<Fun, move_tag, range_iterator_t<Rng>>>;

                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Rng, Fun>())>
                iter_transform_view<all_t<Rng>, Fun> operator()(Rng && rng, Fun fun) const
                {
                    return {all(std::forward<Rng>(rng)), std::move(fun)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(!Concept<Rng, Fun>())>
                void operator()(Rng && rng, Fun fun) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object on which view::iter_transform operates must be a model of the "
                        "InputIterable concept.");
                    CONCEPT_ASSERT_MSG(
                        Invokable<Fun, range_iterator_t<Rng>>(),
                        "The function passed to view::iter_transform must be callable with an argument "
                        "of the range's iterator type.");
                    CONCEPT_ASSERT_MSG(
                        Invokable<Fun, copy_tag, range_iterator_t<Rng>>(),
                        "The function passed to view::iter_transform must be callable with "
                        "copy_tag and an argument of the range's iterator type.");
                    CONCEPT_ASSERT_MSG(
                        Invokable<Fun, move_tag, range_iterator_t<Rng>>(),
                        "The function passed to view::iter_transform must be callable with "
                        "move_tag and an argument of the range's iterator type.");
                }
            #endif
            };

            /// \relates iter_transform_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& iter_transform = static_const<view<iter_transform_fn>>::value;
            }

            struct transform_fn
            {
            private:
                friend view_access;
                template<typename Fun>
                static auto bind(transform_fn transform, Fun fun)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(transform, std::placeholders::_1,
                        protect(std::move(fun))))
                )
            public:
                template<typename Rng, typename Fun>
                using Concept = meta::and_<
                    InputIterable<Rng>,
                    Invokable<Fun, range_reference_t<Rng> &&>>;

                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Rng, Fun>())>
                transform_view<all_t<Rng>, Fun> operator()(Rng && rng, Fun fun) const
                {
                    return {all(std::forward<Rng>(rng)), std::move(fun)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(!Concept<Rng, Fun>())>
                void operator()(Rng && rng, Fun fun) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object on which view::transform operates must be a model of the "
                        "InputIterable concept.");
                    CONCEPT_ASSERT_MSG(
                        Invokable<Fun, range_reference_t<Rng> &&>(),
                        "The function passed to view::transform must be callable with an argument "
                        "of the range's reference type.");
                }
            #endif
            };

            /// \relates transform_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& transform = static_const<view<transform_fn>>::value;
            }
        }
        /// @}
    }
}

#endif
