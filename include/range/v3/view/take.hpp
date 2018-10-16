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

#ifndef RANGES_V3_VIEW_TAKE_HPP
#define RANGES_V3_VIEW_TAKE_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/counted_iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct take_view
          : view_adaptor<take_view<Rng>, Rng, finite>
        {
        private:
            friend range_access;

            range_difference_type_t<Rng> n_ = 0;

            template<bool IsConst>
            using CI = counted_iterator<iterator_t<meta::const_if_c<IsConst, Rng>>>;
            template<bool IsConst>
            using S = sentinel_t<meta::const_if_c<IsConst, Rng>>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
                adaptor() = default;
                template<bool Other,
                    CONCEPT_REQUIRES_(IsConst && !Other)>
                adaptor(adaptor<Other>)
                {}
                CI<IsConst> begin(meta::const_if_c<IsConst, take_view> &rng) const
                {
                    return {ranges::begin(rng.base()), rng.n_};
                }
            };

            template<bool IsConst>
            struct sentinel_adaptor : adaptor_base
            {
                sentinel_adaptor() = default;
                template<bool Other,
                    CONCEPT_REQUIRES_(IsConst && !Other)>
                sentinel_adaptor(sentinel_adaptor<Other>)
                {}
                bool empty(CI<IsConst> const &that, S<IsConst> const &sent) const
                {
                    return 0 == that.count() || sent == that.base();
                }
            };

            adaptor<simple_view<Rng>()> begin_adaptor()
            {
                return {};
            }
            sentinel_adaptor<simple_view<Rng>()> end_adaptor()
            {
                return {};
            }
            CONCEPT_REQUIRES(Range<Rng const>())
            adaptor<true> begin_adaptor() const
            {
                return {};
            }
            CONCEPT_REQUIRES(Range<Rng const>())
            sentinel_adaptor<true> end_adaptor() const
            {
                return {};
            }
        public:
            take_view() = default;
            take_view(Rng rng, range_difference_type_t<Rng> n)
              : take_view::view_adaptor(std::move(rng)), n_{n}
            {
                RANGES_EXPECT(n >= 0);
            }
        };

        namespace view
        {
            struct take_fn
            {
            private:
                friend view_access;

                template<typename Rng,
                    CONCEPT_REQUIRES_(!SizedRange<Rng>() && !is_infinite<Rng>())>
                static take_view<all_t<Rng>> invoke_(Rng &&rng, range_difference_type_t<Rng> n)
                {
                    return {all(static_cast<Rng &&>(rng)), n};
                }

                template<typename Rng,
                    CONCEPT_REQUIRES_(SizedRange<Rng>() || is_infinite<Rng>())>
                static auto invoke_(Rng &&rng, range_difference_type_t<Rng> n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    take_exactly(
                        static_cast<Rng &&>(rng),
                        is_infinite<Rng>() ? n : ranges::min(n, distance(rng)))
                )

                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(take_fn take, Int n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(take, std::placeholders::_1, n))
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Int, CONCEPT_REQUIRES_(!Integral<Int>())>
                static detail::null_pipe bind(take_fn, Int)
                {
                    CONCEPT_ASSERT_MSG(Integral<Int>(),
                        "The object passed to view::take must be a model of the Integral concept.");
                    return {};
                }
            #endif

            public:
                template<typename Rng, CONCEPT_REQUIRES_(InputRange<Rng>())>
                auto operator()(Rng &&rng, range_difference_type_t<Rng> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    take_fn::invoke_(static_cast<Rng &&>(rng), n)
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T, CONCEPT_REQUIRES_(!InputRange<Rng>())>
                void operator()(Rng &&, T &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::take operates must be a model of the InputRange "
                        "concept.");
                    CONCEPT_ASSERT_MSG(Integral<T>(),
                        "The second argument to view::take must be a model of the Integral concept.");
                }
            #endif
            };

            /// \relates take_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<take_fn>, take)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::take_view)

#endif
