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
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
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
            struct first_range_tag {};
            struct second_range_tag {};

            enum class which : unsigned short
            {
                neither, first, second
            };
        }

        template<typename Rng0, typename Rng1>
        struct join_range_view
        {
        private:
            static_assert(std::is_same<range_value_t<Rng0>, range_value_t<Rng1>>::value,
                          "Range value types must be the same to join them");
            Rng0 rng0_;
            Rng1 rng1_;

            template<bool Const>
            static detail::add_const_if_t<Rng0, Const> & declrng0();
            template<bool Const>
            static detail::add_const_if_t<Rng1, Const> & declrng1();

            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<Rng0>
                  , decltype(true ? range_category_t<Rng0>{} : range_category_t<Rng1>{})
                  , decltype(true ? *ranges::begin(declrng0<Const>())
                                  : *ranges::begin(declrng1<Const>()))
                  , decltype(true ? range_difference_t<Rng0>{} : range_difference_t<Rng1>{})
                >
            {
            private:
                friend struct join_range_view;
                friend struct ranges::iterator_core_access;
                using base_range_iterator0 = decltype(ranges::begin(declrng0<Const>()));
                using base_range_iterator1 = decltype(ranges::begin(declrng1<Const>()));
                using join_range_view_ = detail::add_const_if_t<join_range_view, Const>;

                join_range_view_ * rng_;
                union
                {
                    base_range_iterator0 it0_;
                    base_range_iterator1 it1_;
                };
                detail::which which_;

                basic_iterator(join_range_view_ &rng, base_range_iterator0 it,
                    detail::first_range_tag)
                  : rng_(&rng), it0_(std::move(it)), which_(detail::which::first)
                {}
                basic_iterator(join_range_view_ &rng, base_range_iterator1 it,
                    detail::second_range_tag)
                  : rng_(&rng), it1_(std::move(it)), which_(detail::which::second)
                {}
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
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
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

            join_range_view(Rng0 && rng0, Rng1 && rng1)
              : rng0_(rng0), rng1_(rng1)
            {}
            iterator begin()
            {
                return {*this, ranges::begin(rng0_), detail::first_range_tag{}};
            }
            iterator end()
            {
                return {*this, ranges::end(rng1_), detail::second_range_tag{}};
            }
            const_iterator begin() const
            {
                return {*this, ranges::begin(rng0_), detail::first_range_tag{}};
            }
            const_iterator end() const
            {
                return {*this, ranges::end(rng1_), detail::second_range_tag{}};
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            Rng0 & first()
            {
                return rng0_;
            }
            Rng0 const & first() const
            {
                return rng0_;
            }
            Rng1 & second()
            {
                return rng1_;
            }
            Rng1 const & second() const
            {
                return rng1_;
            }
        };

        namespace view
        {
            struct joiner : bindable<joiner>
            {
                template<typename Rng0, typename Rng1>
                static join_range_view<Rng0, Rng1> invoke(joiner, Rng0 && rng0, Rng1 && rng1)
                {
                    return {std::forward<Rng0>(rng0), std::forward<Rng1>(rng1)};
                }
            };

            RANGES_CONSTEXPR joiner join {};
        }
    }
}

#endif
