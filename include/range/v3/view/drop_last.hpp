/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_DROP_LAST_HPP
#define RANGES_V3_VIEW_DROP_LAST_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/iterator/counted_iterator.hpp>
#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/interface.hpp>
#include <range/v3/view/adaptor.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        namespace drop_last_view
        {
            template<typename Rng>
            range_size_t<Rng> get_size(Rng &rng, range_difference_t<Rng> n_)
            {
                RANGES_EXPECT(n_ >= 0);
                range_size_t<Rng> const initial_size = ranges::size(rng);
                range_size_t<Rng> const n = static_cast<range_size_t<Rng>>(n_);
                RANGES_EXPECT(initial_size >= n);
                return initial_size > n ? initial_size - n : 0;
            }

            template<typename Rng>
            auto get_end(Rng &rng, range_difference_t<Rng> n, int) ->
                CPP_ret(iterator_t<Rng>)(
                    requires RandomAccessRange<Rng> && SizedRange<Rng>)
            {
                return begin(rng) +
                    static_cast<range_difference_t<Rng>>(drop_last_view::get_size(rng, n));
            }
            template<typename Rng>
            auto get_end(Rng &rng, range_difference_t<Rng> n, long) ->
                CPP_ret(iterator_t<Rng>)(
                    requires BidirectionalRange<Rng> && CommonRange<Rng>)
            {
                return prev(end(rng), n, begin(rng));
            }

            enum class mode_enum { bidi, forward, sized, invalid };

            template<mode_enum Mode>
            using mode_t = std::integral_constant<mode_enum, Mode>;

            using mode_bidi = mode_t<mode_enum::bidi>;
            using mode_forward = mode_t<mode_enum::forward>;
            using mode_sized = mode_t<mode_enum::sized>;
            using mode_invalid = mode_t<mode_enum::invalid>;

            template<typename Rng>
            constexpr mode_enum get_mode() noexcept
            {
                // keep range bound
                // Sized Bidi O(N)
                return (RandomAccessView<Rng> && SizedView<Rng>) ||
                    (BidirectionalView<Rng> && CommonView<Rng>)
                  ? mode_enum::bidi
                  : SizedView<Rng>
                      ? mode_enum::sized
                      : ForwardView<Rng> ? mode_enum::forward : mode_enum::invalid;

                // max performance
                // Sized Bidi O(1)
                // Sized Bidi use mode::sized instead of mode::bidi - thus become unbound.
                /*return (RandomAccessView<Rng> && SizedView<Rng>) ||
                        (BidirectionalView<Rng> && CommonView<Rng>)
                        ? mode::bidi
                        : SizedView<Rng>
                            ? mode::sized
                            : BidirectionalView<Rng> && CommonView<Rng>
                            ? mode::bidi
                            : ForwardView<Rng>
                                ? mode::forward
                                : mode::invalid;*/
            }

            template<typename Rng>
            using mode_of = mode_t<drop_last_view::get_mode<Rng>()>;
        }  // namespace drop_last_view
    } // namespace detail
    /// \endcond

    template<typename Rng, typename = detail::drop_last_view::mode_of<Rng>>
    struct drop_last_view
    {};

    template<typename Rng>
    struct drop_last_view<Rng, detail::drop_last_view::mode_bidi>
      : view_interface<
            drop_last_view<Rng, detail::drop_last_view::mode_bidi>,
            is_finite<Rng>::value ? finite : range_cardinality<Rng>::value> // finite at best
    {
        CPP_assert(
            (RandomAccessView<Rng> && SizedView<Rng>) ||
            (BidirectionalView<Rng> && CommonView<Rng>));
    private:
        friend range_access;
        using difference_t = range_difference_t<Rng>;

        Rng rng_;
        difference_t n_;
        detail::non_propagating_cache<iterator_t<Rng>> end_;

    public:
        drop_last_view() = default;
        drop_last_view(Rng rng, difference_t n)
          : rng_(std::move(rng)), n_(n)
        {
            RANGES_EXPECT(n >= 0);
        }

        iterator_t<Rng> begin()
        {
            return ranges::begin(rng_);
        }
        sentinel_t<Rng> end()
        {
            if(!end_)
                end_ = detail::drop_last_view::get_end(rng_, n_, 0);
            return *end_;
        }
        template<typename CRng = Rng const>
        auto begin() const ->
            CPP_ret(iterator_t<CRng>)(
                requires RandomAccessRange<CRng> && SizedRange<CRng>)
        {
            return ranges::begin(rng_);
        }
        template<typename CRng = Rng const>
        auto end() const ->
            CPP_ret(iterator_t<CRng>)(
                requires RandomAccessRange<CRng> && SizedRange<CRng>)
        {
            return detail::drop_last_view::get_end(rng_, n_, 0);
        }

        auto CPP_fun(size)()(
            requires SizedRange<Rng>)
        {
            return detail::drop_last_view::get_size(rng_, n_);
        }
        CPP_member
        auto CPP_fun(size)() (const
            requires SizedRange<Rng const>)
        {
            return detail::drop_last_view::get_size(rng_, n_);
        }

        Rng &base()
        {
            return rng_;
        }
        Rng const &base() const
        {
            return rng_;
        }
    };

    template<typename Rng>
    struct drop_last_view<Rng, detail::drop_last_view::mode_forward>
      : view_adaptor<
            drop_last_view<Rng, detail::drop_last_view::mode_forward>,
            Rng,
            is_finite<Rng>::value ? finite : range_cardinality<Rng>::value> // finite at best (but unknown is expected)
    {
        CPP_assert(ForwardView<Rng>);
    private:
        friend range_access;

        using difference_t = range_difference_t<Rng>;
        difference_t n_;
        detail::non_propagating_cache<iterator_t<Rng>> probe_begin;

        struct adaptor
          : adaptor_base
        {
            iterator_t<Rng> probe_;

            adaptor() = default;
            adaptor(iterator_t<Rng> probe_begin)
              : probe_(std::move(probe_begin))
            {}
            void next(iterator_t<Rng> &it)
            {
                ++it;
                ++probe_;
            }
        };

        struct sentinel_adaptor
          : adaptor_base
        {
            template<typename I, typename S>
            bool empty(I const &, adaptor const &ia, S const &s) const
            {
                return ia.probe_ == s;
            }
        };

        adaptor begin_adaptor()
        {
            if (!probe_begin)
                probe_begin = next(begin(this->base()), n_, end(this->base()));
            return {*probe_begin};
        }
        sentinel_adaptor end_adaptor()
        {
            return {};
        }

    public:
        drop_last_view() = default;
        drop_last_view(Rng rng, difference_t n)
          : drop_last_view::view_adaptor(std::move(rng))
          , n_(n)
        {
            RANGES_EXPECT(n >= 0);
        }

        CPP_member
        auto CPP_fun(size)()(
            requires SizedRange<Rng>)
        {
            return detail::drop_last_view::get_size(this->base(), n_);
        }
        CPP_member
        auto CPP_fun(size)()(const
            requires SizedRange<Rng const>)
        {
            return detail::drop_last_view::get_size(this->base(), n_);
        }
    };

    template<typename Rng>
    struct drop_last_view<Rng, detail::drop_last_view::mode_sized>
      : view_interface<
            drop_last_view<Rng, detail::drop_last_view::mode_sized>,
            finite>
    {
        CPP_assert(SizedView<Rng>);
    private:
        friend range_access;

        using difference_t = range_difference_t<Rng>;
        Rng rng_;
        difference_t n_;

    public:
        drop_last_view() = default;
        drop_last_view(Rng rng, difference_t n)
          : rng_(std::move(rng)), n_(n)
        {
            RANGES_EXPECT(n >= 0);
        }

        counted_iterator<iterator_t<Rng>> begin()
        {
            return {ranges::begin(rng_), static_cast<difference_t>(size())};
        }
        template<typename CRng = Rng const>
        auto begin() const ->
            CPP_ret(counted_iterator<iterator_t<CRng>>)(
                requires SizedRange<CRng>)
        {
            return {ranges::begin(rng_), static_cast<difference_t>(size())};
        }
        default_sentinel_t end() const
        {
            return {};
        }
        range_size_t<Rng> size()
        {
            return detail::drop_last_view::get_size(this->base(), n_);
        }
        CPP_member
        auto CPP_fun(size)() (const
            requires SizedRange<Rng const>)
        {
            return detail::drop_last_view::get_size(this->base(), n_);
        }

        Rng &base()
        {
            return rng_;
        }
        Rng const &base() const
        {
            return rng_;
        }
    };

    namespace view
    {
        struct drop_last_fn
        {
        private:
            friend view_access;

            template<typename Int>
            static auto CPP_fun(bind)(drop_last_fn drop_last, Int n)(
                requires Integral<Int>)
            {
                return make_pipeable(std::bind(drop_last, std::placeholders::_1, n));
            }
        public:
            template<typename Rng>
            auto operator()(Rng &&rng, range_difference_t<Rng> n) const ->
                CPP_ret(drop_last_view<all_t<Rng>>)(
                    requires SizedRange<Rng> || ForwardRange<Rng>)
            {
                return {all(static_cast<Rng&&>(rng)), n};
            }
        };

        RANGES_INLINE_VARIABLE(view<drop_last_fn>, drop_last)
    }
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::drop_last_view)

#endif
