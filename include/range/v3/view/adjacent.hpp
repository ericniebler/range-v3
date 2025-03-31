/// \file
// Range v3 library
//
//  Copyright Alex Mills 2023
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_ADJACENT_HPP
#define RANGES_V3_VIEW_ADJACENT_HPP

#include <array>
#include <type_traits>
#include <tuple>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/sliding.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/zip.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace adjacent_view_detail
    {
        using sliding_view_detail::cache;
        using sliding_view_detail::caching;

        template<typename Rng>
        using trailing = sliding_view_detail::trailing<Rng>;
    } // namespace adjacent_view_detail
    /// \endcond

    template(typename Rng, detail::integer_like_ auto N, //
        adjacent_view_detail::cache = adjacent_view_detail::caching<Rng>::value)(
            requires viewable_range<Rng> AND (N > 0))
    struct adjacent_view;

    /// \cond
    namespace adjacent_view_detail
    {
        template<std::size_t N>
        using expansion_t = //
            decltype(std::make_index_sequence<N>{});

        template<typename T, auto>
        using identity_t = T;

        template<typename T, std::size_t... Ns>
        using tuple_fwd_t = //
                std::tuple<identity_t<T, Ns>...>;

        template<typename Rng, std::size_t N, bool = random_access_range<Rng>, typename = expansion_t<N>>
        struct adjacent_tie_;

        template<typename Rng, std::size_t N, std::size_t... Ns>
        struct adjacent_tie_<Rng, N, true, std::index_sequence<Ns...>>
        {
        private:
            using element_type_ = decltype(*std::declval<const iterator_t<Rng>&>());

        public:
            adjacent_tie_() = default;

            auto operator()(auto&& rng) const
                -> tuple_fwd_t<element_type_, Ns...>
            {
                auto counted = views::counted(rng, static_cast<iter_difference_t<decltype(rng)>>(N));
                return tuple_fwd_t<element_type_, Ns...>{counted[Ns]...};
            }
        };

        template<typename Rng, std::size_t N, std::size_t... Ns>
        struct adjacent_tie_<Rng, N, false, std::index_sequence<Ns...>>
        {
        private:
            using element_type_ = decltype(*std::declval<const iterator_t<Rng>&>());
            mutable std::array<std::remove_reference_t<element_type_>*, N> iter_storage_;

        public:
            adjacent_tie_()
              : iter_storage_{}
            {}
            auto operator()(auto&& rng) const
                -> tuple_fwd_t<element_type_, Ns...>
            {
                auto counted = views::counted(rng, static_cast<iter_difference_t<decltype(rng)>>(N));
                for(auto&& [i, c] : views::zip(iter_storage_, counted)) i = &c;
                return tuple_fwd_t<element_type_, Ns...>{*iter_storage_[Ns]...};
            }
        };

        template<typename Rng, std::size_t N>
        struct RANGES_EMPTY_BASES av_base
          : view_adaptor<adjacent_view<Rng, N>, Rng,
                         is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
          , private detail::non_propagating_cache<iterator_t<Rng>, av_base<Rng, N>,
                                                  !random_access_range<Rng>>
        {
            CPP_assert(forward_range<Rng>);
            av_base() = default;
            av_base(Rng rng, std::integral_constant<decltype(N), N>)
              : av_base::view_adaptor(std::move(rng))
            {
                CPP_assert(0 < N);
            }
            CPP_auto_member
            auto CPP_fun(size)()(const //
                requires sized_range<Rng const>)
            {
                auto const count = ranges::size(this->base());
                auto constexpr n = static_cast<range_size_t<Rng const>>(n_);
                return count < n ? 0 : count - n + 1;
            }
            CPP_auto_member
            auto CPP_fun(size)()(
                requires sized_range<Rng>)
            {
                auto const count = ranges::size(this->base());
                auto constexpr n = static_cast<range_size_t<Rng>>(n_);
                return count < n ? 0 : count - n + 1;
            }

        protected:
            using difference_type_ = range_difference_t<Rng>;
            static constexpr auto n_ = static_cast<difference_type_>(N);

            optional<iterator_t<Rng>> & cache() &
            {
                return static_cast<cache_t &>(*this);
            }
            optional<iterator_t<Rng>> const & cache() const &
            {
                return static_cast<cache_t const &>(*this);
            }

        private:
            using cache_t = detail::non_propagating_cache<iterator_t<Rng>, av_base<Rng, N>>;
        };
    } // namespace adjacent_view_detail
    /// \endcond

    /// \addtogroup group-views
    /// @{
    template(typename Rng, detail::integer_like_ auto N)(
            requires viewable_range<Rng> AND (N > 0))
    struct RANGES_EMPTY_BASES adjacent_view<Rng, N, adjacent_view_detail::cache::first>
      : adjacent_view_detail::av_base<Rng, (std::size_t)N>
    {
    private:
        friend range_access;

        iterator_t<Rng> get_first()
        {
            auto & first = this->cache();
            if(!first)
            {
                first = ranges::next(
                        ranges::begin(this->base()), this->n_ - 1, ranges::end(this->base()));
            }
            return *first;
        }

        struct RANGES_EMPTY_BASES adaptor
          : adaptor_base
          , adjacent_view_detail::trailing<Rng>
        {
        private:
            using base_t = adjacent_view_detail::trailing<Rng>;
            static constexpr auto n_ = static_cast<range_difference_t<Rng>>(N);
            adjacent_view_detail::adjacent_tie_<Rng, N> t_;

        public:
            adaptor() = default;
            adaptor(adjacent_view * v)
              : base_t{v->base()}
              , t_()
            {}
            iterator_t<Rng> begin(adjacent_view & v)
            {
                return v.get_first();
            }
            auto read(iterator_t<Rng> const & it) const
                -> decltype(t_(base_t::get(it, n_)))
            {
                return t_(base_t::get(it, n_));
            }
            void next(iterator_t<Rng> & it)
            {
                ++it;
                base_t::next();
            }
            CPP_member
            auto prev(iterator_t<Rng> & it) //
                -> CPP_ret(void)(
                    requires bidirectional_range<Rng>)
            {
                base_t::prev();
                --it;
            }
            CPP_member
            auto advance(iterator_t<Rng> & it, range_difference_t<Rng> n)
                -> CPP_ret(void)(
                    requires random_access_range<Rng>)
            {
                it += n;
            }
        };

        adaptor begin_adaptor()
        {
            return {this};
        }
        meta::if_c<common_range<Rng>, adaptor, adaptor_base> end_adaptor()
        {
            return {this};
        }

    public:
        using adjacent_view::av_base::av_base;
    };

    template(typename Rng, detail::integer_like_ auto N)(
             requires viewable_range<Rng> AND (N > 0))
    struct adjacent_view<Rng, N, adjacent_view_detail::cache::last>
      : adjacent_view_detail::av_base<Rng, (std::size_t)N>
    {
    private:
        friend range_access;

        iterator_t<Rng> get_last()
        {
            auto & last = this->cache();
            if(!last)
            {
                last = ranges::prev(
                        ranges::end(this->base()), this->n_ - 1, ranges::begin(this->base()));
            }
            return *last;
        }

        struct adaptor : adaptor_base
        {
        private:
            static constexpr auto n_ = static_cast<range_difference_t<Rng>>(N);
            adjacent_view_detail::adjacent_tie_<Rng, N> t_;

        public:
            adaptor() = default;
            adaptor(adjacent_view * v)
              : t_()
            {}
            iterator_t<Rng> end(adjacent_view & v)
            {
                return v.get_last();
            }
            auto read(iterator_t<Rng> const & it) const
                -> decltype(t_(uncounted(it)))
            {
                return t_(uncounted(it));
            }
        };

        adaptor begin_adaptor()
        {
            return {this};
        }
        adaptor end_adaptor()
        {
            return {this};
        }

    public:
        using adjacent_view::av_base::av_base;
    };

    template(typename Rng, detail::integer_like_ auto N)(
             requires viewable_range<Rng> AND (N > 0))
    struct adjacent_view<Rng, N, adjacent_view_detail::cache::none>
      : adjacent_view_detail::av_base<Rng, (std::size_t)N>
    {
    private:
        friend range_access;

        template<bool Const>
        struct adaptor : adaptor_base
        {
        private:
            friend adaptor<!Const>;
            using CRng = meta::const_if_c<Const, Rng>;
            static constexpr auto n_ = static_cast<range_difference_t<Rng>>(N);
            adjacent_view_detail::adjacent_tie_<Rng, N> t_;

        public:
            adaptor() = default;
            template(bool Other)(
                    requires Const AND CPP_NOT(Other)) //
            adaptor(adaptor<Other> that)
              : t_(that.t_)
            {}
            iterator_t<CRng> end(meta::const_if_c<Const, adjacent_view> & v) const
            {
                auto const sz = ranges::distance(v.base());
                auto const offset = n_ - 1 < sz ? n_ - 1 : sz;
                return ranges::begin(v.base()) + (sz - offset);
            }
            auto read(iterator_t<CRng> const & it) const
                -> decltype(t_(it))
            {
                return t_(it);
            }
        };

        adaptor<simple_view<Rng>()> begin_adaptor()
        {
            return {};
        }
        CPP_member
        auto begin_adaptor() const //
            -> CPP_ret(adaptor<true>)(
                requires range<Rng const>)
        {
            return {};
        }
        adaptor<simple_view<Rng>()> end_adaptor()
        {
            return {};
        }
        CPP_member
        auto end_adaptor() const //
            -> CPP_ret(adaptor<true>)(
                requires range<Rng const>)
        {
            return {};
        }

    public:
        using adjacent_view::av_base::av_base;
    };

    template<typename Rng, auto N>
    RANGES_INLINE_VAR constexpr bool enable_borrowed_range<adjacent_view<Rng, N>> = //
            enable_borrowed_range<Rng>;

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng, auto N>
    adjacent_view(Rng &&, std::integral_constant<decltype(N), N>) //
        -> adjacent_view<views::all_t<Rng>, N>;
#endif

    namespace views
    {
        // In:  range<T>
        // Out: range<range<T>, N>, where each inner range has $N$ elements.
        struct adjacent_base_fn
        {
            template(typename Rng, auto N)(
                requires viewable_range<Rng> AND forward_range<Rng>)
            constexpr adjacent_view<all_t<Rng>, N> //
            operator()(Rng && rng, std::integral_constant<decltype(N), N> n) const
            {
                return {all(static_cast<Rng &&>(rng)), n};
            }
        };

        template(auto N)(
            requires detail::integer_like_<decltype(N)>)
        struct adjacent_fn
          : adjacent_base_fn
        {
            using adjacent_base_fn::operator();

            constexpr auto operator()() const
            {
                return make_view_closure(bind_back(adjacent_base_fn{},
                                                   std::integral_constant<std::size_t, (std::size_t)N>{}));
            }
        };

        /// \relates adjacent_fn
        /// \ingroup group-views
        template<auto N>
        RANGES_INLINE_VAR constexpr auto adjacent = adjacent_fn<N>{}();
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>

#endif
