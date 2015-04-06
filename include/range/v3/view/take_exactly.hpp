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

#ifndef RANGES_V3_VIEW_TAKE_EXACTLY_HPP
#define RANGES_V3_VIEW_TAKE_EXACTLY_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_interface.hpp>
#include <range/v3/range.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/counted_iterator.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Rng, bool IsRandomAccess = RandomAccessIterable<Rng>()>
            struct take_exactly_view_
              : range_facade<take_exactly_view<Rng>, false>
            {
            private:
                friend range_access;
                using difference_type_ = range_difference_t<Rng>;
                Rng rng_;
                difference_type_ n_;

                RANGES_RELAXED_CONSTEXPR
                counted_cursor<range_iterator_t<Rng>> begin_cursor()
                {
                    return {ranges::begin(rng_), n_};
                }
                template<typename BaseRng = Rng,
                    CONCEPT_REQUIRES_(Iterable<BaseRng const>())>
                RANGES_RELAXED_CONSTEXPR
                counted_cursor<range_iterator_t<BaseRng const>> begin_cursor() const
                {
                    return {ranges::begin(rng_), n_};
                }
                RANGES_RELAXED_CONSTEXPR
                counted_sentinel end_cursor() const
                {
                    return {};
                }
            public:
                RANGES_RELAXED_CONSTEXPR take_exactly_view_() = default;
                RANGES_RELAXED_CONSTEXPR take_exactly_view_(Rng rng, difference_type_ n)
                  : rng_(std::move(rng)), n_(n)
                {
                    RANGES_ASSERT(n >= 0);
                }
                RANGES_RELAXED_CONSTEXPR range_size_t<Rng> size() const
                {
                    return static_cast<range_size_t<Rng>>(n_);
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

            template<typename Rng>
            struct take_exactly_view_<Rng, true>
              : range_interface<take_exactly_view<Rng>>
            {
            private:
                using difference_type_ = range_difference_t<Rng>;
                Rng rng_;
                difference_type_ n_;
            public:
                RANGES_RELAXED_CONSTEXPR take_exactly_view_() = default;
                RANGES_RELAXED_CONSTEXPR take_exactly_view_(Rng rng, difference_type_ n)
                  : rng_(std::move(rng)), n_(n)
                {
                    RANGES_ASSERT(n >= 0);
                }
                RANGES_RELAXED_CONSTEXPR range_iterator_t<Rng> begin()
                {
                    return ranges::begin(rng_);
                }
                RANGES_RELAXED_CONSTEXPR range_iterator_t<Rng> end()
                {
                    return next(ranges::begin(rng_), n_);
                }
                template<typename BaseRng = Rng,
                    CONCEPT_REQUIRES_(Iterable<BaseRng const>())>
                RANGES_RELAXED_CONSTEXPR
                range_iterator_t<BaseRng const> begin() const
                {
                    return ranges::begin(rng_);
                }
                template<typename BaseRng = Rng,
                    CONCEPT_REQUIRES_(Iterable<BaseRng const>())>
                RANGES_RELAXED_CONSTEXPR
                range_iterator_t<BaseRng const> end() const
                {
                    return next(ranges::begin(rng_), n_);
                }
                RANGES_RELAXED_CONSTEXPR range_size_t<Rng> size() const
                {
                    return static_cast<range_size_t<Rng>>(n_);
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
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct take_exactly_view
          : detail::take_exactly_view_<Rng>
        {
            using detail::take_exactly_view_<Rng>::take_exactly_view_;
        };

        namespace view
        {
            struct take_exactly_fn
            {
            private:
                friend view_access;

                template<typename Rng>
                static RANGES_RELAXED_CONSTEXPR take_exactly_view<all_t<Rng>>
                invoke_(Rng && rng, range_difference_t<Rng> n, concepts::InputIterable*)
                {
                    return {all(std::forward<Rng>(rng)), n};
                }
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Range<Rng>() && std::is_lvalue_reference<Rng>())>
                static RANGES_RELAXED_CONSTEXPR range<range_iterator_t<Rng>>
                invoke_(Rng && rng, range_difference_t<Rng> n, concepts::RandomAccessIterable*)
                {
                    return {begin(rng), next(begin(rng), n)};
                }

                template<typename Int,
                    CONCEPT_REQUIRES_(Integral<Int>())>
                static RANGES_RELAXED_CONSTEXPR auto bind(take_exactly_fn take_exactly, Int n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(take_exactly, std::placeholders::_1, n))
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Int,
                    CONCEPT_REQUIRES_(!Integral<Int>())>
                static RANGES_RELAXED_CONSTEXPR detail::null_pipe bind(take_exactly_fn, Int)
                {
                    CONCEPT_ASSERT_MSG(Integral<Int>(),
                        "The object passed to view::take must be a model of the Integral concept.");
                    return {};
                }
            #endif

            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(InputIterable<Rng>())>
                auto RANGES_RELAXED_CONSTEXPR operator()(Rng && rng, range_difference_t<Rng> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    take_exactly_fn::invoke_(std::forward<Rng>(rng), n, iterable_concept<Rng>{})
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!InputIterable<Rng>())>
                void RANGES_RELAXED_CONSTEXPR operator()(Rng &&, T &&) const
                {
                    CONCEPT_ASSERT_MSG(InputIterable<T>(),
                        "The object on which view::take operates must be a model of the InputIterable "
                        "concept.");
                    CONCEPT_ASSERT_MSG(Integral<T>(),
                        "The second argument to view::take must be a model of the Integral concept.");
                }
            #endif
            };

            /// \relates take_exactly_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& take_exactly = static_const<view<take_exactly_fn>>::value;
            }
        }
        /// @}
    }
}

#endif
