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

#ifndef RANGES_V3_VIEW_TRANSFORM_HPP
#define RANGES_V3_VIEW_TRANSFORM_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            constexpr cardinality transform2_cardinality(cardinality c1, cardinality c2)
            {
                return c1 >= 0 || c2 >= 0 ?
                    (c1 >= 0 && c2 >= 0 ? (c1 < c2 ? c1 : c2) : finite) :
                    c1 == finite || c2 == finite ?
                        finite :
                        c1 == unknown || c2 == unknown ?
                            unknown :
                            infinite;
            }
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Fun>
        struct iter_transform_view
          : view_adaptor<iter_transform_view<Rng, Fun>, Rng>
        {
        private:
            friend range_access;
            semiregular_t<function_type<Fun>> fun_;
            using use_sentinel_t =
                meta::or_<meta::not_<BoundedRange<Rng>>, SinglePass<range_iterator_t<Rng>>>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                using fun_ref_ = semiregular_ref_or_val_t<function_type<Fun>, IsConst>;
                fun_ref_ fun_;
            public:
                using value_type =
                    detail::decay_t<decltype(fun_(copy_tag{}, range_iterator_t<Rng>{}))>;
                adaptor() = default;
                adaptor(fun_ref_ fun)
                  : fun_(std::move(fun))
                {}
                auto get(range_iterator_t<Rng> it) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fun_(it)
                )
                auto indirect_move(range_iterator_t<Rng> it) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fun_(move_tag{}, it)
                )
            };

            adaptor<false> begin_adaptor()
            {
                return {fun_};
            }
            meta::if_<use_sentinel_t, adaptor_base, adaptor<false>> end_adaptor()
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(Callable<Fun const, range_iterator_t<Rng>>())
            adaptor<true> begin_adaptor() const
            {
                return {fun_};
            }
            CONCEPT_REQUIRES(Callable<Fun const, range_iterator_t<Rng>>())
            meta::if_<use_sentinel_t, adaptor_base, adaptor<true>> end_adaptor() const
            {
                return {fun_};
            }
        public:
            iter_transform_view() = default;
            iter_transform_view(Rng rng, Fun fun)
              : iter_transform_view::view_adaptor{std::move(rng)}
              , fun_(as_function(std::move(fun)))
            {}
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            constexpr range_size_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            RANGES_CXX14_CONSTEXPR range_size_t<Rng> size()
            {
                return ranges::size(this->base());
            }
        };

        template<typename Rng, typename Fun>
        struct transform_view
          : iter_transform_view<Rng, indirected<Fun>>
        {
            transform_view() = default;
            transform_view(Rng rng, Fun fun)
              : iter_transform_view<Rng, indirected<Fun>>{std::move(rng),
                    indirect(std::move(fun))}
            {}
        };

        template<typename Rng1, typename Rng2, typename Fun>
        struct iter_transform2_view
          : view_facade<
                iter_transform2_view<Rng1, Rng2, Fun>,
                detail::transform2_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>
        {
        private:
            friend range_access;
            semiregular_t<function_type<Fun>> fun_;
            Rng1 rng1_;
            Rng2 rng2_;
            using difference_type_ = common_type_t<range_difference_t<Rng1>, range_difference_t<Rng2>>;
            using size_type_ = meta::_t<std::make_unsigned<difference_type_>>;

            static constexpr cardinality my_cardinality = detail::transform2_cardinality(
                range_cardinality<Rng1>::value,
                range_cardinality<Rng2>::value);

            struct sentinel;
            struct cursor
            {
            private:
                friend sentinel;
                using fun_ref_ = semiregular_ref_or_val_t<function_type<Fun>, true>;
                fun_ref_ fun_;
                range_iterator_t<Rng1> it1_;
                range_iterator_t<Rng2> it2_;

            public:
                using difference_type = difference_type_;
                using single_pass = meta::or_c<
                    (bool) SinglePass<range_iterator_t<Rng1>>(),
                    (bool) SinglePass<range_iterator_t<Rng2>>()>;
                using value_type =
                    detail::decay_t<decltype(fun_(copy_tag{}, range_iterator_t<Rng1>{},
                        range_iterator_t<Rng2>{}))>;

                cursor() = default;
                cursor(fun_ref_ fun, range_iterator_t<Rng1> it1, range_iterator_t<Rng2> it2)
                  : fun_(std::move(fun)), it1_(std::move(it1)), it2_(std::move(it2))
                {}
                auto get() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fun_(it1_, it2_)
                )
                void next()
                {
                    ++it1_;
                    ++it2_;
                }
                bool equal(cursor const &that) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // transformed ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return it1_ == that.it1_ || it2_ == that.it2_;
                }
                CONCEPT_REQUIRES(BidirectionalRange<Rng1>() && BidirectionalRange<Rng2>())
                void prev()
                {
                    --it1_;
                    --it2_;
                }
                CONCEPT_REQUIRES(RandomAccessRange<Rng1>() && RandomAccessRange<Rng2>())
                void advance(difference_type n)
                {
                    ranges::advance(it1_, n);
                    ranges::advance(it2_, n);
                }
                CONCEPT_REQUIRES(
                    SizedSentinel<range_iterator_t<Rng1>, range_iterator_t<Rng1>>() &&
                    SizedSentinel<range_iterator_t<Rng2>, range_iterator_t<Rng2>>())
                difference_type distance_to(cursor const &that) const
                {
                    // Return the smallest distance (in magnitude) of any of the iterator
                    // pairs. This is to accommodate zippers of sequences of different length.
                    difference_type d1 = that.it1_ - it1_, d2 = that.it2_ - it2_;
                    return 0 < d1 ? ranges::min(d1, d2) : ranges::max(d1, d2);
                }
                auto move() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fun_(move_tag{}, it1_, it2_)
                )
            };

            struct sentinel
            {
            private:
                range_sentinel_t<Rng1> end1_;
                range_sentinel_t<Rng2> end2_;
            public:
                sentinel() = default;
                sentinel(detail::any, range_sentinel_t<Rng1> end1, range_sentinel_t<Rng1> end2)
                  : end1_(std::move(end1)), end2_(std::move(end2))
                {}
                bool equal(cursor const &pos) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // transformed ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return pos.it1_ == end1_ || pos.it2_ == end2_;
                }
            };

            using end_cursor_t =
                meta::if_c<
                    BoundedRange<Rng1>() && BoundedRange<Rng2>() &&
                        !SinglePass<range_iterator_t<Rng1>>() &&
                        !SinglePass<range_iterator_t<Rng2>>(),
                    cursor,
                    sentinel>;

            cursor begin_cursor()
            {
                return {fun_, ranges::begin(rng1_), ranges::begin(rng2_)};
            }
            end_cursor_t end_cursor()
            {
                return {fun_, ranges::end(rng1_), ranges::end(rng2_)};
            }
            CONCEPT_REQUIRES(Range<Rng1 const>() && Range<Rng2 const>())
            cursor begin_cursor() const
            {
                return {fun_, ranges::begin(rng1_), ranges::begin(rng2_)};
            }
            CONCEPT_REQUIRES(Range<Rng1 const>() && Range<Rng2 const>())
            end_cursor_t end_cursor() const
            {
                return {fun_, ranges::end(rng1_), ranges::end(rng2_)};
            }
            template <class Self>
            static constexpr size_type_ size_(Self& self)
            {
                return ranges::min(
                    static_cast<size_type_>(ranges::size(self.rng1_)),
                    static_cast<size_type_>(ranges::size(self.rng2_)));
            }
        public:
            iter_transform2_view() = default;
            iter_transform2_view(Rng1 rng1, Rng2 rng2, Fun fun)
              : fun_(as_function(std::move(fun)))
              , rng1_(std::move(rng1))
              , rng2_(std::move(rng2))
            {}
            CONCEPT_REQUIRES(my_cardinality >= 0)
            constexpr size_type_ size() const
            {
                return static_cast<size_type_>(my_cardinality);
            }
            CONCEPT_REQUIRES(my_cardinality < 0 &&
                SizedRange<Rng1 const>() && SizedRange<Rng2 const>())
            constexpr size_type_ size() const
            {
                return size_(*this);
            }
            CONCEPT_REQUIRES(my_cardinality < 0 &&
                SizedRange<Rng1>() && SizedRange<Rng2>())
            RANGES_CXX14_CONSTEXPR size_type_ size()
            {
                return size_(*this);
            }
        };

        template<typename Rng1, typename Rng2, typename Fun>
        struct transform2_view
          : iter_transform2_view<Rng1, Rng2, indirected<Fun>>
        {
            transform2_view() = default;
            transform2_view(Rng1 rng1, Rng2 rng2, Fun fun)
              : iter_transform2_view<Rng1, Rng2, indirected<Fun>>{std::move(rng1),
                    std::move(rng2), indirect(std::move(fun))}
            {}
        };

        namespace view
        {
            struct iter_transform_fn
            {
            private:
                friend view_access;
                template<typename Fun>
                static auto bind(iter_transform_fn iter_transform, Fun fun)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(iter_transform, std::placeholders::_1,
                        protect(std::move(fun))))
                )
            public:
                template<typename Rng, typename Fun>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    Callable<Fun, range_iterator_t<Rng>>,
                    Callable<Fun, copy_tag, range_iterator_t<Rng>>,
                    Callable<Fun, move_tag, range_iterator_t<Rng>>>;

                template<typename Rng1, typename Rng2, typename Fun>
                using Concept2 = meta::and_<
                    InputRange<Rng1>,
                    InputRange<Rng2>,
                    Callable<Fun, range_iterator_t<Rng1>, range_iterator_t<Rng2>>,
                    Callable<Fun, copy_tag, range_iterator_t<Rng1>, range_iterator_t<Rng2>>,
                    Callable<Fun, move_tag, range_iterator_t<Rng1>, range_iterator_t<Rng2>>>;

                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Rng, Fun>())>
                iter_transform_view<all_t<Rng>, Fun> operator()(Rng && rng, Fun fun) const
                {
                    return {all(std::forward<Rng>(rng)), std::move(fun)};
                }

                template<typename Rng1, typename Rng2, typename Fun,
                    CONCEPT_REQUIRES_(Concept2<Rng1, Rng2, Fun>())>
                iter_transform2_view<all_t<Rng1>, all_t<Rng2>, Fun>
                operator()(Rng1 && rng1, Rng2 && rng2, Fun fun) const
                {
                    return {all(std::forward<Rng1>(rng1)), all(std::forward<Rng2>(rng2)), std::move(fun)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(!Concept<Rng, Fun>())>
                void operator()(Rng &&, Fun) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::iter_transform operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, range_iterator_t<Rng>>(),
                        "The function passed to view::iter_transform must be callable with an argument "
                        "of the range's iterator type.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, copy_tag, range_iterator_t<Rng>>(),
                        "The function passed to view::iter_transform must be callable with "
                        "copy_tag and an argument of the range's iterator type.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, move_tag, range_iterator_t<Rng>>(),
                        "The function passed to view::iter_transform must be callable with "
                        "move_tag and an argument of the range's iterator type.");
                }

                template<typename Rng1, typename Rng2, typename Fun,
                    CONCEPT_REQUIRES_(!Concept2<Rng1, Rng2, Fun>())>
                void operator()(Rng1 &&, Rng2 &&, Fun) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng1>(),
                        "The first object on which view::iter_transform operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(InputRange<Rng2>(),
                        "The second object on which view::iter_transform operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, range_iterator_t<Rng1>, range_iterator_t<Rng2>>(),
                        "The function passed to view::iter_transform must be callable with argument "
                        "of the ranges' iterator types.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, copy_tag, range_iterator_t<Rng1>, range_iterator_t<Rng2>>(),
                        "The function passed to view::iter_transform must be callable with "
                        "copy_tag and arguments of the ranges' iterator types.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, move_tag, range_iterator_t<Rng1>, range_iterator_t<Rng2>>(),
                        "The function passed to view::iter_transform must be callable with "
                        "move_tag and arguments of the rangess iterator types.");
                }
            #endif
            };

            /// \relates iter_transform_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<iter_transform_fn>, iter_transform)

            struct transform_fn
            {
            private:
                friend view_access;
                template<typename Fun>
                static auto bind(transform_fn transform, Fun fun)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(transform, std::placeholders::_1,
                        protect(std::move(fun))))
                )
            public:
                template<typename Rng, typename Fun>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    Callable<Fun, range_reference_t<Rng> &&>>;

                template<typename Rng1, typename Rng2, typename Fun>
                using Concept2 = meta::and_<
                    InputRange<Rng1>,
                    InputRange<Rng2>,
                    Callable<Fun, range_reference_t<Rng1> &&, range_reference_t<Rng2> &&>>;

                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Rng, Fun>())>
                transform_view<all_t<Rng>, Fun> operator()(Rng && rng, Fun fun) const
                {
                    return {all(std::forward<Rng>(rng)), std::move(fun)};
                }

                template<typename Rng1, typename Rng2, typename Fun,
                    CONCEPT_REQUIRES_(Concept2<Rng1, Rng2, Fun>())>
                transform2_view<all_t<Rng1>, all_t<Rng2>, Fun>
                operator()(Rng1 && rng1, Rng2 && rng2, Fun fun) const
                {
                    return {all(std::forward<Rng1>(rng1)), all(std::forward<Rng2>(rng2)),
                        std::move(fun)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(!Concept<Rng, Fun>())>
                void operator()(Rng &&, Fun) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::transform operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, range_reference_t<Rng> &&>(),
                        "The function passed to view::transform must be callable with an argument "
                        "of the range's reference type.");
                }

                template<typename Rng1, typename Rng2, typename Fun,
                    CONCEPT_REQUIRES_(!Concept2<Rng1, Rng2, Fun>())>
                void operator()(Rng1 &&, Rng2 &&, Fun) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng1>(),
                        "The first object on which view::transform operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(InputRange<Rng2>(),
                        "The second object on which view::transform operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, range_reference_t<Rng1> &&, range_reference_t<Rng2> &&>(),
                        "The function passed to view::transform must be callable with arguments "
                        "of the ranges' reference types.");
                }
            #endif
            };

            /// \relates transform_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<transform_fn>, transform)
        }
        /// @}
    }
}

#endif
