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

#ifndef RANGES_V3_VIEW_JOIN_HPP
#define RANGES_V3_VIEW_JOIN_HPP

#include <tuple>
#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/variant.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct real_common_type_
            {
                template<typename T>
                static T call(T &&);

                template<typename T, typename...Ts, typename Impl = real_common_type_>
                static auto call(T &&t, Ts &&...ts) ->
                    decltype(true ? (T&&) t : Impl::call((Ts&&) ts...));
            };

            template<typename...Ts>
            using real_common_type_t = decltype(real_common_type_::call(std::declval<Ts>()...));

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
        }

        template<typename...Rngs>
        struct joined_view
          : range_facade<joined_view<Rngs...>,
                meta::or_c<is_infinite<Rngs>::value...>::value>
        {
        private:
            friend range_access;
            using difference_type_ = common_type_t<range_difference_t<Rngs>...>;
            using size_type_ = meta::eval<std::make_unsigned<difference_type_>>;
            static constexpr std::size_t cranges{sizeof...(Rngs)};
            std::tuple<view::all_t<Rngs>...> rngs_;

            struct sentinel;

            struct cursor
            {
                using difference_type = common_type_t<range_difference_t<Rngs>...>;
            private:
                friend struct sentinel;
                joined_view const *rng_;
                tagged_variant<range_iterator_t<view::all_t<Rngs> const>...> its_;

                template<std::size_t N>
                void satisfy(meta::size_t<N>)
                {
                    RANGES_ASSERT(its_.which() == N);
                    if(ranges::get<N>(its_) == end(std::get<N>(rng_->rngs_)))
                    {
                        ranges::set<N + 1>(its_, begin(std::get<N + 1>(rng_->rngs_)));
                        this->satisfy(meta::size_t<N + 1>{});
                    }
                }
                void satisfy(meta::size_t<cranges - 1>)
                {
                    RANGES_ASSERT(its_.which() == cranges - 1);
                }
                struct next_fun
                {
                    cursor *pos;
                    template<typename I, std::size_t N>
                    void operator()(I &it, meta::size_t<N> which) const
                    {
                        ++it;
                        pos->satisfy(which);
                    }
                };
                struct prev_fun
                {
                    cursor *pos;
                    template<typename I>
                    void operator()(I &it, meta::size_t<0>) const
                    {
                        RANGES_ASSERT(it != begin(std::get<0>(pos->rng_->rngs_)));
                        --it;
                    }
                    template<typename I, std::size_t N>
                    void operator()(I &it, meta::size_t<N>) const
                    {
                        if(it == begin(std::get<N>(pos->rng_->rngs_)))
                        {
                            ranges::set<N - 1>(pos->its_, end(std::get<N - 1>(pos->rng_->rngs_)));
                            (*this)(ranges::get<N - 1>(pos->its_), meta::size_t<N - 1>{});
                        }
                        else
                            --it;
                    }
                };
                struct advance_fwd_fun
                {
                    cursor *pos;
                    difference_type n;
                    template<typename Iterator>
                    void operator()(Iterator &it, meta::size_t<cranges - 1>) const
                    {
                        ranges::advance(it, n);
                    }
                    template<typename Iterator, std::size_t N>
                    void operator()(Iterator &it, meta::size_t<N> which) const
                    {
                        auto end = ranges::end(std::get<N>(pos->rng_->rngs_));
                        // BUGBUG If distance(it, end) > n, then using advance_bounded
                        // is O(n) when it need not be since the end iterator position
                        // is actually not interesting. Only the "rest" is needed, which
                        // can sometimes be O(1).
                        auto rest = advance_bounded(it, n, std::move(end));
                        pos->satisfy(which);
                        if(rest != 0)
                            pos->its_.apply_i(advance_fwd_fun{pos, rest});
                    }
                };
                struct advance_rev_fun
                {
                    cursor *pos;
                    difference_type n;
                    template<typename Iterator>
                    void operator()(Iterator &it, meta::size_t<0>) const
                    {
                        ranges::advance(it, n);
                    }
                    template<typename Iterator, std::size_t N>
                    void operator()(Iterator &it, meta::size_t<N>) const
                    {
                        auto begin = ranges::begin(std::get<N>(pos->rng_->rngs_));
                        if(it == begin)
                        {
                            ranges::set<N - 1>(pos->its_, end(std::get<N - 1>(pos->rng_->rngs_)));
                            (*this)(ranges::get<N - 1>(pos->its_), meta::size_t<N - 1>{});
                        }
                        else
                        {
                            auto rest = advance_bounded(it, n, std::move(begin));
                            if(rest != 0)
                                pos->its_.apply_i(advance_rev_fun{pos, rest});
                        }
                    }
                };
                static difference_type distance_to_(meta::size_t<cranges>, cursor const &, cursor const &)
                {
                    RANGES_ASSERT(false);
                    return 0;
                }
                template<std::size_t N>
                static difference_type distance_to_(meta::size_t<N>, cursor const &from, cursor const &to)
                {
                    if(from.its_.which() > N)
                        return cursor::distance_to_(meta::size_t<N + 1>{}, from, to);
                    if(from.its_.which() == N)
                    {
                        if(to.its_.which() == N)
                            return std::distance(ranges::get<N>(from.its_), ranges::get<N>(to.its_));
                        return std::distance(ranges::get<N>(from.its_), end(std::get<N>(from.rng_->rngs_))) +
                            cursor::distance_to_(meta::size_t<N + 1>{}, from, to);
                    }
                    if(from.its_.which() < N && to.its_.which() > N)
                        return distance(std::get<N>(from.rng_->rngs_)) +
                            cursor::distance_to_(meta::size_t<N + 1>{}, from, to);
                    RANGES_ASSERT(to.its_.which() == N);
                    return std::distance(begin(std::get<N>(from.rng_->rngs_)), ranges::get<N>(to.its_));
                }
            public:
                using reference = detail::real_common_type_t<range_reference_t<Rngs const>...>;
                using single_pass = meta::fast_or<SinglePass<range_iterator_t<Rngs>>...>;
                cursor() = default;
                cursor(joined_view const &rng, begin_tag)
                  : rng_(&rng), its_{meta::size_t<0>{}, begin(std::get<0>(rng.rngs_))}
                {
                    this->satisfy(meta::size_t<0>{});
                }
                cursor(joined_view const &rng, end_tag)
                  : rng_(&rng), its_{meta::size_t<cranges-1>{}, end(std::get<cranges-1>(rng.rngs_))}
                {}
                reference current() const
                {
                    // Kind of a dumb implementation. Surely there's a better way.
                    return ranges::get<0>(unique_variant(its_.apply(detail::deref_fun<reference>{})));
                }
                void next()
                {
                    its_.apply_i(next_fun{this});
                }
                bool equal(cursor const &pos) const
                {
                    return its_ == pos.its_;
                }
                CONCEPT_REQUIRES(meta::and_c<(bool)BidirectionalIterable<Rngs>()...>::value)
                void prev()
                {
                    its_.apply_i(prev_fun{this});
                }
                CONCEPT_REQUIRES(meta::and_c<(bool)RandomAccessIterable<Rngs>()...>::value)
                void advance(difference_type n)
                {
                    if(n > 0)
                        its_.apply_i(advance_fwd_fun{this, n});
                    else if(n < 0)
                        its_.apply_i(advance_rev_fun{this, n});
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessIterable<Rngs>()...>::value)
                difference_type distance_to(cursor const &that) const
                {
                    if(its_.which() <= that.its_.which())
                        return cursor::distance_to_(meta::size_t<0>{}, *this, that);
                    return -cursor::distance_to_(meta::size_t<0>{}, that, *this);
                }
            };
            struct sentinel
            {
            private:
                range_sentinel_t<meta::back<meta::list<view::all_t<Rngs>...>> const> end_;
            public:
                sentinel() = default;
                sentinel(joined_view const &rng, end_tag)
                  : end_(end(std::get<cranges - 1>(rng.rngs_)))
                {}
                bool equal(cursor const &pos) const
                {
                    return pos.its_.which() == cranges - 1 &&
                        ranges::get<cranges - 1>(pos.its_) == end_;
                }
            };
            cursor begin_cursor() const
            {
                return {*this, begin_tag{}};
            }
            meta::if_<meta::and_c<(bool)BoundedIterable<Rngs>()...>, cursor, sentinel>
            end_cursor() const
            {
                return {*this, end_tag{}};
            }
        public:
            joined_view() = default;
            explicit joined_view(Rngs &&...rngs)
              : rngs_(view::all(std::forward<Rngs>(rngs))...)
            {}
            CONCEPT_REQUIRES(meta::and_c<(bool)SizedIterable<Rngs>()...>::value)
            size_type_ size() const
            {
                return tuple_foldl(tuple_transform(rngs_, ranges::size), size_type_{0}, plus{});
            }
        };

        namespace view
        {
            struct join_fn
            {
                template<typename...Rngs>
                joined_view<Rngs...> operator()(Rngs &&... rngs) const
                {
                    static_assert(meta::and_c<(bool)InputIterable<Rngs>()...>::value,
                        "Expecting Input Iterables");
                    return joined_view<Rngs...>{std::forward<Rngs>(rngs)...};
                }
            };

            constexpr join_fn join {};
        }
    }
}

#endif
