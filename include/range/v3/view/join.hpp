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

#ifndef RANGES_V3_VIEW_JOIN_HPP
#define RANGES_V3_VIEW_JOIN_HPP

#include <new>
#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/detail/advance_bounded.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            enum class which : unsigned short
            {
                neither, first, second
            };
        }

        template<typename InputRange0, typename InputRange1>
        struct join_range_view : private range_base
        {
        private:
            static_assert(std::is_same<range_value_t<InputRange0>,
                                       range_value_t<InputRange1>>::value,
                          "Range value types must be the same to join them");
            InputRange0 rng0_;
            InputRange1 rng1_;

            template<bool Const>
            using base_range0_t = detail::add_const_if_t<InputRange0, Const>;

            template<bool Const>
            using base_range1_t = detail::add_const_if_t<InputRange1, Const>;

            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<InputRange0>
                  , decltype(true ? range_category_t<InputRange0>{}
                                  : range_category_t<InputRange1>{})
                  , decltype(true ? std::declval<range_reference_t<base_range0_t<Const>>>()
                                  : std::declval<range_reference_t<base_range1_t<Const>>>())
                  , decltype(true ? range_difference_t<InputRange0>{}
                                  : range_difference_t<InputRange1>{})
                >
            {
            private:
                template<bool OtherConst>
                friend struct basic_iterator;
                friend struct join_range_view;
                friend struct ranges::iterator_core_access;
                using base_range_iterator0 = range_iterator_t<base_range0_t<Const>>;
                using base_range_iterator1 = range_iterator_t<base_range1_t<Const>>;
                using join_range_view_ = detail::add_const_if_t<join_range_view, Const>;

                join_range_view_ * rng_;
                union
                {
                    base_range_iterator0 it0_;
                    base_range_iterator1 it1_;
                };
                detail::which which_;

                basic_iterator(join_range_view_ &rng, begin_tag)
                  : rng_(&rng), it0_(ranges::begin(rng.rng0_)), which_(detail::which::first)
                {}
                basic_iterator(join_range_view_ &rng, end_tag)
                  : rng_(&rng), it1_(ranges::end(rng.rng1_)), which_(detail::which::second)
                {}
                typename basic_iterator::reference dereference() const
                {
                    switch(which_)
                    {
                    case detail::which::first:
                        return *it0_;
                    case detail::which::second:
                        RANGES_ASSERT(it1_ != ranges::end(rng_->rng1_));
                        return *it1_;
                    default:
                        RANGES_ASSERT(!"Attempt to dereference an invalid join_range_view iterator");
                        return *it1_;
                    }
                }
                template<bool OtherConst>
                bool equal(basic_iterator<OtherConst> const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    if(which_ != that.which_)
                        return false;
                    switch(which_)
                    {
                    case detail::which::first:
                        return it0_ == that.it0_;
                    case detail::which::second:
                        return it1_ == that.it1_;
                    default:
                        RANGES_ASSERT(!"Attempt to compare invalid join_range_view iterators");
                        return true;
                    }
                }
                void increment()
                {
                    switch(which_)
                    {
                    case detail::which::first:
                        if(++it0_ == ranges::end(rng_->rng0_))
                        {
                            it0_.~base_range_iterator0();
                            which_ = detail::which::neither;
                            ::new((void*)&it1_) base_range_iterator1(ranges::begin(rng_->rng1_));
                            which_ = detail::which::second;
                        }
                        break;
                    case detail::which::second:
                        RANGES_ASSERT(it1_ != ranges::end(rng_->rng1_));
                        ++it1_;
                        break;
                    default:
                        RANGES_ASSERT(!"Attempt to advance an invalid join_range_view iterator");
                        break;
                    }
                }
                void decrement()
                {
                    switch(which_)
                    {
                    case detail::which::first:
                        RANGES_ASSERT(it0_ != ranges::begin(rng_->rng0_));
                        --it0_;
                        break;
                    case detail::which::second:
                        if(it1_ != ranges::begin(rng_->rng1_))
                            --it1_;
                        else
                        {
                            it1_.~base_range_iterator1();
                            which_ = detail::which::neither;
                            ::new((void*)&it0_) base_range_iterator0(std::prev(ranges::end(rng_->rng0_)));
                            which_ = detail::which::first;
                        }
                        break;
                    default:
                        RANGES_ASSERT(!"Attempt to decrement an invalid join_range_view iterator");
                        break;
                    }
                }
                void advance(typename basic_iterator::difference_type n)
                {
                    switch(which_)
                    {
                    case detail::which::first:
                        if(n > 0)
                        {
                            n = detail::advance_bounded(it0_, n, ranges::end(rng_->rng0_));
                            if(it0_ == ranges::end(rng_->rng0_))
                            {
                                it0_.~base_range_iterator0();
                                which_ = detail::which::neither;
                                ::new((void*)&it1_) base_range_iterator1(ranges::begin(rng_->rng1_));
                                which_ = detail::which::second;
                                std::advance(it1_, n);
                            }
                        }
                        else
                            std::advance(it0_, n);
                        break;
                    case detail::which::second:
                        if(n < 0)
                        {
                            n = detail::advance_bounded(it1_, n, ranges::begin(rng_->rng1_));
                            if(n < 0)
                            {
                                it1_.~base_range_iterator1();
                                which_ = detail::which::neither;
                                ::new((void*)&it0_) base_range_iterator0(ranges::end(rng_->rng0_));
                                which_ = detail::which::first;
                                std::advance(it0_, n);
                            }
                        }
                        else
                            std::advance(it1_, n);
                        break;
                    default:
                        RANGES_ASSERT(!"Attempt to advance an invalid join_range_view iterator");
                        break;
                    }
                }
                template<bool OtherConst>
                typename basic_iterator::difference_type
                distance_to(basic_iterator<OtherConst> const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    switch(which_)
                    {
                    case detail::which::first:
                        switch(that.which_)
                        {
                        case detail::which::first:
                            return that.it0_ - it0_;
                        case detail::which::second:
                            return (ranges::end(rng_->rng0_) - it0_) +
                                   (that.it1_ - ranges::begin(rng_->rng1_));
                        default:
                            RANGES_ASSERT(!"Attempt to use an invalid join_range_view iterator");
                            return 0;
                        }
                    case detail::which::second:
                        switch(that.which_)
                        {
                        case detail::which::first:
                            return (ranges::begin(rng_->rng1_) - it1_) +
                                   (that.it0_ - ranges::end(rng_->rng0_));
                            return 0;
                        case detail::which::second:
                            return that.it1_ - it1_;
                        default:
                            RANGES_ASSERT(!"Attempt to use an invalid join_range_view iterator");
                            return 0;
                        }
                    default:
                        RANGES_ASSERT(!"Attempt to use an invalid join_range_view iterator");
                        return 0;
                    }
                }
                void clean()
                    //noexcept(std::is_nothrow_destructible<base_range_iterator0>::value &&
                    //         std::is_nothrow_destructible<base_range_iterator1>::value)
                {
                    switch(which_)
                    {
                    case detail::which::first:
                        it0_.~base_range_iterator0();
                        which_ = detail::which::neither;
                        break;
                    case detail::which::second:
                        it1_.~base_range_iterator1();
                        which_ = detail::which::neither;
                        break;
                    default:
                        break;
                    }
                }
            public:
                constexpr basic_iterator()
                    noexcept(std::is_nothrow_default_constructible<base_range_iterator0>::value)
                  : rng_{}, it0_{}, which_(detail::which::first)
                {}
                basic_iterator(basic_iterator const &that)
                  : basic_iterator{}
                {
                    *this = that;
                }
                basic_iterator(basic_iterator &&that)
                    noexcept(std::is_nothrow_default_constructible<base_range_iterator0>::value &&
                             //std::is_nothrow_destructible<base_range_iterator0>::value &&
                             //std::is_nothrow_destructible<base_range_iterator1>::value &&
                             std::is_nothrow_move_constructible<base_range_iterator0>::value &&
                             std::is_nothrow_move_constructible<base_range_iterator1>::value)
                  : basic_iterator{}
                {
                    *this = std::move(that);
                }
                ~basic_iterator()
                    //noexcept(std::is_nothrow_destructible<base_range_iterator0>::value &&
                    //         std::is_nothrow_destructible<base_range_iterator1>::value)
                {
                    clean();
                }
                basic_iterator & operator=(basic_iterator const &that)
                {
                    clean();
                    rng_ = that.rng_;
                    switch(that.which_)
                    {
                    case detail::which::first:
                        ::new((void*)&it0_) base_range_iterator0(that.it0_);
                        which_ = detail::which::first;
                        break;
                    case detail::which::second:
                        ::new((void*)&it1_) base_range_iterator1(that.it1_);
                        which_ = detail::which::second;
                        break;
                    default:
                        RANGES_ASSERT(!"Copying invalid join_range_view iterator");
                        which_ = detail::which::neither;
                        break;
                    }
                    return *this;
                }
                basic_iterator & operator=(basic_iterator &&that)
                    noexcept(//std::is_nothrow_destructible<base_range_iterator0>::value &&
                             //std::is_nothrow_destructible<base_range_iterator1>::value &&
                             std::is_nothrow_move_constructible<base_range_iterator0>::value &&
                             std::is_nothrow_move_constructible<base_range_iterator1>::value)
                {
                    clean();
                    rng_ = that.rng_;
                    switch(that.which_)
                    {
                    case detail::which::first:
                        ::new((void*)&it0_) base_range_iterator0(std::move(that).it0_);
                        which_ = detail::which::first;
                        break;
                    case detail::which::second:
                        ::new((void*)&it1_) base_range_iterator1(std::move(that).it1_);
                        which_ = detail::which::second;
                        break;
                    default:
                        RANGES_ASSERT(!"Copying invalid join_range_view iterator");
                        which_ = detail::which::neither;
                        break;
                    }
                    return *this;
                }
                // For iterator -> const_iterator conversion
                template<bool OtherConst, enable_if_t<!OtherConst> = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : basic_iterator{}
                {
                    clean();
                    rng_ = that.rng_;
                    switch(that.which_)
                    {
                    case detail::which::first:
                        ::new((void*)&it0_) base_range_iterator0(std::move(that).it0_);
                        which_ = detail::which::first;
                        break;
                    case detail::which::second:
                        ::new((void*)&it1_) base_range_iterator1(std::move(that).it1_);
                        which_ = detail::which::second;
                        break;
                    default:
                        RANGES_ASSERT(!"Copying invalid join_range_view iterator");
                        which_ = detail::which::neither;
                        break;
                    }
                }
            };
        public:
            using iterator       = basic_iterator<false>;
            using const_iterator = basic_iterator<true>;

            join_range_view(InputRange0 && rng0, InputRange1 && rng1)
              : rng0_(rng0), rng1_(rng1)
            {}
            iterator begin()
            {
                return {*this, begin_tag{}};
            }
            const_iterator begin() const
            {
                return {*this, begin_tag{}};
            }
            iterator end()
            {
                return {*this, end_tag{}};
            }
            const_iterator end() const
            {
                return {*this, end_tag{}};
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            InputRange0 & first()
            {
                return rng0_;
            }
            InputRange0 const & first() const
            {
                return rng0_;
            }
            InputRange1 & second()
            {
                return rng1_;
            }
            InputRange1 const & second() const
            {
                return rng1_;
            }
            CONCEPT_REQUIRES(SizedIterable<InputRange0>() &&
                             SizedIterable<InputRange1>())
            range_size_t<InputRange0> size() const
            {
                return ranges::size(rng0_) + ranges::size(rng1_);
            }
        };

        namespace view
        {
            struct joiner : bindable<joiner>
            {
                template<typename InputRange0, typename InputRange1>
                static join_range_view<InputRange0, InputRange1>
                invoke(joiner, InputRange0 && rng0, InputRange1 && rng1)
                {
                    // TODO Make join_range_view work with Iterables instead of Ranges
                    CONCEPT_ASSERT(ranges::Range<InputRange0>());
                    CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputRange0>>());
                    CONCEPT_ASSERT(ranges::Range<InputRange1>());
                    CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputRange1>>());
                    return {std::forward<InputRange0>(rng0), std::forward<InputRange1>(rng1)};
                }
            };

            RANGES_CONSTEXPR joiner join {};
        }
    }
}

#endif
