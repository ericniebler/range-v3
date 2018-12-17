/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_GENERATE_HPP
#define RANGES_V3_VIEW_GENERATE_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/unreachable.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename G>
        struct generate_view
          : view_facade<generate_view<G>, infinite>
        {
        private:
            friend range_access;
            using result_t = invoke_result_t<G &>;
            movesemiregular_t<G> gen_;
            detail::non_propagating_cache<result_t> val_;
            struct cursor
            {
            private:
                generate_view *view_;
            public:
                cursor() = default;
                explicit cursor(generate_view &view)
                  : view_(&view)
                {}
                result_t &&read() const
                {
                    if (!view_->val_)
                        view_->val_.emplace(view_->gen_());
                    return static_cast<result_t &&>(
                        static_cast<result_t &>(*view_->val_));
                }
                void next()
                {
                    view_->val_.reset();
                }
            };
            cursor begin_cursor()
            {
                return cursor{*this};
            }
            unreachable end_cursor() const
            {
                return {};
            }
        public:
            generate_view() = default;
            explicit generate_view(G g)
              : gen_(std::move(g))
            {}
            result_t &cached()
            {
                return *val_;
            }
        };

        namespace view
        {
            struct generate_fn
            {
                template<typename G>
                using Concept = meta::and_<
                    Invocable<G&>,
                    MoveConstructible<G>,
                    std::is_object<detail::decay_t<invoke_result_t<G &>>>,
                    Constructible<detail::decay_t<invoke_result_t<G &>>, invoke_result_t<G &>>,
                    Assignable<detail::decay_t<invoke_result_t<G &>>&, invoke_result_t<G &>>>;

                template<typename G,
                    CONCEPT_REQUIRES_(Concept<G>())>
                generate_view<G> operator()(G g) const
                {
                    return generate_view<G>{std::move(g)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename G,
                    CONCEPT_REQUIRES_(!Concept<G>())>
                void operator()(G) const
                {
                    check<G>();
                }
                template<typename G>
                static void check()
                {
                    CONCEPT_ASSERT_MSG(Invocable<G&>(),
                        "The function object G must be callable with no arguments.");
                    CONCEPT_ASSERT_MSG(MoveConstructible<G>(),
                        "The function object G must be MoveConstructible.");
                    using T = invoke_result_t<G &>;
                    using D = detail::decay_t<T>;
                    CONCEPT_ASSERT_MSG(std::is_object<D>(),
                        "The return type of the function object G must decay to an object type.");
                    CONCEPT_ASSERT_MSG(Constructible<D, T>(),
                        "The decayed return type of the function object G must be Constructible from the "
                        "return type of G.");
                    CONCEPT_ASSERT_MSG(Assignable<D&, T>(),
                        "The decayed return type of the function object G must be Assignable from the "
                        "return type of G.");
                }
            #endif
            };

            /// \relates generate_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(generate_fn, generate)
        }
        /// \@}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::generate_view)

#endif
