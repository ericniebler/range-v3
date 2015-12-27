/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_UTILITY_FIXED_VECTOR_HPP
#define RANGES_V3_UTILITY_FIXED_VECTOR_HPP

#include <tuple>
#include <memory>
#include <iterator>
#include <stdexcept>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/fill_n.hpp>
#include <range/v3/algorithm/move_backward.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/algorithm/swap_ranges.hpp>

namespace ranges
{
    inline namespace v1
    {
        /// \brief Like std::vector, but with a fixed-size buffer, guaranteed to do no dynamic
        /// allocation ever.
        template<typename T, std::size_t N>
        struct fixed_vector
        {
        private:
            union
            {
                char c;
                T buffer_[N];
            };
            std::size_t size_;

            template<typename R>
            fixed_vector &assign_range_(R &&r)
            {
                auto first = ranges::begin(r);
                auto last = ranges::end(r);
                std::size_t n = 0;
                for(T *out = data(); first != last && n < size(); ++first, ++out, ++n)
                    *out = *first;
                for(; first != last; ++first, ++n)
                    this->push_back(*first);
                resize(n);
                return *this;
            }
            template<typename U>
            T *insert_one_(T const *where_, U &&value)
            {
                RANGES_ASSERT(size() != N);
                T *where = next(begin(), where_);
                if(where == end())
                    this->push_back(std::forward<U>(value));
                else
                {
                    this->push_back(std::move(back()));
                    ranges::move_backward(where, end() - 2, end() - 1);
                    *where = std::forward<U>(value);
                }
                return where;
            }
            T *insert_n_(T const *where_, std::size_t count, T const &value)
            {
                RANGES_ASSERT(size() + count <= N);
                T *where = next(begin(), where_);
                T *last = end();
                auto pos = std::max(where, last - std::min(count, size()));
                auto x = std::uninitialized_copy(std::make_move_iterator(pos),
                    std::make_move_iterator(last), pos + count);
                struct G { T *f, *l; ~G() { while(f != l) (--l)->~T(); } } g{pos + count, x};
                std::uninitialized_fill(last, pos + count, value);
                g.f = g.l = nullptr; // disarm the guard
                size_ += count;
                ranges::move_backward(where, pos, last);
                ranges::fill_n(where, last - pos, value);
                return where;
            }
            template<typename I, typename S>
            T *insert_range_(T const *where_, I first, S last, concepts::InputIterator*)
            {
                T *where = next(begin(), where_);
                struct G {std::size_t n; fixed_vector *p; ~G() { if(p) p->resize(n); }} g{size(), this};
                for(; first != last; ++first)
                    this->push_back(*first);
                g.p = nullptr; // disarm the guard
                ranges::rotate(where, begin() + g.n, end());
                return where;
            }
            template<typename I, typename S>
            T *insert_range_(T const *where_, I in_first, S in_last_, concepts::ForwardIterator*)
            {
                I in_last;
                std::size_t count;
                std::tie(count, in_last) = enumerate(in_first, in_last_);
                RANGES_ASSERT(size() + count <= N);
                T *where = next(begin(), where_);
                T *last = end();
                auto pos = std::max(where, last - std::min(count, size()));
                auto x = std::uninitialized_copy(std::make_move_iterator(pos),
                    std::make_move_iterator(last), pos + count);
                struct G { T *f, *l; ~G() { while(f != l) (--l)->~T(); } } g{pos + count, x};
                auto in_part = next(in_first, last - pos);
                std::uninitialized_copy(in_part, in_last, last);
                g.f = g.l = nullptr; // disarm the guard
                size_ += count;
                ranges::move_backward(where, pos, last);
                ranges::copy(in_first, in_part, where);
                return where;
            }
        public:
            using size_type = std::size_t;
            using value_type = T;
            using iterator = T *;
            using const_iterator = T const *;

            CONCEPT_REQUIRES(DefaultConstructible<T>())
            fixed_vector() : size_{0}
            {}
            CONCEPT_REQUIRES(MoveConstructible<T>())
            fixed_vector(fixed_vector &&that) : size_(0)
            {
                size_ += std::uninitialized_copy_n(std::make_move_iterator(that.data()),
                    that.size(), data()) - data();
            }
            CONCEPT_REQUIRES(CopyConstructible<T>())
            fixed_vector(fixed_vector const &that) : size_(0)
            {
                size_ += std::uninitialized_copy_n(that.data(), that.size(), data()) - data();
            }
            CONCEPT_REQUIRES(CopyConstructible<T>())
            fixed_vector(std::initializer_list<T> il) : size_(0)
            {
                RANGES_ASSERT(N >= il.size());
                size_ += std::uninitialized_copy_n(il.begin(), il.size(), data()) - data();
            }
            template<typename R,
                CONCEPT_REQUIRES_(InputRange<R>() && Constructible<T, range_reference_t<R>&&>())>
            fixed_vector(R &&r) : size_(0)
            {
                this->insert(begin(), std::forward<R>(r));
            }
            // \pre Requires (last - first) <= N
            template<typename I, typename S,
                CONCEPT_REQUIRES_(IteratorRange<I, S>() && InputIterator<I>() &&
                    Constructible<T, iterator_reference_t<I>&&>())>
            fixed_vector(I first, S last) : size_(0)
            {
                this->insert(begin(), (I &&) first, (S &&) last);
            }
            ~fixed_vector()
            {
                clear();
            }
            CONCEPT_REQUIRES(Movable<T>())
            fixed_vector &operator=(fixed_vector &&that)
            {
                std::size_t n = min(size(), that.size());
                auto res = ranges::move(that.data(), that.data() + n, data());
                size_ += std::uninitialized_copy(std::make_move_iterator(res.in()),
                    std::make_move_iterator(that.end()), res.out()) - res.out();
                resize(that.size());
                return *this;
            }
            CONCEPT_REQUIRES(Copyable<T>())
            fixed_vector &operator=(fixed_vector const &that)
            {
                std::size_t n = min(size(), that.size());
                auto res = ranges::copy(that.data(), that.data() + n, data());
                size_ += std::uninitialized_copy(res.in(), that.end(), res.out()) - res.out();
                resize(that.size());
                return *this;
            }
            CONCEPT_REQUIRES(Movable<T>())
            fixed_vector &operator=(std::initializer_list<T> il)
            {
                return this->assign_range_(il);
            }
            template<typename R,
                CONCEPT_REQUIRES_(InputRange<R>() && Constructible<T, range_reference_t<R>&&>() &&
                    Assignable<T &, range_reference_t<R>&&>())>
            fixed_vector &operator=(R &&r)
            {
                return this->assign_range_(std::forward<R>(r));
            }
            CONCEPT_REQUIRES(Swappable<T &>())
            void swap(fixed_vector &that) noexcept(is_nothrow_swappable<T &>::value)
            {
                ranges::swap_ranges(*this, that);
            }
            CONCEPT_REQUIRES(Swappable<T &>())
            friend void swap(fixed_vector &lhs, fixed_vector &rhs)
                noexcept(is_nothrow_swappable<T &>::value)
            {
                lhs.swap(rhs);
            }
            std::size_t size() const noexcept
            {
                return size_;
            }
            T *begin() noexcept
            {
                return data();
            }
            T const *begin() const noexcept
            {
                return data();
            }
            T *end() noexcept
            {
                return data() + size();
            }
            T const *end() const noexcept
            {
                return data() + size();
            }
            T *data() noexcept
            {
                return buffer_;
            }
            T const *data() const noexcept
            {
                return buffer_;
            }
            T &operator[](std::size_t n) noexcept
            {
                RANGES_ASSERT(n < size());
                return data()[n];
            }
            T const &operator[](std::size_t n) const noexcept
            {
                RANGES_ASSERT(n < size());
                return data()[n];
            }
            T &at(std::size_t n)
            {
                if(n >= size())
                    throw std::range_error("out of range access of fixed_vector");
                return data()[n];
            }
            T const &at(std::size_t n) const
            {
                if(n >= size())
                    throw std::range_error("out of range access of fixed_vector");
                return data()[n];
            }
            static constexpr std::size_t max_size() noexcept
            {
                return N;
            }
            static constexpr std::size_t capacity() noexcept
            {
                return N;
            }
            bool empty() const
            {
                return 0 == size();
            }
            void clear()
            {
                resize(0);
            }
            T &front()
            {
                RANGES_ASSERT(size() != 0);
                return *begin();
            }
            T const &front() const
            {
                RANGES_ASSERT(size() != 0);
                return *begin();
            }
            T &back()
            {
                RANGES_ASSERT(size() != 0);
                return *prev(end());
            }
            T const &back() const
            {
                RANGES_ASSERT(size() != 0);
                return *prev(end());
            }
            CONCEPT_REQUIRES(CopyConstructible<T>())
            void push_back(T const &t)
            {
                RANGES_ASSERT(size() != N);
                ::new((void *) end()) T(t);
                ++size_;
            }
            CONCEPT_REQUIRES(MoveConstructible<T>())
            void push_back(T &&t)
            {
                RANGES_ASSERT(size() != N);
                ::new((void *) end()) T(std::move(t));
                ++size_;
            }
            void pop_back()
            {
                RANGES_ASSERT(size() != 0);
                prev(end())->~T();
                --size_;
            }
            template<typename...Ts,
                CONCEPT_REQUIRES_(Constructible<T, Ts &&...>())>
            void emplace_back(Ts &&...ts)
            {
                RANGES_ASSERT(size() != N);
                ::new((void *) end()) T(std::forward<Ts>(ts)...);
                ++size_;
            }
            CONCEPT_REQUIRES(DefaultConstructible<T>())
            void resize(std::size_t count)
            {
                while(count > size())
                    emplace_back();
                while(count < size())
                    pop_back();
            }
            CONCEPT_REQUIRES(CopyConstructible<T>())
            void resize(std::size_t count, T const &value)
            {
                while(count > size())
                    this->push_back(value);
                while(count < size())
                    pop_back();
            }
            CONCEPT_REQUIRES(Copyable<T>())
            T *insert(T const *where, T const &value)
            {
                return this->insert_one_(where, value);
            }
            CONCEPT_REQUIRES(Movable<T>())
            T *insert(T const *where, T &&value)
            {
                return this->insert_one_(where, std::move(value));
            }
            T *insert(T const *where, std::size_t count, T const &value)
            {
                return this->insert_n_(where, count, value);
            }
            template<typename I, typename S,
                CONCEPT_REQUIRES_(IteratorRange<I, S>() && Readable<I>() && Movable<T>() &&
                    Constructible<T, iterator_reference_t<I>&&>() &&
                    Assignable<T &, iterator_reference_t<I>&&>())>
            T *insert(T const *where, I first, S last)
            {
                return this->insert_range_(where, std::move(first), std::move(last),
                    iterator_concept<I>{});
            }
            CONCEPT_REQUIRES(Copyable<T>())
            T *insert(T const *where, std::initializer_list<T> il)
            {
                return this->insert_range_(where, il.begin(), il.end(), iterator_concept<T *>{});
            }
            template<typename R,
                CONCEPT_REQUIRES_(InputRange<R>() && Constructible<T, range_reference_t<R>&&>() &&
                     Movable<T>() && Assignable<T &, range_reference_t<R>&&>())>
            T *insert(T const *where, R &&r)
            {
                return this->insert_range_(where, ranges::begin(r), ranges::end(r),
                    iterator_concept<range_iterator_t<R>>{});
            }
        };
    }
}

#endif
