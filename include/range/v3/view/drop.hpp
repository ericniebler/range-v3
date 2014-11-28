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

#ifndef RANGES_V3_VIEW_DROP_HPP
#define RANGES_V3_VIEW_DROP_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_interface.hpp>
#include <range/v3/range.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct drop_view
          : range_interface<drop_view<Rng>, is_infinite<Rng>::value>
        {
        private:
            friend range_access;
            using base_range_t = view::all_t<Rng>;
            using difference_type_ = range_difference_t<Rng>;
            base_range_t rng_;
            difference_type_ n_;

        public:
            drop_view() = default;
            drop_view(Rng && rng, difference_type_ n)
              : rng_(view::all(std::forward<Rng>(rng))), n_(n)
            {
                RANGES_ASSERT(n >= 0);
            }
            range_iterator_t<Rng> begin()
            {
                return next(ranges::begin(rng_), n_);
            }
            range_sentinel_t<Rng> end()
            {
                return ranges::end(rng_);
            }
            CONCEPT_REQUIRES(Iterable<Rng const>())
            range_iterator_t<Rng const> begin() const
            {
                return next(ranges::begin(rng_), n_);
            }
            CONCEPT_REQUIRES(Iterable<Rng const>())
            range_sentinel_t<Rng const> end() const
            {
                return ranges::end(rng_);
            }
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return ranges::size(rng_) - static_cast<range_size_t<Rng>>(n_);
            }
            base_range_t & base()
            {
                return rng_;
            }
            base_range_t const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct drop_fn
            {
            private:
                template<typename Rng>
                static drop_view<Rng>
                invoke_(Rng && rng, range_difference_t<Rng> n, concepts::InputIterable*)
                {
                    return {std::forward<Rng>(rng), n};
                }
                template<typename Rng, CONCEPT_REQUIRES_(!Range<Rng>() && std::is_lvalue_reference<Rng>())>
                static range<range_iterator_t<Rng>, range_sentinel_t<Rng>>
                invoke_(Rng && rng, range_difference_t<Rng> n, concepts::RandomAccessIterable*)
                {
                    return {next(begin(rng), n), end(rng)};
                }
            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputIterable<Rng>())>
                auto operator()(Rng && rng, range_difference_t<Rng> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    drop_fn::invoke_(std::forward<Rng>(rng), n, iterable_concept<Rng>{})
                )
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                auto operator()(Int n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(*this, std::placeholders::_1, n))
                )
            };

            constexpr drop_fn drop {};
        }
    }
}

#endif
