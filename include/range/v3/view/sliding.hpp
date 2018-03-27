/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Tobias Mayer 2016
//  Copyright Casey Carter 2016
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
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace sliding_view_detail
        {
            enum class cache { none, first, last };

            template<typename Rng>
            using caching = std::integral_constant<cache,
                RandomAccessRange<Rng>() && SizedRange<Rng>() ? cache::none :
                BidirectionalRange<Rng>() && BoundedRange<Rng>() ? cache::last :
                cache::first>;
        }

        template<typename Rng,
            sliding_view_detail::cache = sliding_view_detail::caching<Rng>::value>
        struct sliding_view;

        namespace sliding_view_detail {
            template<typename Rng>
            using uncounted_t = decltype(
                ranges::uncounted(std::declval<iterator_t<Rng>&>()));

            template<typename Rng, bool = (bool) RandomAccessRange<Rng>()>
            struct trailing
            {
                trailing() = default;
                constexpr trailing(Rng const &rng)
                  : it_{uncounted(ranges::begin(rng))}
                {}
                constexpr uncounted_t<Rng>
                get(iterator_t<Rng> const &, range_difference_type_t<Rng>) const
                {
                    return it_;
                }
                void next()
                {
                    ++it_;
                }
                CONCEPT_REQUIRES(BidirectionalRange<Rng>())
                void prev()
                {
                    --it_;
                }
            private:
                uncounted_t<Rng> it_;
            };

            template<typename Rng>
            struct trailing<Rng, true>
            {
                trailing() = default;
                constexpr trailing(Rng const &) noexcept
                {}
                constexpr uncounted_t<Rng>
                get(iterator_t<Rng> const &it, range_difference_type_t<Rng> n) const
                {
                    return uncounted(it - (n - 1));
                }
                void next()
                {}
                void prev()
                {}
            };

            template<typename Rng>
            class sv_base
              : public view_adaptor<
                    sliding_view<Rng>,
                    Rng,
                    is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
              , private detail::non_propagating_cache<
                    iterator_t<Rng>,
                    sv_base<Rng>,
                    caching<Rng>::value != cache::none>
            {
            public:
                CONCEPT_ASSERT(ForwardRange<Rng>());
                sv_base() = default;
                sv_base(Rng rng, range_difference_type_t<Rng> n)
                : sv_base::view_adaptor(std::move(rng)), n_(n)
                {
                    RANGES_ASSERT(0 < n_);
                }
                CONCEPT_REQUIRES(SizedRange<Rng const>())
                range_size_type_t<Rng> size() const
                {
                    return size_(ranges::size(this->base()));
                }
                CONCEPT_REQUIRES(SizedRange<Rng>() && !SizedRange<Rng const>())
                range_size_type_t<Rng> size()
                {
                    return size_(ranges::size(this->base()));
                }
            protected:
                range_difference_type_t<Rng> n_;

                optional<iterator_t<Rng>> &cache() &
                {
                    return static_cast<cache_t&>(*this);
                }
                optional<iterator_t<Rng>> const &cache() const&
                {
                    return static_cast<cache_t const&>(*this);
                }
            private:
                using cache_t = detail::non_propagating_cache<
                    iterator_t<Rng>, sv_base<Rng>>;

                range_size_type_t<Rng> size_(range_size_type_t<Rng> count) const
                {
                    auto const n = static_cast<range_size_type_t<Rng>>(n_);
                    return count < n ? 0 : count - n + 1;
                }
            };
        }

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct sliding_view<Rng, sliding_view_detail::cache::first>
          : sliding_view_detail::sv_base<Rng>
        {
        private:
            friend range_access;

            iterator_t<Rng> get_first()
            {
                auto &first = this->cache();
                if(!first)
                {
                    first = ranges::next(
                        ranges::begin(this->base()),
                        this->n_ - 1,
                        ranges::end(this->base()));
                }
                return *first;
            }

            struct adaptor
              : adaptor_base
              , sliding_view_detail::trailing<Rng>
            {
            private:
                using base_t = sliding_view_detail::trailing<Rng>;
                range_difference_type_t<Rng> n_ = {};
            public:
                adaptor() = default;
                adaptor(sliding_view<Rng> const &v)
                  : base_t{v.base()}
                  , n_{v.n_}
                {}
                iterator_t<Rng> begin(sliding_view &v)
                {
                    return v.get_first();
                }
                auto read(iterator_t<Rng> const &it) const ->
                    decltype(view::counted(uncounted(it), n_))
                {
                    return view::counted(base_t::get(it, n_), n_);
                }
                void next(iterator_t<Rng>& it)
                {
                    ++it;
                    base_t::next();
                }
                CONCEPT_REQUIRES(BidirectionalRange<Rng>())
                void prev(iterator_t<Rng>& it)
                {
                    base_t::prev();
                    --it;
                }
                CONCEPT_REQUIRES(RandomAccessRange<Rng>())
                void advance(iterator_t<Rng>& it, range_difference_type_t<Rng> n)
                {
                    it += n;
                }
            };

            adaptor begin_adaptor()
            {
                return {*this};
            }
            meta::if_<BoundedRange<Rng>, adaptor, adaptor_base> end_adaptor() const
            {
                return {*this};
            }
        public:
            using sliding_view::sv_base::sv_base;
        };

        template<typename Rng>
        struct sliding_view<Rng, sliding_view_detail::cache::last>
          : sliding_view_detail::sv_base<Rng>
        {
        private:
            friend range_access;

            iterator_t<Rng> get_last()
            {
                auto &last = this->cache();
                if(!last)
                {
                    last = ranges::prev(
                                ranges::end(this->base()), this->n_ - 1,
                                ranges::begin(this->base()));
                }
                return *last;
            }

            struct adaptor
              : adaptor_base
            {
            private:
                range_difference_type_t<Rng> n_ = {};
            public:
                adaptor() = default;
                adaptor(sliding_view<Rng> const &v)
                  : n_{v.n_}
                {}
                iterator_t<Rng> end(sliding_view &v)
                {
                    return v.get_last();
                }
                auto read(iterator_t<Rng> const &it) const ->
                    decltype(view::counted(uncounted(it), n_))
                {
                    return view::counted(uncounted(it), n_);
                }
            };

            adaptor begin_adaptor() const
            {
                return {*this};
            }
            adaptor end_adaptor()
            {
                return {*this};
            }
        public:
            using sliding_view::sv_base::sv_base;
        };

        template<typename Rng>
        struct sliding_view<Rng, sliding_view_detail::cache::none>
          : sliding_view_detail::sv_base<Rng>
        {
        private:
            friend range_access;

            iterator_t<Rng> get_last() const
            {
                auto const sz = ranges::distance(this->base());
                auto const offset = this->n_ - 1 < sz ? this->n_ - 1 : sz;
                return ranges::begin(this->base()) + (sz - offset);
            }

            struct adaptor
              : adaptor_base
            {
            private:
                range_difference_type_t<Rng> n_ = {};
            public:
                adaptor() = default;
                adaptor(sliding_view<Rng> const &v)
                  : n_{v.n_}
                {}
                iterator_t<Rng> end(sliding_view const &v) const
                {
                    return v.get_last();
                }
                auto read(iterator_t<Rng> const &it) const ->
                    decltype(view::counted(uncounted(it), n_))
                {
                    return view::counted(uncounted(it), n_);
                }
            };

            adaptor begin_adaptor() const
            {
                return {*this};
            }
            adaptor end_adaptor() const
            {
                return {*this};
            }
        public:
            using sliding_view::sv_base::sv_base;
        };

        namespace view
        {
            // In:  Range<T>
            // Out: Range<Range<T>>, where each inner range has $n$ elements.
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
                sliding_view<all_t<Rng>> operator()(Rng && rng, range_difference_type_t<Rng> n) const
                {
                    return {all(static_cast<Rng&&>(rng)), n};
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
