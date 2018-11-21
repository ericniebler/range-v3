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
                constexpr /*c++14*/
                static void n_impl_(I &i, iter_difference_t<I> n,
                    detail::input_iterator_tag);
                template<typename I>
                constexpr /*c++14*/
                static void n_impl_(I &i, iter_difference_t<I> n,
                    detail::bidirectional_iterator_tag);
                template<typename I>
                constexpr /*c++14*/
                static void n_impl_(I &i, iter_difference_t<I> n,
                    detail::random_access_iterator_tag);
                // Is there an advance that is find-able by ADL and is preferred
                // by partial ordering to the poison-pill overload?
                template<typename I>
                constexpr /*c++14*/
                static auto n_(I &i, iter_difference_t<I> n, int) ->
                    decltype(static_cast<void>(advance(i, n)));
                // No advance overload found by ADL, use the default implementation:
                template<typename I>
                constexpr /*c++14*/
                static void n_(I &i, iter_difference_t<I> n, long);
                template<typename I, typename S>
                constexpr /*c++14*/
                static void to_(I &i, S s, sentinel_tag);
                template<typename I, typename S>
                constexpr /*c++14*/
                static void to_(I &i, S s, sized_sentinel_tag);
                template<typename I, typename D, typename S>
                constexpr /*c++14*/
                static D bounded_(I &it, D n, S bound, sentinel_tag,
                    detail::input_iterator_tag);
                template<typename I, typename D>
                constexpr /*c++14*/
                static D bounded_(I &it, D n, I bound, sentinel_tag,
                    detail::bidirectional_iterator_tag);
                template<typename I, typename D, typename S, typename Concept>
                constexpr /*c++14*/
                static D bounded_(I &it, D n, S bound, sized_sentinel_tag,
                    Concept);
            public:
                // Advance a certain number of steps:
                template<typename I>
                constexpr /*c++14*/
                auto operator()(I &i, iter_difference_t<I> n) const ->
                    CPP_ret(void)(requires Iterator<I>)
                {
                    advance_fn::n_(i, n, 0);
                }
                // Advance to a certain position:
                template<typename I, typename S>
                constexpr /*c++14*/
                auto operator()(I &i, S s) const ->
                    CPP_ret(void)(
                        requires Sentinel<S, I> && Assignable<I&, S>)
                {
                    i = std::move(s);
                }
                template<typename I, typename S>
                constexpr /*c++14*/
                auto operator()(I &i, S s) const ->
                    CPP_ret(void)(
                        requires Sentinel<S, I> && !Assignable<I&, S>)
                {
                    advance_fn::to_(i, std::move(s), sentinel_tag_of<S, I>());
                }
                // Advance a certain number of times, with a bound:
                template<typename I, typename S>
                constexpr /*c++14*/
                auto operator()(I &it, iter_difference_t<I> n, S bound) const ->
                    CPP_ret(iter_difference_t<I>)(
                        requires Sentinel<S, I>)
                {
                    return advance_fn::bounded_(it, n, std::move(bound),
                        sentinel_tag_of<S, I>(), iterator_tag_of<I>());
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
            constexpr /*c++14*/
            void advance_fn::n_impl_(I &i, iter_difference_t<I> n,
                detail::input_iterator_tag)
            {
                RANGES_EXPECT(n >= 0);
                for(; n > 0; --n)
                    ++i;
            }
            template<typename I>
            constexpr /*c++14*/
            void advance_fn::n_impl_(I &i, iter_difference_t<I> n,
                detail::bidirectional_iterator_tag)
            {
                if(n > 0)
                    for(; n > 0; --n)
                        ++i;
                else
                    for(; n < 0; ++n)
                        --i;
            }
            template<typename I>
            constexpr /*c++14*/
            void advance_fn::n_impl_(I &i, iter_difference_t<I> n,
                detail::random_access_iterator_tag)
            {
                i += n;
            }
            // Is there an advance that is find-able by ADL and is preferred
            // by partial ordering to the poison-pill overload?
            template<typename I>
            constexpr /*c++14*/
            auto advance_fn::n_(I &i, iter_difference_t<I> n, int) ->
                decltype(static_cast<void>(advance(i, n)))
            {
                advance(i, n);
            }
            // No advance overload found by ADL, use the default implementation:
            template<typename I>
            constexpr /*c++14*/
            void advance_fn::n_(I &i, iter_difference_t<I> n, long)
            {
                advance_fn::n_impl_(i, n, iterator_tag_of<I>{});
            }
            template<typename I, typename S>
            constexpr /*c++14*/
            void advance_fn::to_(I &i, S s, sentinel_tag)
            {
                while(i != s)
                    ++i;
            }
            template<typename I, typename S>
            constexpr /*c++14*/
            void advance_fn::to_(I &i, S s, sized_sentinel_tag)
            {
                iter_difference_t<I> d = s - i;
                RANGES_EXPECT(0 <= d);
                ranges::advance(i, d);
            }
            template<typename I, typename D, typename S>
            constexpr /*c++14*/
            D advance_fn::bounded_(I &it, D n, S bound, sentinel_tag,
                detail::input_iterator_tag)
            {
                RANGES_EXPECT(0 <= n);
                for(; 0 != n && it != bound; --n)
                    ++it;
                return n;
            }
            template<typename I, typename D>
            constexpr /*c++14*/
            D advance_fn::bounded_(I &it, D n, I bound, sentinel_tag,
                detail::bidirectional_iterator_tag)
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
            constexpr /*c++14*/
            D advance_fn::bounded_(I &it, D n, S bound, sized_sentinel_tag,
                Concept)
            {
                RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
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
            template<typename I>
            constexpr /*c++14*/
            auto operator()(I it) const ->
                CPP_ret(I)(requires Iterator<I>)
            {
                return ++it;
            }
            template<typename I>
            constexpr /*c++14*/
            auto operator()(I it, iter_difference_t<I> n) const ->
                CPP_ret(I)(requires Iterator<I>)
            {
                advance(it, n);
                return it;
            }
            template<typename I, typename S>
            constexpr /*c++14*/
            auto operator()(I it, S s) const ->
                CPP_ret(I)(requires Sentinel<S, I>)
            {
                advance(it, std::move(s));
                return it;
            }
            template<typename I, typename S>
            constexpr /*c++14*/
            auto operator()(I it, iter_difference_t<I> n, S bound) const ->
                CPP_ret(I)(requires Sentinel<S, I>)
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
            template<typename I>
            constexpr /*c++14*/
            auto operator()(I it) const ->
                CPP_ret(I)(requires BidirectionalIterator<I>)
            {
                return --it;
            }
            template<typename I>
            constexpr /*c++14*/
            auto operator()(I it, iter_difference_t<I> n) const ->
                CPP_ret(I)(requires BidirectionalIterator<I>)
            {
                advance(it, -n);
                return it;
            }
            template<typename I>
            constexpr /*c++14*/
            auto operator()(I it, iter_difference_t<I> n, I bound) const ->
                CPP_ret(I)(requires BidirectionalIterator<I>)
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
            template<typename I, typename S, typename D>
            constexpr /*c++14*/
            auto impl_i(I begin, S end, D d, sentinel_tag) const ->
                CPP_ret(std::pair<D, I>)(
                    requires not SizedSentinel<I, I>)
            {
                for(; begin != end; ++begin)
                    ++d;
                return {d, begin};
            }
            template<typename I, typename S, typename D>
            constexpr /*c++14*/
            auto impl_i(I begin, S end_, D d, sentinel_tag) const ->
                CPP_ret(std::pair<D, I>)(
                    requires SizedSentinel<I, I>)
            {
                I end = ranges::next(begin, end_);
                auto n = static_cast<D>(end - begin);
                RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
                return {n + d, end};
            }
            template<typename I, typename S, typename D>
            constexpr /*c++14*/
            std::pair<D, I> impl_i(I begin, S end, D d, sized_sentinel_tag) const
            {
                auto n = static_cast<D>(end - begin);
                RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
                return {n + d, ranges::next(begin, end)};
            }
        public:
            template<typename I, typename S, typename D = iter_difference_t<I>>
            constexpr /*c++14*/
            auto operator()(I begin, S end, D d = 0) const ->
                CPP_ret(std::pair<D, I>)(
                    requires Iterator<I> && Sentinel<S, I> && Integral<D>)
            {
                return this->impl_i(std::move(begin), std::move(end), d,
                    sentinel_tag_of<S, I>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_enumerate_fn`
        RANGES_INLINE_VARIABLE(iter_enumerate_fn, iter_enumerate)

        struct iter_distance_fn
        {
        private:
            template<typename I, typename S, typename D>
            constexpr /*c++14*/
            D impl_i(I begin, S end, D d, sentinel_tag) const
            {
                return iter_enumerate(std::move(begin), std::move(end), d).first;
            }
            template<typename I, typename S, typename D>
            constexpr /*c++14*/
            D impl_i(I begin, S end, D d, sized_sentinel_tag) const
            {
                auto n = static_cast<D>(end - begin);
                RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
                return n + d;
            }
        public:
            template<typename I, typename S, typename D = iter_difference_t<I>>
            constexpr /*c++14*/
            auto operator()(I begin, S end, D d = 0) const ->
                CPP_ret(D)(
                    requires Iterator<I> && Sentinel<S, I> && Integral<D>)
            {
                return this->impl_i(std::move(begin), std::move(end), d,
                    sentinel_tag_of<S, I>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_distance_fn`
        RANGES_INLINE_VARIABLE(iter_distance_fn, iter_distance)

        struct iter_distance_compare_fn
        {
        private:
            template<typename I, typename S>
            constexpr /*c++14*/
            int impl_i(I begin, S end, iter_difference_t<I> n, sentinel_tag) const
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
            constexpr /*c++14*/
            int impl_i(I begin, S end, iter_difference_t<I> n, sized_sentinel_tag) const
            {
                iter_difference_t<I> dist = end - begin;
                if(n < dist)
                    return  1;
                if(dist < n)
                    return -1;
                return  0;
            }
        public:
            template<typename I, typename S>
            constexpr /*c++14*/
            auto operator()(I begin, S end, iter_difference_t<I> n) const ->
                CPP_ret(int)(
                    requires InputIterator<I> && Sentinel<S, I>)
            {
                return this->impl_i(std::move(begin), std::move(end), n,
                    sentinel_tag_of<S, I>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_distance_compare_fn`
        RANGES_INLINE_VARIABLE(iter_distance_compare_fn, iter_distance_compare)

        // Like distance(b,e), but guaranteed to be O(1)
        struct iter_size_fn
        {
            template<typename I, typename S>
            constexpr /*c++14*/
            auto operator()(I const& begin, S end) const ->
                CPP_ret(meta::_t<std::make_unsigned<iter_difference_t<I>>>)(
                    requires SizedSentinel<S, I>)
            {
                using size_type = meta::_t<std::make_unsigned<iter_difference_t<I>>>;
                iter_difference_t<I> n = end - begin;
                RANGES_EXPECT(0 <= n);
                return static_cast<size_type>(n);
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
            template<typename U>
            auto operator=(U &&value) ->
                CPP_ret(ostream_iterator &)(
                    requires ConvertibleTo<U, meta::if_<std::is_void<T>, U, T> const&>)
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
                CPP_assert(BidirectionalIterator<I>);
                friend range_access;
                template<typename OtherI>
                friend struct reverse_cursor;
                struct mixin : basic_mixin<reverse_cursor>
                {
                    mixin() = default;
                    using basic_mixin<reverse_cursor>::basic_mixin;
                    constexpr /*c++14*/
                    mixin(I it)
                      : mixin{reverse_cursor{it}}
                    {}
                    constexpr /*c++14*/
                    I base() const
                    {
                        return this->get().base();
                    }
                };

                I it_;

                constexpr /*c++14*/
                reverse_cursor(I it)
                  : it_(std::move(it))
                {}
                constexpr /*c++14*/
                auto read() const -> iter_reference_t<I>
                {
                    return *arrow();
                }
                constexpr /*c++14*/
                I arrow() const
                {
                    return ranges::prev(it_);
                }
                constexpr /*c++14*/
                I base() const
                {
                    return it_;
                }
                template<typename J>
                constexpr /*c++14*/
                auto equal(reverse_cursor<J> const& that) const ->
                    CPP_ret(bool)(
                        requires Sentinel<J, I>)
                {
                    return it_ == that.it_;
                }
                constexpr /*c++14*/
                void next()
                {
                    --it_;
                }
                constexpr /*c++14*/
                void prev()
                {
                    ++it_;
                }
                CPP_member
                constexpr /*c++14*/
                auto advance(iter_difference_t<I> n) ->
                    CPP_ret(void)(
                        requires RandomAccessIterator<I>)
                {
                    it_ -= n;
                }
                template<typename J>
                constexpr /*c++14*/
                auto distance_to(reverse_cursor<J> const &that) const ->
                    CPP_ret(iter_difference_t<I>)(
                        requires SizedSentinel<J, I>)
                {
                    return it_ - that.base();
                }
                constexpr /*c++14*/
                auto CPP_auto_fun(move)() (const)
                (
                    return iter_move(ranges::prev(it_))
                )
            public:
                reverse_cursor() = default;
                template<typename U>
                constexpr /*c++14*/
                CPP_ctor(reverse_cursor)(reverse_cursor<U> const &u)(
                    requires ConvertibleTo<U, I>)
                  : it_(u.base())
                {}
            };
        }  // namespace detail
        /// \endcond

        struct make_reverse_iterator_fn
        {
            template<typename I>
            constexpr /*c++14*/
            auto operator()(I i) const -> CPP_ret(reverse_iterator<I>)(
                requires BidirectionalIterator<I>)
            {
                return reverse_iterator<I>(i);
            }
        };

        RANGES_INLINE_VARIABLE(make_reverse_iterator_fn, make_reverse_iterator)

        template<typename I>
        struct move_iterator
        {
        private:
            CPP_assert(InputIterator<I>);
            I current_ = detail::value_init{};
        public:
            using iterator_type = I;
            using difference_type = iter_difference_t<I>;
            using value_type = iter_value_t<I>;
            using iterator_category = ranges::input_iterator_tag;
            using reference = iter_rvalue_reference_t<I>;

            constexpr move_iterator() = default;
            explicit move_iterator(I i)
              : current_(i)
            {}
            template<typename O>
            CPP_ctor(move_iterator)(move_iterator<O> const &i)(
                requires ConvertibleTo<O, I>)
              : current_(i.base())
            {}
            template<typename O>
            auto operator=(move_iterator<O> const & i) ->
                CPP_ret(move_iterator &)(
                    requires ConvertibleTo<O, I>)
            {
                current_ = i.base();
                return *this;
            }
            I base() const
            {
                return current_;
            }
            auto CPP_auto_fun(operator*)() (const)
            (
                return iter_move(current_)
            )
            move_iterator &operator++()
            {
                ++current_;
                return *this;
            }
            CPP_member
            auto operator++(int) ->
                CPP_ret(void)(
                    requires not ForwardIterator<I>)
            {
                ++current_;
            }
            CPP_member
            auto operator++(int) ->
                CPP_ret(move_iterator)(
                    requires ForwardIterator<I>)
            {
                return move_iterator(current_++);
            }
            CPP_member
            auto operator--() ->
                CPP_ret(move_iterator &)(
                    requires ForwardIterator<I>)
            {
                --current_;
                return *this;
            }
            CPP_member
            auto operator--(int) ->
                CPP_ret(move_iterator)(
                    requires BidirectionalIterator<I>)
            {
                return move_iterator(current_--);
            }
            CPP_member
            auto operator+(difference_type n) const ->
                CPP_ret(move_iterator)(
                    requires RandomAccessIterator<I>)
            {
                return move_iterator(current_ + n);
            }
            CPP_member
            auto operator+=(difference_type n) ->
                CPP_ret(move_iterator &)(
                    requires RandomAccessIterator<I>)
            {
                current_ += n;
                return *this;
            }
            CPP_member
            auto operator-(difference_type n) const ->
                CPP_ret(move_iterator)(
                    requires RandomAccessIterator<I>)
            {
                return move_iterator(current_ - n);
            }
            CPP_member
            auto operator-=(difference_type n) ->
                CPP_ret(move_iterator &)(
                    requires RandomAccessIterator<I>)
            {
                current_ -= n;
                return *this;
            }
            CPP_member
            auto operator[](difference_type n) const ->
                CPP_ret(reference)(
                    requires RandomAccessIterator<I>)
            {
                return iter_move(current_ + n);
            }
        };

        template<typename I1, typename I2>
        auto operator==(move_iterator<I1> const &x, move_iterator<I2> const &y) ->
            CPP_ret(bool)(
                requires EqualityComparableWith<I1, I2>)
        {
            return x.base() == y.base();
        }
        template<typename I1, typename I2>
        auto operator!=(move_iterator<I1> const &x, move_iterator<I2> const &y) ->
            CPP_ret(bool)(
                requires EqualityComparableWith<I1, I2>)
        {
            return !(x == y);
        }
        template<typename I1, typename I2>
        auto operator<(move_iterator<I1> const &x, move_iterator<I2> const &y) ->
            CPP_ret(bool)(
                requires StrictTotallyOrderedWith<I1, I2>)
        {
            return x.base() < y.base();
        }
        template<typename I1, typename I2>
        auto operator<=(move_iterator<I1> const &x, move_iterator<I2> const &y) ->
            CPP_ret(bool)(
                requires StrictTotallyOrderedWith<I1, I2>)
        {
            return !(y < x);
        }
        template<typename I1, typename I2>
        auto operator>(move_iterator<I1> const &x, move_iterator<I2> const &y) ->
            CPP_ret(bool)(
                requires StrictTotallyOrderedWith<I1, I2>)
        {
            return y < x;
        }
        template<typename I1, typename I2>
        auto operator>=(move_iterator<I1> const &x, move_iterator<I2> const &y) ->
            CPP_ret(bool)(
                requires StrictTotallyOrderedWith<I1, I2>)
        {
            return !(x < y);
        }

        template<typename I1, typename I2>
        auto operator-(move_iterator<I1> const &x, move_iterator<I2> const &y) ->
            CPP_ret(iter_difference_t<I2>)(
                requires SizedSentinel<I1, I2>)
        {
            return x.base() - y.base();
        }
        template<typename I>
        auto operator+(iter_difference_t<I> n, move_iterator<I> const &x) ->
            CPP_ret(move_iterator<I>)(
                requires RandomAccessIterator<I>)
        {
            return x + n;
        }

        struct make_move_iterator_fn
        {
            template<typename I>
            constexpr auto operator()(I it) const ->
                CPP_ret(move_iterator<I>)(
                    requires InputIterator<I>)
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
            template<typename OS>
            constexpr explicit CPP_ctor(move_sentinel)(move_sentinel<OS> const &that)(
                requires ConvertibleTo<OS, S>)
              : sent_(that.base())
            {}
            template<typename OS>
            auto operator=(move_sentinel<OS> const &that) ->
                CPP_ret(move_sentinel &)(
                    requires ConvertibleTo<OS, S>)
            {
                sent_ = that.base();
                return *this;
            }
            S base() const
            {
                return sent_;
            }
        };

        template<typename I, typename S>
        auto operator==(move_iterator<I> const &i, move_sentinel<S> const &s) ->
            CPP_ret(bool)(
                requires Sentinel<S, I>)
        {
            return i.base() == s.base();
        }
        template<typename I, typename S>
        auto operator==(move_sentinel<S> const &s, move_iterator<I> const &i) ->
            CPP_ret(bool)(
                requires Sentinel<S, I>)
        {
            return s.base() == i.base();
        }
        template<typename I, typename S>
        auto operator!=(move_iterator<I> const &i, move_sentinel<S> const &s) ->
            CPP_ret(bool)(
                requires Sentinel<S, I>)
        {
            return i.base() != s.base();
        }
        template<typename I, typename S>
        auto operator!=(move_sentinel<S> const &s, move_iterator<I> const &i) ->
            CPP_ret(bool)(
                requires Sentinel<S, I>)
        {
            return s.base() != i.base();
        }

        struct make_move_sentinel_fn
        {
            template<typename I>
            constexpr auto operator()(I i) const ->
                CPP_ret(move_iterator<I>)(
                    requires InputIterator<I>)
            {
                return move_iterator<I>{detail::move(i)};
            }

            template<typename S>
            constexpr auto operator()(S s) const ->
                CPP_ret(move_sentinel<S>)(
                    requires Semiregular<S> && !InputIterator<S>)
            {
                return move_sentinel<S>{detail::move(s)};
            }
        };

        RANGES_INLINE_VARIABLE(make_move_sentinel_fn, make_move_sentinel)

        /// \cond
        namespace detail
        {
            template<typename I, bool IsReadable>
            struct move_into_cursor_types_
            {};

            template<typename I>
            struct move_into_cursor_types_<I, true>
            {
                using value_type = iter_value_t<I>;
                using single_pass = meta::bool_<(bool)SinglePass<I>>;
            };

            template<typename I>
            using move_into_cursor_types = move_into_cursor_types_<I, (bool) Readable<I>>;

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
                template<typename T>
                auto write(T &&t) noexcept(noexcept(*it_ = std::move(t))) ->
                    CPP_ret(void)(
                        requires Writable<I, aux::move_t<T>>)
                {
                    *it_ = std::move(t);
                }
                template<typename T>
                auto write(T &&t) const noexcept(noexcept(*it_ = std::move(t))) ->
                    CPP_ret(void)(
                        requires Writable<I, aux::move_t<T>>)
                {
                    *it_ = std::move(t);
                }
                CPP_member
                auto read() const
                    noexcept(noexcept(*std::declval<I const&>())) ->
                    CPP_ret(iter_reference_t<I>)(
                        requires Readable<I>)
                {
                    return *it_;
                }
                CPP_member
                auto equal(move_into_cursor const &that) const ->
                    CPP_ret(bool)(
                        requires InputIterator<I>)
                {
                    return it_ == that.it_;
                }
                CPP_member
                auto prev() ->
                    CPP_ret(void)(
                        requires BidirectionalIterator<I>)
                {
                    --it_;
                }
                CPP_member
                auto advance(iter_difference_t<I> n) ->
                    CPP_ret(void)(
                        requires RandomAccessIterator<I>)
                {
                    it_ += n;
                }
                CPP_member
                auto distance_to(move_into_cursor const &that) const ->
                    CPP_ret(iter_difference_t<I>)(
                        requires SizedSentinel<I, I>)
                {
                    return that.it_ - it_;
                }
                template<typename II = I const>
                constexpr /*c++14*/
                auto move() const noexcept(has_nothrow_iter_move<II>::value) ->
                    CPP_ret(iter_rvalue_reference_t<II>)(
                        requires Same<I const, II> && Readable<II>)
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
            constexpr I recounted(I const &, I i, iter_difference_t<I>)
            {
                return i;
            }

            struct uncounted_fn
            {
                template<typename I>
                constexpr
                auto operator()(I i) const ->
                    decltype(uncounted((I &&)i))
                {
                    return uncounted((I &&)i);
                }
            };

            struct recounted_fn
            {
                template<typename I, typename J>
                constexpr
                auto operator()(I i, J j, iter_difference_t<J> n) const ->
                    decltype(recounted((I &&)i, (J &&)j, n))
                {
                    return recounted((I &&)i, (J &&)j, n);
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

/// \cond
namespace std
{
    template<typename Container>
    struct iterator_traits< ::ranges::back_insert_iterator<Container>>
      : ::ranges::detail::std_output_iterator_traits<>
    {};

    template<typename Container>
    struct iterator_traits< ::ranges::front_insert_iterator<Container>>
      : ::ranges::detail::std_output_iterator_traits<>
    {};

    template<typename Container>
    struct iterator_traits< ::ranges::insert_iterator<Container>>
      : ::ranges::detail::std_output_iterator_traits<>
    {};

    template<typename T, typename Char, typename Traits>
    struct iterator_traits< ::ranges::ostream_iterator<T, Char, Traits>>
      : ::ranges::detail::std_output_iterator_traits<>
    {};

    template<typename Char, typename Traits>
    struct iterator_traits< ::ranges::ostreambuf_iterator<Char, Traits>>
      : ::ranges::detail::std_output_iterator_traits<>
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
