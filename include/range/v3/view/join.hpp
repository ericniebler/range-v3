// Boost.Range library
//
//  Copyright Eric Niebler 2013-2014.
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

#include <tuple>
#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/variant.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/detail/advance_bounded.hpp>

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
                template<typename InputIterator>
                Reference operator()(InputIterator const &it) const
                {
                    return *it;
                }
            };
        }

        template<typename...InputIterables>
        struct joined_view
          : range_facade<joined_view<InputIterables...>,
                logical_or<is_infinite<InputIterables>::value...>::value>
        {
        private:
            friend range_core_access;
            using difference_type = common_type_t<range_difference_t<InputIterables>...>;
            using size_type = meta_quote<std::make_unsigned>::apply<difference_type>;
            static constexpr std::size_t cranges{sizeof...(InputIterables)};
            std::tuple<InputIterables...> rngs_;

            struct sentinel;

            struct cursor
            {
                using difference_type = common_type_t<range_difference_t<InputIterables>...>;
            private:
                friend struct sentinel;
                joined_view const *rng_;
                tagged_variant<range_iterator_t<InputIterables const>...> its_;

                template<std::size_t N>
                void satisfy(size_t<N>)
                {
                    RANGES_ASSERT(its_.which() == N);
                    if(ranges::get<N>(its_) == ranges::end(std::get<N>(rng_->rngs_)))
                    {
                        ranges::set<N + 1>(its_, ranges::begin(std::get<N + 1>(rng_->rngs_)));
                        this->satisfy(size_t<N + 1>{});
                    }
                }
                void satisfy(size_t<cranges - 1>)
                {
                    RANGES_ASSERT(its_.which() == cranges - 1);
                }
                struct next_fun
                {
                    cursor *pos;
                    template<typename InputIterator, std::size_t N>
                    void operator()(InputIterator &it, size_t<N> which) const
                    {
                        ++it;
                        pos->satisfy(which);
                    }
                };
                struct prev_fun
                {
                    cursor *pos;
                    template<typename InputIterator>
                    void operator()(InputIterator &it, size_t<0>) const
                    {
                        RANGES_ASSERT(it != ranges::begin(std::get<0>(pos->rng_->rngs_)));
                        --it;
                    }
                    template<typename InputIterator, std::size_t N>
                    void operator()(InputIterator &it, size_t<N>) const
                    {
                        if(it == ranges::begin(std::get<N>(pos->rng_->rngs_)))
                        {
                            ranges::set<N - 1>(pos->its_, ranges::end(std::get<N - 1>(pos->rng_->rngs_)));
                            (*this)(ranges::get<N - 1>(pos->its_), size_t<N - 1>{});
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
                    void operator()(Iterator &it, size_t<cranges - 1>) const
                    {
                        std::advance(it, n);
                    }
                    template<typename Iterator, std::size_t N>
                    void operator()(Iterator &it, size_t<N> which) const
                    {
                        auto end = ranges::end(std::get<N>(pos->rng_->rngs_));
                        auto rest = detail::advance_bounded(it, n, std::move(end));
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
                    void operator()(Iterator &it, size_t<0>) const
                    {
                        std::advance(it, n);
                    }
                    template<typename Iterator, std::size_t N>
                    void operator()(Iterator &it, size_t<N>) const
                    {
                        auto begin = ranges::begin(std::get<N>(pos->rng_->rngs_));
                        if(it == begin)
                        {
                            ranges::set<N - 1>(pos->its_, ranges::end(std::get<N - 1>(pos->rng_->rngs_)));
                            (*this)(ranges::get<N - 1>(pos->its_), size_t<N - 1>{});
                        }
                        else
                        {
                            auto rest = detail::advance_bounded(it, n, std::move(begin));
                            if(rest != 0)
                                pos->its_.apply_i(advance_rev_fun{pos, rest});
                        }
                    }
                };
                static difference_type distance_to_(size_t<cranges>, cursor const &, cursor const &)
                {
                    RANGES_ASSERT(false);
                    return 0;
                }
                template<std::size_t N>
                static difference_type distance_to_(size_t<N>, cursor const &from, cursor const &to)
                {
                    if(from.its_.which() > N)
                        return cursor::distance_to_(size_t<N + 1>{}, from, to);
                    if(from.its_.which() == N)
                    {
                        if(to.its_.which() == N)
                            return std::distance(ranges::get<N>(from.its_), ranges::get<N>(to.its_));
                        return std::distance(ranges::get<N>(from.its_), ranges::end(std::get<N>(from.rng_->rngs_))) +
                            cursor::distance_to_(size_t<N + 1>{}, from, to);
                    }
                    if(from.its_.which() < N && to.its_.which() > N)
                        return ranges::distance(std::get<N>(from.rng_->rngs_)) +
                            cursor::distance_to_(size_t<N + 1>{}, from, to);
                    RANGES_ASSERT(to.its_.which() == N);
                    return std::distance(ranges::begin(std::get<N>(from.rng_->rngs_)), ranges::get<N>(to.its_));
                }
            public:
                using reference = detail::real_common_type_t<range_reference_t<InputIterables const>...>;
                using single_pass =
                    logical_or<std::is_same<range_category_t<InputIterables>,
                        std::input_iterator_tag>::value...>;
                cursor() = default;
                cursor(joined_view const &rng, begin_tag)
                  : rng_(&rng), its_{size_t<0>{}, ranges::begin(std::get<0>(rng.rngs_))}
                {
                    this->satisfy(size_t<0>{});
                }
                cursor(joined_view const &rng, end_tag)
                  : rng_(&rng), its_{size_t<cranges-1>{}, ranges::end(std::get<cranges-1>(rng.rngs_))}
                {}
                reference current() const
                {
                    // Kind of a dumb implementation. Surely there's a better way.
                    return ranges::get<0>(ranges::unique_variant(its_.apply(detail::deref_fun<reference>{})));
                }
                void next()
                {
                    its_.apply_i(next_fun{this});
                }
                bool equal(cursor const &pos) const
                {
                    return its_ == pos.its_;
                }
                CONCEPT_REQUIRES(logical_and<(ranges::BidirectionalIterator<range_iterator_t<InputIterables>>())...>::value)
                void prev()
                {
                    its_.apply_i(prev_fun{this});
                }
                CONCEPT_REQUIRES(logical_and<(ranges::RandomAccessIterator<range_iterator_t<InputIterables>>())...>::value)
                void advance(difference_type n)
                {
                    if(n > 0)
                        its_.apply_i(advance_fwd_fun{this, n});
                    else if(n < 0)
                        its_.apply_i(advance_rev_fun{this, n});
                }
                CONCEPT_REQUIRES(logical_and<(ranges::RandomAccessIterator<range_iterator_t<InputIterables>>())...>::value)
                difference_type distance_to(cursor const &that) const
                {
                    if(its_.which() <= that.its_.which())
                        return cursor::distance_to_(size_t<0>{}, *this, that);
                    return -cursor::distance_to_(size_t<0>{}, that, *this);
                }
            };
            struct sentinel
            {
            private:
                range_sentinel_t<typelist_back_t<typelist<InputIterables...>> const> end_;
            public:
                sentinel() = default;
                sentinel(joined_view const &rng, end_tag)
                  : end_(ranges::end(std::get<cranges - 1>(rng.rngs_)))
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
            detail::conditional_t<
                logical_and<(ranges::Range<InputIterables>())...>::value, cursor, sentinel>
            end_cursor() const
            {
                return {*this, end_tag{}};
            }
        public:
            joined_view() = default;
            explicit joined_view(InputIterables &&...rngs)
              : rngs_(std::forward<InputIterables>(rngs)...)
            {}
            CONCEPT_REQUIRES(logical_and<(ranges::SizedIterable<InputIterables>())...>::value)
            size_type size() const
            {
                return ranges::tuple_foldl(
                    ranges::tuple_transform(rngs_, ranges::size),
                    0,
                    [](size_type i, size_type j) { return i + j; });
            }
        };

        namespace view
        {
            struct joiner : bindable<joiner>
            {
                template<typename...InputIterables>
                static joined_view<InputIterables...>
                invoke(joiner, InputIterables &&... rngs)
                {
                    static_assert(logical_and<(ranges::Iterable<InputIterables>())...>::value,
                        "Expecting Iterables");
                    static_assert(logical_and<(ranges::InputIterator<range_iterator_t<InputIterables>>())...>::value,
                        "Expecting Input Iterables");
                    return joined_view<InputIterables...>{std::forward<InputIterables>(rngs)...};
                }
            };

            RANGES_CONSTEXPR joiner join {};
        }
    }
}

#endif
