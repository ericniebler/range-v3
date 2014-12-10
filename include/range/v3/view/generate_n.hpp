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

#ifndef RANGES_V3_VIEW_GENERATE_N_HPP
#define RANGES_V3_VIEW_GENERATE_N_HPP

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
        struct generate_n_view
          : range_facade<generate_n_view<G>>
        {
        private:
            friend struct range_access;
            optional<G> gen_;
            std::size_t n_;
            template<bool IsConst>
            struct cursor
            {
            private:
                using gen_t = meta::apply<meta::add_const_if_c<IsConst>, G>;
                gen_t *gen_;
                std::size_t n_;
            public:
                using single_pass = std::true_type;
                cursor() = default;
                cursor(gen_t &g, std::size_t n)
                  : gen_(&g), n_(n)
                {}
                constexpr bool done() const
                {
                    return 0 == n_;
                }
                auto current() const -> decltype((*gen_)())
                {
                    return (*gen_)();
                }
                void next()
                {
                    RANGES_ASSERT(0 != n_);
                    --n_;
                }
            };
            CONCEPT_REQUIRES(!Function<G const>())
            cursor<false> begin_cursor()
            {
                RANGES_ASSERT(!!gen_);
                return {*gen_, n_};
            }
            CONCEPT_REQUIRES(Function<G const>())
            cursor<true> begin_cursor() const
            {
                RANGES_ASSERT(!!gen_);
                return {*gen_, n_};
            }
        public:
            generate_n_view() = default;
            explicit generate_n_view(G g, std::size_t n)
              : gen_(std::move(g)), n_(n)
            {}
            std::size_t size() const
            {
                return n_;
            }
        };

        namespace view
        {
            struct generate_n_fn
            {
                template<typename G>
                using Concept = meta::and_<
                    Function<G>,
                    meta::not_<Same<void, concepts::Function::result_t<G>>>>;

                template<typename G,
                    CONCEPT_REQUIRES_(Concept<G>())>
                generate_n_view<G> operator()(G g, std::size_t n) const
                {
                    return generate_n_view<G>{std::move(g), n};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename G,
                    CONCEPT_REQUIRES_(!Concept<G>())>
                void operator()(G, std::size_t) const
                {
                    CONCEPT_ASSERT_MSG(Function<G>(),
                        "The argument to view::generate must be a function that is callable with "
                        "no arguments");
                    CONCEPT_ASSERT_MSG(meta::not_<Same<void, concepts::Function::result_t<G>>>(),
                        "The return type of the function G must not be void.");
                }
            #endif
            };

            /// \sa `generate_n_fn`
            /// \ingroup group-views
            constexpr generate_n_fn generate_n{};
        }
        /// @}
    }
}

#endif
