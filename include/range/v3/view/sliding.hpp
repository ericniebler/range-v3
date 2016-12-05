/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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
#include <range/v3/detail/optional.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/view.hpp>
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
          , private detail::non_propagating_cache<
                range_iterator_t<Rng>,
                sliding_view<Rng>,
                !(RandomAccessRange<Rng>() && SizedRange<Rng>())>
        {
        private:
            CONCEPT_ASSERT(ForwardRange<Rng>());
            friend range_access;
            struct adaptor;

            static constexpr bool cache_enabled =
                !(RandomAccessRange<Rng>() && SizedRange<Rng>());

            range_difference_t<Rng> n_;

            CONCEPT_REQUIRES(!cache_enabled)
            range_iterator_t<Rng> get_first() const
            {
                auto const sz = ranges::size(this->base());
                auto const n = static_cast<range_size_t<Rng>>(n_ - 1) < sz ?
                    n_ - 1 : static_cast<range_difference_t<Rng>>(sz);
                return ranges::begin(this->base()) + n;
            }
            CONCEPT_REQUIRES(cache_enabled)
            range_iterator_t<Rng> get_first()
            {
                using cache_t = detail::non_propagating_cache<
                    range_iterator_t<Rng>, sliding_view<Rng>>;
                auto &first = static_cast<cache_t&>(*this);
                if (!first)
                {
                    first = ranges::next(
                                ranges::begin(this->base()), n_ - 1,
                                ranges::end(this->base()));
                }
                return *first;
            }

            CONCEPT_REQUIRES(!cache_enabled)
            adaptor begin_adaptor() const
            {
                return {*this};
            }
            CONCEPT_REQUIRES(cache_enabled)
            adaptor begin_adaptor()
            {
                return {*this};
            }
            meta::if_<BoundedRange<Rng>, adaptor, adaptor_base> end_adaptor() const
            {
                return {*this};
            }
            range_size_t<Rng> size_(range_size_t<Rng> count) const
            {
                auto const n = static_cast<range_size_t<Rng>>(n_);
                return count < n ? 0 : count - n + 1;
            }
        public:
            sliding_view() = default;
            sliding_view(Rng rng, range_difference_t<Rng> n)
              : sliding_view::view_adaptor(std::move(rng)), n_(n)
            {
                RANGES_ASSERT(0 < n_);
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            range_size_t<Rng> size() const
            {
                return size_(ranges::size(this->base()));
            }
            CONCEPT_REQUIRES(SizedRange<Rng>() && !SizedRange<Rng const>())
            range_size_t<Rng> size()
            {
                return size_(ranges::size(this->base()));
            }
        };

        /// \cond
        namespace sliding_view_detail
        {
            template<typename Rng>
            using uncounted_t = decltype(
                ranges::uncounted(std::declval<range_iterator_t<Rng>&>()));

            template<typename Rng, bool = (bool) RandomAccessRange<Rng>()>
            struct trailing
            {
                trailing() = default;
                constexpr trailing(Rng const &rng)
                  : it_{uncounted(ranges::begin(rng))}
                {}
                constexpr uncounted_t<Rng>
                get(range_iterator_t<Rng> const &, range_difference_t<Rng>) const
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
                get(range_iterator_t<Rng> const &it, range_difference_t<Rng> n) const
                {
                    return uncounted(it - (n - 1));
                }
                void next()
                {}
                void prev()
                {}
            };
        }

        template<typename Rng>
        struct sliding_view<Rng>::adaptor
          : adaptor_base
          , private sliding_view_detail::trailing<Rng>
        {
        private:
            using base_t = sliding_view_detail::trailing<Rng>;
            range_difference_t<Rng> n_ = {};
        public:
            adaptor() = default;
            adaptor(sliding_view<Rng> const &v)
              : base_t{v.base()}
              , n_{v.n_}
            {}
            template<typename View>
            range_iterator_t<Rng> begin(View &v) const
            {
                return v.get_first();
            }
            auto get(range_iterator_t<Rng> const &it) const ->
                decltype(view::counted(uncounted(it), n_))
            {
                return view::counted(base_t::get(it, n_), n_);
            }
            void next(range_iterator_t<Rng>& it)
            {
                ++it;
                base_t::next();
            }
            CONCEPT_REQUIRES(BidirectionalRange<Rng>())
            void prev(range_iterator_t<Rng>& it)
            {
                base_t::prev();
                --it;
            }
            CONCEPT_REQUIRES(RandomAccessRange<Rng>())
            void advance(range_iterator_t<Rng>& it, range_difference_t<Rng> n)
            {
                it += n;
            }
        };
        /// \endcond

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
