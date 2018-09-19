/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
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
#include <range/v3/utility/swap.hpp> // for iter_swap
#include <range/v3/utility/move.hpp> // for iter_move
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
            template<typename I, typename D>
            void advance(I&, D) = delete;

            struct advance_fn
            {
            private:
                template<typename I>
                RANGES_CXX14_CONSTEXPR
                static void n_impl_(I &i, difference_type_t<I> n, concepts::InputIterator *);
                template<typename I>
                RANGES_CXX14_CONSTEXPR
                static void n_impl_(I &i, difference_type_t<I> n, concepts::BidirectionalIterator *);
                template<typename I>
                RANGES_CXX14_CONSTEXPR
                static void n_impl_(I &i, difference_type_t<I> n, concepts::RandomAccessIterator *);
                // Is there an advance that is find-able by ADL and is preferred
                // by partial ordering to the poison-pill overload?
                template<typename I>
                RANGES_CXX14_CONSTEXPR
                static auto n_(I &i, difference_type_t<I> n, int) ->
                    decltype(static_cast<void>(advance(i, n)));
                // No advance overload found by ADL, use the default implementation:
                template<typename I>
                RANGES_CXX14_CONSTEXPR
                static void n_(I &i, difference_type_t<I> n, long);
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
                void operator()(I &i, difference_type_t<I> n) const
                {
                    advance_fn::n_(i, n, 0);
                }
                // Advance to a certain position:
                template<typename I, typename S,
                    CONCEPT_REQUIRES_(Sentinel<S, I>() && Assignable<I&, S>())>
                RANGES_CXX14_CONSTEXPR
                void operator()(I &i, S s) const
                {
                    i = std::move(s);
                }
                template<typename I, typename S,
                    CONCEPT_REQUIRES_(Sentinel<S, I>() && !Assignable<I&, S>())>
                RANGES_CXX14_CONSTEXPR
                void operator()(I &i, S s) const
                {
                    advance_fn::to_(i, std::move(s), sized_sentinel_concept<S, I>());
                }
                // Advance a certain number of times, with a bound:
                template<typename I, typename S,
                    CONCEPT_REQUIRES_(Sentinel<S, I>())>
                RANGES_CXX14_CONSTEXPR
                difference_type_t<I> operator()(I &it, difference_type_t<I> n, S bound) const
                {
                    return advance_fn::bounded_(it, n, std::move(bound),
                        sized_sentinel_concept<S, I>(), iterator_concept<I>());
                }
            };
        }

        /// \ingroup group-utility
        /// \sa `advance_fn`
        /// Not to spec: advance is an ADL customization point
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(adl_advance_detail::advance_fn, advance)
        }

        namespace adl_advance_detail
        {
            template<typename I>
            RANGES_CXX14_CONSTEXPR
            void advance_fn::n_impl_(I &i, difference_type_t<I> n, concepts::InputIterator *)
            {
                RANGES_EXPECT(n >= 0);
                for(; n > 0; --n)
                    ++i;
            }
            template<typename I>
            RANGES_CXX14_CONSTEXPR
            void advance_fn::n_impl_(I &i, difference_type_t<I> n, concepts::BidirectionalIterator *)
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
            void advance_fn::n_impl_(I &i, difference_type_t<I> n, concepts::RandomAccessIterator *)
            {
                i += n;
            }
            // Is there an advance that is find-able by ADL and is preferred
            // by partial ordering to the poison-pill overload?
            template<typename I>
            RANGES_CXX14_CONSTEXPR
            auto advance_fn::n_(I &i, difference_type_t<I> n, int) ->
                decltype(static_cast<void>(advance(i, n)))
            {
                advance(i, n);
            }
            // No advance overload found by ADL, use the default implementation:
            template<typename I>
            RANGES_CXX14_CONSTEXPR
            void advance_fn::n_(I &i, difference_type_t<I> n, long)
            {
                advance_fn::n_impl_(i, n, iterator_concept<I>{});
            }
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
                difference_type_t<I> d = s - i;
                RANGES_EXPECT(0 <= d);
                ranges::advance(i, d);
            }
            template<typename I, typename D, typename S>
            RANGES_CXX14_CONSTEXPR
            D advance_fn::bounded_(I &it, D n, S bound, concepts::Sentinel*,
                concepts::InputIterator*)
            {
                RANGES_EXPECT(0 <= n);
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
                RANGES_EXPECT((Same<I, S>() || 0 <= n));
                D d = bound - it;
                RANGES_EXPECT(0 <= n ? 0 <= d : 0 >= d);
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
            I operator()(I it, difference_type_t<I> n) const
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
            I operator()(I it, difference_type_t<I> n, S bound) const
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
            I operator()(I it, difference_type_t<I> n = 1) const
            {
                advance(it, -n);
                return it;
            }
            template<typename I,
                CONCEPT_REQUIRES_(BidirectionalIterator<I>())>
            RANGES_CXX14_CONSTEXPR
            I operator()(I it, difference_type_t<I> n, I bound) const
            {
                advance(it, -n, std::move(bound));
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
                RANGES_EXPECT((Same<I, S>() || 0 <= n));
                return {n + d, end};
            }
            template<typename I, typename S, typename D>
            RANGES_CXX14_CONSTEXPR
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::SizedSentinel*) const
            {
                auto n = static_cast<D>(end - begin);
                RANGES_EXPECT((Same<I, S>() || 0 <= n));
                return {n + d, ranges::next(begin, end)};
            }
        public:
            template<typename I, typename S, typename D = difference_type_t<I>,
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
                RANGES_EXPECT((Same<I, S>() || 0 <= n));
                return n + d;
            }
        public:
            template<typename I, typename S, typename D = difference_type_t<I>,
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
            int impl_i(I begin, S end, difference_type_t<I> n, concepts::Sentinel*) const
            {
                if(n < 0)
                    return 1;
                for(; n > 0; --n, ++begin)
                {
                    if(begin == end)
                        return -1;
                }
                return begin == end ? 0 : 1;
            }
            template<typename I, typename S>
            RANGES_CXX14_CONSTEXPR
            int impl_i(I begin, S end, difference_type_t<I> n, concepts::SizedSentinel*) const
            {
                difference_type_t<I> dist = end - begin;
                if(n < dist)
                    return  1;
                if(dist < n)
                    return -1;
                return  0;
            }
        public:
            template<typename I, typename S,
                CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>())>
            RANGES_CXX14_CONSTEXPR
            int operator()(I begin, S end, difference_type_t<I> n) const
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
            template<typename I, typename S,
                CONCEPT_REQUIRES_(SizedSentinel<S, I>())>
            RANGES_CXX14_CONSTEXPR
            size_type_t<I> operator()(I const& begin, S end) const
            {
                difference_type_t<I> n = end - begin;
                RANGES_EXPECT(0 <= n);
                return static_cast<size_type_t<I>>(n);
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_size_fn`
        RANGES_INLINE_VARIABLE(iter_size_fn, iter_size)

        template<typename Container>
        struct back_insert_iterator
        {
            using container_type = Container;
            using difference_type = std::ptrdiff_t;

            constexpr back_insert_iterator() = default;
            explicit back_insert_iterator(Container &x)
              : container_(std::addressof(x))
            {}
            back_insert_iterator &operator=(typename Container::value_type const &value)
            {
                container_->push_back(value);
                return *this;
            }
            back_insert_iterator &operator=(typename Container::value_type &&value)
            {
                container_->push_back(std::move(value));
                return *this;
            }
            back_insert_iterator &operator*()
            {
                return *this;
            }
            back_insert_iterator &operator++()
            {
                return *this;
            }
            back_insert_iterator operator++(int)
            {
                return *this;
            }
        private:
            Container *container_ = nullptr;
        };

        struct back_inserter_fn
        {
            template<typename Container>
            constexpr back_insert_iterator<Container> operator()(Container &x) const
            {
                return back_insert_iterator<Container>{x};
            }
        };

        /// \ingroup group-utility
        /// \sa `back_inserter_fn`
        RANGES_INLINE_VARIABLE(back_inserter_fn, back_inserter)

        template<typename Container>
        struct front_insert_iterator
        {
            using container_type = Container;
            using difference_type = std::ptrdiff_t;

            constexpr front_insert_iterator() = default;
            explicit front_insert_iterator(Container &x)
              : container_(std::addressof(x))
            {}
            front_insert_iterator &operator=(typename Container::value_type const &value)
            {
                container_->push_front(value);
                return *this;
            }
            front_insert_iterator &operator=(typename Container::value_type &&value)
            {
                container_->push_front(std::move(value));
                return *this;
            }
            front_insert_iterator &operator*()
            {
                return *this;
            }
            front_insert_iterator &operator++()
            {
                return *this;
            }
            front_insert_iterator operator++(int)
            {
                return *this;
            }
        private:
            Container *container_ = nullptr;
        };

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

        template<typename Container>
        struct insert_iterator
        {
            using container_type = Container;
            using difference_type = std::ptrdiff_t;

            constexpr insert_iterator() = default;
            explicit insert_iterator(Container &x, typename Container::iterator w)
              : container_(std::addressof(x)), where_(w)
            {}
            insert_iterator &operator=(typename Container::value_type const &value)
            {
                where_ = ranges::next(container_->insert(where_, value));
                return *this;
            }
            insert_iterator &operator=(typename Container::value_type &&value)
            {
                where_ = ranges::next(container_->insert(where_, std::move(value)));
                return *this;
            }
            insert_iterator &operator*()
            {
                return *this;
            }
            insert_iterator &operator++()
            {
                return *this;
            }
            insert_iterator &operator++(int)
            {
                return *this;
            }
        private:
            Container* container_ = nullptr;
            typename Container::iterator where_ = detail::value_init{};
        };

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
            using difference_type = std::ptrdiff_t;
            using char_type = Char;
            using traits_type = Traits;
            using ostream_type = std::basic_ostream<Char, Traits>;

            constexpr ostream_iterator() = default;
            ostream_iterator(ostream_type &s, Char const *d = nullptr) noexcept
              : sout_(&s), delim_(d)
            {}
            template<typename U, typename V = meta::if_<std::is_void<T>, U, T>,
                CONCEPT_REQUIRES_(ConvertibleTo<U, V const&>())>
            ostream_iterator& operator=(U &&value)
            {
                RANGES_EXPECT(sout_);
                *sout_ << value;
                if(delim_)
                    *sout_ << delim_;
                return *this;
            }
            ostream_iterator& operator*()
            {
                return *this;
            }
            ostream_iterator& operator++()
            {
                return *this;
            }
            ostream_iterator& operator++(int)
            {
                return *this;
            }
        private:
            ostream_type *sout_;
            Char const *delim_;
        };

        template<typename Char, typename Traits = std::char_traits<Char>>
        struct ostreambuf_iterator
        {
        public:
            typedef ptrdiff_t difference_type;
            typedef Char char_type;
            typedef Traits traits_type;
            typedef std::basic_streambuf<Char, Traits> streambuf_type;
            typedef std::basic_ostream<Char, Traits> ostream_type;

            constexpr ostreambuf_iterator() = default;
            ostreambuf_iterator(ostream_type &s) noexcept
              : ostreambuf_iterator(s.rdbuf())
            {
            }
            ostreambuf_iterator(streambuf_type *s) noexcept
              : sbuf_(s)
            {
                RANGES_ASSERT(s != nullptr);
            }
            ostreambuf_iterator &operator=(Char c)
            {
                RANGES_ASSERT(sbuf_ != nullptr);
                if(!failed_)
                    failed_ = (sbuf_->sputc(c) == Traits::eof());
                return *this;
            }
            ostreambuf_iterator &operator*()
            {
                return *this;
            }
            ostreambuf_iterator &operator++()
            {
                return *this;
            }
            ostreambuf_iterator &operator++(int)
            {
                return *this;
            }
            bool failed() const noexcept
            {
                return failed_;
            }
        private:
            streambuf_type *sbuf_ = nullptr;
            bool failed_ = false;
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
                auto read() const -> reference_t<I>
                {
                    return *arrow();
                }
                RANGES_CXX14_CONSTEXPR
                I arrow() const
                {
                    return ranges::prev(it_);
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
                void advance(difference_type_t<I> n)
                {
                    it_ -= n;
                }
                CONCEPT_REQUIRES(SizedSentinel<I, I>())
                RANGES_CXX14_CONSTEXPR
                difference_type_t<I>
                distance_to(reverse_cursor const &that) const
                {
                    return it_ - that.base();
                }
                RANGES_CXX14_CONSTEXPR
                auto move() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    iter_move(ranges::prev(it_))
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

        template<typename I>
        struct move_iterator
        {
        private:
            CONCEPT_ASSERT(InputIterator<I>());
            I current_ = detail::value_init{};
        public:
            using iterator_type = I;
            using difference_type = difference_type_t<I>;
            using value_type = value_type_t<I>;
            using iterator_category = input_iterator_tag;
            using reference = rvalue_reference_t<I>;

            constexpr move_iterator() = default;
            explicit move_iterator(I i)
              : current_(i)
            {}
            template<typename O,
                CONCEPT_REQUIRES_(ConvertibleTo<O, I>())>
            move_iterator(move_iterator<O> const &i)
              : current_(i.base())
            {}
            template<typename O,
                CONCEPT_REQUIRES_(ConvertibleTo<O, I>())>
            move_iterator &operator=(move_iterator<O> const & i)
            {
                current_ = i.base();
                return *this;
            }
            I base() const
            {
                return current_;
            }
            auto operator*() const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                iter_move(current_)
            )
            move_iterator &operator++()
            {
                ++current_;
                return *this;
            }
            CONCEPT_REQUIRES(!ForwardIterator<I>())
            void operator++(int)
            {
                ++current_;
            }
            CONCEPT_REQUIRES(ForwardIterator<I>())
            move_iterator operator++(int)
            {
                return move_iterator(current_++);
            }
            CONCEPT_REQUIRES(BidirectionalIterator<I>())
            move_iterator &operator--()
            {
                --current_;
                return *this;
            }
            CONCEPT_REQUIRES(BidirectionalIterator<I>())
            move_iterator operator--(int)
            {
                return move_iterator(current_--);
            }
            CONCEPT_REQUIRES(RandomAccessIterator<I>())
            move_iterator operator+(difference_type n) const
            {
                return move_iterator(current_ + n);
            }
            CONCEPT_REQUIRES(RandomAccessIterator<I>())
            move_iterator &operator+=(difference_type n)
            {
                current_ += n;
                return *this;
            }
            CONCEPT_REQUIRES(RandomAccessIterator<I>())
            move_iterator operator-(difference_type n) const
            {
                return move_iterator(current_ - n);
            }
            CONCEPT_REQUIRES(RandomAccessIterator<I>())
            move_iterator &operator-=(difference_type n)
            {
                current_ -= n;
                return *this;
            }
            CONCEPT_REQUIRES(RandomAccessIterator<I>())
            reference operator[](difference_type n) const
            {
                return iter_move(current_ + n);
            }
        };

        template<typename I1, typename I2,
            CONCEPT_REQUIRES_(EqualityComparable<I1, I2>())>
        bool operator==(move_iterator<I1> const &x, move_iterator<I2> const &y)
        {
            return x.base() == y.base();
        }
        template<typename I1, typename I2,
            CONCEPT_REQUIRES_(EqualityComparable<I1, I2>())>
        bool operator!=(move_iterator<I1> const &x, move_iterator<I2> const &y)
        {
            return !(x == y);
        }
        template<typename I1, typename I2,
            CONCEPT_REQUIRES_(TotallyOrdered<I1, I2>())>
        bool operator<(move_iterator<I1> const &x, move_iterator<I2> const &y)
        {
            return x.base() < y.base();
        }
        template<typename I1, typename I2,
            CONCEPT_REQUIRES_(TotallyOrdered<I1, I2>())>
        bool operator<=(move_iterator<I1> const &x, move_iterator<I2> const &y)
        {
            return !(y < x);
        }
        template<typename I1, typename I2,
            CONCEPT_REQUIRES_(TotallyOrdered<I1, I2>())>
        bool operator>(move_iterator<I1> const &x, move_iterator<I2> const &y)
        {
            return y < x;
        }
        template<typename I1, typename I2,
            CONCEPT_REQUIRES_(TotallyOrdered<I1, I2>())>
        bool operator>=(move_iterator<I1> const &x, move_iterator<I2> const &y)
        {
            return !(x < y);
        }

        template<typename I1, typename I2,
            CONCEPT_REQUIRES_(SizedSentinel<I1, I2>())>
        difference_type_t<I2> operator-(move_iterator<I1> const &x, move_iterator<I2> const &y)
        {
            return x.base() - y.base();
        }
        template<typename I,
            CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
        move_iterator<I> operator+(difference_type_t<I> n, move_iterator<I> const &x)
        {
            return x + n;
        }

        CONCEPT_ASSERT(InputIterator<move_iterator<int*>>());

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
        bool operator==(move_sentinel<S> const &s, move_iterator<I> const &i)
        {
            return s.base() == i.base();
        }
        template<typename I, typename S,
            CONCEPT_REQUIRES_(Sentinel<S, I>())>
        bool operator!=(move_iterator<I> const &i, move_sentinel<S> const &s)
        {
            return i.base() != s.base();
        }
        template<typename I, typename S,
            CONCEPT_REQUIRES_(Sentinel<S, I>())>
        bool operator!=(move_sentinel<S> const &s, move_iterator<I> const &i)
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
                using value_type = value_type_t<I>;
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
                        return this->get().it_;
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
                    CONCEPT_REQUIRES_(Writable<I, aux::move_t<T>>())>
                void write(T &&t) noexcept(noexcept(*it_ = std::move(t)))
                {
                    *it_ = std::move(t);
                }
                template<typename T,
                    CONCEPT_REQUIRES_(Writable<I, aux::move_t<T>>())>
                void write(T &&t) const noexcept(noexcept(*it_ = std::move(t)))
                {
                    *it_ = std::move(t);
                }
                CONCEPT_REQUIRES(Readable<I>())
                reference_t<I> read() const
                    noexcept(noexcept(*std::declval<I const&>()))
                {
                    return *it_;
                }
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
                void advance(difference_type_t<I> n)
                {
                    it_ += n;
                }
                CONCEPT_REQUIRES(SizedSentinel<I, I>())
                difference_type_t<I> distance_to(move_into_cursor const &that) const
                {
                    return that.it_ - it_;
                }
                template<typename II = I,
                    CONCEPT_REQUIRES_(Same<I, II>() && Readable<II>())>
                RANGES_CXX14_CONSTEXPR
                rvalue_reference_t<II const> move() const
                    noexcept(noexcept(iter_move(std::declval<II const&>())))
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
            constexpr I recounted(I const &, I i, difference_type_t<I>)
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
                auto operator()(I i, J j, difference_type_t<J> n) const ->
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

        /// \cond
        namespace detail
        {
            struct std_output_iterator_traits
            {
                using iterator_category = std::output_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = void;
                using reference = void;
                using pointer = void;
            };
        }
        /// \endcond
    }
}

/// \cond
namespace std
{
    template<typename Container>
    struct iterator_traits< ::ranges::back_insert_iterator<Container>>
      : ::ranges::detail::std_output_iterator_traits
    {};

    template<typename Container>
    struct iterator_traits< ::ranges::front_insert_iterator<Container>>
      : ::ranges::detail::std_output_iterator_traits
    {};

    template<typename Container>
    struct iterator_traits< ::ranges::insert_iterator<Container>>
      : ::ranges::detail::std_output_iterator_traits
    {};

    template<typename T, typename Char, typename Traits>
    struct iterator_traits< ::ranges::ostream_iterator<T, Char, Traits>>
      : ::ranges::detail::std_output_iterator_traits
    {};

    template<typename Char, typename Traits>
    struct iterator_traits< ::ranges::ostreambuf_iterator<Char, Traits>>
      : ::ranges::detail::std_output_iterator_traits
    {};

    template<typename I>
    struct iterator_traits< ::ranges::move_iterator<I>>
    {
        using iterator_category = std::input_iterator_tag;
        using difference_type = typename ::ranges::move_iterator<I>::difference_type;
        using value_type = typename ::ranges::move_iterator<I>::value_type;
        using reference = typename ::ranges::move_iterator<I>::reference;
        using pointer = meta::_t<std::add_pointer<reference>>;
    };
}
/// \endcond

#endif
