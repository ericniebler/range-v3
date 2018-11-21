/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Gonzalo Brito Gadeschi 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_AT_HPP
#define RANGES_V3_AT_HPP

#include <stdexcept>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/index.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// Checked indexed range access.
        ///
        /// \ingroup group-core
        struct at_fn
        {
            /// \return `begin(rng)[n]`
            template<typename Rng>
            constexpr /*c++14*/
            auto operator()(Rng &&rng, range_difference_t<Rng> n) const ->
                CPP_ret(range_reference_t<Rng>)(
                    requires RandomAccessRange<Rng> && SizedRange<Rng>)
            {
                // Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67371 in GCC 5
                check_throw(rng, n);
                return ranges::begin(rng)[n];
            }

            /// \return `begin(rng)[n]`
            template<typename Rng, typename T, typename D = range_difference_t<Rng>>
            constexpr /*c++14*/
            auto operator()(Rng &&rng, T &&t) const ->
                CPP_ret(range_reference_t<Rng>)(
                    requires RandomAccessRange<Rng> && SizedRange<Rng> &&
                        !Same<uncvref_t<T>, D> && ConvertibleTo<T, D>)
            {
                return (*this)((Rng &&) rng, static_cast<D>((T &&) t));
            }

            /// \cond
            template<typename R, typename T>
            auto operator()(R &&, T &&) const ->
                CPP_ret(void)(
                    requires not index_detail::Indexable<R, T>)
            {
                CPP_assert_msg(RandomAccessRange<R>,
                    "ranges::at(rng, idx): rng argument must be a model of the RandomAccessRange concept.");
                CPP_assert_msg(ConvertibleTo<T, range_difference_t<R>>,
                    "ranges::at(rng, idx): idx argument must be convertible to range_difference_t<rng>.");
            }

        private:
            template<typename Rng>
            constexpr /*c++14*/
            static void check_throw(Rng &&rng, range_difference_t<Rng> n)
            {
                (n < 0 || n >= ranges::distance(rng)) ? throw std::out_of_range("ranges::at") : void(0);
            }
            /// \endcond
        };

        /// Checked indexed range access.
        ///
        /// \ingroup group-core
        /// \sa `at_fn`
        RANGES_INLINE_VARIABLE(at_fn, at)
    }
}

#endif
