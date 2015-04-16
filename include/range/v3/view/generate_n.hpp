/// \file
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
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>

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
            using result_t = concepts::Function::result_t<G>;
            semiregular_t<G> gen_;
            semiregular_t<result_t> val_;
            std::size_t n_;
            struct cursor
            {
            private:
                generate_n_view *rng_;
                std::size_t n_;
            public:
                using single_pass = std::true_type;
                RANGE_RELAXED_CONSTEXPR cursor() = default;
                RANGE_RELAXED_CONSTEXPR cursor(generate_n_view &rng, std::size_t n)
                  : rng_(&rng), n_(n)
                {}
                bool done() const
                {
                    return 0 == n_;
                }
                RANGE_RELAXED_CONSTEXPR result_t current() const
                {
                    return rng_->val_;
                }
                RANGES_RELAXED_CONSTEXPR
                void next()
                {
                    RANGES_ASSERT(0 != n_);
                    if(0 != --n_)
                        rng_->next();
                }
            };
            RANGE_RELAXED_CONSTEXPR void next()
            {
                val_ = gen_();
            }
            RANGE_RELAXED_CONSTEXPR cursor begin_cursor()
            {
                return {*this, n_};
            }
        public:
            RANGES_RELAXED_CONSTEXPR generate_n_view() : gen_{}, n_{} {}
            RANGES_RELAXED_CONSTEXPR
            explicit generate_n_view(G g, std::size_t n)
              : gen_(std::move(g)), val_{}, n_(n)
            {
                if(0 != n)
                    next();
            }
            RANGE_RELAXED_CONSTEXPR result_t & cached()
            {
                return val_;
            }
            RANGES_RELAXED_CONSTEXPR
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
                RANGES_RELAXED_CONSTEXPR
                generate_n_view<G> operator()(G g, std::size_t n) const
                {
                    return generate_n_view<G>{std::move(g), n};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename G,
                    CONCEPT_REQUIRES_(!Concept<G>())>
                RANGES_RELAXED_CONSTEXPR
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

            /// \relates generate_n_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& generate_n = static_const<generate_n_fn>::value;
            }
        }
        /// @}
    }
}

#endif
