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

#ifndef RANGES_V3_VIEW_INTERSPERSE_HPP
#define RANGES_V3_VIEW_INTERSPERSE_HPP

#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
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
            intersperse_view() = default;
            constexpr intersperse_view(Rng rng, range_value_type_t<Rng> val)
              : intersperse_view::view_adaptor{detail::move(rng)}, val_(detail::move(val))
            {}
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            constexpr range_size_type_t<Rng> size() const
            {
                return size_(ranges::size(this->base()));
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            RANGES_CXX14_CONSTEXPR range_size_type_t<Rng> size()
            {
                return size_(ranges::size(this->base()));
            }
        private:
            friend range_access;
            template<bool Const>
            struct cursor_adaptor
              : adaptor_base
            {
            private:
                friend struct cursor_adaptor<!Const>;
                using CRng = meta::const_if_c<Const, Rng>;
                bool toggle_ = false;
                range_value_type_t<Rng> val_;
            public:
                cursor_adaptor() = default;
                explicit constexpr cursor_adaptor(range_value_type_t<Rng> const &val)
                  : val_{val}
                {}
                template<bool Other,
                    CONCEPT_REQUIRES_(Const && !Other)>
                cursor_adaptor(cursor_adaptor<Other> that)
                  : toggle_(that.toggle_)
                  , val_(std::move(that.val_))
                {}
                template<typename View>
                RANGES_CXX14_CONSTEXPR iterator_t<CRng> begin(View &view)
                {
                    auto first = ranges::begin(view.base());
                    toggle_ = first != ranges::end(view.base());
                    return first;
                }
                constexpr range_value_type_t<Rng> read(iterator_t<CRng> const &it) const
                {
                    return toggle_ ? *it : val_;
                }
                CONCEPT_REQUIRES(Sentinel<iterator_t<CRng>, iterator_t<CRng>>())
                constexpr bool equal(iterator_t<CRng> const &it0, iterator_t<CRng> const &it1,
                    cursor_adaptor const &other) const
                {
                    return it0 == it1 && toggle_ == other.toggle_;
                }
                RANGES_CXX14_CONSTEXPR void next(iterator_t<CRng> &it)
                {
                    if(toggle_)
                        ++it;
                    toggle_ = !toggle_;
                }
                CONCEPT_REQUIRES(BidirectionalRange<CRng>())
                RANGES_CXX14_CONSTEXPR void prev(iterator_t<CRng> &it)
                {
                    toggle_ = !toggle_;
                    if(toggle_)
                        --it;
                }
                CONCEPT_REQUIRES(SizedSentinel<iterator_t<CRng>, iterator_t<CRng>>())
                constexpr range_difference_type_t<Rng> distance_to(iterator_t<CRng> const &it,
                    iterator_t<CRng> const &other_it, cursor_adaptor const &other) const
                {
                    return (other_it - it) * 2 + (other.toggle_ - toggle_);
                }
                CONCEPT_REQUIRES(RandomAccessRange<CRng>())
                RANGES_CXX14_CONSTEXPR
                void advance(iterator_t<CRng> &it, range_difference_type_t<Rng> n)
                {
                    ranges::advance(it, n >= 0 ? (n + toggle_) / 2 : (n - !toggle_) / 2);
                    if(n % 2 != 0)
                        toggle_ = !toggle_;
                }
            };
            template<bool Const>
            struct sentinel_adaptor
              : adaptor_base
            {
            private:
                using CRng = meta::const_if_c<Const, Rng>;
            public:
                sentinel_adaptor() = default;
                template<bool Other,
                    CONCEPT_REQUIRES_(Const && !Other)>
                sentinel_adaptor(sentinel_adaptor<Other>)
                {}
                static constexpr bool empty(iterator_t<CRng> const &it,
                    cursor_adaptor<Const> const &, sentinel_t<CRng> const &sent)
                {
                    return it == sent;
                }
            };
            CONCEPT_REQUIRES(Range<Rng const>())
            constexpr cursor_adaptor<true> begin_adaptor() const
            {
                return cursor_adaptor<true>{val_};
            }
            RANGES_CXX14_CONSTEXPR cursor_adaptor<false> begin_adaptor()
            {
                return cursor_adaptor<false>{val_};
            }
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(Range<CRng>() &&
                    (BoundedRange<CRng>() && !SinglePass<iterator_t<CRng>>()))>
            constexpr cursor_adaptor<true> end_adaptor() const
            {
                return cursor_adaptor<true>{val_};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng>() && !SinglePass<iterator_t<Rng>>())
            RANGES_CXX14_CONSTEXPR cursor_adaptor<false> end_adaptor()
            {
                return cursor_adaptor<false>{val_};
            }
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(Range<CRng>() &&
                    (!BoundedRange<CRng>() || SinglePass<iterator_t<CRng>>()))>
            constexpr sentinel_adaptor<true> end_adaptor() const noexcept
            {
                return {};
            }
            CONCEPT_REQUIRES(!BoundedRange<Rng>() || SinglePass<iterator_t<Rng>>())
            RANGES_CXX14_CONSTEXPR sentinel_adaptor<false> end_adaptor() noexcept
            {
                return {};
            }
            constexpr range_size_type_t<Rng> size_(range_size_type_t<Rng> const n) const noexcept
            {
                return n ? n * 2 - 1 : 0;
            }

            range_value_type_t<Rng> val_;
        };

        namespace view
        {
            struct intersperse_fn
            {
            private:
                friend view_access;
                template<typename T, CONCEPT_REQUIRES_(Copyable<T>())>
                static auto bind(intersperse_fn intersperse, T t)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    make_pipeable(std::bind(intersperse, std::placeholders::_1, std::move(t)))
                )
            public:
                template<typename Rng, typename T = range_value_type_t<Rng>>
                using Constraint = meta::and_<
                    InputRange<Rng>,
                    ConvertibleTo<T, range_value_type_t<Rng>>,
                    ConvertibleTo<range_reference_t<Rng>, range_value_type_t<Rng>>,
                    SemiRegular<range_value_type_t<Rng>>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Constraint<Rng>())>
                constexpr auto operator()(Rng &&rng, range_value_type_t<Rng> val) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    intersperse_view<all_t<Rng>>{all(static_cast<Rng &&>(rng)), std::move(val)}
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(!Constraint<Rng, T>())>
                void operator()(Rng &&, T &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::intersperse operates must be a model of the "
                        "InputRange concept.");
                    using V = range_value_type_t<Rng>;
                    CONCEPT_ASSERT_MSG(ConvertibleTo<T, V>(),
                        "The value to intersperse in the range must be convertible to the range's "
                        "value type.");
                    CONCEPT_ASSERT_MSG(ConvertibleTo<range_reference_t<Rng>, V>(),
                        "The range's reference type must be convertible to the range's "
                        "value type.");
                    CONCEPT_ASSERT_MSG(SemiRegular<V>(),
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

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::intersperse_view)

#endif
