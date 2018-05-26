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

#ifndef RANGES_V3_VIEW_CHUNK_HPP
#define RANGES_V3_VIEW_CHUNK_HPP

#include <functional>
#include <limits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, bool = (bool) ForwardRange<Rng>()>
        struct chunk_view
          : view_adaptor<
                chunk_view<Rng>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            friend range_access;
            CONCEPT_ASSERT(ForwardRange<Rng>());

#ifdef RANGES_WORKAROUND_MSVC_711347
            template<bool Const, typename I = iterator_t<meta::const_if_c<Const, Rng>>>
            static constexpr bool CanSizedSentinel = SizedSentinel<I, I>();
#else // ^^^ workaround / no workaround vvv
            template<bool Const>
            static constexpr bool CanSizedSentinel() noexcept
            {
                using I = iterator_t<meta::const_if_c<Const, Rng>>;
                return (bool) SizedSentinel<I, I>();
            }
#endif // RANGES_WORKAROUND_MSVC_711347

            template<bool Const>
            using offset_t =
                meta::if_c<
                    BidirectionalRange<meta::const_if_c<Const, Rng>>() ||
#ifdef RANGES_WORKAROUND_MSVC_711347
                        chunk_view::CanSizedSentinel<Const>,
#else // ^^^ workaround / no workaround vvv
                        chunk_view::CanSizedSentinel<Const>(),
#endif // RANGES_WORKAROUND_MSVC_711347
                    range_difference_type_t<Rng>,
                    constant<range_difference_type_t<Rng>, 0>>;

            range_difference_type_t<Rng> n_ = 0;

#ifdef RANGES_WORKAROUND_MSVC_711347
            template<bool Const, bool CanSized = CanSizedSentinel<Const>>
#else // ^^^ workaround / no workaround vvv
            template<bool Const>
#endif // RANGES_WORKAROUND_MSVC_711347
            struct RANGES_EMPTY_BASES adaptor
              : adaptor_base
              , private box<offset_t<Const>>
            {
            private:
#ifdef RANGES_WORKAROUND_MSVC_711347
                template <bool, bool>
                friend struct adaptor;
#else // ^^^ workaround / no workaround vvv
                friend struct adaptor<!Const>;
#endif // RANGES_WORKAROUND_MSVC_711347
                using CRng = meta::const_if_c<Const, Rng>;

                range_difference_type_t<CRng> n_;
                sentinel_t<CRng> end_;

                RANGES_CXX14_CONSTEXPR
                offset_t<Const> const &offset() const
                {
                    offset_t<Const> const &result = this->box<offset_t<Const>>::get();
                    RANGES_EXPECT(0 <= result && result < n_);
                    return result;
                }
                RANGES_CXX14_CONSTEXPR
                offset_t<Const> &offset()
                {
                    return const_cast<offset_t<Const> &>(
                        const_cast<adaptor const &>(*this).offset());
                }
            public:
                adaptor() = default;
                constexpr adaptor(meta::const_if_c<Const, chunk_view> &cv)
                  : box<offset_t<Const>>{0}
                  , n_((RANGES_EXPECT(0 < cv.n_), cv.n_))
                  , end_(ranges::end(cv.base()))
                {}
                template<bool Other,
                    CONCEPT_REQUIRES_(Const && !Other)>
                constexpr adaptor(adaptor<Other> that)
                  : box<offset_t<Const>>(that.offset())
                  , n_(that.n_)
                  , end_(that.end_)
                {}
                RANGES_CXX14_CONSTEXPR
                auto read(iterator_t<CRng> const &it) const ->
                    decltype(view::take(make_iterator_range(it, end_), n_))
                {
                    RANGES_EXPECT(it != end_);
                    RANGES_EXPECT(0 == offset());
                    return view::take(make_iterator_range(it, end_), n_);
                }
                RANGES_CXX14_CONSTEXPR
                void next(iterator_t<CRng> &it)
                {
                    RANGES_EXPECT(it != end_);
                    RANGES_EXPECT(0 == offset());
                    offset() = ranges::advance(it, n_, end_);
                }
                CONCEPT_REQUIRES(BidirectionalRange<CRng>())
                RANGES_CXX14_CONSTEXPR
                void prev(iterator_t<CRng> &it)
                {
                    ranges::advance(it, -n_ + offset());
                    offset() = 0;
                }

#ifdef RANGES_WORKAROUND_MSVC_711347
                CONCEPT_REQUIRES(CanSized)
#else // ^^^ workaround / no workaround vvv
                CONCEPT_REQUIRES(CanSizedSentinel<Const>())
#endif // RANGES_WORKAROUND_MSVC_711347
                RANGES_CXX14_CONSTEXPR
                range_difference_type_t<Rng> distance_to(iterator_t<CRng> const &here,
                    iterator_t<CRng> const &there, adaptor const &that) const
                {
                    auto const delta = (there - here) + (that.offset() - offset());
                    // This can fail for cyclic base ranges when the chunk size does not divide the
                    // cycle length. Such iterator pairs are NOT in the domain of -.
                    RANGES_ENSURE(0 == delta % n_);
                    return delta / n_;
                }
                CONCEPT_REQUIRES(RandomAccessRange<CRng>())
                RANGES_CXX14_CONSTEXPR
                void advance(iterator_t<CRng> &it, range_difference_type_t<Rng> n)
                {
                    using Limits = std::numeric_limits<range_difference_type_t<Rng>>;
                    if(0 < n)
                    {
                        RANGES_EXPECT(0 == offset());
                        RANGES_EXPECT(n <= Limits::max() / n_);
                        auto const remainder = ranges::advance(it, n * n_, end_) % n_;
                        RANGES_EXPECT(0 <= remainder && remainder < n_);
                        offset() = remainder;
                    }
                    else if(0 > n)
                    {
                        RANGES_EXPECT(n >= Limits::min() / n_);
                        ranges::advance(it, n * n_ + offset());
                        offset() = 0;
                    }
                }
            };

            RANGES_CXX14_CONSTEXPR
            adaptor<simple_view<Rng>()> begin_adaptor()
            {
                return adaptor<simple_view<Rng>()>{*this};
            }
#ifdef RANGES_WORKAROUND_MSVC_711347
            template<bool BB = true, CONCEPT_REQUIRES_(ForwardRange<Rng const>())>
            constexpr adaptor<BB> begin_adaptor() const
#else // ^^^ workaround / no workaround vvv
            CONCEPT_REQUIRES(ForwardRange<Rng const>())
            constexpr adaptor<true> begin_adaptor() const
#endif // RANGES_WORKAROUND_MSVC_711347
            {
                return adaptor<true>{*this};
            }
            RANGES_CXX14_CONSTEXPR
            range_size_type_t<Rng> size_(range_difference_type_t<Rng> base_size) const
            {
                CONCEPT_ASSERT(SizedRange<Rng const>());
                base_size = base_size / n_ + (0 != (base_size % n_));
                return static_cast<range_size_type_t<Rng>>(base_size);
            }
        public:
            chunk_view() = default;
            constexpr chunk_view(Rng rng, range_difference_type_t<Rng> n)
              : chunk_view::view_adaptor(detail::move(rng))
              , n_((RANGES_EXPECT(0 < n), n))
            {}
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            RANGES_CXX14_CONSTEXPR
            range_size_type_t<Rng> size() const
            {
                return size_(ranges::distance(this->base()));
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            RANGES_CXX14_CONSTEXPR
            range_size_type_t<Rng> size()
            {
                return size_(ranges::distance(this->base()));
            }
        };

        template<typename Rng>
        struct chunk_view<Rng, false>
          : view_facade<
                chunk_view<Rng, false>,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            friend range_access;
            CONCEPT_ASSERT(InputRange<Rng>() && !ForwardRange<Rng>());

            using iter_cache_t = detail::non_propagating_cache<iterator_t<Rng>>;

            mutable compressed_tuple<
                Rng,                          // base
                range_difference_type_t<Rng>, // n
                range_difference_type_t<Rng>, // remainder
                iter_cache_t                  // it
            > data_{};

            RANGES_CXX14_CONSTEXPR Rng &base() noexcept
            {
                return ranges::get<0>(data_);
            }
            constexpr Rng const &base() const noexcept
            {
                return ranges::get<0>(data_);
            }
            RANGES_CXX14_CONSTEXPR range_difference_type_t<Rng> &n() noexcept
            {
                return ranges::get<1>(data_);
            }
            constexpr range_difference_type_t<Rng> const &n() const noexcept
            {
                return ranges::get<1>(data_);
            }

            RANGES_CXX14_CONSTEXPR range_difference_type_t<Rng> &remainder() noexcept
            {
                return ranges::get<2>(data_);
            }
            constexpr range_difference_type_t<Rng> const &remainder() const noexcept
            {
                return ranges::get<2>(data_);
            }

            constexpr iter_cache_t &it_cache() const noexcept
            {
                return ranges::get<3>(data_);
            }
            RANGES_CXX14_CONSTEXPR iterator_t<Rng> &it() noexcept
            {
                return *it_cache();
            }
            constexpr iterator_t<Rng> const &it() const noexcept
            {
                return *it_cache();
            }

            struct outer_cursor
            {
            private:
                struct inner_view
                  : view_facade<inner_view, finite>
                {
                private:
                    friend range_access;

                    using value_type = range_value_type_t<Rng>;

                    chunk_view *rng_ = nullptr;

                    RANGES_CXX14_CONSTEXPR
                    bool done() const noexcept
                    {
                        RANGES_EXPECT(rng_);
                        return rng_->remainder() == 0;
                    }
                    RANGES_CXX14_CONSTEXPR
                    bool equal(default_sentinel) const noexcept
                    {
                        return done();
                    }
                    RANGES_CXX14_CONSTEXPR
                    reference_t<iterator_t<Rng>> read() const
                    {
                        RANGES_EXPECT(!done());
                        return *rng_->it();
                    }
                    RANGES_CXX14_CONSTEXPR
                    rvalue_reference_t<iterator_t<Rng>> move() const
                    {
                        RANGES_EXPECT(!done());
                        return ranges::iter_move(rng_->it());
                    }
                    RANGES_CXX14_CONSTEXPR
                    void next()
                    {
                        RANGES_EXPECT(!done());
                        ++rng_->it();
                        --rng_->remainder();
                        if(rng_->remainder() != 0 && rng_->it() == ranges::end(rng_->base()))
                            rng_->remainder() = 0;
                    }
                    CONCEPT_REQUIRES(SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>())
                    RANGES_CXX14_CONSTEXPR
                    range_difference_type_t<Rng> distance_to(default_sentinel) const
                    {
                        RANGES_EXPECT(rng_);
                        auto const d = ranges::end(rng_->base()) - rng_->it();
                        return ranges::min(d, rng_->remainder());
                    }
                public:
                    inner_view() = default;
                    constexpr explicit inner_view(chunk_view &view) noexcept
                      : rng_{&view}
                    {}
                    CONCEPT_REQUIRES(SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>())
                    RANGES_CXX14_CONSTEXPR
                    range_size_type_t<Rng> size()
                    {
                        auto const d = distance_to(default_sentinel{});
                        return static_cast<range_size_type_t<Rng>>(d);
                    }
                };

                chunk_view *rng_ = nullptr;

            public:
                using value_type = inner_view;

                outer_cursor() = default;
                constexpr explicit outer_cursor(chunk_view &view) noexcept
                  : rng_{&view}
                {}
                RANGES_CXX14_CONSTEXPR
                inner_view read() const
                {
                    RANGES_EXPECT(!done());
                    return inner_view{*rng_};
                }
                RANGES_CXX14_CONSTEXPR
                bool done() const
                {
                    RANGES_EXPECT(rng_);
                    return rng_->it() == ranges::end(rng_->base()) && rng_->remainder() != 0;
                }
                RANGES_CXX14_CONSTEXPR
                bool equal(default_sentinel) const
                {
                    return done();
                }
                RANGES_CXX14_CONSTEXPR
                void next()
                {
                    RANGES_EXPECT(!done());
                    ranges::advance(rng_->it(), rng_->remainder(), ranges::end(rng_->base()));
                    rng_->remainder() = rng_->n();
                }
                CONCEPT_REQUIRES(SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>())
                RANGES_CXX14_CONSTEXPR
                range_difference_type_t<Rng> distance_to(default_sentinel) const
                {
                    RANGES_EXPECT(rng_);
                    auto d = ranges::end(rng_->base()) - rng_->it();
                    if(d < rng_->remainder())
                        return 1;

                    d -= rng_->remainder();
                    d = (d + rng_->n() - 1) / rng_->n();
                    d += (rng_->remainder() != 0);
                    return d;
                }
            };

            RANGES_CXX14_CONSTEXPR
            outer_cursor begin_cursor() noexcept
            {
                it_cache() = ranges::begin(base());
                return outer_cursor{*this};
            }
            RANGES_CXX14_CONSTEXPR
            range_size_type_t<Rng> size_(range_difference_type_t<Rng> base_size) const
            {
                CONCEPT_ASSERT(SizedRange<Rng>());
                auto const n = this->n();
                base_size = base_size / n + (0 != base_size % n);
                return static_cast<range_size_type_t<Rng>>(base_size);
            }
        public:
            chunk_view() = default;
            RANGES_CXX14_CONSTEXPR
            chunk_view(Rng rng, range_difference_type_t<Rng> n)
              : data_{detail::move(rng), (RANGES_EXPECT(0 < n), n), n, nullopt}
            {}
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            RANGES_CXX14_CONSTEXPR
            range_size_type_t<Rng> size() const
            {
                return size_(ranges::distance(base()));
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            RANGES_CXX14_CONSTEXPR
            range_size_type_t<Rng> size()
            {
                return size_(ranges::distance(base()));
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
            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputRange<Rng>())>
                chunk_view<all_t<Rng>> operator()(Rng &&rng, range_difference_type_t<Rng> n) const
                {
                    return {all(static_cast<Rng &&>(rng)), n};
                }

                // For the sake of better error messages:
            #ifndef RANGES_DOXYGEN_INVOKED
            private:
                template<typename Int,
                    CONCEPT_REQUIRES_(!Integral<Int>())>
                static detail::null_pipe bind(chunk_fn, Int)
                {
                    CONCEPT_ASSERT_MSG(Integral<Int>(),
                        "The object passed to view::chunk must be Integral");
                    return {};
                }
            public:
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!(InputRange<Rng>() && Integral<T>()))>
                void operator()(Rng &&, T) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument to view::chunk must satisfy the InputRange concept");
                    CONCEPT_ASSERT_MSG(Integral<T>(),
                        "The second argument to view::chunk must satisfy the Integral concept");
                }
            #endif
            };

            /// \relates chunk_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<chunk_fn>, chunk)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::chunk_view)

#endif
