/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_ALL_HPP
#define RANGES_V3_VIEW_ALL_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/data.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            struct CanEmptyRange_
              : concepts::refines<concepts::Range>
            {
                template<typename T>
                auto requires_(T &t) -> decltype(
                    concepts::valid_expr(
                        empty(t)
                    ));
            };

            template<typename Rng>
            using CanEmptyRange = concepts::models<CanEmptyRange_, Rng>;
        } // namespace detail
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct ref_view
          : view_interface<ref_view<Rng>, range_cardinality<Rng>::value>
        {
        private:
            CONCEPT_ASSERT(std::is_object<Rng>::value && Range<Rng>() && !View<Rng>());

            Rng *rng_ = nullptr;
        public:
            ref_view() = default;
            constexpr explicit ref_view(Rng &rng) noexcept
              : rng_{&rng}
            {}
            constexpr iterator_t<Rng> begin() const
            RANGES_AUTO_RETURN_NOEXCEPT
            (
                ranges::begin(*rng_)
            )
            constexpr sentinel_t<Rng> end() const
            RANGES_AUTO_RETURN_NOEXCEPT
            (
                ranges::end(*rng_)
            )
            template<typename R = Rng,
                CONCEPT_REQUIRES_(SizedRange<R>())>
            constexpr auto size() const
                RANGES_DECLTYPE_NOEXCEPT(ranges::size(std::declval<R &>()))
            {
                return ranges::size(*rng_);
            }
            template<typename R = Rng,
                CONCEPT_REQUIRES_(detail::CanEmptyRange<R>())>
            constexpr auto empty() const
                RANGES_DECLTYPE_NOEXCEPT(ranges::empty(std::declval<R &>()))
            {
                return ranges::empty(*rng_);
            }
            template<typename R = Rng,
                CONCEPT_REQUIRES_(ContiguousRange<R>())>
            constexpr auto data() const
                RANGES_DECLTYPE_NOEXCEPT(ranges::data(std::declval<R &>()))
            {
                return ranges::data(*rng_);
            }

            template<class I, class S,
                CONCEPT_REQUIRES_(ConvertibleTo<iterator_t<Rng>, I>() &&
                    ConvertibleTo<sentinel_t<Rng>, S>())>
            operator iterator_range<I, S>() const
            {
                return iterator_range<I, S>{begin(), end()};
            }

            template<class I, class S,
                CONCEPT_REQUIRES_(SizedRange<Rng>() &&
                    ConvertibleTo<iterator_t<Rng>, I>() &&
                    ConvertibleTo<sentinel_t<Rng>, S>())>
            operator sized_iterator_range<I, S>() const
            {
                return sized_iterator_range<I, S>{begin(), end(), size()};
            }
        };

        namespace view
        {
            struct all_fn : pipeable<all_fn>
            {
                /// If it's a view already, pass it though.
                template<typename T,
                    CONCEPT_REQUIRES_(View<uncvref_t<T>>())>
                RANGES_CXX14_CONSTEXPR uncvref_t<T> operator()(T &&t) const
                    noexcept(noexcept(uncvref_t<T>(uncvref_t<T>(static_cast<T &&>(t)))))
                {
                    return static_cast<T &&>(t);
                }
                template<typename T,
                    CONCEPT_REQUIRES_(View<T>())>
                constexpr T operator()(std::reference_wrapper<T> ref) const
                    noexcept(std::is_nothrow_constructible<T, T &>::value)
                {
                    return ref.get();
                }
                template<typename T,
                    CONCEPT_REQUIRES_(View<T>())>
                constexpr T operator()(ranges::reference_wrapper<T> ref) const
                    noexcept(std::is_nothrow_constructible<T, T &>::value)
                {
                    return ref.get();
                }

                /// If it's container-like, turn it into a view.
                template<typename T,
                    CONCEPT_REQUIRES_(Range<T>() && !View<uncvref_t<T>>())>
                constexpr ref_view<meta::_t<std::remove_reference<T>>>
                operator()(T &&t) const noexcept
                {
                    static_assert(std::is_lvalue_reference<T>::value,
                        "Cannot get a view of a temporary container");
                    return ref_view<meta::_t<std::remove_reference<T>>>{t};
                }
                template<typename T,
                    CONCEPT_REQUIRES_(Range<T>() && !View<T>())>
                constexpr ref_view<T>
                operator()(std::reference_wrapper<T> ref) const noexcept
                {
                    return ref_view<T>(ref.get());
                }
                template<typename T,
                    CONCEPT_REQUIRES_(Range<T>() && !View<T>())>
                constexpr ref_view<T>
                operator()(ranges::reference_wrapper<T> ref) const noexcept
                {
                    return ref_view<T>(ref.get());
                }
            };

            /// \relates all_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(all_fn, all)

            template<typename Rng>
            using all_t = decltype(all(std::declval<Rng>()));
        }
        /// @}
    }
}

#endif
