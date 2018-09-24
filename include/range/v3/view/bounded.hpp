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
            CPP_template(typename R = Rng const)(
                requires Range<R &>)
            sentinel_t<R> end_(std::false_type) const
            {
                return ranges::end(rng_);
            }
            CPP_template(typename R = Rng const)(
                requires Range<R &>)
            iterator_t<R> end_(std::true_type) const
            {
                return ranges::begin(rng_) + ranges::distance(rng_);
            }
        public:
            bounded_view() = default;
            explicit bounded_view(Rng rng)
              : rng_(detail::move(rng))
            {}
            Rng & base()
            {
                return rng_;
            }
            Rng const & base() const
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
            auto size() -> CPP_ret(range_size_type_t<Rng>)(
                requires SizedRange<Rng>)
            {
                return ranges::size(rng_);
            }

            CPP_template(typename R = Rng const)(
                requires Range<R &>)
            detail::bounded_iterator_t<R> begin() const
            {
                return detail::bounded_iterator_t<R>{ranges::begin(rng_)};
            }
            CPP_template(typename R = Rng const)(
                requires Range<R &>)
            detail::bounded_iterator_t<R> end() const
            {
                return detail::bounded_iterator_t<R>{
                    end_(meta::bool_<detail::RA_and_Sized<R>>{})};
            }
            CPP_member
            auto size() const -> CPP_ret(range_size_type_t<Rng>)(
                requires SizedRange<Rng const>)
            {
                return ranges::size(rng_);
            }
        };

        namespace view
        {
            struct bounded_fn
            {
                CPP_template(typename Rng)(
                    requires Range<Rng> && !BoundedRange<Rng>)
                bounded_view<all_t<Rng>> operator()(Rng &&rng) const
                {
                    return bounded_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
                }
                CPP_template(typename Rng)(
                    requires Range<Rng> && BoundedRange<Rng>)
                all_t<Rng> operator()(Rng &&rng) const
                {
                    return all(static_cast<Rng &&>(rng));
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                CPP_template(typename Rng)(
                    requires not Range<Rng>)
                void operator()(Rng &&) const
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
