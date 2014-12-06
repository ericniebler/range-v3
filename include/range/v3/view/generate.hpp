// Range v3 library
//
//  Copyright Eric Niebler 2014
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
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/optional.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename G>
        struct generate_view
          : range_facade<generate_view<G>, true>
        {
        private:
            friend struct range_access;
            optional<G> gen_;
            template<bool IsConst>
            struct cursor
            {
            private:
                using gen_t = meta::apply<meta::add_const_if_c<IsConst>, G>;
                gen_t *gen_;
            public:
                using single_pass = std::true_type;
                cursor() = default;
                cursor(gen_t &g)
                  : gen_(&g)
                {}
                constexpr bool done() const
                {
                    return false;
                }
                auto current() const -> decltype((*gen_)())
                {
                    return (*gen_)();
                }
                void next() const
                {}
            };
            CONCEPT_REQUIRES(!Function<G const>())
            cursor<false> begin_cursor()
            {
                RANGES_ASSERT(!!gen_);
                return {*gen_};
            }
            CONCEPT_REQUIRES(Function<G const>())
            cursor<true> begin_cursor() const
            {
                RANGES_ASSERT(!!gen_);
                return {*gen_};
            }
        public:
            generate_view() = default;
            explicit generate_view(G g)
              : gen_(std::move(g))
            {}
        };

        namespace view
        {
            struct generate_fn
            {
                template<typename G>
                using Concept = meta::and_<
                    Function<G>,
                    meta::not_<Same<void, concepts::Function::result_t<G>>>>;

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
                    CONCEPT_ASSERT_MSG(Function<G>(),
                        "The argument to view::generate must be a function that is callable with "
                        "no arguments");
                    CONCEPT_ASSERT_MSG(meta::not_<Same<void, concepts::Function::result_t<G>>>(),
                        "The return type of the function G must not be void.");
                }
            #endif
            };

            /// \sa `generate_fn`
            /// \ingroup group-views
            constexpr generate_fn generate{};
        }
        /// \@}
    }
}

#endif
