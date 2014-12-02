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
#include <algorithm> // for iter_swap
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
        struct advance_fn
        {
            template<typename I>
            void operator()(I &i, iterator_difference_t<I> n) const
            {
                // Use ADL here to give custom iterator types (like counted_iterator)
                // a chance to optimize it (see view/counted.hpp)
                using std::advance;
                advance(i, n);
            }
        };

        /// \ingroup group-utility
        /// \sa `advance_fn`
        constexpr advance_fn advance{};

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

        /// \ingroup group-utility
        /// \sa `advance_to_fn`
        constexpr advance_to_fn advance_to{};

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

        /// \ingroup group-utility
        /// \sa `advance_bounded_fn`
        constexpr advance_bounded_fn advance_bounded {};

        struct next_fn
        {
            template<typename I>
            I operator()(I it, iterator_difference_t<I> n = 1) const
            {
                advance(it, n);
                return it;
            }
        };

        /// \ingroup group-utility
        /// \sa `next_fn`
        constexpr next_fn next{};

        struct prev_fn
        {
            template<typename I>
            I operator()(I it, iterator_difference_t<I> n = 1) const
            {
                advance(it, -n);
                return it;
            }
        };

        /// \ingroup group-utility
        /// \sa `prev_fn`
        constexpr prev_fn prev {};

        struct next_to_fn
        {
            template<typename I, typename S>
            I operator()(I it, S s) const
            {
                advance_to(it, std::move(s));
                return it;
            }
        };

        /// \ingroup group-utility
        /// \sa `next_to_fn`
        constexpr next_to_fn next_to{};

        struct next_bounded_fn
        {
            template<typename I, typename S>
            I operator()(I it, iterator_difference_t<I> n, S bound) const
            {
                advance_bounded(it, n, std::move(bound));
                return it;
            }
        };

        /// \ingroup group-utility
        /// \sa `next_bounded_fn`
        constexpr next_bounded_fn next_bounded{};

        struct iter_enumerate_fn
        {
        private:
            template<typename I, typename S, typename D>
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::IteratorRange*, concepts::IteratorRange*) const
            {
                for(; begin != end; ++begin)
                    ++d;
                return {d, begin};
            }
            template<typename I, typename S, typename D>
            std::pair<D, I> impl_i(I begin, S end_, D d, concepts::IteratorRange*, concepts::SizedIteratorRange*) const
            {
                I end = next_to(begin, end_);
                return {(end - begin) + d, end};
            }
            template<typename I, typename S, typename D, typename Concept>
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::SizedIteratorRange*, Concept) const
            {
                return {(end - begin) + d, next_to(begin, end)};
            }
        public:
            template<typename I, typename S, typename D = iterator_difference_t<I>,
                CONCEPT_REQUIRES_(InputIterator<I>() && IteratorRange<I, S>() && Integral<D>())>
            std::pair<D, I> operator()(I begin, S end, D d = 0) const
            {
                return this->impl_i(std::move(begin), std::move(end), d,
                    sized_iterator_range_concept<I, S>(), sized_iterator_range_concept<I, I>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_enumerate_fn`
        constexpr iter_enumerate_fn iter_enumerate {};

        struct iter_distance_fn
        {
        private:
            template<typename I, typename S, typename D>
            D impl_i(I begin, S end, D d, concepts::IteratorRange*) const
            {
                return iter_enumerate(std::move(begin), std::move(end), d).first;
            }
            template<typename I, typename S, typename D>
            D impl_i(I begin, S end, D d, concepts::SizedIteratorRange*) const
            {
                return static_cast<D>(end - begin) + d;
            }
        public:
            template<typename I, typename S, typename D = iterator_difference_t<I>,
                CONCEPT_REQUIRES_(InputIterator<I>() && IteratorRange<I, S>() && Integral<D>())>
            D operator()(I begin, S end, D d = 0) const
            {
                return this->impl_i(std::move(begin), std::move(end), d,
                    sized_iterator_range_concept<I, S>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_distance_fn`
        constexpr iter_distance_fn iter_distance {};

        struct iter_distance_compare_fn
        {
        private:
            template<typename I, typename S>
            int impl_i(I begin, S end, iterator_difference_t<I> n, concepts::IteratorRange*) const
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
            int impl_i(I begin, S end, iterator_difference_t<I> n, concepts::SizedIteratorRange*) const
            {
                iterator_difference_t<I> dist = static_cast<iterator_difference_t<I>>(end - begin);
                if (dist > n)
                    return  1;
                else if (dist < n)
                    return -1;
                else
                    return  0;
            }
        public:
            template<typename I, typename S,
                CONCEPT_REQUIRES_(InputIterator<I>() && IteratorRange<I, S>())>
            int operator()(I begin, S end, iterator_difference_t<I> n) const
            {
                return this->impl_i(std::move(begin), std::move(end), n,
                    sized_iterator_range_concept<I, S>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_distance_compare_fn`
        constexpr iter_distance_compare_fn iter_distance_compare {};

        // Like distance(b,e), but guaranteed to be O(1)
        struct iter_size_fn
        {
            template<typename I, typename S, CONCEPT_REQUIRES_(SizedIteratorRange<I, S>())>
            iterator_size_t<I> operator()(I begin, S end) const
            {
                RANGES_ASSERT(0 <= (end - begin));
                return static_cast<iterator_size_t<I>>(end - begin);
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_size_fn`
        constexpr iter_size_fn iter_size {};

        struct iter_swap_fn
        {
            template<typename T, typename U,
                CONCEPT_REQUIRES_(
                    ForwardIterator<T>() &&
                    ForwardIterator<U>() &&
                    Swappable<iterator_reference_t<T>, iterator_reference_t<U>>())>
            void operator()(T t, U u) const
            {
                using std::iter_swap;
                iter_swap(std::forward<T>(t), std::forward<U>(u));
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_swap_fn`
        constexpr iter_swap_fn iter_swap {};

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

        /// \ingroup group-utility
        /// \sa `back_inserter_fn`
        constexpr back_inserter_fn back_inserter {};

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
        /// @}

        /// \cond
        namespace adl_uncounted_recounted_detail
        {
            template<typename I>
            I uncounted(I i)
            {
                return i;
            }

            template<typename I>
            I recounted(I const &, I i, iterator_difference_t<I>)
            {
                return i;
            }

            template<typename I>
            I recounted(I const &, I i)
            {
                return i;
            }

            struct uncounted_fn
            {
                template<typename I>
                auto operator()(I i) const ->
                    decltype(uncounted((I&&)i))
                {
                    return uncounted((I&&)i);
                }
            };

            struct recounted_fn
            {
                template<typename I, typename J>
                auto operator()(I i, J j, iterator_difference_t<J> n) const ->
                    decltype(recounted((I&&)i, (J&&)j, n))
                {
                    return recounted((I&&)i, (J&&)j, n);
                }

                template<typename I, typename J>
                auto operator()(I i, J j) const ->
                    decltype(recounted((I&&)i, (J&&)j))
                {
                    return recounted((I&&)i, (J&&)j);
                }
            };
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{
        constexpr adl_uncounted_recounted_detail::uncounted_fn uncounted{};
        constexpr adl_uncounted_recounted_detail::recounted_fn recounted{};
        /// @}
    }
}

#endif
