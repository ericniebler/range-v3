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

#ifndef RANGES_V3_VIEW_TAKE_HPP
#define RANGES_V3_VIEW_TAKE_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_interface.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct take_view
          : range_facade<take_view<Rng>, false>
        {
        private:
            friend range_access;
            using difference_type_ = range_difference_t<Rng>;
            Rng rng_;
            difference_type_ n_;

            template<bool IsConst>
            struct sentinel
            {
            public:
                using BaseRng = meta::apply<meta::add_const_if_c<IsConst>, Rng>;
                using base_iterator = range_iterator_t<BaseRng>;
                using base_sentinel = range_sentinel_t<BaseRng>;
                base_sentinel sent_;
            public:
                RANGES_RELAXED_CONSTEXPR sentinel() = default;
                RANGES_RELAXED_CONSTEXPR sentinel(base_sentinel sent)
                  : sent_(sent)
                {}
                RANGES_RELAXED_CONSTEXPR bool equal(detail::counted_cursor<base_iterator> const &that) const
                {
                    return 0 == that.count() || that.base() == sent_;
                }
            };

            RANGES_RELAXED_CONSTEXPR detail::counted_cursor<range_iterator_t<Rng>> begin_cursor()
            {
                return {ranges::begin(rng_), n_};
            }
            template<typename BaseRng = Rng,
                CONCEPT_REQUIRES_(Iterable<BaseRng const>())>
            RANGES_RELAXED_CONSTEXPR detail::counted_cursor<range_iterator_t<BaseRng const>> begin_cursor() const
            {
                return {ranges::begin(rng_), n_};
            }
            RANGES_RELAXED_CONSTEXPR sentinel<false> end_cursor()
            {
                return {ranges::end(rng_)};
            }
            template<typename BaseRng = Rng,
                CONCEPT_REQUIRES_(Iterable<BaseRng const>())>
            RANGES_RELAXED_CONSTEXPR sentinel<true> end_cursor() const
            {
                return {ranges::end(rng_)};
            }
        public:
            RANGES_RELAXED_CONSTEXPR take_view() = default;
            RANGES_RELAXED_CONSTEXPR take_view(Rng rng, difference_type_ n)
              : rng_(std::move(rng)), n_(n)
            {
                RANGES_ASSERT(n >= 0);
            }
            RANGES_RELAXED_CONSTEXPR Rng & base()
            {
                return rng_;
            }
            RANGES_RELAXED_CONSTEXPR Rng const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct take_fn
            {
            private:
                friend view_access;

                template<typename Rng,
                    CONCEPT_REQUIRES_(!SizedIterable<Rng>() && !is_infinite<Rng>())>
                RANGES_RELAXED_CONSTEXPR
                static take_view<all_t<Rng>> invoke_(Rng && rng, range_difference_t<Rng> n)
                {
                    return {all(std::forward<Rng>(rng)), n};
                }

                template<typename Rng,
                    CONCEPT_REQUIRES_(SizedIterable<Rng>() || is_infinite<Rng>())>
                RANGES_RELAXED_CONSTEXPR
                static auto invoke_(Rng && rng, range_difference_t<Rng> n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    take_exactly(
                        std::forward<Rng>(rng),
                        is_infinite<Rng>() ? n : std::min(n, distance(rng)))
                )

                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static RANGES_RELAXED_CONSTEXPR
                auto bind(take_fn take, Int n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(binder_1<take_fn, Int>(take, n))
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Int, CONCEPT_REQUIRES_(!Integral<Int>())>
                static RANGES_RELAXED_CONSTEXPR detail::null_pipe bind(take_fn, Int)
                {
                    CONCEPT_ASSERT_MSG(Integral<Int>(),
                        "The object passed to view::take must be a model of the Integral concept.");
                    return {};
                }
            #endif

            public:
                template<typename Rng, CONCEPT_REQUIRES_(InputIterable<Rng>())>
                RANGES_RELAXED_CONSTEXPR
                auto operator()(Rng && rng, range_difference_t<Rng> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    take_fn::invoke_(std::forward<Rng>(rng), n)
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T, CONCEPT_REQUIRES_(!InputIterable<Rng>())>
                RANGES_RELAXED_CONSTEXPR void operator()(Rng &&, T &&) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<T>(),
                        "The object on which view::take operates must be a model of the InputIterable "
                        "concept.");
                    CONCEPT_ASSERT_MSG(Integral<T>(),
                        "The second argument to view::take must be a model of the Integral concept.");
                }
            #endif
            };

            /// \relates take_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& take = static_const<view<take_fn>>::value;
            }
        }
        /// @}
    }
}

#endif
