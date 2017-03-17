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
        template<typename Rng>
        struct shared_view
          : view_interface<shared_view<Rng>,
            range_cardinality<Rng>::value>
        {
        private:
            // shared storage
            std::shared_ptr<Rng> rng_ptr_;

            // range of the shared storage
            using range_t = view::all_t<Rng>;
            range_t rng_;


        public:
            shared_view() = default;

            // construct from a shared_ptr
            RANGES_NDEBUG_CONSTEXPR shared_view(std::shared_ptr<Rng> pt)
              : rng_ptr_(std::move(pt)),
                rng_{view::all(*rng_ptr_)}
            {}

            // construct from a range rvalue
            RANGES_NDEBUG_CONSTEXPR shared_view(Rng && t)
              : rng_ptr_{std::make_shared<Rng>(std::move(t))},
                rng_{view::all(*rng_ptr_)}
            {}

            // use the stored range's begin and end
            iterator_t<range_t> begin() const
            {
                return ranges::begin(rng_);
            }
            sentinel_t<range_t> end() const
            {
                return ranges::end(rng_);
            }

            // use the const-most size() function provided by the range
            CONCEPT_REQUIRES(SizedRange<const range_t>())
            range_size_type_t<range_t> size() const
            {
                return ranges::size(rng_);
            }
            CONCEPT_REQUIRES(SizedRange<range_t>() && !SizedRange<const range_t>())
            range_size_type_t<range_t> size()
            {
                return ranges::size(rng_);
            }

            // shared storage access
            const std::shared_ptr<Rng> & get_shared() const
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
                template<typename Rng,
                    CONCEPT_REQUIRES_(Range<Rng>())>
                shared_view<Rng> operator()(std::shared_ptr<Rng> pt) const
                {
                    return {std::move(pt)};
                }

#ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Range<Rng>())>
                void operator()(std::shared_ptr<Rng>) const
                {
                    CONCEPT_ASSERT_MSG(Range<Rng>(),
                        "The object on which view::shared operates must be a "
                        "model of the Range concept.");
                }
#endif

                template<typename Rng,
                    CONCEPT_REQUIRES_(Range<Rng>()),
                    typename std::enable_if<std::is_rvalue_reference<Rng&&>::value, bool>::type = 0>
                shared_view<typename std::remove_reference<Rng>::type> operator()(Rng && t) const
                {
                    return {std::move(t)};
                }

#ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(Range<Rng>()),
                    typename std::enable_if<!std::is_rvalue_reference<Rng&&>::value, bool>::type E = 0>
                void operator()(Rng &&) const
                {
                    static_assert(E, "view::shared needs an rvalue reference"
                                     "to build a shared object.");
                }

                template<typename Rng,
                    CONCEPT_REQUIRES_(!Range<Rng>())>
                void operator()(Rng) const
                {
                    CONCEPT_ASSERT_MSG(Range<Rng>(),
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
