// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_INDIRECT_HPP
#define RANGES_V3_VIEW_INDIRECT_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputRange>
        struct indirect_range_view
            // TODO desparately need a range adaptor
          : range_facade<indirect_range_view<InputRange>>
        {
        private:
            friend struct range_core_access;
            InputRange rng_;
            using base_range_iterator = range_iterator_t<InputRange const>;

            struct impl
            {
                base_range_iterator it_;
                constexpr impl()
                  : it_{}
                {}
                impl(base_range_iterator it)
                  : it_(std::move(it))
                {}
                auto current() const -> decltype(**it_)
                {
                    return **it_;
                }
                // UGH, these should be automatically generated with an adaptor class
                bool equal(impl const &that) const
                {
                    return it_ == that.it_;
                }
                void next()
                {
                    ++it_;
                }
                template<typename Rng = InputRange,
                    CONCEPT_REQUIRES(BidirectionalIterable<Rng>())>
                void prev()
                {
                    --it_;
                }
                template<typename Rng = InputRange,
                    CONCEPT_REQUIRES(RandomAccessIterable<Rng>())>
                void advance(range_difference_t<Rng> n)
                {
                    it_ += n;
                }
                template<typename Rng = InputRange,
                    CONCEPT_REQUIRES(RandomAccessIterable<Rng>())>
                range_difference_t<Rng> distance_to(impl const &that)
                {
                    return that.it_ - it_;
                }
            };
            impl begin_impl() const
            {
                return {ranges::begin(base())};
            }
            impl end_impl() const
            {
                return {ranges::end(base())};
            }
        public:
            explicit indirect_range_view(InputRange && rng)
              : rng_(std::forward<InputRange>(rng))
            {}
            InputRange & base()
            {
                return rng_;
            }
            InputRange const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct indirecter : bindable<indirecter>, pipeable<indirecter>
            {
                template<typename InputRange>
                static indirect_range_view<InputRange>
                invoke(indirecter, InputRange && rng)
                {
                    CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                    return indirect_range_view<InputRange>{std::forward<InputRange>(rng)};
                }
            };

            RANGES_CONSTEXPR indirecter indirect{};
        }
    }
}

#endif
