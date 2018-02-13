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

#ifndef RANGES_V3_VIEW_TAKE_EXACTLY_HPP
#define RANGES_V3_VIEW_TAKE_EXACTLY_HPP

#include <type_traits>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/iterator_range.hpp>
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
            template<typename Rng>
            struct is_random_access_bounded_
              : meta::bool_<(bool) RandomAccessRange<Rng>() && (bool) BoundedRange<Rng>()>
            {};

            // BUGBUG Per the discussion in https://github.com/ericniebler/stl2/issues/63, it's
            // unclear if we can infer anything from RandomAccessRange<Rng>() && BoundedRange<Rng>()
            template<typename Rng,
                bool IsRandomAccessBounded /*= is_random_access_bounded_<Rng>::value*/>
            struct take_exactly_view_
              : view_interface<take_exactly_view_<Rng, IsRandomAccessBounded>>
            {
            private:
                friend range_access;
                using difference_type_ = range_difference_type_t<Rng>;
                Rng rng_;
                difference_type_ n_;

            public:
                take_exactly_view_() = default;
                take_exactly_view_(Rng rng, difference_type_ n)
                  : rng_(std::move(rng)), n_(n)
                {
                    RANGES_EXPECT(n >= 0);
                }
                counted_iterator<iterator_t<Rng>> begin()
                {
                    return {ranges::begin(rng_), n_};
                }
                CONCEPT_template(typename BaseRng = Rng)(
                    requires Range<BaseRng const>())
                (counted_iterator<iterator_t<BaseRng const>>) begin() const
                {
                    return {ranges::begin(rng_), n_};
                }
                default_sentinel end() const
                {
                    return {};
                }
                range_size_type_t<Rng> size() const
                {
                    return static_cast<range_size_type_t<Rng>>(n_);
                }
                Rng & base()
                {
                    return rng_;
                }
                Rng const & base() const
                {
                    return rng_;
                }
            };

            template<typename Rng>
            struct take_exactly_view_<Rng, true>
              : view_interface<take_exactly_view_<Rng, true>, finite>
            {
            private:
                using difference_type_ = range_difference_type_t<Rng>;
                Rng rng_;
                difference_type_ n_;
            public:
                take_exactly_view_() = default;
                take_exactly_view_(Rng rng, difference_type_ n)
                  : rng_(std::move(rng)), n_(n)
                {
                    RANGES_EXPECT(n >= 0);
                    RANGES_EXPECT(!(bool)SizedRange<Rng>() || n <= ranges::distance(rng_));
                }
                iterator_t<Rng> begin()
                {
                    return ranges::begin(rng_);
                }
                iterator_t<Rng> end()
                {
                    return next(ranges::begin(rng_), n_);
                }
                CONCEPT_template(typename BaseRng = Rng)(
                    requires Range<BaseRng const>())
                (iterator_t<BaseRng const>) begin() const
                {
                    return ranges::begin(rng_);
                }
                CONCEPT_template(typename BaseRng = Rng)(
                    requires Range<BaseRng const>())
                (iterator_t<BaseRng const>) end() const
                {
                    return next(ranges::begin(rng_), n_);
                }
                range_size_type_t<Rng> size() const
                {
                    return static_cast<range_size_type_t<Rng>>(n_);
                }
                Rng & base()
                {
                    return rng_;
                }
                Rng const & base() const
                {
                    return rng_;
                }
            };
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        using take_exactly_view = detail::take_exactly_view_<Rng>;

        namespace view
        {
            struct take_exactly_fn
            {
            private:
                friend view_access;

                template<typename Rng>
                static take_exactly_view<all_t<Rng>>
                invoke_(Rng &&rng, range_difference_type_t<Rng> n, input_range_tag)
                {
                    return {all(static_cast<Rng &&>(rng)), n};
                }
                CONCEPT_template(typename Rng)(
                    requires !View<uncvref_t<Rng>>() && True<std::is_lvalue_reference<Rng>>())
                (static iterator_range<iterator_t<Rng>>)
                invoke_(Rng &&rng, range_difference_type_t<Rng> n, random_access_range_tag)
                {
                    return {begin(rng), next(begin(rng), n)};
                }

                CONCEPT_template(typename Int)(
                    requires Integral<Int>())
                (static auto) bind(take_exactly_fn take_exactly, Int n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(take_exactly, std::placeholders::_1, n))
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                CONCEPT_template(typename Int)(
                    requires !Integral<Int>())
                (static detail::null_pipe) bind(take_exactly_fn, Int)
                {
                    CONCEPT_assert_msg(Integral<Int>(),
                        "The object passed to view::take must be a model of the Integral concept.");
                    return {};
                }
            #endif

            public:
                CONCEPT_template(typename Rng)(
                    requires InputRange<Rng>())
                (auto) operator()(Rng &&rng, range_difference_type_t<Rng> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    take_exactly_fn::invoke_(static_cast<Rng &&>(rng), n, range_tag_of<Rng>{})
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                CONCEPT_template(typename Rng, typename T)(
                    requires !InputRange<Rng>())
                (void) operator()(Rng &&, T &&) const
                {
                    CONCEPT_assert_msg(InputRange<T>(),
                        "The object on which view::take operates must be a model of the InputRange "
                        "concept.");
                    CONCEPT_assert_msg(Integral<T>(),
                        "The second argument to view::take must be a model of the Integral concept.");
                }
            #endif
            };

            /// \relates take_exactly_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<take_exactly_fn>, take_exactly)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::detail::take_exactly_view_)

#endif
