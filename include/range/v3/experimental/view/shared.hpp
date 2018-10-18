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
#ifndef RANGES_V3_EXPERIMENTAL_VIEW_SHARED_HPP
#define RANGES_V3_EXPERIMENTAL_VIEW_SHARED_HPP

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
        namespace experimental
        {
            template<typename Rng>
            struct shared_view
              : view_interface<shared_view<Rng>,
                range_cardinality<Rng>::value>
            {
            private:
                // shared storage
                std::shared_ptr<Rng> rng_ptr_;

            public:
                shared_view() = default;

                // construct from a range rvalue
                explicit shared_view(Rng && t)
                  : rng_ptr_{std::make_shared<Rng>(std::move(t))}
                {}

                // use the stored range's begin and end
                iterator_t<Rng> begin() const
                {
                    return ranges::begin(*rng_ptr_);
                }
                sentinel_t<Rng> end() const
                {
                    return ranges::end(*rng_ptr_);
                }

                CONCEPT_REQUIRES(SizedRange<Rng>())
                range_size_type_t<Rng> size() const
                {
                    return ranges::size(*rng_ptr_);
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
#ifndef RANGES_DOXYGEN_INVOKED
                    template<typename Rng,
                        CONCEPT_REQUIRES_(!Range<Rng>())>
                    void operator()(std::shared_ptr<Rng>) const
                    {
                        CONCEPT_ASSERT_MSG(Range<Rng>(),
                            "The object on which view::shared operates must be "
                            "a model of the Range concept.");
                    }
#endif

                    template<typename Rng,
                        CONCEPT_REQUIRES_(Range<Rng>()
                                          && !View<Rng>()
                                          && !std::is_reference<Rng>::value)>
                    shared_view<Rng> operator()(Rng && t) const
                    {
                        return shared_view<Rng>{std::move(t)};
                    }

#ifndef RANGES_DOXYGEN_INVOKED
                    template<typename Rng,
                        CONCEPT_REQUIRES_(!Range<Rng>()
                                          || View<Rng>()
                                          || std::is_reference<Rng>::value)>
                    void operator()(Rng &&) const
                    {
                        CONCEPT_ASSERT_MSG(Range<Rng>(),
                            "The object on which view::shared operates must be "
                            "a model of the Range concept.");
                        CONCEPT_ASSERT_MSG(!View<Rng>(),
                            "view::shared cannot be constructed from a view. "
                            "Please copy the original view instead.");
                        CONCEPT_ASSERT_MSG(!std::is_reference<Rng>::value,
                            "view::shared needs an rvalue to build a shared "
                            "object.");
                    }
#endif
                };

                /// \relates all_fn
                /// \ingroup group-views
                RANGES_INLINE_VARIABLE(shared_fn, shared)

                template<typename Rng>
                using shared_t =
                    detail::decay_t<decltype(shared(std::declval<Rng>()))>;

            } // namespace view
            /// @}
        } // namespace experimental
    } // namespace v3
} // namespace ranges

#endif // include guard
