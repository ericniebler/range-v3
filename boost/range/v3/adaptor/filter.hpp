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

#ifndef BOOST_RANGE_V3_ADAPTOR_FILTER_HPP
#define BOOST_RANGE_V3_ADAPTOR_FILTER_HPP

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
            namespace detail
            {
                auto filter_range_category(std::input_iterator_tag)           -> std::input_iterator_tag;
                auto filter_range_category(std::forward_iterator_tag)         -> std::forward_iterator_tag;
                auto filter_range_category(std::bidirectional_iterator_tag)   -> std::bidirectional_iterator_tag;
            }

            template<typename Rng, typename Pred>
            struct filter_range
            {
            private:
                using base_range_category =
                    typename std::iterator_traits<
                        decltype(detail::adl_begin(std::declval<Rng const &>()))
                    >::iterator_category;

                Rng rng_;
                Pred pred_;

                // FltRng is either filter_range or filter_range const.
                template<typename FltRng>
                struct basic_iterator
                  : boost::iterator_facade<
                        basic_iterator<FltRng>
                      , typename std::remove_reference<
                            decltype(*detail::adl_begin(std::declval<FltRng &>().rng_))
                        >::type
                      , decltype(detail::filter_range_category(base_range_category{}))
                    >
                {
                    basic_iterator()
                      : rng_{}, it_{}
                    {}
                    // For iterator -> const_iterator conversion
                    template<typename OtherFltRng,
                             typename = typename std::enable_if<
                                            !std::is_const<OtherFltRng>::value>::type>
                    basic_iterator(basic_iterator<OtherFltRng> that)
                      : rng_(that.rng_), it_(std::move(that.it_))
                    {}
                private:
                    friend struct filter_range;
                    friend class boost::iterator_core_access;
                    using base_range_iterator = decltype(detail::adl_begin(std::declval<FltRng &>().rng_));

                    FltRng *rng_;
                    base_range_iterator it_;

                    basic_iterator(FltRng &rng, base_range_iterator it)
                      : rng_(&rng), it_(std::move(it))
                    {
                        satisfy();
                    }
                    void increment()
                    {
                        BOOST_ASSERT(it_ != detail::adl_end(rng_->rng_));
                        ++it_; satisfy();
                    }
                    void decrement()
                    {
                        while(!rng_->pred_(*--it_)) {}
                    }
                    bool equal(basic_iterator const &that) const
                    {
                        BOOST_ASSERT(rng_ == that.rng_);
                        return it_ == that.it_;
                    }
                    auto dereference() const -> decltype(*it_)
                    {
                        BOOST_ASSERT(it_ != detail::adl_end(rng_->rng_));
                        return *it_;
                    }
                    void satisfy()
                    {
                        auto const e = detail::adl_end(rng_->rng_);
                        while(it_ != e && !rng_->pred_(*it_))
                            ++it_;
                    }
                };
            public:
                using iterator       = basic_iterator<filter_range>;
                using const_iterator = basic_iterator<filter_range const>;

                filter_range(Rng && rng, Pred pred)
                  : rng_(std::forward<Rng>(rng)), pred_(std::move(pred))
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

            constexpr struct filterer
            {
            private:
                template<typename Pred>
                class filterer1
                {
                    Pred pred_;
                public:
                    filterer1(Pred pred)
                      : pred_(std::move(pred))
                    {}
                    template<typename Rng>
                    friend filter_range<Rng, Pred> operator|(Rng && rng, filterer1 && pred)
                    {
                        return {std::forward<Rng>(rng), std::move(pred.pred_)};
                    }
                    template<typename Rng>
                    friend filter_range<Rng, Pred> operator|(Rng && rng, filterer1 const & pred)
                    {
                        return {std::forward<Rng>(rng), pred.pred_};
                    }
                };
            public:
                template<typename Rng, typename Pred>
                filter_range<Rng, Pred> operator()(Rng && rng, Pred pred) const
                {
                    return {std::forward<Rng>(rng), std::move(pred)};
                }
                template<typename Pred>
                constexpr filterer1<Pred> operator()(Pred pred) const
                {
                    return pred;
                }
            } filter {};
        }
    }
}

#endif
