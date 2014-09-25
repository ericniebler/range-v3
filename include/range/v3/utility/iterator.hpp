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

#include <algorithm> // for iter_swap
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
            template<typename I>
            void operator()(I &i, iterator_difference_t<I> n) const
            {
                // Use ADL here to give custom iterator types (like counted_iterator)
                // a chance to optimize it (see view/counted.hpp)
                using std::advance;
                advance(i, n);
            }
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

        struct iterator_range_enumerate_fn
        {
        private:
            template<typename I, typename S, typename D>
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::IteratorRange*) const
            {
                for(; begin != end; ++begin)
                    ++d;
                return {d, begin};
            }
            template<typename I, typename S, typename D>
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::SizedIteratorRange*) const
            {
                return {(end - begin) + d, next_to(begin, end)};
            }
        public:
            template<typename I, typename S, typename D = iterator_difference_t<I>,
                CONCEPT_REQUIRES_(InputIterator<I>() && IteratorRange<I, S>() && Integral<D>())>
            std::pair<D, I> operator()(I begin, S end, D d = 0) const
            {
                return this->impl_i(std::move(begin), std::move(end), d, sized_iterator_range_concept<I, S>());
            }
        };

        RANGES_CONSTEXPR iterator_range_enumerate_fn iterator_range_enumerate {};

        struct iterator_range_distance_fn
        {
        private:
            template<typename I, typename S, typename D>
            D impl_i(I begin, S end, D d, concepts::IteratorRange*) const
            {
                return iterator_range_enumerate(std::move(begin), std::move(end), d).first;
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

        RANGES_CONSTEXPR iterator_range_distance_fn iterator_range_distance {};

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

        namespace adl_iter_swap_detail
        {
            using std::iter_swap;
            struct iter_swap_fn
            {
                template<typename T, typename U,
                    CONCEPT_REQUIRES_(
                        ForwardIterator<T>() &&
                        ForwardIterator<U>() &&
                        Swappable<iterator_reference_t<T>, iterator_reference_t<U>>())>
                void operator()(T t, U u) const
                {
                    iter_swap(std::forward<T>(t), std::forward<U>(u));
                }
            };
        }

        RANGES_CONSTEXPR adl_iter_swap_detail::iter_swap_fn iter_swap {};

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

        RANGES_CONSTEXPR adl_uncounted_recounted_detail::uncounted_fn uncounted{};
        RANGES_CONSTEXPR adl_uncounted_recounted_detail::recounted_fn recounted{};
    }
}

#endif
