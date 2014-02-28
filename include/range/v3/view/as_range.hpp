//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_VIEW_AS_RANGE_HPP
#define RANGES_V3_VIEW_AS_RANGE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Iterable>
        struct as_range_view
          : range_adaptor<as_range_view<Iterable>, Iterable, is_infinite<Iterable>::value>
        {
        private:
            friend range_core_access;

            template<bool Const>
            struct impl
            {
                using base_iterable_t = detail::add_const_if_t<Iterable, Const>;
                using base_iterator_t = range_iterator_t<base_iterable_t>;
                using base_sentinel_t = range_sentinel_t<base_iterable_t>;

                base_iterator_t it_;
                base_sentinel_t se_;
                bool is_sentinel_;

                // For iterator->const_iterator conversions
                CONCEPT_REQUIRES(!Const)
                operator impl<!Const>() const
                {
                    return {it_, se_, is_sentinel_};
                }
                auto current() const -> decltype(*it_)
                {
                    RANGES_ASSERT(!is_sentinel_ && it_ != se_);
                    return *it_;
                }
                template<bool OtherConst>
                bool equal(impl<OtherConst> const &that) const
                {
                    if(is_sentinel_ && that.is_sentinel_)
                        return true;
                    if(is_sentinel_ && !that.is_sentinel_)
                        return that.it_ == se_;
                    if(that.is_sentinel_ && !is_sentinel_)
                        return it_ == that.se_;
                    return it_ == that.it_;
                }
                void next()
                {
                    RANGES_ASSERT(!is_sentinel_ && it_ != se_);
                    ++it_;
                }
                CONCEPT_REQUIRES(BidirectionalIterable<Iterable>())
                void prev()
                {
                    clean();
                    --it_;
                }
                CONCEPT_REQUIRES(RandomAccessIterable<Iterable>())
                void advance(range_difference_t<Iterable> n)
                {
                    clean();
                    it_ += n;
                }
                template<bool OtherConst,
                    CONCEPT_REQUIRES_(RandomAccessIterable<Iterable>())>
                range_difference_t<Iterable> distance_to(impl<OtherConst> const &that) const
                {
                    clean();
                    that.clean();
                    return that.it_ - it_;
                }
                void clean()
                {
                    if(is_sentinel_)
                    {
                        while(it_ != se_)
                            ++it_;
                        is_sentinel_ = false;
                    }
                }
            };
            impl<false> begin_impl()
            {
                return {ranges::begin(this->base()), ranges::end(this->base()), false};
            }
            impl<true> begin_impl() const
            {
                return {ranges::begin(this->base()), ranges::end(this->base()), false};
            }
            impl<false> end_impl()
            {
                return {ranges::begin(this->base()), ranges::end(this->base()), true};
            }
            impl<true> end_impl() const
            {
                return {ranges::begin(this->base()), ranges::end(this->base()), true};
            }
        public:
            explicit as_range_view(Iterable && rng)
              : range_adaptor_t<as_range_view>(std::forward<Iterable>(rng))
            {}
        };

        namespace view
        {
            struct as_ranger : bindable<as_ranger>, pipeable<as_ranger>
            {
                template<typename InputIterable>
                static as_range_view<InputIterable>
                invoke(as_ranger, InputIterable && rng)
                {
                    CONCEPT_ASSERT(ranges::InputIterable<InputIterable>());
                    CONCEPT_ASSERT(!ranges::Range<InputIterable>());
                    return as_range_view<InputIterable>{std::forward<InputIterable>(rng)};
                }
            };

            RANGES_CONSTEXPR as_ranger as_range{};
        }
    }
}

#endif
