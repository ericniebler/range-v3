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

#ifndef RANGE_V3_JOIN_RANGE_HPP
#define RANGE_V3_JOIN_RANGE_HPP

#include <new>
#include <cassert>
#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_facade.hpp>

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

            template<typename Iter, typename Diff>
            void advance_fwd_bounded(Iter & it, Iter end, Diff & n, std::bidirectional_iterator_tag)
            {
                while(n > 0 && it != end)
                {
                    ++it;
                    --n;
                }
            }

            template<typename Iter, typename Diff>
            void advance_fwd_bounded(Iter & it, Iter end, Diff & n, std::random_access_iterator_tag)
            {
                auto const room = end - it;
                if(room < n)
                {
                    it = end;
                    n -= room;
                }
                else
                {
                    it += n;
                    n = 0;
                }
            }

            template<typename Iter, typename Diff>
            void advance_fwd_bounded(Iter & it, Iter end, Diff & n)
            {
                assert(n >= 0);
                advance_fwd_bounded(it, end, n, typename std::iterator_traits<Iter>::iterator_category{});
            }

            template<typename Iter, typename Diff>
            void advance_back_bounded(Iter & it, Iter begin, Diff & n, std::bidirectional_iterator_tag)
            {
                while(n < 0 && it != begin)
                {
                    --it;
                    ++n;
                }
            }

            template<typename Iter, typename Diff>
            void advance_back_bounded(Iter & it, Iter begin, Diff & n, std::random_access_iterator_tag)
            {
                auto const room = -(it - begin);
                if(n < room)
                {
                    it = begin;
                    n -= room;
                }
                else
                {
                    it += n;
                    n = 0;
                }
            }

            template<typename Iter, typename Diff>
            void advance_back_bounded(Iter & it, Iter begin, Diff & n)
            {
                assert(n <= 0);
                advance_back_bounded(it, begin, n, typename std::iterator_traits<Iter>::iterator_category{});
            }
        }

        template<typename Rng0, typename Rng1>
        struct join_range
        {
        private:
            static_assert(std::is_same<range_value_t<Rng0>, range_value_t<Rng1>>::value,
                          "Range value types must be the same to join them");
            Rng0 rng0_;
            Rng1 rng1_;

            // FltRng is either join_range or join_range const.
            template<typename JoinRng>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<JoinRng>
                  , range_value_t<Rng0>
                  , decltype(true ? range_category_t<Rng0>{} : range_category_t<Rng1>{})
                  , decltype(true ? *ranges::begin(std::declval<JoinRng &>().rng0_)
                                  : *ranges::begin(std::declval<JoinRng &>().rng1_))
                  , decltype(true ? range_difference_t<Rng0>{} : range_difference_t<Rng1>{})
                >
            {
            private:
                friend struct join_range;
                friend struct ranges::iterator_core_access;
                using base_range_iterator0 = decltype(ranges::begin(std::declval<JoinRng &>().rng0_));
                using base_range_iterator1 = decltype(ranges::begin(std::declval<JoinRng &>().rng1_));

                JoinRng * rng_;
                union
                {
                    base_range_iterator0 it0_;
                    base_range_iterator1 it1_;
                };
                detail::which which_;

                basic_iterator(JoinRng &rng, base_range_iterator0 it, detail::first_range_tag)
                  : rng_(&rng), it0_(std::move(it)), which_(detail::which::first)
                {}
                basic_iterator(JoinRng &rng, base_range_iterator1 it, detail::second_range_tag)
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
                        assert(it1_ != ranges::end(rng_->rng1_));
                        ++it1_;
                        break;
                    default:
                        assert(!"Attempt to advance an invalid join_range iterator");
                        break;
                    }
                }
                void decrement()
                {
                    switch(which_)
                    {
                    case detail::which::first:
                        assert(it0_ != ranges::begin(rng_->rng0_));
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
                        assert(!"Attempt to decrement an invalid join_range iterator");
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
                            detail::advance_fwd_bounded(it0_, ranges::end(rng_->rng0_), n);
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
                            detail::advance_back_bounded(it1_, ranges::begin(rng_->rng1_), n);
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
                        assert(!"Attempt to advance an invalid join_range iterator");
                        break;
                    }
                }
                typename basic_iterator::difference_type distance_to(basic_iterator const &that) const
                {
                    assert(rng_ == that.rng_);
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
                            assert(!"Attempt to use an invalid join_range iterator");
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
                            assert(!"Attempt to use an invalid join_range iterator");
                            return 0;
                        }
                    default:
                        assert(!"Attempt to use an invalid join_range iterator");
                        return 0;
                    }
                }
                bool equal(basic_iterator const &that) const
                {
                    assert(rng_ == that.rng_);
                    if(which_ != that.which_)
                        return false;
                    switch(which_)
                    {
                    case detail::which::first:
                        return it0_ == that.it0_;
                    case detail::which::second:
                        return it1_ == that.it1_;
                    default:
                        assert(!"Attempt to compare invalid join_range iterators");
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
                        assert(it1_ != ranges::end(rng_->rng1_));
                        return *it1_;
                    default:
                        assert(!"Attempt to dereference an invalid join_range iterator");
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
                basic_iterator()
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
                        assert(!"Copying invalid join_range iterator");
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
                        assert(!"Copying invalid join_range iterator");
                        which_ = detail::which::neither;
                        break;
                    }
                    return *this;
                }
                // For iterator -> const_iterator conversion
                template<typename OtherFltRng,
                         typename = typename std::enable_if<
                                        !std::is_const<OtherFltRng>::value>::type>
                basic_iterator(basic_iterator<OtherFltRng> that)
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
                        assert(!"Copying invalid join_range iterator");
                        which_ = detail::which::neither;
                        break;
                    }
                }
            };
        public:
            using iterator       = basic_iterator<join_range>;
            using const_iterator = basic_iterator<join_range const>;

            join_range(Rng0 && rng0, Rng1 && rng1)
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

        struct joiner
        {
            template<typename Rng0, typename Rng1>
            join_range<Rng0, Rng1> operator()(Rng0 && rng0, Rng1 && rng1) const
            {
                return {std::forward<Rng0>(rng0), std::forward<Rng1>(rng1)};
            }
        };

        constexpr bindable<joiner> join {};
    }
}

#endif
