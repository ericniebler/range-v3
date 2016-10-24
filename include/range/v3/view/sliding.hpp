/// \file
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

#ifndef RANGES_V3_VIEW_SLIDING_HPP
#define RANGES_V3_VIEW_SLIDING_HPP

#include <utility>
#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct sliding_view
          : view_adaptor<
                sliding_view<Rng>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            CONCEPT_ASSERT(ForwardRange<Rng>());
            range_difference_t<Rng> n_;
            friend range_access;
            struct adaptor;
            adaptor begin_adaptor() const
            {
                return adaptor{n_, ranges::end(this->base())};
            }
        public:
            sliding_view() = default;
            sliding_view(Rng rng, range_difference_t<Rng> n)
              : sliding_view::view_adaptor(std::move(rng)), n_(n)
            {
                RANGES_ASSERT(0 < n_);
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            range_size_t<Rng> size() const
            {
                auto sz = ranges::distance(this->base());
                return static_cast<range_size_t<Rng>>(sz);
            }
        };

        template<typename Rng>
        struct sliding_view<Rng>::adaptor
          : adaptor_base
        {
        private:
            range_difference_t<Rng> n_;
            range_sentinel_t<Rng> end_;
        public:
            adaptor() = default;
            adaptor(range_difference_t<Rng> n, range_sentinel_t<Rng> end)
              : n_(n), end_(end)
            {}
            auto get(range_iterator_t<Rng> it) const ->
                decltype(view::take(make_iterator_range(std::move(it), end_), n_))
            {
                RANGES_ASSERT(it != end_);
                return view::take(make_iterator_range(std::move(it), end_), n_);
            }
            void next(range_iterator_t<Rng> &it)
            {
                RANGES_ASSERT(it != end_);
                ++it;
            }
            CONCEPT_REQUIRES(BidirectionalRange<Rng>())
            void prev(range_iterator_t<Rng> &it)
            {
                --it;
            }
            CONCEPT_REQUIRES(
                SizedSentinel<range_iterator_t<Rng>, range_iterator_t<Rng>>())
            range_difference_t<Rng> distance_to(range_iterator_t<Rng> const &here,
                range_iterator_t<Rng> const &there, adaptor const &/*that*/) const
            {
                // This assertion is true for all range types except cyclic ranges:
                return there - here;
            }
            CONCEPT_REQUIRES(RandomAccessRange<Rng>())
            void advance(range_iterator_t<Rng> &it, range_difference_t<Rng> n)
            {
                //RANGES_ASSERT(end_ - it <= n);
                it += n;
            }
        };

        namespace view
        {
            // In:  Range<T>
            // Out: Range<Range<T>>, where each inner range has $n$ elements.
            //                       The last $n$/$s$ ranges may have fewer.
            struct sliding_fn
            {
            private:
                friend view_access;
                template<typename Int,
                    CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(sliding_fn sliding, Int n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(sliding, std::placeholders::_1, n))
                )
            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(ForwardRange<Rng>())>
                sliding_view<all_t<Rng>> operator()(Rng && rng, range_difference_t<Rng> n) const
                {
                    return {all(std::forward<Rng>(rng)), n};
                }

                // For the sake of better error messages:
            #ifndef RANGES_DOXYGEN_INVOKED
            private:
                template<typename Int,
                    CONCEPT_REQUIRES_(!Integral<Int>())>
                static detail::null_pipe bind(sliding_fn, Int)
                {
                    CONCEPT_ASSERT_MSG(Integral<Int>(),
                        "The object passed to view::sliding must be Integral");
                    return {};
                }
            public:
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!(ForwardRange<Rng>() && Integral<T>()))>
                void operator()(Rng &&, T) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "The first argument to view::sliding must be a model of the ForwardRange concept");
                    CONCEPT_ASSERT_MSG(Integral<T>(),
                        "The second argument to view::sliding must be a model of the Integral concept");
                }
            #endif
            };

            /// \relates sliding_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<sliding_fn>, sliding)
        }
        /// @}
    }
}

#endif
