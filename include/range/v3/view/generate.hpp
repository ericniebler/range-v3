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
                template<typename G, CONCEPT_REQUIRES_(Function<G>())>
                generate_view<G> operator()(G g) const
                {
                    return generate_view<G>{std::move(g)};
                }
            };

            constexpr generate_fn generate{};
        }
    }
}

#endif
