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

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-utility
        /// @{
        namespace adl_advance_detail
        {
            template<typename I>
            RANGES_RELAXED_CONSTEXPR void advance_impl(I &i, iterator_difference_t<I> n, concepts::InputIterator *)
            {
                RANGES_ASSERT(n >= 0);
                for(; n > 0; --n)
                    ++i;
            }
            template<typename I>
            RANGES_RELAXED_CONSTEXPR void advance_impl(I &i, iterator_difference_t<I> n, concepts::BidirectionalIterator *)
            {
                if(n > 0)
                    for(; n > 0; --n)
                        ++i;
                else
                    for(; n < 0; ++n)
                        --i;
            }
            template<typename I>
            RANGES_RELAXED_CONSTEXPR void advance_impl(I &i, iterator_difference_t<I> n, concepts::RandomAccessIterator *)
            {
                i += n;
            }
            // Handle range-v3 iterators specially, since many range-v3 iterators will want to
            // decrement an iterator that is bidirectional from the perspective of range-v3,
            // but only input from the perspective of std::advance.
            template<typename Cur, typename Sent>
            RANGES_RELAXED_CONSTEXPR void advance(basic_iterator<Cur, Sent> &i, iterator_difference_t<basic_iterator<Cur, Sent>> n)
            {
                adl_advance_detail::advance_impl(i, n, iterator_concept<basic_iterator<Cur, Sent>>{});
            }


            template<class It>
            RANGES_RELAXED_CONSTEXPR void advance(It& i, iterator_difference_t<It> n) {
                return adl_advance_detail::advance_impl(i, n, iterator_concept<It>{});
            }

            struct advance_fn
            {
            private:
                template<typename I, typename S>
                static RANGES_RELAXED_CONSTEXPR void to_(I &i, S s, concepts::IteratorRange*);
                template<typename I, typename S>
                static RANGES_RELAXED_CONSTEXPR void to_(I &i, S s, concepts::SizedIteratorRange*);
                template<typename I, typename D, typename S>
                static RANGES_RELAXED_CONSTEXPR D bounded_(I &it, D n, S bound, concepts::IteratorRange*,
                    concepts::InputIterator*);
                template<typename I, typename D, typename S>
                static RANGES_RELAXED_CONSTEXPR D bounded_(I &it, D n, S bound, concepts::IteratorRange*,
                    concepts::BidirectionalIterator*);
                template<typename I, typename D, typename S, typename Concept>
                static RANGES_RELAXED_CONSTEXPR D bounded_(I &it, D n, S bound, concepts::SizedIteratorRange*, Concept);
            public:
                // Advance a certain number of steps:
                template<typename I,
                    CONCEPT_REQUIRES_(WeakIterator<I>())>
                RANGES_RELAXED_CONSTEXPR void operator()(I &i, iterator_difference_t<I> n) const
                {
                    ranges::adl_advance_detail::advance(i, n);
                }
                // Advance to a certain position:
                template<typename I,
                    CONCEPT_REQUIRES_(WeakIterator<I>())>
                RANGES_RELAXED_CONSTEXPR void operator()(I &i, I s) const
                {
                    i = std::move(s);
                }
                template<typename I, typename S,
                    CONCEPT_REQUIRES_(IteratorRange<I, S>())>
                RANGES_RELAXED_CONSTEXPR void operator()(I &i, S s) const
                {
                    advance_fn::to_(i, std::move(s), sized_iterator_range_concept<I, S>());
                }
                // Advance a certain number of times, with a bound:
                template<typename I, typename S,
                    CONCEPT_REQUIRES_(IteratorRange<I, S>())>
                RANGES_RELAXED_CONSTEXPR iterator_difference_t<I> operator()(I &it, iterator_difference_t<I> n, S bound) const
                {
                    return advance_fn::bounded_(it, n, std::move(bound),
                        sized_iterator_range_concept<I, S>(), iterator_concept<I>());
                }
            };
        }

        /// \ingroup group-utility
        /// \sa `advance_fn`
        namespace
        {
            constexpr auto&& advance = static_const<adl_advance_detail::advance_fn>::value;
        }

        namespace adl_advance_detail
        {
            template<typename I, typename S>
            RANGES_RELAXED_CONSTEXPR void advance_fn::to_(I &i, S s, concepts::IteratorRange*)
            {
                while(i != s)
                    ++i;
            }
            template<typename I, typename S>
            RANGES_RELAXED_CONSTEXPR void advance_fn::to_(I &i, S s, concepts::SizedIteratorRange*)
            {
                ranges::advance(i, s - i);
            }
            template<typename I, typename D, typename S>
            RANGES_RELAXED_CONSTEXPR D advance_fn::bounded_(I &it, D n, S bound, concepts::IteratorRange*,
                concepts::InputIterator*)
            {
                RANGES_ASSERT(0 <= n);
                for(; 0 != n && it != bound; --n)
                    ++it;
                return n;
            }
            template<typename I, typename D, typename S>
            RANGES_RELAXED_CONSTEXPR D advance_fn::bounded_(I &it, D n, S bound, concepts::IteratorRange*,
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
            RANGES_RELAXED_CONSTEXPR D advance_fn::bounded_(I &it, D n, S bound, concepts::SizedIteratorRange*, Concept)
            {
                D d = bound - it;
                if(0 <= n ? n >= d : n <= d)
                {
                    ranges::advance(it, bound);
                    return n - d;
                }
                ranges::advance(it, n);
                return 0;
            }
        }

        struct next_fn
        {
            template<typename I,
                CONCEPT_REQUIRES_(WeakIterator<I>())>
            RANGES_RELAXED_CONSTEXPR I operator()(I it) const
            {
                ++it;
                return it;
            }
            template<typename I,
                CONCEPT_REQUIRES_(WeakIterator<I>())>
            RANGES_RELAXED_CONSTEXPR I operator()(I it, iterator_difference_t<I> n) const
            {
                ranges::advance(it, n);
                return it;
            }
            template<typename I, typename S,
                CONCEPT_REQUIRES_(IteratorRange<I, S>())>
            RANGES_RELAXED_CONSTEXPR I operator()(I it, S s) const
            {
                ranges::advance(it, std::move(s));
                return it;
            }
            template<typename I, typename S,
                CONCEPT_REQUIRES_(IteratorRange<I, S>())>
            RANGES_RELAXED_CONSTEXPR I operator()(I it, iterator_difference_t<I> n, S bound) const
            {
                ranges::advance(it, n, std::move(bound));
                return it;
            }
        };

        /// \ingroup group-utility
        /// \sa `next_fn`
        namespace
        {
            constexpr auto&& next = static_const<next_fn>::value;
        }

        struct prev_fn
        {
            template<typename I>
            RANGES_RELAXED_CONSTEXPR I operator()(I it, iterator_difference_t<I> n = 1) const
            {
                advance(it, -n);
                return it;
            }
        };

        /// \ingroup group-utility
        /// \sa `prev_fn`
        namespace
        {
            constexpr auto&& prev = static_const<prev_fn>::value;
        }

        struct iter_enumerate_fn
        {
        private:
            template<typename I, typename S, typename D>
            RANGES_RELAXED_CONSTEXPR
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::IteratorRange*, concepts::IteratorRange*) const
            {
                for(; begin != end; ++begin)
                    ++d;
                return {d, begin};
            }
            template<typename I, typename S, typename D>
            RANGES_RELAXED_CONSTEXPR
            std::pair<D, I> impl_i(I begin, S end_, D d, concepts::IteratorRange*, concepts::SizedIteratorRange*) const
            {
                I end = ranges::next(begin, end_);
                return {(end - begin) + d, end};
            }
            template<typename I, typename S, typename D, typename Concept>
            RANGES_RELAXED_CONSTEXPR
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::SizedIteratorRange*, Concept) const
            {
                return {(end - begin) + d, ranges::next(begin, end)};
            }
        public:
            template<typename I, typename S, typename D = iterator_difference_t<I>,
                CONCEPT_REQUIRES_(InputIterator<I>() && IteratorRange<I, S>() && Integral<D>())>
            RANGES_RELAXED_CONSTEXPR
            std::pair<D, I> operator()(I begin, S end, D d = 0) const
            {
                return this->impl_i(std::move(begin), std::move(end), d,
                    sized_iterator_range_concept<I, S>(), sized_iterator_range_concept<I, I>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_enumerate_fn`
        namespace
        {
            constexpr auto&& iter_enumerate = static_const<iter_enumerate_fn>::value;
        }

        struct iter_distance_fn
        {
        private:
            template<typename I, typename S, typename D>
            RANGES_RELAXED_CONSTEXPR
            D impl_i(I begin, S end, D d, concepts::IteratorRange*) const
            {
                return iter_enumerate(std::move(begin), std::move(end), d).first;
            }
            template<typename I, typename S, typename D>
            RANGES_RELAXED_CONSTEXPR
            D impl_i(I begin, S end, D d, concepts::SizedIteratorRange*) const
            {
                return static_cast<D>(end - begin) + d;
            }
        public:
            template<typename I, typename S, typename D = iterator_difference_t<I>,
                CONCEPT_REQUIRES_(InputIterator<I>() && IteratorRange<I, S>() && Integral<D>())>
            RANGES_RELAXED_CONSTEXPR
            D operator()(I begin, S end, D d = 0) const
            {
                return this->impl_i(std::move(begin), std::move(end), d,
                    sized_iterator_range_concept<I, S>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_distance_fn`
        namespace
        {
            constexpr auto&& iter_distance = static_const<iter_distance_fn>::value;
        }

        struct iter_distance_compare_fn
        {
        private:
            template<typename I, typename S>
            RANGES_RELAXED_CONSTEXPR
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
            RANGES_RELAXED_CONSTEXPR
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
            RANGES_RELAXED_CONSTEXPR
            int operator()(I begin, S end, iterator_difference_t<I> n) const
            {
                return this->impl_i(std::move(begin), std::move(end), n,
                    sized_iterator_range_concept<I, S>());
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_distance_compare_fn`
        namespace
        {
            constexpr auto&& iter_distance_compare = static_const<iter_distance_compare_fn>::value;
        }

        // Like distance(b,e), but guaranteed to be O(1)
        struct iter_size_fn
        {
            template<typename I, typename S, CONCEPT_REQUIRES_(SizedIteratorRange<I, S>())>
            RANGES_RELAXED_CONSTEXPR iterator_size_t<I> operator()(I begin, S end) const
            {
                RANGES_ASSERT(0 <= (end - begin));
                return static_cast<iterator_size_t<I>>(end - begin);
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_size_fn`
        namespace
        {
            constexpr auto&& iter_size = static_const<iter_size_fn>::value;
        }

        struct iter_swap_fn
        {
            template<typename Readable0, typename Readable1,
                CONCEPT_REQUIRES_(IndirectlySwappable<Readable0, Readable1>())>
            RANGES_RELAXED_CONSTEXPR void operator()(Readable0 a, Readable1 b) const
                noexcept(is_nothrow_indirectly_swappable<Readable0, Readable1>::value)
            {
                indirect_swap(std::move(a), std::move(b));
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_swap_fn`
        namespace
        {
            constexpr auto&& iter_swap = static_const<iter_swap_fn>::value;
        }

        struct iter_move_fn
        {
            template<typename I,
                CONCEPT_REQUIRES_(Readable<I>())>
            RANGES_RELAXED_CONSTEXPR iterator_rvalue_reference_t<I> operator()(I const &i) const
                noexcept(noexcept(indirect_move(i)))
            {
                return indirect_move(i);
            }
        };

        /// \ingroup group-utility
        /// \sa `iter_move_fn`
        namespace
        {
            constexpr auto&& iter_move = static_const<iter_move_fn>::value;
        }

        template<typename Cont>
        struct back_insert_iterator
        {
        private:
            Cont *cont_;
        public:
            using difference_type = std::ptrdiff_t;
            RANGES_RELAXED_CONSTEXPR back_insert_iterator() = default;
            RANGES_RELAXED_CONSTEXPR explicit back_insert_iterator(Cont &cont) noexcept
              : cont_(&cont)
            {}
            RANGES_RELAXED_CONSTEXPR back_insert_iterator const &operator=(typename Cont::value_type v) const
            {
                cont_->push_back(std::move(v));
                return *this;
            }
            RANGES_RELAXED_CONSTEXPR back_insert_iterator const &operator*() const
            {
                return *this;
            }
            RANGES_RELAXED_CONSTEXPR back_insert_iterator &operator++()
            {
                return *this;
            }
            RANGES_RELAXED_CONSTEXPR back_insert_iterator &operator++(int)
            {
                return *this;
            }
        };

        struct back_inserter_fn
        {
            template<typename Cont>
            RANGES_RELAXED_CONSTEXPR back_insert_iterator<Cont> operator()(Cont &cont) const
            {
                return back_insert_iterator<Cont>{cont};
            }
        };

        /// \ingroup group-utility
        /// \sa `back_inserter_fn`
        namespace
        {
            constexpr auto&& back_inserter = static_const<back_inserter_fn>::value;
        }

        template<typename T = void, typename Char = char, typename Traits = std::char_traits<Char>>
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
            template<typename U,
                meta::if_<
                    meta::or_<
                        std::is_void<T>,
                        std::is_convertible<U, meta::eval<std::add_lvalue_reference<T const>>>>, int> = 0>
            ostream_iterator const &operator=(U &&t) const
            {
                RANGES_ASSERT(sout_);
                using V = meta::if_<std::is_void<T>, U, T>;
                *sout_ << static_cast<V const &>(t);
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
            RANGES_RELAXED_CONSTEXPR I uncounted(I i)
            {
                return i;
            }

            template<typename I>
            RANGES_RELAXED_CONSTEXPR I recounted(I const &, I i, iterator_difference_t<I>)
            {
                return i;
            }

            template<typename I>
            RANGES_RELAXED_CONSTEXPR I recounted(I const &, I i)
            {
                return i;
            }

            struct uncounted_fn
            {
                template<typename I>
                RANGES_RELAXED_CONSTEXPR auto operator()(I i) const ->
                    decltype(uncounted((I&&)i))
                {
                    return uncounted((I&&)i);
                }
            };

            struct recounted_fn
            {
                template<typename I, typename J>
                RANGES_RELAXED_CONSTEXPR auto operator()(I i, J j, iterator_difference_t<J> n) const ->
                    decltype(recounted((I&&)i, (J&&)j, n))
                {
                    return recounted((I&&)i, (J&&)j, n);
                }

                template<typename I, typename J>
                RANGES_RELAXED_CONSTEXPR auto operator()(I i, J j) const ->
                    decltype(recounted((I&&)i, (J&&)j))
                {
                    return recounted((I&&)i, (J&&)j);
                }
            };
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{
        namespace
        {
            constexpr auto&& uncounted = static_const<adl_uncounted_recounted_detail::uncounted_fn>::value;
            constexpr auto&& recounted = static_const<adl_uncounted_recounted_detail::recounted_fn>::value;
        }
        /// @}
    }
}

#endif
