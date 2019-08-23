/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ACTION_SLICE_HPP
#define RANGES_V3_ACTION_SLICE_HPP

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/interface.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct slice_fn
        {
        private:
            friend action_access;

            template<typename D>
            using diff_t = range_difference_t<D>;

            // Overloads for the pipe syntax: rng | actions::slice(from, to)
            template<typename D>
            static auto CPP_fun(bind)(slice_fn slice, D from, D to)( //
                requires integral<D>)
            {
                return bind_back(slice, from, to);
            }
            template<typename D>
            static auto CPP_fun(bind)(slice_fn slice, D from, detail::from_end_<D> to)( //
                requires integral<D>)
            {
                return bind_back(slice, from, to);
            }
            template<typename D>
            static auto CPP_fun(bind)(slice_fn slice, detail::from_end_<D> from,
                                      detail::from_end_<D> to)( //
                requires integral<D>)
            {
                return bind_back(slice, from, to);
            }
            template<typename D>
            static auto CPP_fun(bind)(slice_fn slice, D from, end_fn const & to)( //
                requires integral<D>)
            {
                return bind_back(slice, from, to);
            }
            template<typename D>
            static auto CPP_fun(bind)(slice_fn slice, detail::from_end_<D> from,
                                      end_fn const & to)( //
                requires integral<D>)
            {
                return bind_back(slice, from, to);
            }

        public:
            template<typename Rng, typename I = iterator_t<Rng>>
            auto operator()(Rng && rng, diff_t<Rng> from, diff_t<Rng> to) const
                -> CPP_ret(Rng)( //
                    requires forward_range<Rng> && erasable_range<Rng &, I, I>)
            {
                RANGES_EXPECT(0 <= from && 0 <= to && from <= to);
                RANGES_EXPECT(!sized_range<Rng> || to <= distance(rng));
                ranges::actions::erase(rng, begin(rng), next(begin(rng), from));
                ranges::actions::erase(rng, next(begin(rng), to - from), end(rng));
                return static_cast<Rng &&>(rng);
            }

            template<typename Rng, typename I = iterator_t<Rng>>
            auto operator()(Rng && rng, diff_t<Rng> from,
                            detail::from_end_<diff_t<Rng>> to) const -> CPP_ret(Rng)( //
                requires bidirectional_range<Rng> && erasable_range<Rng &, I, I>)
            {
                RANGES_EXPECT(0 <= from && to.dist_ <= 0);
                RANGES_EXPECT(!sized_range<Rng> || from - to.dist_ <= distance(rng));
                ranges::actions::erase(rng, begin(rng), next(begin(rng), from));
                if(to.dist_ != 0)
                {
                    auto const last = next(begin(rng), end(rng));
                    ranges::actions::erase(rng, prev(last, -to.dist_), last);
                }
                return static_cast<Rng &&>(rng);
            }

            template<typename Rng, typename I = iterator_t<Rng>>
            auto operator()(Rng && rng, detail::from_end_<diff_t<Rng>> from,
                            detail::from_end_<diff_t<Rng>> to) const -> CPP_ret(Rng)( //
                requires bidirectional_range<Rng> && erasable_range<Rng &, I, I>)
            {
                RANGES_EXPECT(from.dist_ <= 0 && to.dist_ <= 0 && from.dist_ <= to.dist_);
                RANGES_EXPECT(!sized_range<Rng> || 0 <= distance(rng) + from.dist_);
                auto last = next(begin(rng), end(rng));
                ranges::actions::erase(rng, prev(last, -to.dist_), last);
                last = next(begin(rng), end(rng));
                ranges::actions::erase(
                    rng, begin(rng), prev(last, to.dist_ - from.dist_));
                return static_cast<Rng &&>(rng);
            }

            template<typename Rng, typename I = iterator_t<Rng>>
            auto operator()(Rng && rng, diff_t<Rng> from, end_fn const &) const
                -> CPP_ret(Rng)( //
                    requires forward_range<Rng> && erasable_range<Rng &, I, I>)
            {
                RANGES_EXPECT(0 <= from);
                RANGES_EXPECT(!sized_range<Rng> || from <= distance(rng));
                ranges::actions::erase(rng, begin(rng), next(begin(rng), from));
                return static_cast<Rng &&>(rng);
            }

            template<typename Rng, typename I = iterator_t<Rng>>
            auto operator()(Rng && rng, detail::from_end_<diff_t<Rng>> from,
                            end_fn const &) const -> CPP_ret(Rng)( //
                requires bidirectional_range<Rng> && erasable_range<Rng &, I, I>)
            {
                RANGES_EXPECT(from.dist_ <= 0);
                RANGES_EXPECT(!sized_range<Rng> || 0 <= distance(rng) + from.dist_);
                auto const last = next(begin(rng), end(rng));
                ranges::actions::erase(rng, begin(rng), prev(last, -from.dist_));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \ingroup group-actions
        /// \relates slice_fn
        /// \sa action
        RANGES_INLINE_VARIABLE(action<slice_fn>, slice)
    } // namespace actions
    /// @}
} // namespace ranges

#endif
