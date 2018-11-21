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
#ifndef RANGES_V3_VIEW_BOUNDED_HPP
#define RANGES_V3_VIEW_BOUNDED_HPP

#include <type_traits>
#include <range/v3/begin_end.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{

        namespace detail
        {
            /// \cond
            CPP_def
            (
                template(typename R)
                concept RA_and_Sized,
                    RandomAccessRange<R> && SizedRange<R>
            );

            template<typename R>
            using bounded_iterator_t = meta::if_c<RA_and_Sized<R>,
                iterator_t<R>,
                common_iterator_t<iterator_t<R>, sentinel_t<R>>>;
            /// \endcond
        }

        template<typename Rng>
        struct bounded_view
          : view_interface<bounded_view<Rng>, range_cardinality<Rng>::value>
        {
        private:
            CPP_assert(View<Rng>);
            CPP_assert(!BoundedView<Rng>);

            Rng rng_;

            sentinel_t<Rng> end_(std::false_type)
            {
                return ranges::end(rng_);
            }
            iterator_t<Rng> end_(std::true_type)
            {
                return ranges::begin(rng_) + ranges::distance(rng_);
            }
            template<typename R = Rng const>
            auto end_(std::false_type) const ->
                CPP_ret(sentinel_t<R>)(
                    requires Range<R &>)
            {
                return ranges::end(rng_);
            }
            template<typename R = Rng const>
            auto end_(std::true_type) const ->
                CPP_ret(iterator_t<R>)(
                    requires Range<R &>)
            {
                return ranges::begin(rng_) + ranges::distance(rng_);
            }
        public:
            bounded_view() = default;
            explicit bounded_view(Rng rng)
              : rng_(detail::move(rng))
            {}
            Rng base() const
            {
                return rng_;
            }

            detail::bounded_iterator_t<Rng> begin()
            {
                return detail::bounded_iterator_t<Rng>{ranges::begin(rng_)};
            }
            detail::bounded_iterator_t<Rng> end()
            {
                return detail::bounded_iterator_t<Rng>{
                    end_(meta::bool_<detail::RA_and_Sized<Rng>>{})};
            }
            CPP_member
            auto CPP_fun(size)() (
                requires SizedRange<Rng>)
            {
                return ranges::size(rng_);
            }

            template<typename R = Rng const>
            auto begin() const ->
                CPP_ret(detail::bounded_iterator_t<R>)(
                    requires Range<R &>)
            {
                return detail::bounded_iterator_t<R>{ranges::begin(rng_)};
            }
            template<typename R = Rng const>
            auto end() const ->
                CPP_ret(detail::bounded_iterator_t<R>)(
                    requires Range<R &>)
            {
                return detail::bounded_iterator_t<R>{
                    end_(meta::bool_<detail::RA_and_Sized<R>>{})};
            }
            CPP_member
            auto CPP_fun(size)() (const
                requires SizedRange<Rng const>)
            {
                return ranges::size(rng_);
            }
        };

        namespace view
        {
            struct bounded_fn
            {
                template<typename Rng>
                auto operator()(Rng &&rng) const ->
                    CPP_ret(bounded_view<all_t<Rng>>)(
                        requires Range<Rng> && !BoundedRange<Rng>)
                {
                    return bounded_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
                }
                template<typename Rng>
                auto operator()(Rng &&rng) const ->
                    CPP_ret(all_t<Rng>)(
                        requires Range<Rng> && BoundedRange<Rng>)
                {
                    return all(static_cast<Rng &&>(rng));
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng>
                auto operator()(Rng &&) const ->
                    CPP_ret(void)(
                        requires not Range<Rng>)
                {
                    CPP_assert_msg(Range<Rng>,
                        "Rng is not a model of the Range concept");
                }
            #endif
            };

            /// \relates bounded_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<bounded_fn>, bounded)

            template<typename Rng>
            using bounded_t =
                decltype(bounded(std::declval<Rng>()));
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::bounded_view)

#endif
