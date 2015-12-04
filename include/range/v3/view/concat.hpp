/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_CONCAT_HPP
#define RANGES_V3_VIEW_CONCAT_HPP

#include <tuple>
#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/variant.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            // Dereference the iterator and coerce the return type
            template<typename Reference>
            struct deref_fun
            {
                template<typename I>
                Reference operator()(I const &it) const
                {
                    return *it;
                }
            };

            template<typename State, typename Value>
            using concat_cardinality =
                std::integral_constant<cardinality,
                    State::value == infinite || Value::value == infinite ?
                        infinite :
                        State::value == unknown || Value::value == unknown ?
                            unknown :
                            State::value == finite || Value::value == finite ?
                                finite :
                                static_cast<cardinality>(State::value + Value::value)>;
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename...Rngs>
        struct concat_view
          : view_facade<concat_view<Rngs...>,
                meta::fold<
                    meta::list<range_cardinality<Rngs>...>,
                    std::integral_constant<cardinality, static_cast<cardinality>(0)>,
                    meta::quote<detail::concat_cardinality>>::value>
        {
        private:
            friend range_access;
            using difference_type_ = common_type_t<range_difference_t<Rngs>...>;
            using size_type_ = meta::_t<std::make_unsigned<difference_type_>>;
            static constexpr std::size_t cranges{sizeof...(Rngs)};
            std::tuple<Rngs...> rngs_;

            template<bool IsConst>
            struct sentinel;

            template<bool IsConst>
            struct cursor
            {
                using difference_type = common_type_t<range_difference_t<Rngs>...>;
            private:
                friend struct sentinel<IsConst>;
                template<typename T>
                using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
                using concat_view_t = constify_if<concat_view>;
                concat_view_t *rng_;
                variant<range_iterator_t<constify_if<Rngs>>...> its_;

                template<std::size_t N>
                void satisfy(meta::size_t<N>)
                {
                    RANGES_ASSERT(its_.index() == N);
                    if(ranges::get<N>(its_) == end(std::get<N>(rng_->rngs_)))
                    {
                        ranges::emplace<N + 1>(its_, begin(std::get<N + 1>(rng_->rngs_)));
                        this->satisfy(meta::size_t<N + 1>{});
                    }
                }
                void satisfy(meta::size_t<cranges - 1>)
                {
                    RANGES_ASSERT(its_.index() == cranges - 1);
                }
                struct next_fun
                {
                    cursor *pos;
                    template<typename I, std::size_t N>
                    void operator()(indexed_element<I, N> it) const
                    {
                        ++it.get();
                        pos->satisfy(meta::size_t<N>{});
                    }
                };
                struct prev_fun
                {
                    cursor *pos;
                    template<typename I>
                    void operator()(indexed_element<I, 0> it) const
                    {
                        RANGES_ASSERT(it.get() != begin(std::get<0>(pos->rng_->rngs_)));
                        --it.get();
                    }
                    template<typename I, std::size_t N>
                    void operator()(indexed_element<I, N> it) const
                    {
                        if(it.get() == begin(std::get<N>(pos->rng_->rngs_)))
                        {
                            auto &&rng = std::get<N - 1>(pos->rng_->rngs_);
                            ranges::emplace<N - 1>(pos->its_,
                                ranges::next(ranges::begin(rng), ranges::end(rng)));
                            pos->its_.visit_i(*this);
                        }
                        else
                            --it.get();
                    }
                };
                struct advance_fwd_fun
                {
                    cursor *pos;
                    difference_type n;
                    template<typename I>
                    void operator()(indexed_element<I, cranges - 1> it) const
                    {
                        ranges::advance(it.get(), n);
                    }
                    template<typename I, std::size_t N>
                    void operator()(indexed_element<I, N> it) const
                    {
                        auto end = ranges::end(std::get<N>(pos->rng_->rngs_));
                        // BUGBUG If distance(it, end) > n, then using bounded advance
                        // is O(n) when it need not be since the end iterator position
                        // is actually not interesting. Only the "rest" is needed, which
                        // can sometimes be O(1).
                        auto rest = ranges::advance(it.get(), n, std::move(end));
                        pos->satisfy(meta::size_t<N>{});
                        if(rest != 0)
                            pos->its_.visit_i(advance_fwd_fun{pos, rest});
                    }
                };
                struct advance_rev_fun
                {
                    cursor *pos;
                    difference_type n;
                    template<typename I>
                    void operator()(indexed_element<I, 0> it) const
                    {
                        ranges::advance(it.get(), n);
                    }
                    template<typename I, std::size_t N>
                    void operator()(indexed_element<I, N> it) const
                    {
                        auto begin = ranges::begin(std::get<N>(pos->rng_->rngs_));
                        if(it.get() == begin)
                        {
                            auto &&rng = std::get<N - 1>(pos->rng_->rngs_);
                            ranges::emplace<N - 1>(pos->its_,
                                ranges::next(ranges::begin(rng), ranges::end(rng)));
                            pos->its_.visit_i(*this);
                        }
                        else
                        {
                            auto rest = ranges::advance(it.get(), n, std::move(begin));
                            if(rest != 0)
                                pos->its_.visit_i(advance_rev_fun{pos, rest});
                        }
                    }
                };
                [[noreturn]] static difference_type distance_to_(meta::size_t<cranges>, cursor const &, cursor const &)
                {
                    RANGES_ENSURE(false);
                }
                template<std::size_t N>
                static difference_type distance_to_(meta::size_t<N>, cursor const &from, cursor const &to)
                {
                    if(from.its_.index() > N)
                        return cursor::distance_to_(meta::size_t<N + 1>{}, from, to);
                    if(from.its_.index() == N)
                    {
                        if(to.its_.index() == N)
                            return distance(ranges::get<N>(from.its_), ranges::get<N>(to.its_));
                        return distance(ranges::get<N>(from.its_), end(std::get<N>(from.rng_->rngs_))) +
                            cursor::distance_to_(meta::size_t<N + 1>{}, from, to);
                    }
                    if(from.its_.index() < N && to.its_.index() > N)
                        return distance(std::get<N>(from.rng_->rngs_)) +
                            cursor::distance_to_(meta::size_t<N + 1>{}, from, to);
                    RANGES_ASSERT(to.its_.index() == N);
                    return distance(begin(std::get<N>(from.rng_->rngs_)), ranges::get<N>(to.its_));
                }
            public:
                // BUGBUG what about rvalue_reference and common_reference?
                using reference = common_reference_t<range_reference_t<constify_if<Rngs>>...>;
                using single_pass = meta::fast_or<SinglePass<range_iterator_t<Rngs>>...>;
                cursor() = default;
                cursor(concat_view_t &rng, begin_tag)
                  : rng_(&rng), its_{emplaced_index<0>, begin(std::get<0>(rng.rngs_))}
                {
                    this->satisfy(meta::size_t<0>{});
                }
                cursor(concat_view_t &rng, end_tag)
                  : rng_(&rng), its_{emplaced_index<cranges-1>, end(std::get<cranges-1>(rng.rngs_))}
                {}
                reference get() const
                {
                    // Kind of a dumb implementation. Surely there's a better way.
                    return ranges::get<0>(unique_variant(its_.visit(detail::deref_fun<reference>{})));
                }
                void next()
                {
                    its_.visit_i(next_fun{this});
                }
                bool equal(cursor const &pos) const
                {
                    return its_ == pos.its_;
                }
                CONCEPT_REQUIRES(meta::and_c<(bool)BidirectionalRange<Rngs>()...>::value)
                void prev()
                {
                    its_.visit_i(prev_fun{this});
                }
                CONCEPT_REQUIRES(meta::and_c<(bool)RandomAccessRange<Rngs>()...>::value)
                void advance(difference_type n)
                {
                    if(n > 0)
                        its_.visit_i(advance_fwd_fun{this, n});
                    else if(n < 0)
                        its_.visit_i(advance_rev_fun{this, n});
                }
                CONCEPT_REQUIRES(meta::and_c<(bool)
                    SizedIteratorRange<range_iterator_t<Rngs>,
                        range_iterator_t<Rngs>>()...>::value)
                difference_type distance_to(cursor const &that) const
                {
                    if(its_.index() <= that.its_.index())
                        return cursor::distance_to_(meta::size_t<0>{}, *this, that);
                    return -cursor::distance_to_(meta::size_t<0>{}, that, *this);
                }
            };
            template<bool IsConst>
            struct sentinel
            {
            private:
                template<typename T>
                using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
                using concat_view_t = constify_if<concat_view>;
                range_sentinel_t<constify_if<meta::back<meta::list<Rngs...>>>> end_;
            public:
                sentinel() = default;
                sentinel(concat_view_t &rng, end_tag)
                  : end_(end(std::get<cranges - 1>(rng.rngs_)))
                {}
                bool equal(cursor<IsConst> const &pos) const
                {
                    return pos.its_.index() == cranges - 1 &&
                        ranges::get<cranges - 1>(pos.its_) == end_;
                }
            };
            cursor<false> begin_cursor()
            {
                return {*this, begin_tag{}};
            }
            meta::if_<meta::and_c<(bool)BoundedRange<Rngs>()...>, cursor<false>, sentinel<false>>
            end_cursor()
            {
                return {*this, end_tag{}};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool)Range<Rngs const>()...>())
            cursor<true> begin_cursor() const
            {
                return {*this, begin_tag{}};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool)Range<Rngs const>()...>())
            meta::if_<meta::and_c<(bool)BoundedRange<Rngs>()...>, cursor<true>, sentinel<true>>
            end_cursor() const
            {
                return {*this, end_tag{}};
            }
        public:
            concat_view() = default;
            explicit concat_view(Rngs...rngs)
              : rngs_{std::move(rngs)...}
            {}
            CONCEPT_REQUIRES(meta::and_c<(bool)SizedRange<Rngs>()...>::value)
            constexpr size_type_ size() const
            {
                return range_cardinality<concat_view>::value >= 0 ?
                    (size_type_)range_cardinality<concat_view>::value :
                    tuple_foldl(tuple_transform(rngs_, ranges::size), size_type_{0}, plus{});
            }
        };

        namespace view
        {
            struct concat_fn
            {
                template<typename...Rngs>
                concat_view<all_t<Rngs>...> operator()(Rngs &&... rngs) const
                {
                    static_assert(meta::and_c<(bool)InputRange<Rngs>()...>::value,
                        "Expecting Input Ranges");
                    return concat_view<all_t<Rngs>...>{all(std::forward<Rngs>(rngs))...};
                }
            };

            /// \relates concat_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& concat = static_const<concat_fn>::value;
            }
        }
        /// @}
    }
}

#endif
