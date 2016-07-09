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

#ifndef RANGES_V3_VIEW_INTERSPERSE_HPP
#define RANGES_V3_VIEW_INTERSPERSE_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/size.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct intersperse_view
          : view_adaptor<
                intersperse_view<Rng>,
                Rng,
                (range_cardinality<Rng>::value > 0) ?
                    static_cast<cardinality>(range_cardinality<Rng>::value * 2 - 1) :
                    range_cardinality<Rng>::value>
        {
        private:
            friend range_access;
            struct sentinel_adaptor;
            range_value_t<Rng> val_;
            struct cursor_adaptor : adaptor_base
            {
            private:
                friend sentinel_adaptor;
                bool toggl_;
                range_value_t<Rng> val_;
            public:
                cursor_adaptor() = default;
                cursor_adaptor(range_value_t<Rng> val, bool at_end)
                  : toggl_(!at_end), val_(std::move(val))
                {}
                range_value_t<Rng> get(range_iterator_t<Rng> it) const
                {
                    return toggl_ ? *it : val_;
                }
                bool equal(range_iterator_t<Rng> it0, range_iterator_t<Rng> it1,
                    cursor_adaptor const & other) const
                {
                    return it0 == it1 && toggl_ == other.toggl_;
                }
                void next(range_iterator_t<Rng> & it)
                {
                    if(toggl_)
                        ++it;
                    toggl_ = !toggl_;
                }
                CONCEPT_REQUIRES(BidirectionalRange<Rng>())
                void prev(range_iterator_t<Rng> & it)
                {
                    toggl_ = !toggl_;
                    if(toggl_)
                        --it;
                }
                CONCEPT_REQUIRES(
                    SizedSentinel<range_iterator_t<Rng>, range_iterator_t<Rng>>())
                range_difference_t<Rng> distance_to(range_iterator_t<Rng> it,
                    range_iterator_t<Rng> other_it, cursor_adaptor const &other) const
                {
                    auto d = other_it - it;
                    if(d > 0)
                        return d * 2 - (toggl_ != other.toggl_);
                    if(d < 0)
                        return d * 2 + (toggl_ != other.toggl_);
                    return other.toggl_ - toggl_;
                }
                CONCEPT_REQUIRES(RandomAccessRange<Rng>())
                void advance(range_iterator_t<Rng> &it, range_difference_t<Rng> n)
                {
                    ranges::advance(it, n >= 0 ? (n + toggl_) / 2 : (n - !toggl_) / 2);
                    if(n % 2 != 0)
                        toggl_ = !toggl_;
                }
            };
            struct sentinel_adaptor : adaptor_base
            {
                bool empty(range_iterator_t<Rng> it, cursor_adaptor const &,
                    range_sentinel_t<Rng> sent) const
                {
                    return it == sent;
                }
            };
            cursor_adaptor begin_adaptor() const
            {
                return {val_, ranges::empty(this->mutable_base())};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng>() && !SinglePass<range_iterator_t<Rng>>())
            cursor_adaptor end_adaptor() const
            {
                return {val_, true};
            }
            CONCEPT_REQUIRES(!BoundedRange<Rng>() || SinglePass<range_iterator_t<Rng>>())
            sentinel_adaptor end_adaptor() const
            {
                return {};
            }
        public:
            intersperse_view() = default;
            intersperse_view(Rng rng, range_value_t<Rng> val)
              : intersperse_view::view_adaptor{std::move(rng)}, val_(std::move(val))
            {}
            CONCEPT_REQUIRES(SizedRange<Rng>())
            range_size_t<Rng> size() const
            {
                auto tmp = ranges::size(this->mutable_base());
                return tmp ? tmp * 2 - 1 : 0;
            }
        };

        namespace view
        {
            struct intersperse_fn
            {
            private:
                friend view_access;
                template<typename T, CONCEPT_REQUIRES_(SemiRegular<T>())>
                static auto bind(intersperse_fn intersperse, T t)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(intersperse, std::placeholders::_1, std::move(t)))
                )
            public:
                template<typename Rng, typename T = range_value_t<Rng>>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    ConvertibleTo<T, range_value_t<Rng>>,
                    ConvertibleTo<range_reference_t<Rng>, range_value_t<Rng>>,
                    SemiRegular<range_value_t<Rng>>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Concept<Rng>())>
                intersperse_view<all_t<Rng>> operator()(Rng && rng, range_value_t<Rng> val) const
                {
                    return {all(std::forward<Rng>(rng)), {std::move(val)}};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!Concept<Rng, T>())>
                void operator()(Rng &&, T &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::intersperse operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(ConvertibleTo<T, range_value_t<Rng>>(),
                        "The value to intersperse in the range must be convertible to the range's "
                        "value type.");
                    CONCEPT_ASSERT_MSG(ConvertibleTo<range_reference_t<Rng>, range_value_t<Rng>>(),
                        "The range's reference type must be convertible to the range's "
                        "value type.");
                    CONCEPT_ASSERT_MSG(SemiRegular<range_value_t<Rng>>(),
                        "The range on which view::intersperse operates must have a value type that "
                        "models the SemiRegular concept; that is, it must be default constructible, "
                        "copy and move constructible, and destructible.");
                }
            #endif
            };

            /// \relates intersperse_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<intersperse_fn>, intersperse)
        }
    }
}

#endif
