/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_REVERSE_HPP
#define RANGES_V3_VIEW_REVERSE_HPP

#include <iterator>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct RANGES_EMPTY_BASES reverse_view
          : view_interface<reverse_view<Rng>, range_cardinality<Rng>::value>
          , private detail::non_propagating_cache<
                iterator_t<Rng>, reverse_view<Rng>, !BoundedRange<Rng>()>
        {
        private:
            CONCEPT_ASSERT(BidirectionalRange<Rng>());

            Rng rng_;

            RANGES_CXX14_CONSTEXPR
            reverse_iterator<iterator_t<Rng>> begin_(std::true_type)
            {
                return make_reverse_iterator(ranges::end(rng_));
            }
            RANGES_CXX14_CONSTEXPR
            reverse_iterator<iterator_t<Rng>> begin_(std::false_type)
            {
                using cache_t = detail::non_propagating_cache<iterator_t<Rng>, reverse_view<Rng>>;
                auto &end_ = static_cast<cache_t &>(*this);
                if(!end_)
                    end_ = ranges::next(ranges::begin(rng_), ranges::end(rng_));
                return make_reverse_iterator(*end_);
            }
            template<typename T>
            using not_self_ =
                meta::if_c<!std::is_same<reverse_view, detail::decay_t<T>>::value, T>;
        public:
            reverse_view() = default;
            explicit constexpr reverse_view(Rng rng)
              : rng_(detail::move(rng))
            {}
            template<typename O,
                CONCEPT_REQUIRES_(ViewableRange<not_self_<O>>() &&
                    BidirectionalRange<O>() && Constructible<Rng, view::all_t<O>>())>
            explicit constexpr reverse_view(O&& o)
              : rng_(view::all(static_cast<O &&>(o)))
            {}
            Rng base() const
            {
                return rng_;
            }
            RANGES_CXX14_CONSTEXPR
            reverse_iterator<iterator_t<Rng>> begin()
            {
                return begin_(meta::bool_<(bool) BoundedRange<Rng>()>{});
            }
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(BoundedRange<CRng>())>
            constexpr reverse_iterator<iterator_t<CRng>> begin() const
            {
                return make_reverse_iterator(ranges::end(rng_));
            }
            RANGES_CXX14_CONSTEXPR
            reverse_iterator<iterator_t<Rng>> end()
            {
                return make_reverse_iterator(ranges::begin(rng_));
            }
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(BoundedRange<CRng>())>
            constexpr reverse_iterator<iterator_t<CRng>> end() const
            {
                return make_reverse_iterator(ranges::begin(rng_));
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            constexpr range_size_type_t<Rng> size() const
            {
                return ranges::size(rng_);
            }
        };

#if !defined(__GNUC__) || defined(__clang__) || __GNUC__ >= 5
        template<typename Rng>
        struct reverse_view<reverse_view<Rng>>
          : identity_adaptor<Rng>
        {
            CONCEPT_ASSERT(BidirectionalRange<Rng>());

            reverse_view() = default;
            explicit constexpr reverse_view(reverse_view<Rng> const &rng)
              : identity_adaptor<Rng>(rng.base())
            {}
        };
#endif // !GCC4

        namespace view
        {
            struct reverse_fn
            {
                template<typename Rng>
                using Constraint = BidirectionalRange<Rng>;

                template<typename Rng, CONCEPT_REQUIRES_(Constraint<Rng>())>
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 5
                // Avoid GCC5 bug that ODR-uses std::declval?!?
                RANGES_CXX14_CONSTEXPR
#else
                constexpr
#endif
                auto operator()(Rng &&rng) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    reverse_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))}
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Rng, CONCEPT_REQUIRES_(!Constraint<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(BidirectionalRange<Rng>(),
                        "The object on which view::reverse operates must model the "
                        "BidirectionalRange concept.");
                }
            #endif
            };

            /// \relates reverse_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<reverse_fn>, reverse)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::reverse_view)

#endif
