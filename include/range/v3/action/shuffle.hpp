/// \file
// Range v3 library
//
//  Copyright Filip Matzner 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_SHUFFLE_HPP
#define RANGES_V3_ACTION_SHUFFLE_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/algorithm/shuffle.hpp>
#include <range/v3/functional/bind.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct shuffle_fn
        {
        private:
            friend action_access;
            template<typename Gen>
            static auto CPP_fun(bind)(shuffle_fn shuffle, Gen && gen)( //
                requires uniform_random_bit_generator<Gen>)
            {
                return bind_back(shuffle, static_cast<Gen &&>(gen));
            }
#ifdef RANGES_WORKAROUND_MSVC_OLD_LAMBDA
            template<typename Gen>
            struct lamduh
            {
                Gen & gen_;

                template<typename Rng>
                auto operator()(Rng && rng) const
                    -> invoke_result_t<shuffle_fn, Rng, Gen &>
                {
                    return shuffle_fn{}(static_cast<Rng &&>(rng), gen_);
                }
            };

            template<typename Gen>
            static lamduh<Gen> CPP_fun(bind)(shuffle_fn, Gen & gen)( //
                requires uniform_random_bit_generator<Gen>)
            {
                return {gen};
            }
#else  // ^^^ workaround / no workaround vvv
            template<typename Gen>
            static auto CPP_fun(bind)(shuffle_fn, Gen & gen)( //
                requires uniform_random_bit_generator<Gen>)
            {
                return [&gen](auto && rng)
                           -> invoke_result_t<shuffle_fn, decltype(rng), Gen &> {
                    return shuffle_fn{}(static_cast<decltype(rng)>(rng), gen);
                };
            }
#endif // RANGES_WORKAROUND_MSVC_OLD_LAMBDA

        public:
            template<typename Rng, typename Gen>
            auto operator()(Rng && rng, Gen && gen) const -> CPP_ret(Rng)( //
                requires random_access_range<Rng> && permutable<iterator_t<Rng>> &&
                    uniform_random_bit_generator<std::remove_reference_t<Gen>> &&
                        convertible_to<invoke_result_t<Gen &>, range_difference_t<Rng>>)
            {
                ranges::shuffle(rng, static_cast<Gen &&>(gen));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \relates shuffle_fn
        /// \sa `action`
        RANGES_INLINE_VARIABLE(action<shuffle_fn>, shuffle)
    } // namespace actions
    /// @}
} // namespace ranges

#endif
