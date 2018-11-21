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
            Rng base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            CPP_def
            (
                template(typename Rng, typename Pred)
                concept DropWhileViewConcept,
                    InputRange<Rng> &&
                    IndirectPredicate<Pred, iterator_t<Rng>>
            );

            struct drop_while_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(drop_while_fn drop_while, Pred pred)
                {
                    return make_pipeable(std::bind(drop_while, std::placeholders::_1, protect(std::move(pred))));
                }
            public:
                template<typename Rng, typename Pred>
                auto operator()(Rng &&rng, Pred pred) const ->
                    CPP_ret(drop_while_view<all_t<Rng>, Pred>)(
                        requires DropWhileViewConcept<Rng, Pred>)
                {
                    return {all(static_cast<Rng &&>(rng)), std::move(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred>
                auto operator()(Rng &&, Pred) const ->
                    CPP_ret(void)(
                        requires not DropWhileViewConcept<Rng, Pred>)
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The first argument to view::drop_while must be a model of the "
                        "InputRange concept");
                    CPP_assert_msg(IndirectPredicate<Pred, iterator_t<Rng>>,
                        "The second argument to view::drop_while must be callable with "
                        "an argument of the range's common reference type, and its return value "
                        "must be convertible to bool");
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
