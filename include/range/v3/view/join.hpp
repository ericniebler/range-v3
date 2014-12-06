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

#ifndef RANGES_V3_VIEW_JOIN_HPP
#define RANGES_V3_VIEW_JOIN_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/numeric.hpp> // for accumulate
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct join_view
          : range_adaptor<join_view<Rng>, Rng,
                is_infinite<Rng>::value || is_infinite<range_value_t<Rng>>::value>
        {
        private:
            CONCEPT_ASSERT(Iterable<Rng>());
            CONCEPT_ASSERT(Iterable<range_value_t<Rng>>());
            using size_type = common_type_t<range_size_t<Rng>, range_size_t<range_value_t<Rng>>>;

            friend range_access;
            view::all_t<range_value_t<Rng>> cur_;

            struct adaptor : adaptor_base
            {
            private:
                join_view *rng_;
                range_iterator_t<range_value_t<Rng>> it_;
                void satisfy(range_iterator_t<Rng> &it)
                {
                    auto &cur = rng_->cur_;
                    auto const end = ranges::end(rng_->mutable_base());
                    while(it_ == ranges::end(cur))
                    {
                        if(++it == end)
                            break;
                        cur = view::all(*it);
                        it_ = ranges::begin(cur);
                    }
                }
            public:
                using single_pass = std::true_type;
                adaptor() = default;
                adaptor(join_view &rng)
                  : rng_(&rng), it_{}
                {}
                range_iterator_t<Rng> begin(join_view &)
                {
                    auto it = ranges::begin(rng_->mutable_base());
                    auto const end = ranges::end(rng_->mutable_base());
                    if(it != end)
                    {
                        rng_->cur_ = view::all(*it);
                        it_ = ranges::begin(rng_->cur_);
                        satisfy(it);
                    }
                    return it;
                }
                bool equal(range_iterator_t<Rng> const &it, range_iterator_t<Rng> const &other_it,
                    adaptor const &other_adapt) const
                {
                    RANGES_ASSERT(rng_ == other_adapt.rng_);
                    return it == other_it && it_ == other_adapt.it_;
                }
                void next(range_iterator_t<Rng> &it)
                {
                    ++it_;
                    satisfy(it);
                }
                auto current(range_iterator_t<Rng> const &) const -> decltype(*it_)
                {
                    return *it_;
                }
            };
            adaptor begin_adaptor()
            {
                return {*this};
            }
            adaptor end_adaptor()
            {
                return {*this};
            }
            // TODO: could support const iteration if range_reference_t<Rng> is a true reference.
        public:
            join_view() = default;
            explicit join_view(Rng &&rng)
              : range_adaptor_t<join_view>{std::forward<Rng>(rng)}, cur_{}
            {}
            CONCEPT_REQUIRES(!is_infinite<Rng>() && ForwardIterable<Rng>() &&
                             SizedIterable<range_value_t<Rng>>())
            size_type size() const
            {
                return accumulate(view::transform(this->base(), ranges::size), size_type{0});
            }
        };

        namespace view
        {
            struct join_fn
            {
                template<typename Rng>
                using Concept = meta::and_<
                    Iterable<Rng>,
                    Iterable<range_value_t<Rng>>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Concept<Rng>())>
                join_view<Rng> operator()(Rng && rng) const
                {
                    return join_view<Rng>{std::forward<Rng>(rng)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Concept<Rng>())>
                void operator()(Rng &&) const
                {
                    // Must be a range of ranges
                    CONCEPT_ASSERT_MSG(Iterable<Rng>(),
                        "The argument to view::join must be a model of the Iterable concept.");
                    CONCEPT_ASSERT_MSG(Iterable<range_value_t<Rng>>(),
                        "The value type of the range passed to view::join must model the Iterable "
                        "concept.");
                }
            #endif
            };

            /// \sa `join_fn`
            /// \ingroup group-views
            constexpr view<join_fn> join{};
        }
        /// @}
    }
}

#endif
