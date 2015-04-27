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

#ifndef RANGES_V3_VIEW_CHUNK_HPP
#define RANGES_V3_VIEW_CHUNK_HPP

#include <atomic>
#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>
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
        struct chunk_view
          : range_adaptor<chunk_view<Rng>, Rng>
        {
        private:
            CONCEPT_ASSERT(ForwardIterable<Rng>());
            using offset_t =
                meta::if_<
                    BidirectionalIterable<Rng>,
                    mutable_<std::atomic<range_difference_t<Rng>>>,
                    constant<range_difference_t<Rng>, 0>>;
            range_difference_t<Rng> n_;
            friend range_access;
            struct adaptor;
            adaptor begin_adaptor() const
            {
                return adaptor{n_, ranges::end(this->base())};
            }
        public:
            chunk_view() = default;
            chunk_view(Rng rng, range_difference_t<Rng> n)
              : range_adaptor_t<chunk_view>(std::move(rng)), n_(n)
            {
                RANGES_ASSERT(0 < n_);
            }
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                auto sz = ranges::distance(this->base());
                return sz / n_ + (0 != sz % n_);
            }
        };

        template<typename Rng>
        struct chunk_view<Rng>::adaptor
          : adaptor_base, offset_t
        {
        private:
            range_difference_t<Rng> n_;
            range_sentinel_t<Rng> end_;
            offset_t & offset() {return *this;}
            offset_t const & offset() const {return *this;}
        public:
            adaptor() = default;
            adaptor(range_difference_t<Rng> n, range_sentinel_t<Rng> end)
              : offset_t{0}, n_(n), end_(end)
            {}
            auto current(range_iterator_t<Rng> it) const
            RANGES_DECLTYPE_AUTO_RETURN(
                view::take(make_range(std::move(it), end_), n_)
            )
            void next(range_iterator_t<Rng> &it)
            {
                offset() = ranges::advance(it, n_, end_);
            }
            CONCEPT_REQUIRES(BidirectionalIterable<Rng>())
            void prev(range_iterator_t<Rng> &it)
            {
                ranges::advance(it, -n_ + offset());
                offset() = 0;
            }
            CONCEPT_REQUIRES(RandomAccessIterable<Rng>())
            range_difference_t<Rng> distance_to(range_iterator_t<Rng> const &here,
                range_iterator_t<Rng> const &there, adaptor const &that) const
            {
                RANGES_ASSERT(0 == ((there - here) + that.offset() - offset()) % n_);
                return ((there - here) + that.offset() - offset()) / n_;
            }
            CONCEPT_REQUIRES(RandomAccessIterable<Rng>())
            void advance(range_iterator_t<Rng> &it, range_difference_t<Rng> n)
            {
                if(0 < n)
                    offset() = ranges::advance(it, n * n_ + offset(), end_);
                else if(0 > n)
                    offset() = (ranges::advance(it, n * n_ + offset()), 0);
            }
        };

        namespace view
        {
            // In:  Range<T>
            // Out: Range<Range<T>>, where each inner range has $n$ elements.
            //                       The last range may have fewer.
            struct chunk_fn
            {
            private:
                friend view_access;
                template<typename Int,
                    CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(chunk_fn chunk, Int n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(chunk, std::placeholders::_1, n))
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Int,
                    CONCEPT_REQUIRES_(!Integral<Int>())>
                static detail::null_pipe bind(chunk_fn, Int)
                {
                    CONCEPT_ASSERT_MSG(Integral<Int>(),
                        "The object passed to view::chunk must be Integral");
                    return {};
                }
            #endif
            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(ForwardIterable<Rng>())>
                chunk_view<all_t<Rng>> operator()(Rng && rng, range_difference_t<Rng> n) const
                {
                    return {all(std::forward<Rng>(rng)), n};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!(ForwardIterable<Rng>() && Integral<T>()))>
                void operator()(Rng &&, T) const
                {
                    CONCEPT_ASSERT_MSG(ForwardIterable<Rng>(),
                        "The first argument to view::chunk must be a model of the ForwardIterable concept");
                    CONCEPT_ASSERT_MSG(Integral<T>(),
                        "The second argument to view::chunk must be a model of the Integral concept");
                }
            #endif
            };

            namespace
            {
                constexpr auto &&chunk = static_const<view<chunk_fn>>::value;
            }
        }
        /// @}
    }
}

#endif
