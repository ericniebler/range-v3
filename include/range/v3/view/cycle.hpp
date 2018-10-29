/// \file cycle.hpp
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2015
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_CYCLE_HPP
#define RANGES_V3_VIEW_CYCLE_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        ///@{
        template<typename Rng, bool /* = (bool) is_infinite<Rng>() */>
        struct RANGES_EMPTY_BASES cycled_view
          : view_facade<cycled_view<Rng>, infinite>
          , private detail::non_propagating_cache<
                iterator_t<Rng>, cycled_view<Rng>, !BoundedRange<Rng>()>
        {
        private:
            CONCEPT_ASSERT(ForwardRange<Rng>() && !is_infinite<Rng>::value);
            friend range_access;
            Rng rng_;

            using cache_t = detail::non_propagating_cache<
                iterator_t<Rng>, cycled_view<Rng>, !BoundedRange<Rng>()>;

            template<bool IsConst>
            struct cursor
            {
            private:
                friend struct cursor<!IsConst>;
                template<typename T>
                using constify_if = meta::const_if_c<IsConst, T>;
                using cycled_view_t = constify_if<cycled_view>;
                using CRng = constify_if<Rng>;
                using iterator = iterator_t<CRng>;

                cycled_view_t *rng_{};
                iterator it_{};
                std::intmax_t n_ = 0;

                iterator get_end_(std::true_type, bool = false) const
                {
                    return ranges::end(rng_->rng_);
                }
                template<bool CanBeEmpty = false>
                iterator get_end_(std::false_type, meta::bool_<CanBeEmpty> = {}) const
                {
                    auto &end_ = static_cast<cache_t &>(*rng_);
                    RANGES_EXPECT(CanBeEmpty || end_);
                    if(CanBeEmpty && !end_)
                        end_ = ranges::next(it_, ranges::end(rng_->rng_));
                    return *end_;
                }
                void set_end_(std::true_type) const
                {}
                void set_end_(std::false_type) const
                {
                    auto &end_ = static_cast<cache_t &>(*rng_);
                    if(!end_)
                        end_ = it_;
                }
            public:
                cursor() = default;
                cursor(cycled_view_t &rng)
                  : rng_(&rng), it_(ranges::begin(rng.rng_))
                {}
                template<bool Other,
                    CONCEPT_REQUIRES_(IsConst && !Other)>
                cursor(cursor<Other> that)
                  : rng_(that.rng_)
                  , it_(std::move(that.it_))
                {}
                constexpr bool equal(default_sentinel) const
                {
                    return false;
                }
                auto read() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    *it_
                )
                bool equal(cursor const &pos) const
                {
                    RANGES_EXPECT(rng_ == pos.rng_);
                    return n_ == pos.n_ && it_ == pos.it_;
                }
                void next()
                {
                    auto const end = ranges::end(rng_->rng_);
                    RANGES_EXPECT(it_ != end);
                    if(++it_ == end)
                    {
                        ++n_;
                        this->set_end_(BoundedRange<CRng>());
                        it_ = ranges::begin(rng_->rng_);
                    }
                }
                CONCEPT_REQUIRES(BidirectionalRange<CRng>())
                void prev()
                {
                    if(it_ == ranges::begin(rng_->rng_))
                    {
                        RANGES_EXPECT(n_ > 0); // decrementing the begin iterator?!
                        --n_;
                        it_ = this->get_end_(BoundedRange<CRng>());
                    }
                    --it_;
                }
                CONCEPT_REQUIRES(RandomAccessRange<CRng>())
                void advance(std::intmax_t n)
                {
                    auto const begin = ranges::begin(rng_->rng_);
                    auto const end = this->get_end_(BoundedRange<CRng>(), meta::bool_<true>());
                    auto const dist = end - begin;
                    auto const d = it_ - begin;
                    auto const off = (d + n) % dist;
                    n_ += (d + n) / dist;
                    RANGES_EXPECT(n_ >= 0);
                    using D = range_difference_type_t<Rng>;
                    it_ = begin + static_cast<D>(off < 0 ? off + dist : off);
                }
                CONCEPT_REQUIRES(SizedSentinel<iterator, iterator>())
                std::intmax_t distance_to(cursor const &that) const
                {
                    RANGES_EXPECT(that.rng_ == rng_);
                    auto const begin = ranges::begin(rng_->rng_);
                    auto const end = this->get_end_(BoundedRange<Rng>(), meta::bool_<true>());
                    auto const dist = end - begin;
                    return (that.n_ - n_) * dist + (that.it_ - it_);
                }
            };

            CONCEPT_REQUIRES(!simple_view<Rng>() || !BoundedRange<Rng const>())
            cursor<false> begin_cursor()
            {
                return {*this};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng const>())
            cursor<true> begin_cursor() const
            {
                return {*this};
            }

        public:
            cycled_view() = default;
            /// \pre <tt>!empty(rng)</tt>
            explicit cycled_view(Rng rng)
              : rng_(std::move(rng))
            {
                RANGES_EXPECT(!ranges::empty(rng_));
            }
        };

        template<typename Rng>
        struct cycled_view<Rng, true>
          : identity_adaptor<Rng>
        {
            CONCEPT_ASSERT(is_infinite<Rng>());
            using identity_adaptor<Rng>::identity_adaptor;
        };

        namespace view
        {
            /// Returns an infinite range that endlessly repeats the source
            /// range.
            struct cycle_fn
            {
                /// \pre <tt>!empty(rng)</tt>
                template<typename Rng, CONCEPT_REQUIRES_(ForwardRange<Rng>())>
                cycled_view<all_t<Rng>> operator()(Rng &&rng) const
                {
                    return cycled_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
                }

#ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, CONCEPT_REQUIRES_(!ForwardRange<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "The object on which view::cycle operates must model "
                        "the ForwardRange concept.");
                }
#endif
            };

            /// \relates cycle_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<cycle_fn>, cycle)
       } // namespace view
       /// @}
    } // namespace v3
} // namespace ranges

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::cycled_view)

#endif
