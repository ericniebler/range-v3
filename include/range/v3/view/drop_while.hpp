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

#ifndef RANGES_V3_VIEW_DROP_WHILE_HPP
#define RANGES_V3_VIEW_DROP_WHILE_HPP

#include <utility>
#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/functional.hpp>
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
            semiregular_t<function_type<Pred>> pred_;
            optional<range_iterator_t<Rng>> begin_;

            range_iterator_t<Rng> get_begin_()
            {
                if(!begin_)
                    begin_ = find_if_not(rng_, std::ref(pred_));
                return *begin_;
            }
        public:
            drop_while_view() = default;
            drop_while_view(drop_while_view &&that)
              : drop_while_view::view_interface(std::move(that))
              , rng_(std::move(that).rng_), pred_(std::move(that).pred_), begin_{}
            {}
            drop_while_view(drop_while_view const &that)
              : drop_while_view::view_interface(that)
              , rng_(that.rng_), pred_(that.pred_), begin_{}
            {}
            drop_while_view(Rng rng, Pred pred)
              : rng_(std::move(rng)), pred_(as_function(std::move(pred))), begin_{}
            {}
            drop_while_view& operator=(drop_while_view &&that)
            {
                rng_ = std::move(that).rng_;
                pred_ = std::move(that).pred_;
                begin_.reset();
                return *this;
            }
            drop_while_view& operator=(drop_while_view const &that)
            {
                rng_ = that.rng_;
                pred_ = that.pred_;
                begin_.reset();
                return *this;
            }
            range_iterator_t<Rng> begin()
            {
                return get_begin_();
            }
            range_sentinel_t<Rng> end()
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
            public:
                template<typename Rng, typename Pred>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    IndirectCallablePredicate<Pred, range_iterator_t<Rng>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Concept<Rng, Pred>())>
                drop_while_view<all_t<Rng>, Pred>
                operator()(Rng && rng, Pred pred) const
                {
                    return {all(std::forward<Rng>(rng)), std::move(pred)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Concept<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument to view::drop_while must be a model of the "
                        "InputRange concept");
                    CONCEPT_ASSERT_MSG(IndirectCallablePredicate<Pred, range_iterator_t<Rng>>(),
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

#endif
