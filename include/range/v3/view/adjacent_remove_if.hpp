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

#ifndef RANGES_V3_VIEW_ADJACENT_REMOVE_IF_HPP
#define RANGES_V3_VIEW_ADJACENT_REMOVE_IF_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct adjacent_remove_if_view
          : view_adaptor<
                adjacent_remove_if_view<Rng, Pred>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            friend range_access;
            semiregular_t<function_type<Pred>> pred_;
            optional<range_iterator_t<Rng>> begin_;

            struct adaptor : adaptor_base
            {
            private:
                adjacent_remove_if_view *rng_;
                void satisfy(range_iterator_t<Rng> &it) const
                {
                    auto const end = ranges::end(rng_->mutable_base());
                    auto &&pred = rng_->pred_;
                    if(it == end)
                        return;
                    auto next = it;
                    for(; ++next != end; it = next)
                        if(!pred(*it, *next))
                            return;
                }
            public:
                adaptor() = default;
                adaptor(adjacent_remove_if_view &rng)
                  : rng_(&rng)
                {}
                range_iterator_t<Rng> begin(adjacent_remove_if_view &) const
                {
                    auto &beg = rng_->begin_;
                    if(!beg)
                    {
                        beg = ranges::begin(rng_->mutable_base());
                        this->satisfy(*beg);
                    }
                    return *beg;
                }
                void next(range_iterator_t<Rng> &it) const
                {
                    RANGES_ASSERT(it != ranges::end(rng_->mutable_base()));
                    this->satisfy(++it);
                }
                void prev() = delete;
                void distance_to() = delete;
            };
            adaptor begin_adaptor()
            {
                return {*this};
            }
            adaptor end_adaptor()
            {
                return {*this};
            }
        public:
            adjacent_remove_if_view() = default;
            adjacent_remove_if_view(adjacent_remove_if_view const &that)
              : adjacent_remove_if_view::view_adaptor(that)
              , pred_(that.pred_)
              , begin_{}
            {}
            adjacent_remove_if_view(Rng rng, Pred pred)
              : adjacent_remove_if_view::view_adaptor{std::move(rng)}
              , pred_(as_function(std::move(pred)))
              , begin_{}
            {}
            adjacent_remove_if_view& operator=(adjacent_remove_if_view &&that)
            {
                this->adjacent_remove_if_view::view_adaptor::operator=(std::move(that));
                pred_ = std::move(that).pred_;
                begin_.reset();
                return *this;
            }
            adjacent_remove_if_view& operator=(adjacent_remove_if_view const &that)
            {
                this->adjacent_remove_if_view::view_adaptor::operator=(that);
                pred_ = that.pred_;
                begin_.reset();
                return *this;
            }
       };

        namespace view
        {
            struct adjacent_remove_if_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(adjacent_remove_if_fn adjacent_remove_if, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(adjacent_remove_if, std::placeholders::_1,
                        protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename Pred>
                using Concept = meta::and_<
                    ForwardRange<Rng>,
                    IndirectCallablePredicate<Pred, range_iterator_t<Rng>,
                        range_iterator_t<Rng>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred>())>
                adjacent_remove_if_view<all_t<Rng>, Pred> operator()(Rng && rng, Pred pred) const
                {
                    return {all(std::forward<Rng>(rng)), std::move(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "Rng must model the ForwardRange concept");
                    CONCEPT_ASSERT_MSG(IndirectCallablePredicate<Pred, range_iterator_t<Rng>,
                        range_iterator_t<Rng>>(),
                        "Function Pred must be callable with two arguments of the range's common "
                        "reference type, and it must return something convertible to bool.");
                }
            #endif
            };

            /// \relates adjacent_remove_if_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<adjacent_remove_if_fn>, adjacent_remove_if)
        }
        /// @}
    }
}

#endif
