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

#ifndef RANGES_V3_VIEW_FLATTEN_HPP
#define RANGES_V3_VIEW_FLATTEN_HPP

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

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct flatten_view
          : range_adaptor<flatten_view<Rng>, Rng,
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
                flatten_view *rng_;
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
                adaptor(flatten_view &rng)
                  : rng_(&rng), it_{}
                {}
                range_iterator_t<Rng> begin(flatten_view &)
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
            flatten_view() = default;
            explicit flatten_view(Rng &&rng)
              : range_adaptor_t<flatten_view>{std::forward<Rng>(rng)}, cur_{}
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
            struct flatten_fn : pipeable<flatten_fn>
            {
                template<typename Rng>
                flatten_view<Rng> operator()(Rng && rng) const
                {
                    // Must be a range of ranges
                    CONCEPT_ASSERT(Iterable<Rng>());
                    CONCEPT_ASSERT(Iterable<range_value_t<Rng>>());
                    return flatten_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            constexpr flatten_fn flatten{};
        }
    }
}

#endif
