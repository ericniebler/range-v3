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

#ifndef RANGES_V3_VIEW_SLICE_HPP
#define RANGES_V3_VIEW_SLICE_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/counted_iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/drop_exactly.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Rng, typename Int>
            iterator_t<Rng> pos_at_(Rng &&rng, Int i, input_range_tag,
                std::true_type)
            {
                RANGES_EXPECT(0 <= i);
                return next(ranges::begin(rng), i);
            }

            template<typename Rng, typename Int>
            iterator_t<Rng> pos_at_(Rng &&rng, Int i, bidirectional_range_tag,
                std::false_type)
            {
                if(0 > i)
                {
                    // If it's not common and we know the size, faster to count from the front
                    if(SizedRange<Rng> && !CommonRange<Rng>)
                        return next(ranges::begin(rng), distance(rng) + i);
                    // Otherwise, probably faster to count from the back.
                    return next(ranges::next(ranges::begin(rng), ranges::end(rng)), i);
                }
                return next(ranges::begin(rng), i);
            }

            template<typename Rng, typename Int>
            iterator_t<Rng> pos_at_(Rng &&rng, Int i, input_range_tag,
                std::false_type)
            {
                RANGES_EXPECT(i >= 0 || (bool)SizedRange<Rng> || (bool)ForwardRange<Rng>);
                if(0 > i)
                    return next(ranges::begin(rng), distance(rng) + i);
                return next(ranges::begin(rng), i);
            }

            template<typename Rng, bool IsRandomAccess>
            struct slice_view_
              : view_facade<slice_view<Rng>, finite>
            {
            private:
                friend range_access;
                Rng rng_;
                range_difference_t<Rng> from_, count_;
                detail::non_propagating_cache<iterator_t<Rng>> begin_;

                iterator_t<Rng> get_begin_()
                {
                    if(!begin_)
                        begin_ = detail::pos_at_(rng_, from_, range_tag_of<Rng>{},
                            is_infinite<Rng>{});
                    return *begin_;
                }
            public:
                slice_view_() = default;
                slice_view_(Rng rng, range_difference_t<Rng> from, range_difference_t<Rng> count)
                  : rng_(std::move(rng)), from_(from), count_(count)
                {}
                counted_iterator<iterator_t<Rng>> begin()
                {
                    return make_counted_iterator(get_begin_(), count_);
                }
                default_sentinel end()
                {
                    return {};
                }
                auto size() const
                {
                    using size_type = meta::_t<std::make_unsigned<range_difference_t<Rng>>>;
                    return static_cast<size_type>(count_);
                }
                Rng base() const
                {
                    return rng_;
                }
            };

            template<typename Rng>
            struct slice_view_<Rng, true>
              : view_interface<slice_view<Rng>, finite>
            {
            private:
                Rng rng_;
                range_difference_t<Rng> from_, count_;
            public:
                slice_view_() = default;
                slice_view_(Rng rng, range_difference_t<Rng> from, range_difference_t<Rng> count)
                  : rng_(std::move(rng)), from_(from), count_(count)
                {
                    RANGES_EXPECT(0 <= count_);
                }
                iterator_t<Rng> begin()
                {
                    return detail::pos_at_(rng_, from_, range_tag_of<Rng>{},
                        is_infinite<Rng>{});
                }
                iterator_t<Rng> end()
                {
                    return detail::pos_at_(rng_, from_, range_tag_of<Rng>{},
                        is_infinite<Rng>{}) + count_;
                }
                template<typename BaseRng = Rng>
                auto begin() const ->
                    CPP_ret(iterator_t<BaseRng const>)(
                        requires Range<BaseRng const>)
                {
                    return detail::pos_at_(rng_, from_, range_tag_of<Rng>{},
                        is_infinite<Rng>{});
                }
                template<typename BaseRng = Rng>
                auto end() const ->
                    CPP_ret(iterator_t<BaseRng const>)(
                        requires Range<BaseRng const>)
                {
                    return detail::pos_at_(rng_, from_, range_tag_of<Rng>{},
                        is_infinite<Rng>{}) + count_;
                }
                auto size() const
                {
                    using size_type = meta::_t<std::make_unsigned<range_difference_t<Rng>>>;
                    return static_cast<size_type>(count_);
                }
                Rng base() const
                {
                    return rng_;
                }
            };
        }
        /// \endcond

        /// \cond
        namespace _end_
        {
            template<typename Int>
            auto operator-(end_fn, Int dist) ->
                CPP_ret(detail::from_end_<meta::_t<std::make_signed<Int>>>)(
                    requires Integral<Int>)
            {
                RANGES_EXPECT(0 <= static_cast<meta::_t<std::make_signed<Int>>>(dist));
                return {-static_cast<meta::_t<std::make_signed<Int>>>(dist)};
            }
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct slice_view
          : detail::slice_view_<Rng, (bool) RandomAccessRange<Rng>>
        {
            using detail::slice_view_<Rng, (bool) RandomAccessRange<Rng>>::slice_view_;
        };

        namespace view
        {
            struct slice_fn
            {
            private:
                friend view_access;

                template<typename Rng>
                static slice_view<all_t<Rng>>
                invoke_(Rng &&rng, range_difference_t<Rng> from, range_difference_t<Rng> count,
                    input_range_tag, range_tag = {})
                {
                    return {all(static_cast<Rng &&>(rng)), from, count};
                }
                template<typename Rng>
                static auto invoke_(Rng &&rng, range_difference_t<Rng> from,
                        range_difference_t<Rng> count, random_access_range_tag,
                        common_range_tag = {}) ->
                    CPP_ret(iterator_range<iterator_t<Rng>>)(
                        requires not View<uncvref_t<Rng>> && std::is_lvalue_reference<Rng>::value)
                {
                    auto it = detail::pos_at_(rng, from, range_tag_of<Rng>{}, is_infinite<Rng>{});
                    return {it, it + count};
                }

                // Overloads for the pipe syntax: rng | view::slice(from,to)
                template<typename Int>
                static auto CPP_fun(bind)(slice_fn slice, Int from, Int to)(
                    requires Integral<Int>)
                {
                    return make_pipeable(std::bind(slice, std::placeholders::_1, from, to));
                }
                template<typename Int>
                static auto CPP_fun(bind)(slice_fn slice, Int from, detail::from_end_<Int> to)(
                    requires Integral<Int>)
                {
                    return make_pipeable(std::bind(slice, std::placeholders::_1, from, to));
                }
                template<typename Int>
                static auto CPP_fun(bind)(slice_fn slice, detail::from_end_<Int> from,
                    detail::from_end_<Int> to)(
                    requires Integral<Int>)
                {
                    return make_pipeable(std::bind(slice, std::placeholders::_1, from, to));
                }
                template<typename Int>
                static auto CPP_fun(bind)(slice_fn, Int from, end_fn)(
                    requires Integral<Int>)
                {
                    return make_pipeable(std::bind(ranges::view::drop_exactly,
                        std::placeholders::_1, from));
                }
                template<typename Int>
                static auto CPP_fun(bind)(slice_fn slice, detail::from_end_<Int> from, end_fn to)(
                    requires Integral<Int>)
                {
                    return make_pipeable(std::bind(slice, std::placeholders::_1, from, to));
                }

            public:
                // slice(rng, 2, 4)
                template<typename Rng>
                auto operator()(Rng &&rng, range_difference_t<Rng> from,
                        range_difference_t<Rng> to) const ->
                    CPP_ret(decltype(slice_fn::invoke_(
                            static_cast<Rng &&>(rng), from, to - from, range_tag_of<Rng>{})))(
                        requires InputRange<Rng>)
                {
                    RANGES_EXPECT(0 <= from && from <= to);
                    return slice_fn::invoke_(static_cast<Rng &&>(rng), from, to - from,
                        range_tag_of<Rng>{});
                }
                // slice(rng, 4, end-2)
                //  TODO Support Forward, non-Sized ranges by returning a range that
                //       doesn't know it's size?
                template<typename Rng>
                auto operator()(Rng &&rng, range_difference_t<Rng> from,
                        detail::from_end_<range_difference_t<Rng>> to) const ->
                    CPP_ret(decltype(slice_fn::invoke_(
                            static_cast<Rng &&>(rng), from, distance(rng) + to.dist_ - from,
                            range_tag_of<Rng>{})))(
                        requires InputRange<Rng> && SizedRange<Rng>)
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    RANGES_EXPECT(0 <= from);
                    RANGES_ASSERT(from <= distance(rng) + to.dist_);
                    return slice_fn::invoke_(static_cast<Rng &&>(rng), from,
                        distance(rng) + to.dist_ - from, range_tag_of<Rng>{});
                }
                // slice(rng, end-4, end-2)
                template<typename Rng>
                auto operator()(Rng &&rng, detail::from_end_<range_difference_t<Rng>> from,
                        detail::from_end_<range_difference_t<Rng>> to) const ->
                    CPP_ret(decltype(slice_fn::invoke_(
                            static_cast<Rng &&>(rng), from.dist_, to.dist_ - from.dist_,
                            range_tag_of<Rng>{}, common_range_tag_of<Rng>{})))(
                        requires ForwardRange<Rng> || (InputRange<Rng> && SizedRange<Rng>))
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    RANGES_EXPECT(from.dist_ <= to.dist_);
                    return slice_fn::invoke_(static_cast<Rng &&>(rng), from.dist_,
                        to.dist_ - from.dist_, range_tag_of<Rng>{},
                        common_range_tag_of<Rng>{});
                }
                // slice(rng, 4, end)
                template<typename Rng>
                auto operator()(Rng &&rng, range_difference_t<Rng> from, end_fn) const ->
                    CPP_ret(decltype(ranges::view::drop_exactly(static_cast<Rng &&>(rng), from)))(
                        requires InputRange<Rng>)
                {
                    RANGES_EXPECT(0 <= from);
                    return ranges::view::drop_exactly(static_cast<Rng &&>(rng), from);
                }
                // slice(rng, end-4, end)
                template<typename Rng>
                auto operator()(Rng &&rng, detail::from_end_<range_difference_t<Rng>> from, end_fn) const ->
                    CPP_ret(decltype(slice_fn::invoke_(
                            static_cast<Rng &&>(rng), from.dist_, -from.dist_, range_tag_of<Rng>{},
                            common_range_tag_of<Rng>{})))(
                        requires ForwardRange<Rng> || (InputRange<Rng> && SizedRange<Rng>))
                {
                    static_assert(!is_infinite<Rng>(),
                        "Can't index from the end of an infinite range!");
                    return slice_fn::invoke_(static_cast<Rng &&>(rng), from.dist_,
                        -from.dist_, range_tag_of<Rng>{},
                        common_range_tag_of<Rng>{});
                }
            };

            struct _slice_fn
              : slice_fn
            {
                using slice_fn::operator();
            #ifndef RANGES_DOXYGEN_INVOKED
                //
                // These overloads are strictly so that users get better error messages
                // when they try to slice things in a way that doesn't support the operation.
                //

                // slice(rng, 2, 4)
                template<typename Rng>
                auto operator()(Rng &&, range_difference_t<Rng>, range_difference_t<Rng>) const ->
                    CPP_ret(void)(
                        requires not InputRange<Rng>)
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The object to be sliced must be a model of the InputRange concept.");
                }
                // slice(rng, 4, end-2)
                template<typename Rng>
                auto operator()(Rng &&, range_difference_t<Rng>,
                        detail::from_end_<range_difference_t<Rng>>) const ->
                    CPP_ret(void)(
                        requires not (InputRange<Rng> && SizedRange<Rng>))
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The object to be sliced must be a model of the InputRange concept.");
                    CPP_assert_msg(SizedRange<Rng>,
                        "When slicing a range with a positive start offset and a stop offset "
                        "measured from the end, the range must be a model of the SizedRange "
                        "concept; that is, its size must be known.");
                }
                // slice(rng, end-4, end-2)
                template<typename Rng>
                auto operator()(Rng &&, detail::from_end_<range_difference_t<Rng>>,
                        detail::from_end_<range_difference_t<Rng>>) const ->
                    CPP_ret(void)(
                        requires not ((InputRange<Rng> && SizedRange<Rng>) || ForwardRange<Rng>))
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The object to be sliced must be a model of the InputRange concept.");
                    CPP_assert_msg(SizedRange<Rng> || ForwardRange<Rng>,
                        "When slicing a range with a start and stop offset measured from the end, "
                        "the range must either be a model of the SizedRange concept (its size "
                        "must be known), or it must be a model of the ForwardRange concept.");
                }
                // slice(rng, 4, end)
                template<typename Rng>
                auto operator()(Rng &&, range_difference_t<Rng>, end_fn) const ->
                    CPP_ret(void)(
                        requires not (InputRange<Rng>))
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The object to be sliced must be a model of the InputRange concept.");
                }
                // slice(rng, end-4, end)
                template<typename Rng>
                auto operator()(Rng &&, detail::from_end_<range_difference_t<Rng>>,
                        end_fn) const ->
                    CPP_ret(void)(
                        requires not ((InputRange<Rng> && SizedRange<Rng>) || ForwardRange<Rng>))
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The object to be sliced must be a model of the InputRange concept.");
                    CPP_assert_msg(SizedRange<Rng> || ForwardRange<Rng>,
                        "When slicing a range with a start and stop offset measured from the end, "
                        "the range must either be a model of the SizedRange concept (its size "
                        "must be known), or it must be a model of the ForwardRange concept.");
                }
            #endif
            };

            /// \relates _slice_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<_slice_fn>, slice)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::slice_view)

#endif
