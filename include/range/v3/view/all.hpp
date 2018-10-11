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
#ifndef RANGES_V3_VIEW_ALL_HPP
#define RANGES_V3_VIEW_ALL_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/size.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct all_fn : pipeable<all_fn>
            {
            private:
                template<typename T>
                static iterator_range<iterator_t<T>, sentinel_t<T>>
                from_container(T &t, range_tag, sentinel_tag)
                {
                    return {begin(t), end(t)};
                }

                template<typename T>
                static sized_iterator_range<iterator_t<T>, sentinel_t<T>>
                from_container(T &t, sized_range_tag, sentinel_tag)
                {
                    return {begin(t), end(t), size(t)};
                }

                template<typename T>
                static iterator_range<iterator_t<T>, sentinel_t<T>>
                from_container(T &t, sized_range_tag, sized_sentinel_tag)
                {
                    RANGES_ASSERT(size(t) == size(begin(t), end(t)));
                    return {begin(t), end(t)};
                }

                template<typename T>
                using sirc_t = sentinel_tag_of<sentinel_t<T>, iterator_t<T>>;

                template<typename T>
                using from_container_t =
                    decltype(all_fn::from_container(
                        std::declval<T &>(),
                        sized_range_tag_of<T>(),
                        sirc_t<T>()));

                /// If it's a view already, pass it though.
                template<typename T>
                static auto from_range(T &&t) ->
                    CPP_ret(T)(
                        requires View<uncvref_t<T>>)
                {
                    return static_cast<T &&>(t);
                }

                /// If it is container-like, turn it into a view, being careful
                /// to preserve the Sized-ness of the range.
                template<typename T>
                static auto from_range(T &&t) ->
                    CPP_ret(from_container_t<T>)(
                        requires not View<uncvref_t<T>>)
                {
                    static_assert(std::is_lvalue_reference<T>::value,
                        "Cannot get a view of a temporary container");
                    return all_fn::from_container(t, sized_range_tag_of<T>(), sirc_t<T>());
                }

                // TODO handle char const * by turning it into a delimited range?

                template<typename T>
                using from_range_t = decltype(all_fn::from_range(std::declval<T>()));

            public:
                template<typename T>
                auto operator()(T &&t) const ->
                    CPP_ret(from_range_t<T>)(
                        requires Range<T>)
                {
                    return all_fn::from_range(static_cast<T &&>(t));
                }

                template<typename T>
                auto operator()(std::reference_wrapper<T> ref) const ->
                    CPP_ret(ranges::reference_wrapper<T>)(
                        requires Range<T &>)
                {
                    return ranges::ref(ref.get());
                }
            };

            /// \relates all_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(all_fn, all)

            template<typename Rng>
            using all_t =
                meta::_t<std::decay<decltype(all(std::declval<Rng>()))>>;
        }
        /// @}
    }
}

#endif
