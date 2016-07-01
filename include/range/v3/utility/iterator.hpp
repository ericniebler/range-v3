/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_ITERATOR_HPP
#define RANGES_V3_UTILITY_ITERATOR_HPP

#include <new>
#include <utility>
#include <iterator>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/swap.hpp> // for indirect_swap
#include <range/v3/utility/move.hpp> // for indirect_move
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/basic_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
        namespace adl_advance_detail
        {
            using std::advance;

            template<typename I>
            RANGES_CXX14_CONSTEXPR
            void advance_impl(I &i, iterator_difference_t<I> n, concepts::InputIterator *)
            {
                RANGES_ASSERT(n >= 0);
                for(; n > 0; --n)
                    ++i;
            }
            template<typename I>
            RANGES_CXX14_CONSTEXPR
            void advance_impl(I &i, iterator_difference_t<I> n, concepts::BidirectionalIterator *)
            {
                if(n > 0)
                    for(; n > 0; --n)
                        ++i;
                else
                    for(; n < 0; ++n)
                        --i;
            }
            template<typename I>
            RANGES_CXX14_CONSTEXPR
            void advance_impl(I &i, iterator_difference_t<I> n, concepts::RandomAccessIterator *)
            {
                i += n;
            }
            // Handle range-v3 iterators specially, since many range-v3 iterators will want to
            // decrement an iterator that is bidirectional from the perspective of range-v3,
            // but only input from the perspective of std::advance.
            template<typename Cur, typename Sent>
            RANGES_CXX14_CONSTEXPR
            void advance(basic_iterator<Cur, Sent> &i, iterator_difference_t<basic_iterator<Cur, Sent>> n)
            {
                adl_advance_detail::advance_impl(i, n, iterator_concept<basic_iterator<Cur, Sent>>{});
            }
            // Hijack std::advance for raw pointers, since std::advance is not constexpr
            template<typename T>
            RANGES_CXX14_CONSTEXPR
            void advance(T*& i, iterator_difference_t<T*> n)
            {
                adl_advance_detail::advance_impl(i, n, iterator_concept<T*>{});
            }

            struct advance_fn
            {
            private:
                template<typename I, typename S>
                RANGES_CXX14_CONSTEXPR
                static void to_(I &i, S s, concepts::Sentinel*);
                template<typename I, typename S>
                RANGES_CXX14_CONSTEXPR
                static void to_(I &i, S s, concepts::SizedSentinel*);
                template<typename I, typename D, typename S>
                RANGES_CXX14_CONSTEXPR
                static D bounded_(I &it, D n, S bound, concepts::Sentinel*,
                    concepts::InputIterator*);
                template<typename I, typename D>
                RANGES_CXX14_CONSTEXPR
                static D bounded_(I &it, D n, I bound, concepts::Sentinel*,
                    concepts::BidirectionalIterator*);
                template<typename I, typename D, typename S, typename Concept>
                RANGES_CXX14_CONSTEXPR
                static D bounded_(I &it, D n, S bound, concepts::SizedSentinel*,
                    Concept);
            public:
                // Advance a certain number of steps:
                template<typename I,
                    CONCEPT_REQUIRES_(Iterator<I>())>
                RANGES_CXX14_CONSTEXPR
                void operator()(I &i, iterator_difference_t<I> n) const
                {
                    // Use ADL here to give custom iterator types (like counted_iterator)
                    // a chance to optimize it (see view/counted.hpp)
                    advance(i, n);
                }
                // Advance to a certain position:
                template<typename I, typename S,
                    CONCEPT_REQUIRES_(Sentinel<S, I>() && Assignable<I&, S&&>())>
                RANGES_CXX14_CONSTEXPR
                void operator()(I &i, S s) const
                {
                    i = std::move(s);
                }
                template<typename I, typename S,
                    CONCEPT_REQUIRES_(Sentinel<S, I>() && !Assignable<I&, S&&>())>
                RANGES_CXX14_CONSTEXPR
                void operator()(I &i, S s) const
                {
                    advance_fn::to_(i, std::move(s), sized_sentinel_concept<S, I>());
                }
                // Advance a certain number of times, with a bound:
                template<typename I, typename S,
                    CONCEPT_REQUIRES_(Sentinel<S, I>())>
                RANGES_CXX14_CONSTEXPR
                iterator_difference_t<I> operator()(I &it, iterator_difference_t<I> n, S bound) const
                {
                    return advance_fn::bounded_(it, n, std::move(bound),
                        sized_sentinel_concept<S, I>(), iterator_concept<I>());
                }
            };
        }

        /// \ingroup group-utility
        /// \sa `advance_fn`
        RANGES_INLINE_VARIABLE(adl_advance_detail::advance_fn,advance)

        namespace adl_advance_detail
        {
            template<typename I, typename S>
            RANGES_CXX14_CONSTEXPR
            void advance_fn::to_(I &i, S s, concepts::Sentinel*)
            {
                while(i != s)
                    ++i;
            }
            template<typename I, typename S>
            RANGES_CXX14_CONSTEXPR
            void advance_fn::to_(I &i, S s, concepts::SizedSentinel*)
            {
                iterator_difference_t<I> d = s - i;
                RANGES_ASSERT(0 <= d);
                ranges::advance(i, d);
            }
            template<typename I, typename D, typename S>
            RANGES_CXX14_CONSTEXPR
            D advance_fn::bounded_(I &it, D n, S bound, concepts::Sentinel*,
                concepts::InputIterator*)
            {
                RANGES_ASSERT(0 <= n);
                for(; 0 != n && it != bound; --n)
                    ++it;
                return n;
            }
            template<typename I, typename D>
            RANGES_CXX14_CONSTEXPR
            D advance_fn::bounded_(I &it, D n, I bound, concepts::Sentinel*,
                concepts::BidirectionalIterator*)
            {
                if(0 <= n)
                    for(; 0 != n && it != bound; --n)
                        ++it;
                else
                    for(; 0 != n && it != bound; ++n)
                        --it;
                return n;
            }
            template<typename I, typename D, typename S, typename Concept>
            RANGES_CXX14_CONSTEXPR
            D advance_fn::bounded_(I &it, D n, S bound, concepts::SizedSentinel*,
                Concept)
            {
                RANGES_ASSERT((Same<I, S>() || 0 <= n));
                D d = bound - it;
                RANGES_ASSERT(0 <= n ? 0 <= d : 0 >= d);
                if(0 <= n ? n >= d : n <= d)
                {
                    ranges::advance(it, std::move(bound));
                    return n - d;
                }
                ranges::advance(it, n);
                return 0;
            }
        }

        struct next_fn
        {
            template<typename I,
                CONCEPT_REQUIRES_(Iterator<I>())>
            RANGES_CXX14_CONSTEXPR
            I operator()(I it) const
            {
                return ++it;
            }
            template<typename I,
                CONCEPT_REQUIRES_(Iterator<I>())>
            RANGES_CXX14_CONSTEXPR
            I operator()(I it, iterator_difference_t<I> n) const
            {
                advance(it, n);
                return it;
            }
            template<typename I, typename S,
                CONCEPT_REQUIRES_(Sentinel<S, I>())>
            RANGES_CXX14_CONSTEXPR
            I operator()(I it, S s) const
            {
                advance(it, std::move(s));
                return it;
            }
            template<typename I, typename S,
                CONCEPT_REQUIRES_(Sentinel<S, I>())>
            RANGES_CXX14_CONSTEXPR
            I operator()(I it, iterator_difference_t<I> n, S bound) const
            {
                advance(it, n, std::move(bound));
                return it;
            }
        };

        /// \ingroup group-utility
        /// \sa `next_fn`
        RANGES_INLINE_VARIABLE(next_fn, next)

        struct prev_fn
        {
            template<typename I,
                CONCEPT_REQUIRES_(BidirectionalIterator<I>())>
            RANGES_CXX14_CONSTEXPR
            I operator()(I it, iterator_difference_t<I> n = 1) const
            {
                advance(it, -n);
                return it;
            }
        };

        /// \ingroup group-utility
        /// \sa `prev_fn`
        RANGES_INLINE_VARIABLE(prev_fn, prev)

        struct iter_enumerate_fn
        {
        private:
            template<typename I, typename S, typename D,
                CONCEPT_REQUIRES_(!SizedSentinel<I, I>())>
            RANGES_CXX14_CONSTEXPR
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::Sentinel*) const
            {
                for(; begin != end; ++begin)
                    ++d;
                return {d, begin};
            }
            template<typename I, typename S, typename D,
                CONCEPT_REQUIRES_(SizedSentinel<I, I>())>
            RANGES_CXX14_CONSTEXPR
            std::pair<D, I> impl_i(I begin, S end_, D d, concepts::Sentinel*) const
            {
                I end = ranges::next(begin, end_);
                auto n = static_cast<D>(end - begin);
                RANGES_ASSERT((Same<I, S>() || 0 <= n));
                return {n + d, end};
            }
            template<typename I, typename S, typename D>
            RANGES_CXX14_CONSTEXPR
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::SizedSentinel*) const
            {
                auto n = static_cast<D>(end - begin);
                RANGES_ASSERT((Same<I, S>() || 0 <= n));
                return {n + d, ranges::next(begin, end)};
            }
        public:
            template<typename I, typename S, typename D = iterator_difference_t<I>,
                CONCEPT_REQUIRES_(Iterator<I>() && Sentinel<S, I>() && Integral<D>())>
            RANGES_CXX14_CONSTEXPR
            std::pair<D, I> operator()(I begin, S end, D d = 0) const
            {
                return this->impl_i(std::move(begin), std::move(end), d,
                    sized_sentinel_concept<S, I>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_enumerate_fn`
        RANGES_INLINE_VARIABLE(iter_enumerate_fn, iter_enumerate)

        struct iter_distance_fn
        {
        private:
            template<typename I, typename S, typename D>
            RANGES_CXX14_CONSTEXPR
            D impl_i(I begin, S end, D d, concepts::Sentinel*) const
            {
                return iter_enumerate(std::move(begin), std::move(end), d).first;
            }
            template<typename I, typename S, typename D>
            RANGES_CXX14_CONSTEXPR
            D impl_i(I begin, S end, D d, concepts::SizedSentinel*) const
            {
                auto n = static_cast<D>(end - begin);
                RANGES_ASSERT((Same<I, S>() || 0 <= n));
                return n + d;
            }
        public:
            template<typename I, typename S, typename D = iterator_difference_t<I>,
                CONCEPT_REQUIRES_(Iterator<I>() && Sentinel<S, I>() && Integral<D>())>
            RANGES_CXX14_CONSTEXPR
            D operator()(I begin, S end, D d = 0) const
            {
                return this->impl_i(std::move(begin), std::move(end), d,
                    sized_sentinel_concept<S, I>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_distance_fn`
        RANGES_INLINE_VARIABLE(iter_distance_fn, iter_distance)

        struct iter_distance_compare_fn
        {
        private:
            template<typename I, typename S>
            RANGES_CXX14_CONSTEXPR
            int impl_i(I begin, S end, iterator_difference_t<I> n, concepts::Sentinel*) const
            {
                if (n >= 0) {
                    for (; n > 0; --n) {
                        if (begin == end) {
                            return -1;
                        }
                        ++begin;
                    }
                    return begin == end ? 0 : 1;
                }
                else {
                    return 1;
                }
            }
            template<typename I, typename S>
            RANGES_CXX14_CONSTEXPR
            int impl_i(I begin, S end, iterator_difference_t<I> n, concepts::SizedSentinel*) const
            {
                iterator_difference_t<I> dist = end - begin;
                if (dist > n)
                    return  1;
                else if (dist < n)
                    return -1;
                else
                    return  0;
            }
        public:
            template<typename I, typename S,
                CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>())>
            RANGES_CXX14_CONSTEXPR
            int operator()(I begin, S end, iterator_difference_t<I> n) const
            {
                return this->impl_i(std::move(begin), std::move(end), n,
                    sized_sentinel_concept<S, I>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_distance_compare_fn`
        RANGES_INLINE_VARIABLE(iter_distance_compare_fn, iter_distance_compare)

        // Like distance(b,e), but guaranteed to be O(1)
        struct iter_size_fn
        {
            template<typename I, typename S, CONCEPT_REQUIRES_(SizedSentinel<S, I>())>
            RANGES_CXX14_CONSTEXPR
            iterator_size_t<I> operator()(I begin, S end) const
            {
                iterator_difference_t<I> n = end - begin;
                RANGES_ASSERT(0 <= n);
                return static_cast<iterator_size_t<I>>(n);
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_size_fn`
        RANGES_INLINE_VARIABLE(iter_size_fn, iter_size)

        struct iter_swap_fn
        {
            template<typename Readable0, typename Readable1,
                CONCEPT_REQUIRES_(IndirectlySwappable<Readable0, Readable1>())>
            RANGES_CXX14_CONSTEXPR
            void operator()(Readable0 a, Readable1 b) const
                noexcept(is_nothrow_indirectly_swappable<Readable0, Readable1>::value)
            {
                indirect_swap(std::move(a), std::move(b));
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_swap_fn`
        RANGES_INLINE_VARIABLE(iter_swap_fn, iter_swap)

        struct iter_move_fn
        {
            template<typename I,
                CONCEPT_REQUIRES_(Readable<I>())>
            RANGES_CXX14_CONSTEXPR
            iterator_rvalue_reference_t<I> operator()(I const &i) const
                noexcept(noexcept(indirect_move(i)))
            {
                return indirect_move(i);
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_move_fn`
        RANGES_INLINE_VARIABLE(iter_move_fn, iter_move)

        /// \cond
        namespace detail
        {
            template<typename Cont>
            struct back_insert_cursor
            {
            private:
                friend range_access;
                struct mixin : basic_mixin<back_insert_cursor>
                {
                    mixin() = default;
                    using basic_mixin<back_insert_cursor>::basic_mixin;
                    explicit mixin(Cont &cont) noexcept
                      : basic_mixin<back_insert_cursor>{back_insert_cursor{cont}}
                    {}
                };
                Cont *cont_;
                explicit back_insert_cursor(Cont &cont) noexcept
                  : cont_(&cont)
                {}
                void next() const
                {}
                void set(typename Cont::value_type const &v) const
                {
                    cont_->push_back(v);
                }
                void set(typename Cont::value_type &&v) const
                {
                    cont_->push_back(std::move(v));
                }
            public:
                constexpr back_insert_cursor()
                  : cont_{}
                {}
            };
        }
        /// \endcond

        template<typename Cont>
        using back_insert_iterator = basic_iterator<detail::back_insert_cursor<Cont>>;

        struct back_inserter_fn
        {
            template<typename Cont>
            back_insert_iterator<Cont> operator()(Cont &cont) const
            {
                return back_insert_iterator<Cont>{cont};
            }
        };

        /// \ingroup group-utility
        /// \sa `back_inserter_fn`
        RANGES_INLINE_VARIABLE(back_inserter_fn, back_inserter)

        /// \cond
        namespace detail
        {
            template<typename Cont>
            struct front_insert_cursor
            {
            private:
                friend range_access;
                struct mixin : basic_mixin<front_insert_cursor>
                {
                    mixin() = default;
                    using basic_mixin<front_insert_cursor>::basic_mixin;
                    explicit mixin(Cont &cont) noexcept
                      : basic_mixin<front_insert_cursor>{front_insert_cursor{cont}}
                    {}
                };
                Cont *cont_;
                explicit front_insert_cursor(Cont &cont) noexcept
                  : cont_(&cont)
                {}
                void next() const
                {}
                void set(typename Cont::value_type const &v) const
                {
                    cont_->push_front(v);
                }
                void set(typename Cont::value_type &&v) const
                {
                    cont_->push_front(std::move(v));
                }
            public:
                constexpr front_insert_cursor()
                  : cont_{}
                {}
            };
        }
        /// \endcond

        template<typename Cont>
        using front_insert_iterator = basic_iterator<detail::front_insert_cursor<Cont>>;

        struct front_inserter_fn
        {
            template<typename Cont>
            front_insert_iterator<Cont> operator()(Cont &cont) const
            {
                return front_insert_iterator<Cont>{cont};
            }
        };

        /// \ingroup group-utility
        /// \sa `front_inserter_fn`
        RANGES_INLINE_VARIABLE(front_inserter_fn, front_inserter)

        /// \cond
        namespace detail
        {
            template<typename Cont>
            struct insert_cursor
            {
            private:
                friend range_access;
                Cont *cont_;
                typename Cont::iterator where_;
                struct mixin : basic_mixin<insert_cursor>
                {
                    mixin() = default;
                    using basic_mixin<insert_cursor>::basic_mixin;
                    explicit mixin(Cont &cont, typename Cont::iterator where) noexcept
                      : basic_mixin<insert_cursor>{insert_cursor{cont, std::move(where)}}
                    {}
                };
                explicit insert_cursor(Cont &cont, typename Cont::iterator where) noexcept
                  : cont_(&cont), where_(where)
                {}
                void next() const
                {}
                void set(typename Cont::value_type const &v)
                {
                    where_ = ranges::next(cont_->insert(where_, v));
                }
                void set(typename Cont::value_type &&v)
                {
                    where_ = ranges::next(cont_->insert(where_, std::move(v)));
                }
            public:
                constexpr insert_cursor()
                  : cont_{}, where_{}
                {}
            };
        }
        /// \cond

        template<typename Cont>
        using insert_iterator = basic_iterator<detail::insert_cursor<Cont>>;

        struct inserter_fn
        {
            template<typename Cont>
            insert_iterator<Cont> operator()(Cont &cont, typename Cont::iterator where) const
            {
                return insert_iterator<Cont>{cont, std::move(where)};
            }
        };

        /// \ingroup group-utility
        /// \sa `inserter_fn`
        RANGES_INLINE_VARIABLE(inserter_fn, inserter)

        template<typename T = void, typename Char = char, typename Traits = std::char_traits<Char>>
        struct ostream_iterator
        {
        private:
            std::basic_ostream<Char, Traits> *sout_;
            Char const *delim_;
            struct proxy
            {
                std::basic_ostream<Char, Traits> *sout_;
                Char const *delim_;
                template<typename U,
                    typename V = meta::if_<std::is_void<T>, U, T>,
                    meta::if_<std::is_convertible<U, V const &>, int> = 0>
                proxy &operator=(U &&t)
                {
                    RANGES_ASSERT(sout_);
                    *sout_ << static_cast<V const &>(t);
                    if(delim_)
                        *sout_ << delim_;
                    return *this;
                }
            };
        public:
            using difference_type = std::ptrdiff_t;
            using char_type = Char;
            using traits_type = Traits;
            ostream_iterator() = default;
            ostream_iterator(std::basic_ostream<Char, Traits> &sout, Char const *delim = nullptr) noexcept
              : sout_(&sout), delim_(delim)
            {}
            proxy operator*() const noexcept
            {
                return {sout_, delim_};
            }
            ostream_iterator &operator++()
            {
                return *this;
            }
            ostream_iterator operator++(int)
            {
                return *this;
            }
        };

        /// \cond
        namespace detail
        {
            template<typename I>
            struct reverse_cursor
            {
            private:
                CONCEPT_ASSERT(BidirectionalIterator<I>());
                friend range_access;
                template<typename OtherI>
                friend struct reverse_cursor;
                struct mixin : basic_mixin<reverse_cursor>
                {
                    mixin() = default;
                    using basic_mixin<reverse_cursor>::basic_mixin;
                    RANGES_CXX14_CONSTEXPR
                    mixin(I it)
                      : mixin{reverse_cursor{it}}
                    {}
                    RANGES_CXX14_CONSTEXPR
                    I base() const
                    {
                        return this->get().base();
                    }
                };

                I it_;

                RANGES_CXX14_CONSTEXPR
                reverse_cursor(I it)
                  : it_(std::move(it))
                {}
                RANGES_CXX14_CONSTEXPR
                auto get() const ->
                    decltype(*it_)
                {
                    I tmp(it_);
                    return *(--tmp);
                }
                RANGES_CXX14_CONSTEXPR
                I base() const
                {
                    return it_;
                }
                RANGES_CXX14_CONSTEXPR
                bool equal(reverse_cursor const& that) const
                {
                    return it_ == that.it_;
                }
                RANGES_CXX14_CONSTEXPR
                void next()
                {
                    --it_;
                }
                RANGES_CXX14_CONSTEXPR
                void prev()
                {
                    ++it_;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                RANGES_CXX14_CONSTEXPR
                void advance(iterator_difference_t<I> n)
                {
                    it_ -= n;
                }
                CONCEPT_REQUIRES(SizedSentinel<I, I>())
                RANGES_CXX14_CONSTEXPR
                iterator_difference_t<I>
                distance_to(reverse_cursor const &that) const
                {
                    return it_ - that.base();
                }
                RANGES_CXX14_CONSTEXPR
                auto move() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    iter_move(it_)
                )
            public:
                reverse_cursor() = default;
                template<typename U,
                    CONCEPT_REQUIRES_(ConvertibleTo<U, I>())>
                RANGES_CXX14_CONSTEXPR
                reverse_cursor(reverse_cursor<U> const &u)
                  : it_(u.base())
                {}
            };
        }  // namespace detail
        /// \endcond

        template<typename I>
        RANGES_CXX14_CONSTEXPR
        reverse_iterator<I> make_reverse_iterator(I i)
        {
            return reverse_iterator<I>(i);
        }

        /// \cond
        namespace detail
        {
            template<typename I>
            struct move_cursor
            {
            private:
                CONCEPT_ASSERT(InputIterator<I>());
                friend range_access;
                using single_pass = std::true_type;
                //using single_pass = SinglePass<I>;
                using value_type = iterator_value_t<I>;
                using difference_type = iterator_difference_t<I>;

                struct mixin
                  : basic_mixin<move_cursor>
                {
                    mixin() = default;
                    using basic_mixin<move_cursor>::basic_mixin;
                    constexpr explicit mixin(I it)
                      : mixin{move_cursor(detail::move(it))}
                    {}
                    I base() const
                    {
                        return this->get().it_;
                    }
                };

                I it_;

                constexpr move_cursor(I it)
                  : it_(it)
                {}
                auto get() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    iter_move(it_)
                )
                void next()
                {
                    ++it_;
                }
                CONCEPT_REQUIRES(ForwardIterator<I>())
                bool equal(move_cursor const &that) const
                {
                    return it_ == that.it_;
                }
                //CONCEPT_REQUIRES(BidirectionalIterator<I>())
                //void prev()
                //{
                //    --it_;
                //}
                //CONCEPT_REQUIRES(RandomAccessIterator<I>())
                //void advance(iterator_difference_t<I> n)
                //{
                //    it_ += n;
                //}
                CONCEPT_REQUIRES(SizedSentinel<I, I>())
                iterator_difference_t<I> distance_to(move_cursor const &that) const
                {
                    return that.it_ - it_;
                }
                auto move() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    iter_move(it_)
                )
            public:
                constexpr move_cursor()
                  : it_{}
                {}
            };
        }
        /// \endcond

        struct make_move_iterator_fn
        {
            template<typename I,
                CONCEPT_REQUIRES_(InputIterator<I>())>
            constexpr move_iterator<I> operator()(I it) const
            {
                return move_iterator<I>{detail::move(it)};
            }
        };

        RANGES_INLINE_VARIABLE(make_move_iterator_fn, make_move_iterator)

        template<typename S>
        struct move_sentinel
        {
        private:
            S sent_;
        public:
            constexpr move_sentinel()
              : sent_{}
            {}
            constexpr explicit move_sentinel(S s)
              : sent_(detail::move(s))
            {}
            template<typename OS,
                CONCEPT_REQUIRES_(ConvertibleTo<OS, S>())>
            constexpr explicit move_sentinel(move_sentinel<OS> const &that)
              : sent_(that.base())
            {}
            template<typename OS,
                CONCEPT_REQUIRES_(ConvertibleTo<OS, S>())>
            move_sentinel &operator=(move_sentinel<OS> const &that)
            {
                sent_ = that.base();
                return *this;
            }
            S base() const
            {
                return sent_;
            }
        };

        template<typename I, typename S,
            CONCEPT_REQUIRES_(Sentinel<S, I>())>
        bool operator==(move_iterator<I> const &i, move_sentinel<S> const &s)
        {
            return i.base() == s.base();
        }
        template<typename I, typename S,
            CONCEPT_REQUIRES_(Sentinel<S, I>())>
        bool operator==(move_sentinel<S> const &s, move_iterator<S> const &i)
        {
            return s.base() == i.base();
        }
        template<typename I, typename S,
            CONCEPT_REQUIRES_(Sentinel<S, I>())>
        bool operator!=(move_iterator<S> const &i, move_sentinel<S> const &s)
        {
            return i.base() != s.base();
        }
        template<typename I, typename S,
            CONCEPT_REQUIRES_(Sentinel<S, I>())>
        bool operator!=(move_sentinel<S> const &s, move_iterator<S> const &i)
        {
            return s.base() != i.base();
        }

        struct make_move_sentinel_fn
        {
            template<typename I,
                CONCEPT_REQUIRES_(InputIterator<I>())>
            constexpr move_iterator<I> operator()(I i) const
            {
                return move_iterator<I>{detail::move(i)};
            }

            template<typename S,
                CONCEPT_REQUIRES_(SemiRegular<S>() && !InputIterator<S>())>
            constexpr move_sentinel<S> operator()(S s) const
            {
                return move_sentinel<S>{detail::move(s)};
            }
        };

        RANGES_INLINE_VARIABLE(make_move_sentinel_fn, make_move_sentinel)

        /// \cond
        namespace detail
        {
            template<typename I, bool IsReadable = (bool) Readable<I>()>
            struct move_into_cursor_types
            {};

            template<typename I>
            struct move_into_cursor_types<I, true>
            {
                using value_type = iterator_value_t<I>;
                using single_pass = SinglePass<I>;
            };

            template<typename I>
            struct move_into_cursor
              : move_into_cursor_types<I>
            {
            private:
                friend range_access;
                struct mixin : basic_mixin<move_into_cursor>
                {
                    mixin() = default;
                    using basic_mixin<move_into_cursor>::basic_mixin;
                    explicit mixin(I it)
                      : mixin{move_into_cursor{std::move(it)}}
                    {}
                    I base() const
                    {
                        move_into_cursor const &this_ = this->basic_mixin<move_into_cursor>::get();
                        return this_.it_;
                    }
                };

                I it_;

                explicit move_into_cursor(I it)
                  : it_(std::move(it))
                {}
                void next()
                {
                    ++it_;
                }
                template<typename T,
                    CONCEPT_REQUIRES_(Writable<I, aux::move_t<T> &&>())>
                void set(T &&t) noexcept(noexcept(*it_ = std::move(t)))
                {
                    *it_ = std::move(t);
                }
                template<typename T,
                    CONCEPT_REQUIRES_(Writable<I, aux::move_t<T> &&>())>
                void set(T &&t) const noexcept(noexcept(*it_ = std::move(t)))
                {
                    *it_ = std::move(t);
                }
                CONCEPT_REQUIRES(Readable<I>())
                auto get() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    *it_
                )
                CONCEPT_REQUIRES(InputIterator<I>())
                bool equal(move_into_cursor const &that) const
                {
                    return it_ == that.it_;
                }
                CONCEPT_REQUIRES(BidirectionalIterator<I>())
                void prev()
                {
                    --it_;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                void advance(iterator_difference_t<I> n)
                {
                    it_ += n;
                }
                CONCEPT_REQUIRES(SizedSentinel<I, I>())
                iterator_difference_t<I> distance_to(move_into_cursor const &that) const
                {
                    return that.it_ - it_;
                }
                CONCEPT_REQUIRES(Readable<I>())
                RANGES_CXX14_CONSTEXPR
                iterator_rvalue_reference_t<I> move() const
                    noexcept(noexcept(iter_move(it_)))
                {
                    return iter_move(it_);
                }
            public:
                constexpr move_into_cursor()
                  : it_{}
                {}
            };
        }
        /// \endcond

        struct move_into_fn
        {
            template<typename I>
            constexpr move_into_iterator<I> operator()(I it) const
            {
                return move_into_iterator<I>{std::move(it)};
            }
        };

        /// \ingroup group-utility
        /// \sa `move_into_fn`
        RANGES_INLINE_VARIABLE(move_into_fn, move_into)
        /// @}

        /// \cond
        namespace adl_uncounted_recounted_detail
        {
            template<typename I>
            constexpr I uncounted(I i)
            {
                return i;
            }

            template<typename I>
            constexpr I recounted(I const &, I i, iterator_difference_t<I>)
            {
                return i;
            }

            struct uncounted_fn
            {
                template<typename I>
                constexpr
                auto operator()(I i) const ->
                    decltype(uncounted((I&&)i))
                {
                    return uncounted((I&&)i);
                }
            };

            struct recounted_fn
            {
                template<typename I, typename J>
                constexpr
                auto operator()(I i, J j, iterator_difference_t<J> n) const ->
                    decltype(recounted((I&&)i, (J&&)j, n))
                {
                    return recounted((I&&)i, (J&&)j, n);
                }
            };
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{
        RANGES_INLINE_VARIABLE(adl_uncounted_recounted_detail::uncounted_fn,
                               uncounted)
        RANGES_INLINE_VARIABLE(adl_uncounted_recounted_detail::recounted_fn,
                               recounted)
        /// @}
    }
}

#endif
