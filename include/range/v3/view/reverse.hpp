/// \file
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

#ifndef RANGES_V3_VIEW_REVERSE_HPP
#define RANGES_V3_VIEW_REVERSE_HPP

#include <utility>
#include <iterator>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Rng>
            using reverse_end_ =
                meta::if_<
                    BoundedRange<Rng>,
                    meta::nil_,
                    box<optional<range_iterator_t<Rng>>, end_tag>>;
        }

        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct reverse_view
          : view_adaptor<reverse_view<Rng>, Rng>
          , private detail::reverse_end_<Rng>
        {
        private:
            CONCEPT_ASSERT(BidirectionalRange<Rng>());
            friend range_access;

            // BoundedRange == true
            range_iterator_t<Rng> get_end_(std::true_type) const
            {
                return ranges::end(this->mutable_base());
            }
            // BoundedRange == false
            range_iterator_t<Rng> get_end_(std::false_type)
            {
                auto &end_ = ranges::get<end_tag>(*this);
                if(!end_)
                    end_ = ranges::next(
                        ranges::begin(this->mutable_base()),
                        ranges::end(this->mutable_base()));
                return *end_;
            }
            void dirty_(std::true_type) const
            {}
            void dirty_(std::false_type)
            {
                auto &end_ = ranges::get<end_tag>(*this);
                end_.reset();
            }

            // A rather convoluted implementation to avoid the problem std::reverse_iterator
            // has adapting iterators that return references to internal data.
            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                using reverse_view_t = meta::invoke<meta::add_const_if_c<IsConst>, reverse_view>;
                reverse_view_t *rng_;
            public:
                adaptor() = default;
                adaptor(reverse_view_t &rng)
                  : rng_(&rng)
                {}
                range_iterator_t<Rng> begin(reverse_view_t &rng) const
                {
                    auto it = rng.get_end_(BoundedRange<Rng>());
                    ranges::advance(it, -1, ranges::begin(rng.mutable_base()));
                    return it;
                }
                range_iterator_t<Rng> end(reverse_view_t &rng) const
                {
                    return rng.get_end_(BoundedRange<Rng>());
                }
                void next(range_iterator_t<Rng> &it) const
                {
                    if(0 != ranges::advance(it, -1, ranges::begin(rng_->mutable_base())))
                        it = rng_->get_end_(BoundedRange<Rng>());
                }
                void prev(range_iterator_t<Rng> &it) const
                {
                    if(0 != ranges::advance(it, 1, ranges::end(rng_->mutable_base())))
                        it = ranges::begin(rng_->mutable_base());
                }
                CONCEPT_REQUIRES(RandomAccessRange<Rng>())
                void advance(range_iterator_t<Rng> &it, range_difference_t<Rng> n) const
                {
                    if(n > 0)
                        ranges::advance(it, -n + 1), this->next(it);
                    else if(n < 0)
                        this->prev(it), ranges::advance(it, -n - 1);
                }
                CONCEPT_REQUIRES(
                    SizedSentinel<range_iterator_t<Rng>, range_iterator_t<Rng>>())
                range_difference_t<Rng>
                distance_to(range_iterator_t<Rng> const &here, range_iterator_t<Rng> const &there,
                    adaptor const &other_adapt) const
                {
                    (void)rng_; (void)other_adapt;
                    RANGES_ASSERT(rng_ == other_adapt.rng_);
                    if(there == ranges::end(rng_->mutable_base()))
                        return here == ranges::end(rng_->mutable_base())
                            ? 0 : (here - ranges::begin(rng_->mutable_base())) + 1;
                    else if(here == ranges::end(rng_->mutable_base()))
                        return (ranges::begin(rng_->mutable_base()) - there) - 1;
                    return here - there;
                }
            };
            adaptor<false> begin_adaptor()
            {
                return {*this};
            }
            adaptor<false> end_adaptor()
            {
                return {*this};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng const>())
            adaptor<true> begin_adaptor() const
            {
                return {*this};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng const>())
            adaptor<true> end_adaptor() const
            {
                return {*this};
            }
            // SizedRange == true
            range_size_t<Rng> size_(std::true_type)
            {
                return ranges::size(this->base());
            }
            // SizedRange == false, RandomAccessRange == true
            range_size_t<Rng> size_(std::false_type)
            {
                return ranges::iter_size(this->begin(), this->end());
            }
        public:
            reverse_view() = default;
            reverse_view(reverse_view &&that)
              : reverse_view::view_adaptor{std::move(that)}
              , detail::reverse_end_<Rng>{}
            {}
            reverse_view(reverse_view const &that)
              : reverse_view::view_adaptor{that}
              , detail::reverse_end_<Rng>{}
            {}
            explicit reverse_view(Rng rng)
              : reverse_view::view_adaptor{std::move(rng)}
              , detail::reverse_end_<Rng>{}
            {}
            reverse_view& operator=(reverse_view &&that)
            {
                this->reverse_view::view_adaptor::operator=(std::move(that));
                this->dirty_(BoundedRange<Rng>{});
                return *this;
            }
            reverse_view& operator=(reverse_view const &that)
            {
                this->reverse_view::view_adaptor::operator=(that);
                this->dirty_(BoundedRange<Rng>{});
                return *this;
            }
            CONCEPT_REQUIRES(SizedRange<Rng>() || RandomAccessRange<Rng>())
            range_size_t<Rng> size()
            {
                return this->size_(SizedRange<Rng>());
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            range_size_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct reverse_fn
            {
                template<typename Rng>
                using Concept = BidirectionalRange<Rng>;

                template<typename Rng, CONCEPT_REQUIRES_(Concept<Rng>())>
                reverse_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    return reverse_view<all_t<Rng>>{all(std::forward<Rng>(rng))};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Rng, CONCEPT_REQUIRES_(!Concept<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(BidirectionalRange<Rng>(),
                        "The object on which view::reverse operates must be a model of the "
                        "BidirectionalRange concept.");
                }
            #endif
            };

            /// \relates reverse_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<reverse_fn>, reverse)
        }
        /// @}
    }
}

#endif
