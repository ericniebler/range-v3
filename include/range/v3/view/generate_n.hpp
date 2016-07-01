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

#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename G>
        struct generate_n_view
          : view_facade<generate_n_view<G>, finite>
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
                cursor() = default;
                cursor(generate_n_view &rng, std::size_t n)
                  : rng_(&rng), n_(n)
                {}
                bool done() const
                {
                    return 0 == n_;
                }
                result_t get() const
                {
                    return rng_->val_;
                }
                void next()
                {
                    RANGES_ASSERT(0 != n_);
                    if(0 != --n_)
                        rng_->next();
                }
            };
            void next()
            {
                val_ = gen_();
            }
            cursor begin_cursor()
            {
                return {*this, n_};
            }
        public:
            generate_n_view() = default;
            explicit generate_n_view(G g, std::size_t n)
              : gen_(std::move(g)), val_{}, n_(n)
            {
                if(0 != n)
                    next();
            }
            result_t & cached()
            {
                return val_;
            }
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

            /// \relates generate_n_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(generate_n_fn, generate_n)
        }
        /// @}
    }
}

#endif
