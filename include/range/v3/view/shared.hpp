/// \file
// Range v3 library
//
//  Copyright Filip Matzner 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_SHARED_HPP
#define RANGES_V3_VIEW_SHARED_HPP

#include <memory>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename T>
        struct shared_view
          : view_interface<shared_view<T>>
        {
        private:
            // shared storage
            std::shared_ptr<T> rng_ptr_;

            // range of the shared storage
            using range_t = view::all_t<T>;
            range_t rng_;


        public:
            shared_view() = default;

            // construct from a shared_ptr
            RANGES_NDEBUG_CONSTEXPR shared_view(std::shared_ptr<T> pt)
              : rng_ptr_(std::move(pt)),
                rng_{view::all(*rng_ptr_)}
            {}

            // construct from a range rvalue
            RANGES_NDEBUG_CONSTEXPR shared_view(T && t)
              : rng_ptr_{std::make_shared<T>(std::move(t))},
                rng_{view::all(*rng_ptr_)}
            {}

            // use the stored range's begin and end
            range_iterator_t<range_t> begin() const
            {
                return ranges::begin(rng_);
            }
            range_sentinel_t<range_t> end() const
            {
                return ranges::end(rng_);
            }

            // use the const-most size() function provided by the range
            CONCEPT_REQUIRES(SizedRange<const range_t>())
            range_size_t<range_t> size() const
            {
                return ranges::size(rng_);
            }
            CONCEPT_REQUIRES(SizedRange<range_t>() && !SizedRange<const range_t>())
            range_size_t<range_t> size()
            {
                return ranges::size(rng_);
            }

            // shared storage access
            const std::shared_ptr<T> & get_shared() const
            {
                return rng_ptr_;
            }
        };

        /// \relates all
        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct shared_fn : pipeable<shared_fn>
            {
            public:
                template<typename T,
                    CONCEPT_REQUIRES_(Range<T>())>
                shared_view<T> operator()(std::shared_ptr<T> pt) const
                {
                    return {std::move(pt)};
                }

#ifndef RANGES_DOXYGEN_INVOKED
                template<typename T,
                    CONCEPT_REQUIRES_(!Range<T>())>
                void operator()(std::shared_ptr<T>) const
                {
                    CONCEPT_ASSERT_MSG(Range<T>(),
                        "The object on which view::shared operates must be a "
                        "model of the Range concept.");
                }
#endif

                template<typename T,
                    CONCEPT_REQUIRES_(Range<T>()),
                    typename std::enable_if<std::is_rvalue_reference<T&&>::value, bool>::type = 0>
                shared_view<typename std::remove_reference<T>::type> operator()(T && t) const
                {
                    return {std::move(t)};
                }

#ifndef RANGES_DOXYGEN_INVOKED
                template<typename T,
                    CONCEPT_REQUIRES_(Range<T>()),
                    typename std::enable_if<!std::is_rvalue_reference<T&&>::value, bool>::type E = 0>
                void operator()(T &&) const
                {
                    static_assert(E, "view::shared needs an rvalue reference"
                                     "to build a shared object.");
                }

                template<typename T,
                    CONCEPT_REQUIRES_(!Range<T>())>
                void operator()(T) const
                {
                    CONCEPT_ASSERT_MSG(Range<T>(),
                        "The object on which view::shared operates must be a "
                        "model of the Range concept.");
                }
#endif
            };

            /// \relates all_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(shared_fn, shared)

            template<typename Rng>
            using shared_t =
                meta::_t<std::decay<decltype(shared(std::declval<Rng>()))>>;

        } // namespace view
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
