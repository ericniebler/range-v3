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
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Fun>
        struct transform_view
          : range_adaptor<transform_view<Rng, Fun>, Rng>
        {
        private:
            friend range_access;
            using reference_t = concepts::Invokable::result_t<Fun, range_value_t<Rng>>;
            semiregular_invokable_t<Fun> fun_;
            // Forward ranges must always return references. If the result of calling the function
            // is not a reference, this range is input-only.
            using single_pass = meta::or_<
                SinglePass<range_iterator_t<Rng>>,
                meta::not_<std::is_reference<reference_t>>>;
            using use_sentinel_t = meta::or_<meta::not_<BoundedIterable<Rng>>, single_pass>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                semiregular_invokable_ref_t<Fun, IsConst> fun_;
            public:
                using single_pass = transform_view::single_pass;
                adaptor() = default;
                adaptor(semiregular_invokable_ref_t<Fun, IsConst> fun)
                  : fun_(std::move(fun))
                {}
                auto current(range_iterator_t<Rng> it) const ->
                    decltype(fun_(*it))
                {
                    return fun_(*it);
                }
            };

            CONCEPT_REQUIRES(!Invokable<Fun const, range_value_t<Rng>>())
            adaptor<false> begin_adaptor()
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(Invokable<Fun const, range_value_t<Rng>>())
            adaptor<true> begin_adaptor() const
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(use_sentinel_t())
            adaptor_base end_adaptor() const
            {
                return {};
            }
            CONCEPT_REQUIRES(!use_sentinel_t() && !Invokable<Fun const, range_value_t<Rng>>())
            adaptor<false> end_adaptor()
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(!use_sentinel_t() && Invokable<Fun const, range_value_t<Rng>>())
            adaptor<true> end_adaptor() const
            {
                return {fun_};
            }
        public:
            transform_view() = default;
            transform_view(Rng && rng, Fun fun)
              : range_adaptor_t<transform_view>{std::forward<Rng>(rng)}
              , fun_(invokable(std::move(fun)))
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct transform_fn
            {
            private:
                friend view_access;
                template<typename Fun>
                static auto bind(transform_fn transform, Fun fun)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(transform, std::placeholders::_1, protect(std::move(fun))))
                )
            public:
                template<typename Rng, typename Fun>
                using Concept = meta::and_<
                    InputIterable<Rng>,
                    Invokable<Fun, range_value_t<Rng>>>;

                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Rng, Fun>())>
                transform_view<Rng, Fun> operator()(Rng && rng, Fun fun) const
                {
                    return {std::forward<Rng>(rng), std::move(fun)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(!Concept<Rng, Fun>())>
                void operator()(Rng && rng, Fun fun) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The object on which view::transform operates must be a model of the "
                        "InputIterable concept.");
                    CONCEPT_ASSERT_MSG(Invokable<Fun, range_value_t<Rng>>(),
                        "The function passed to view::transform must be callable with objects "
                        "of the range's value type.");
                }
            #endif
            };

            /// \sa `transform_fn`
            /// \ingroup group-views
            constexpr view<transform_fn> transform{};
        }
        /// @}
    }
}

#endif
