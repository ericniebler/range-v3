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
            template<typename InputIterator>
            static void
            impl(InputIterator &it, iterator_difference_t<InputIterator> n,
                concepts::InputIterator)
            {
                RANGES_ASSERT(0 <= n);
                for(; n != 0; --n)
                    ++it;
            }
            template<typename BidirectionalIterator>
            static void
            impl(BidirectionalIterator &it, iterator_difference_t<BidirectionalIterator> n,
                concepts::BidirectionalIterator)
            {
                if(0 <= n)
                    for(; n != 0; --n)
                        ++it;
                else
                    for(; n != 0; ++n)
                        --it;
            }
            template<typename RandomAccessIterator>
            static void
            impl(RandomAccessIterator &it, iterator_difference_t<RandomAccessIterator> n,
                concepts::RandomAccessIterator)
            {
                it += n;
            }
        public:
            template<typename InputIterator>
            void
            operator()(InputIterator &it, iterator_difference_t<InputIterator> n) const
            {
                advance_fn::impl(it, n, iterator_concept_t<InputIterator>{});
            }
            /// \cond
            template<typename InputIterator>
            void operator()(counted_iterator<InputIterator> &it,
                iterator_difference_t<InputIterator> n) const;
            /// \endcond
        };

        RANGES_CONSTEXPR advance_fn advance{};

        struct advance_bounded_fn
        {
        private:
            template<typename InputIterator, typename Sentinel>
            static iterator_difference_t<InputIterator>
            fwd(InputIterator &it, iterator_difference_t<InputIterator> n, Sentinel end,
                concepts::InputIterator)
            {
                while(n > 0 && it != end)
                {
                    ++it;
                    --n;
                }
                return n;
            }
            template<typename RandomAccessIterator, typename Sentinel>
            static iterator_difference_t<RandomAccessIterator>
            fwd(RandomAccessIterator &it, iterator_difference_t<RandomAccessIterator> n, Sentinel end,
                concepts::RandomAccessIterator)
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
            template<typename InputIterator, typename Sentinel>
            static iterator_difference_t<InputIterator>
            back(InputIterator &, iterator_difference_t<InputIterator> n, Sentinel,
                concepts::InputIterator)
            {
                RANGES_ASSERT(false);
                return n;
            }
            template<typename BidirectionalIterator>
            static iterator_difference_t<BidirectionalIterator>
            back(BidirectionalIterator &it, iterator_difference_t<BidirectionalIterator> n,
                BidirectionalIterator begin, concepts::BidirectionalIterator)
            {
                while(n < 0 && it != begin)
                {
                    --it;
                    ++n;
                }
                return n;
            }
            template<typename RandomAccessIterator>
            static iterator_difference_t<RandomAccessIterator>
            back(RandomAccessIterator &it, iterator_difference_t<RandomAccessIterator> n,
                RandomAccessIterator begin, concepts::RandomAccessIterator)
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
            template<typename InputIterator, typename Sentinel>
            iterator_difference_t<InputIterator>
            operator()(InputIterator &it, iterator_difference_t<InputIterator> n,
                Sentinel bound) const
            {
                using impl = advance_bounded_fn;
                if(0 <= n)
                    return impl::fwd(it, n, std::move(bound), iterator_concept_t<InputIterator>{});
                else
                    return impl::back(it, n, std::move(bound), iterator_concept_t<InputIterator>{});
            }
            /// \cond
            template<typename InputIterator, typename Sentinel>
            iterator_difference_t<InputIterator>
            operator()(counted_iterator<InputIterator> &it, iterator_difference_t<InputIterator> n,
                counted_iterator<InputIterator> bound) const
            {
                auto const dist = bound.count() - it.count();
                if(0 <= n ? n >= dist : n <= dist)
                    return (it = bound), n - dist;
                else
                    return ranges::advance(it, n), 0;
            }
            template<typename InputIterator, typename Sentinel>
            iterator_difference_t<InputIterator>
            operator()(counted_iterator<InputIterator> &it, iterator_difference_t<InputIterator> n,
                counted_sentinel<InputIterator> bound) const
            {
                auto const dist = bound.count() - it.count();
                if(0 <= n ? n >= dist : n <= dist)
                    return ranges::advance(it, dist), n - dist;
                else
                    return ranges::advance(it, n), 0;
            }
            /// \endcond
        };

        RANGES_CONSTEXPR advance_bounded_fn advance_bounded {};

        struct next_fn
        {
            template<typename InputIterator>
            InputIterator operator()(InputIterator it, iterator_difference_t<InputIterator> n = 1) const
            {
                ranges::advance(it, n);
                return it;
            }
        };

        RANGES_CONSTEXPR next_fn next{};

        struct prev_fn
        {
            template<typename BidirectionalIterator>
            BidirectionalIterator operator()(BidirectionalIterator it,
                iterator_difference_t<BidirectionalIterator> n = 1) const
            {
                ranges::advance(it, -n);
                return it;
            }
        };

        RANGES_CONSTEXPR prev_fn prev {};

        /// \cond
        template<typename InputIterator>
        void advance_fn::operator()(counted_iterator<InputIterator> &it,
            iterator_difference_t<InputIterator> n) const
        {
            it = counted_iterator<InputIterator>{ranges::next(it.base(), n), it.count() + n};
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
            back_insert_iterator &operator=(typename Cont::value_type v)
            {
                cont_->push_back(std::move(v));
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
    }
}

#endif
