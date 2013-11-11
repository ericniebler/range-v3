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

#ifndef BOOST_RANGE_V3_ADAPTOR_TRANSFORM_HPP
#define BOOST_RANGE_V3_ADAPTOR_TRANSFORM_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/v3/detail/adl_begin_end.hpp>

namespace boost
{
    namespace range
    {
        inline namespace v3
        {
            template<typename Rng, typename Fun>
            struct transform_range
            {
            private:
                Rng rng_;
                Fun fun_;

                template<typename TfxRng>
                struct basic_iterator
                  : boost::iterator_facade<
                        basic_iterator<TfxRng>
                      , typename std::remove_reference<
                            decltype(std::declval<TfxRng &>().fun_(
                                *detail::adl_begin(std::declval<TfxRng &>().rng_)))
                        >::type
                      , typename std::iterator_traits<
                            decltype(detail::adl_begin(std::declval<TfxRng &>().rng_))
                        >::iterator_category
                      , decltype(std::declval<TfxRng &>().fun_(
                            *detail::adl_begin(std::declval<TfxRng &>().rng_)))
                      , typename std::iterator_traits<
                            decltype(detail::adl_begin(std::declval<TfxRng &>().rng_))
                        >::difference_type
                    >
                {
                    basic_iterator()
                      : rng_{}, it_{}
                    {}
                    // For iterator -> const_iterator conversion
                    template<typename OtherTfxRng,
                             typename = typename std::enable_if<
                                            !std::is_const<OtherTfxRng>::value>::type>
                    basic_iterator(basic_iterator<OtherTfxRng> that)
                      : rng_(that.rng_), it_(std::move(that.it_))
                    {}
                private:
                    friend struct transform_range;
                    friend class boost::iterator_core_access;
                    using base_range_iterator = decltype(detail::adl_begin(std::declval<TfxRng &>().rng_));

                    TfxRng *rng_;
                    base_range_iterator it_;

                    basic_iterator(TfxRng &rng, base_range_iterator it)
                      : rng_(&rng), it_(std::move(it))
                    {}
                    void increment()
                    {
                        BOOST_ASSERT(it_ != detail::adl_end(rng_->rng_));
                        ++it_;
                    }
                    void decrement()
                    {
                        BOOST_ASSERT(it_ != detail::adl_end(rng_->rng_));
                        --it_;
                    }
                    void advance(typename basic_iterator::difference_type n)
                    {
                        it_ += n;
                    }
                    typename basic_iterator::difference_type distance_to(basic_iterator const &that) const
                    {
                        BOOST_ASSERT(rng_ == that.rng_);
                        return that.it_ - it_;
                    }
                    bool equal(basic_iterator const &that) const
                    {
                        BOOST_ASSERT(rng_ == that.rng_);
                        return it_ == that.it_;
                    }
                    auto dereference() const -> decltype(rng_->fun_(*it_))
                    {
                        BOOST_ASSERT(it_ != detail::adl_end(rng_->rng_));
                        return rng_->fun_(*it_);
                    }
                };

            public:
                using iterator       = basic_iterator<transform_range>;
                using const_iterator = basic_iterator<transform_range const>;

                transform_range(Rng && rng, Fun fun)
                  : rng_(std::forward<Rng>(rng)), fun_(std::move(fun))
                {}
                iterator begin()
                {
                    return {*this, detail::adl_begin(rng_)};
                }
                iterator end()
                {
                    return {*this, detail::adl_end(rng_)};
                }
                const_iterator begin() const
                {
                    return {*this, detail::adl_begin(rng_)};
                }
                const_iterator end() const
                {
                    return {*this, detail::adl_end(rng_)};
                }
                bool operator!() const
                {
                    return begin() == end();
                }
                explicit operator bool() const
                {
                    return begin() != end();
                }
                Rng & base()
                {
                    return rng_;
                }
                Rng const & base() const
                {
                    return rng_;
                }
            };

            constexpr struct transformer
            {
            private:
                template<typename Fun>
                class transformer1
                {
                    Fun fun_;
                public:
                    transformer1(Fun fun)
                      : fun_(std::move(fun))
                    {}
                    template<typename Rng>
                    friend transform_range<Rng, Fun> operator|(Rng && rng, transformer1 && fun)
                    {
                        return {std::forward<Rng>(rng), std::move(fun.fun_)};
                    }
                    template<typename Rng>
                    friend transform_range<Rng, Fun> operator|(Rng && rng, transformer1 const & fun)
                    {
                        return {std::forward<Rng>(rng), fun.fun_};
                    }
                };
            public:
                template<typename Rng, typename Fun>
                transform_range<Rng, Fun> operator()(Rng && rng, Fun fun) const
                {
                    return {std::forward<Rng>(rng), std::move(fun)};
                }
                template<typename Fun>
                constexpr transformer1<Fun> operator()(Fun fun) const
                {
                    return fun;
                }
            } transform {};
        }
    }
}

#endif
