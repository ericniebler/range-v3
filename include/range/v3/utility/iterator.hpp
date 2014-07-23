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
            static void impl(I &it, iterator_difference_t<I> n, concepts::InputIterator*)
            {
                RANGES_ASSERT(0 <= n);
                for(; n != 0; --n)
                    ++it;
            }
            template<typename I>
            static void impl(I &it, iterator_difference_t<I> n, concepts::BidirectionalIterator*)
            {
                if(0 <= n)
                    for(; n != 0; --n)
                        ++it;
                else
                    for(; n != 0; ++n)
                        --it;
            }
            template<typename I>
            static void impl(I &it, iterator_difference_t<I> n, concepts::RandomAccessIterator*)
            {
                it += n;
            }
        public:
            template<typename I>
            void operator()(I &it, iterator_difference_t<I> n) const
            {
                advance_fn::impl(it, n, iterator_concept<I>());
            }
            /// \internal
            template<typename I>
            void operator()(counted_iterator<I> &it, iterator_difference_t<I> n) const
            {
                it = counted_iterator<I>{next(std::move(it.base_reference()), n), it.count() + n};
            }
            /// \endinternal
        };

        RANGES_CONSTEXPR advance_fn advance{};

        struct advance_to_fn
        {
        private:
            template<typename I, typename S>
            static void impl(I &i, S s, concepts::IteratorRange*)
            {
                while(i != s)
                    ++i;
            }
            template<typename I, typename S>
            static void impl(I &i, S s, concepts::SizedIteratorRange*)
            {
                advance(i, s - i);
            }
        public:
            template<typename I>
            void operator()(I &i, I s) const
            {
                i = std::move(s);
            }
            template<typename I, typename S>
            void operator()(I &i, S s) const
            {
                advance_to_fn::impl(i, std::move(s), sized_iterator_range_concept<I, S>());
            }
        };

        RANGES_CONSTEXPR advance_to_fn advance_to{};

        struct advance_bounded_fn
        {
        private:
            template<typename I, typename D, typename S>
            static D impl(I &it, D n, S bound, concepts::IteratorRange*, concepts::InputIterator*)
            {
                RANGES_ASSERT(0 <= n);
                for(; 0 != n && it != bound; --n)
                    ++it;
                return n;
            }
            template<typename I, typename D, typename S>
            static D impl(I &it, D n, S bound, concepts::IteratorRange*,
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
            static D impl(I &it, D n, S bound, concepts::SizedIteratorRange*, Concept)
            {
                D d = bound - it;
                if(0 <= n ? n >= d : n <= d)
                {
                    advance_to(it, bound);
                    return n - d;
                }
                advance(it, n);
                return 0;
            }
        public:
            template<typename I, typename S>
            iterator_difference_t<I> operator()(I &it, iterator_difference_t<I> n, S bound) const
            {
                return advance_bounded_fn::impl(it, n, std::move(bound),
                    sized_iterator_range_concept<I, S>(), iterator_concept<I>());
            }
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

        struct next_to_fn
        {
            template<typename I, typename S>
            I operator()(I it, S s) const
            {
                advance_to(it, std::move(s));
                return it;
            }
        };

        RANGES_CONSTEXPR next_to_fn next_to{};

        // Like distance(b,e), but guaranteed to be O(1)
        struct iterator_range_size_fn
        {
            template<typename I, typename S, CONCEPT_REQUIRES_(SizedIteratorRange<I, S>())>
            iterator_size_t<I> operator()(I begin, S end) const
            {
                RANGES_ASSERT(0 <= (end - begin));
                return static_cast<iterator_size_t<I>>(end - begin);
            }
        };

        RANGES_CONSTEXPR iterator_range_size_fn iterator_range_size {};

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
                RANGES_ASSERT(next(j.base(), n) == i);
                return {i, j.count() + n};
            }

            template<typename I>
            I operator()(I const &, I i) const
            {
                return i;
            }

            template<typename I, CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            counted_iterator<I>
            operator()(counted_iterator<I> const &j, I i) const
            {
                return {i, j.count() + (i - j.base())};
            }
        };

        RANGES_CONSTEXPR recounted_fn recounted{};
    }
}

#endif
