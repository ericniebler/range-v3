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

#ifndef RANGES_V3_VIEW_DROP_WHILE_HPP
#define RANGES_V3_VIEW_DROP_WHILE_HPP

#include <utility>
#include <functional>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct drop_while_view
          : view_interface<drop_while_view<Rng, Pred>, is_finite<Rng>::value ? finite : unknown>
        {
        private:
            friend range_access;
            Rng rng_;
            semiregular_t<Pred> pred_;
            detail::non_propagating_cache<iterator_t<Rng>> begin_;

            iterator_t<Rng> get_begin_()
            {
                if(!begin_)
                    begin_ = find_if_not(rng_, std::ref(pred_));
                return *begin_;
            }
        public:
            drop_while_view() = default;
            drop_while_view(Rng rng, Pred pred)
              : rng_(std::move(rng)), pred_(std::move(pred))
            {}
            iterator_t<Rng> begin()
            {
                return get_begin_();
            }
            sentinel_t<Rng> end()
            {
                return ranges::end(rng_);
            }
            Rng & base()
            {
                return rng_;
            }
            Rng const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct drop_while_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(drop_while_fn drop_while, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(drop_while, std::placeholders::_1, protect(std::move(pred))))
                )

                template<typename Pred, typename Proj>
                static auto bind(drop_while_fn drop_while, Pred pred, Proj proj)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(drop_while, std::placeholders::_1, 
                        protect(std::move(pred)), protect(std::move(proj))))
                )
            public:
                template<typename Rng, typename Pred, typename Proj = ident>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    IndirectPredicate<Pred, projected<iterator_t<Rng>, Proj>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred>())>
                drop_while_view<all_t<Rng>, Pred>
                operator()(Rng && rng, Pred pred) const
                {
                    return {all(static_cast<Rng&&>(rng)), std::move(pred)};
                }

                template<typename Rng, typename Pred, typename Proj,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred, Proj>())>
                RANGES_CXX14_CONSTEXPR
                auto operator()(Rng && rng, Pred pred, Proj proj) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    drop_while_view<all_t<Rng>, composed<Pred, Proj>>{
                        all(static_cast<Rng&&>(rng)),
                        compose(std::move(pred), std::move(proj))
                    }
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument to view::drop_while must be a model of the "
                        "InputRange concept");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, iterator_t<Rng>>(),
                        "The second argument to view::drop_while must be callable with "
                        "an argument of the range's common reference type, and its return value "
                        "must be convertible to bool");
                }

                template<typename Rng, typename Pred, typename Proj,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred, Proj>())>
                void operator()(Rng &&, Pred, Proj) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument to view::drop_while must be a model of the "
                        "InputRange concept");
                    using Itr = iterator_t<Rng>;
                    CONCEPT_ASSERT_MSG(IndirectInvocable<Proj, Itr>(),
                        "The projection function must accept arguments of the iterator's "
                        "value type, reference type, and common reference type.");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, projected<Itr, Proj>>(),
                        "The second argument to view::drop_while must accept "
                        "values returned by the projection function.");
                }
            #endif
            };

            /// \relates drop_while_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<drop_while_fn>, drop_while)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::drop_while_view)

#endif
