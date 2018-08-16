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

#ifndef RANGES_V3_ACTION_SLICE_HPP
#define RANGES_V3_ACTION_SLICE_HPP

#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {

    /// \cond
    namespace _end_ {
    template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
    detail::from_end_<meta::_t<std::make_signed<Int>>> operator-(end_fn, Int dist) {
        RANGES_EXPECT(0 <= static_cast<meta::_t<std::make_signed<Int>>>(dist));
        return {-static_cast<meta::_t<std::make_signed<Int>>>(dist)};
    }
    }
    /// \endcond

        /// \addtogroup group-actions
        /// @{
        namespace action
        {
            struct slice_fn
            {
            private:
                friend action_access;

              // Overloads for the pipe syntax: rng | view::slice(from,to)
              template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
              static auto bind(slice_fn slice, D from, D to)
              RANGES_DECLTYPE_AUTO_RETURN
              (
                  std::bind(slice, std::placeholders::_1, from, to)
              )
              template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
              static auto bind(slice_fn slice, D from, detail::from_end_<D> to)
              RANGES_DECLTYPE_AUTO_RETURN
              (
                  std::bind(slice, std::placeholders::_1, from, to)
              )
              template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
              static auto bind(slice_fn slice, detail::from_end_<D> from, detail::from_end_<D> to)
              RANGES_DECLTYPE_AUTO_RETURN
              (
                  std::bind(slice, std::placeholders::_1, from, to)
              )
              template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
              static auto bind(slice_fn slice, D from, const end_fn &to)
              RANGES_DECLTYPE_AUTO_RETURN
              (
                  std::bind(slice, std::placeholders::_1, from, to)
              )
              template<typename D, CONCEPT_REQUIRES_(Integral<D>())>
              static auto bind(slice_fn slice, detail::from_end_<D> from, const end_fn &to)
              RANGES_DECLTYPE_AUTO_RETURN
              (
                  std::bind(slice, std::placeholders::_1, from, to)
              )
            public:

              template<typename Rng,
                  typename I = iterator_t<Rng>,
                  typename D = range_difference_type_t<Rng>,
                  CONCEPT_REQUIRES_(ForwardRange<Rng>() && ErasableRange<Rng, I, I>())>
              Rng operator()(Rng &&rng, D from, D to) const
                {
                    RANGES_EXPECT(0 <= from && 0 <= to && from <= to);
                    ranges::action::erase(rng, next(begin(rng), to), end(rng));
                    ranges::action::erase(rng, begin(rng), next(begin(rng), from));
                    return static_cast<Rng &&>(rng);
                }

              template<typename Rng,
                  typename I = iterator_t<Rng>,
                  typename D = range_difference_type_t<Rng>,
                  CONCEPT_REQUIRES_(BidirectionalRange<Rng>() && ErasableRange<Rng, I, I>())>
              Rng operator()(Rng &&rng, D from, detail::from_end_<D> to) const {
                  RANGES_EXPECT(0 <= from && to.dist_ <= 0);
                  ranges::action::erase(rng, begin(rng), next(begin(rng), from));
                  ranges::action::erase(rng, prev(end(rng), -to.dist_), end(rng));
                  return static_cast<Rng &&>(rng);
              }

              template<typename Rng,
                  typename I = iterator_t<Rng>,
                  typename D = range_difference_type_t<Rng>,
                  CONCEPT_REQUIRES_(BidirectionalRange<Rng>() && ErasableRange<Rng, I, I>())>
              Rng operator()(Rng &&rng, detail::from_end_<D> from, detail::from_end_<D> to) const {
                  RANGES_EXPECT(from.dist_ <= 0 && to.dist_ <= 0 && from.dist_ <= to.dist_);
                  ranges::action::erase(rng, begin(rng), prev(end(rng), -from.dist_));
                  ranges::action::erase(rng, prev(end(rng), -to.dist_), end(rng));
                  return static_cast<Rng &&>(rng);
              }

              template<typename Rng,
                  typename I = iterator_t<Rng>,
                  typename D = range_difference_type_t<Rng>,
                  CONCEPT_REQUIRES_(ForwardRange<Rng>() && ErasableRange<Rng, I, I>())>
              Rng operator()(Rng &&rng, D from, const end_fn &) const {
                  RANGES_EXPECT(0 <= from);
                  ranges::action::erase(rng, begin(rng), next(begin(rng), from));
                  return static_cast<Rng &&>(rng);
              }

              template<typename Rng,
                  typename I = iterator_t<Rng>,
                  typename D = range_difference_type_t<Rng>,
                  CONCEPT_REQUIRES_(BidirectionalRange<Rng>() && ErasableRange<Rng, I, I>())>
              Rng operator()(Rng &&rng, detail::from_end_<D> from, const end_fn &) const {
                  RANGES_EXPECT(from.dist_ <= 0);
                  ranges::action::erase(rng, begin(rng), prev(end(rng), -from.dist_));
                  return static_cast<Rng &&>(rng);
              }

              template<typename A>
              using is_end_type = meta::or_<meta::is<A, detail::from_end_>, std::is_same<A, end_fn>>;

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T, typename U,
                    CONCEPT_REQUIRES_(!ForwardRange<Rng>())>
                void operator()(Rng &&, T &&, U &&) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "The object on which action::slice operates must be a model of the "
                        "ForwardRange concept.");
                    using I = iterator_t<Rng>;
                    CONCEPT_ASSERT_MSG(ErasableRange<Rng, I, I>(),
                        "The object on which action::slice operates must allow element "
                        "removal.");
                    CONCEPT_ASSERT_MSG(meta::or_<BidirectionalRange<Rng>,
                        meta::negate<meta::or_<is_end_type<T>, is_end_type<U>>>>(),
                        "To slice from the end the object on which action::slice operates it must be a model"
                        "of the BidirectionalRange concept.");

                }
            #endif
            };

            /// \ingroup group-actions
            /// \relates slice_fn
            /// \sa action
            RANGES_INLINE_VARIABLE(action<slice_fn>, slice)
        }
        /// @}
    }
}

#endif
