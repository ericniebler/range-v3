// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_ITERATOR_HPP
#define RANGES_V3_UTILITY_ITERATOR_HPP

#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct advance_fn
        {
        private:
            template<typename I>
            static void
            impl(I &it, iterator_difference_t<I> n, concepts::InputIterator*)
            {
                RANGES_ASSERT(0 <= n);
                for(; n != 0; --n)
                    ++it;
            }
            template<typename I>
            static void
            impl(I &it, iterator_difference_t<I> n, concepts::BidirectionalIterator*)
            {
                if(0 <= n)
                    for(; n != 0; --n)
                        ++it;
                else
                    for(; n != 0; ++n)
                        --it;
            }
            template<typename I>
            static void
            impl(I &it, iterator_difference_t<I> n, concepts::RandomAccessIterator*)
            {
                it += n;
            }
        public:
            template<typename I>
            void
            operator()(I &it, iterator_difference_t<I> n) const
            {
                advance_fn::impl(it, n, iterator_concept<I>());
            }
            /// \cond
            template<typename I>
            void operator()(counted_iterator<I> &it, iterator_difference_t<I> n) const;
            /// \endcond
        };

        RANGES_CONSTEXPR advance_fn advance{};

        struct advance_bounded_fn
        {
        private:
            template<typename I, typename S>
            static iterator_difference_t<I>
            fwd(I &it, iterator_difference_t<I> n, S end, concepts::InputIterator*)
            {
                while(n > 0 && it != end)
                {
                    ++it;
                    --n;
                }
                return n;
            }
            template<typename I, typename S>
            static iterator_difference_t<I>
            fwd(I &it, iterator_difference_t<I> n, S end, concepts::RandomAccessIterator*)
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
                return n;
            }
            template<typename I, typename S>
            static iterator_difference_t<I>
            back(I &, iterator_difference_t<I> n, S, concepts::InputIterator*)
            {
                RANGES_ASSERT(false);
                return n;
            }
            template<typename I>
            static iterator_difference_t<I>
            back(I &it, iterator_difference_t<I> n, I begin, concepts::BidirectionalIterator*)
            {
                while(n < 0 && it != begin)
                {
                    --it;
                    ++n;
                }
                return n;
            }
            template<typename I>
            static iterator_difference_t<I>
            back(I &it, iterator_difference_t<I> n, I begin, concepts::RandomAccessIterator*)
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
                return n;
            }
        public:
            template<typename I, typename S>
            iterator_difference_t<I>
            operator()(I &it, iterator_difference_t<I> n, S bound) const
            {
                using impl = advance_bounded_fn;
                if(0 <= n)
                    return impl::fwd(it, n, std::move(bound), iterator_concept<I>());
                else
                    return impl::back(it, n, std::move(bound), iterator_concept<I>());
            }
            /// \cond
            template<typename I, typename S>
            iterator_difference_t<I>
            operator()(counted_iterator<I> &it, iterator_difference_t<I> n,
                counted_iterator<I> bound) const
            {
                auto const dist = bound.count() - it.count();
                if(0 <= n ? n >= dist : n <= dist)
                    return (it = bound), n - dist;
                else
                    return advance(it, n), 0;
            }
            template<typename I, typename S>
            iterator_difference_t<I>
            operator()(counted_iterator<I> &it, iterator_difference_t<I> n,
                counted_sentinel<I> bound) const
            {
                auto const dist = bound.count() - it.count();
                if(0 <= n ? n >= dist : n <= dist)
                    return advance(it, dist), n - dist;
                else
                    return advance(it, n), 0;
            }
            /// \endcond
        };

        RANGES_CONSTEXPR advance_bounded_fn advance_bounded {};

        struct next_fn
        {
            template<typename I>
            I operator()(I it, iterator_difference_t<I> n = 1) const
            {
                advance(it, n);
                return it;
            }
        };

        RANGES_CONSTEXPR next_fn next{};

        struct prev_fn
        {
            template<typename I>
            I operator()(I it, iterator_difference_t<I> n = 1) const
            {
                advance(it, -n);
                return it;
            }
        };

        RANGES_CONSTEXPR prev_fn prev {};

        /// \cond
        template<typename I>
        void advance_fn::operator()(counted_iterator<I> &it, iterator_difference_t<I> n) const
        {
            it = counted_iterator<I>{next(it.base(), n), it.count() + n};
        }
        /// \endcond

        template<typename Cont>
        struct back_insert_iterator
        {
        private:
            Cont *cont_;
        public:
            using difference_type = std::ptrdiff_t;
            back_insert_iterator() = default;
            explicit back_insert_iterator(Cont &cont) noexcept
              : cont_(&cont)
            {}
            back_insert_iterator const &operator=(typename Cont::value_type v) const
            {
                cont_->push_back(std::move(v));
                return *this;
            }
            back_insert_iterator const &operator*() const
            {
                return *this;
            }
            back_insert_iterator &operator++()
            {
                return *this;
            }
            back_insert_iterator &operator++(int)
            {
                return *this;
            }
        };

        struct back_inserter_fn
        {
            template<typename Cont>
            back_insert_iterator<Cont> operator()(Cont &cont) const
            {
                return back_insert_iterator<Cont>{cont};
            }
        };

        RANGES_CONSTEXPR back_inserter_fn back_inserter {};

        template<typename T, typename Char = char, typename Traits = std::char_traits<Char>>
        struct ostream_iterator
        {
        private:
            std::basic_ostream<Char, Traits> *sout_;
            Char const *delim_;
        public:
            using difference_type = std::ptrdiff_t;
            using char_type = Char;
            using traits_type = Traits;
            ostream_iterator() = default;
            ostream_iterator(std::basic_ostream<Char, Traits> &sout, Char const *delim = nullptr)
              : sout_(&sout), delim_(delim)
            {}
            ostream_iterator const &operator=(T const &t) const
            {
                RANGES_ASSERT(sout_);
                *sout_ << t;
                if(delim_)
                    *sout_ << delim_;
                return *this;
            }
            ostream_iterator<T> const &operator*() const
            {
                return *this;
            }
            ostream_iterator<T> &operator++()
            {
                return *this;
            }
            ostream_iterator<T> &operator++(int)
            {
                return *this;
            }
        };

        struct uncounted_fn
        {
            template<typename I>
            I operator()(I i) const
            {
                return i;
            }

            template<typename I>
            I operator()(counted_iterator<I> i) const
            {
                return i.base();
            }
        };

        RANGES_CONSTEXPR uncounted_fn uncounted{};

        struct recounted_fn
        {
            template<typename I>
            I operator()(I const &, I i, iterator_difference_t<I>) const
            {
                return i;
            }

            template<typename I>
            counted_iterator<I>
            operator()(counted_iterator<I> const &j, I i, iterator_difference_t<I> n) const
            {
                return {i, j.count() + n};
            }
        };

        RANGES_CONSTEXPR recounted_fn recounted{};

        struct advance_to_fn
        {
            template<typename I>
            I operator()(I const &, I i) const
            {
                return i;
            }

            template<typename I, typename S>
            I operator()(I i, S s) const
            {
                while(i != s)
                    ++i;
                return i;
            }
        };

        RANGES_CONSTEXPR advance_to_fn advance_to{};
    }
}

#endif
