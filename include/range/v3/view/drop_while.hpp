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

#ifndef RANGES_V3_VIEW_DROP_WHILE_HPP
#define RANGES_V3_VIEW_DROP_WHILE_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_interface.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
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
          : range_interface<drop_while_view<Rng, Pred>, is_infinite<Rng>::value>
        {
        private:
            friend range_access;
            using base_range_t = view::all_t<Rng>;
            using difference_type_ = range_difference_t<Rng>;
            base_range_t rng_;
            semiregular_invokable_t<Pred> pred_;

        public:
            drop_while_view() = default;
            drop_while_view(Rng && rng, Pred pred)
              : rng_(view::all(std::forward<Rng>(rng))), pred_(invokable(std::move(pred)))
            {}
            range_iterator_t<Rng> begin()
            {
                return find_if_not(rng_, std::ref(pred_));
            }
            range_sentinel_t<Rng> end()
            {
                return ranges::end(rng_);
            }
            CONCEPT_REQUIRES(Iterable<Rng const>() && Invokable<Pred const, range_value_t<Rng>>())
            range_iterator_t<Rng const> begin() const
            {
                return find_if_not(rng_, std::ref(pred_));
            }
            CONCEPT_REQUIRES(Iterable<Rng const>() && Invokable<Pred const, range_value_t<Rng>>())
            range_sentinel_t<Rng const> end() const
            {
                return ranges::end(rng_);
            }
            base_range_t & base()
            {
                return rng_;
            }
            base_range_t const & base() const
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
            public:
                template<typename Rng, typename Pred>
                using Concept = meta::and_<
                    InputIterable<Rng>,
                    InvokablePredicate<Pred, range_value_t<Rng>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred>())>
                drop_while_view<Rng, Pred>
                operator()(Rng && rng, Pred pred) const
                {
                    return {std::forward<Rng>(rng), std::move(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<Rng>(),
                        "The first argument to view::drop_while must be a model of the "
                        "InputIterable concept");
                    CONCEPT_ASSERT_MSG(InvokablePredicate<Pred, range_value_t<Rng>>(),
                        "The second argument to view::drop_while must be callable with "
                        "an argument of the range's value type, and its return value must "
                        "be convertible to bool");
                }
            #endif
            };

            /// \sa `drop_while_fn`
            /// \ingroup group-views
            constexpr view<drop_while_fn> drop_while{};
        }
        /// @}
    }
}

#endif
