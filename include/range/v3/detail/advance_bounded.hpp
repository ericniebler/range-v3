// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_DETAIL_ADVANCE_BOUNDED_HPP
#define RANGES_V3_DETAIL_ADVANCE_BOUNDED_HPP

#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct advance_bounded_
            {
            private:
                template<typename InputIterator, typename Diff, typename Sentinel>
                static Diff fwd(InputIterator & it, Diff n, Sentinel end,
                    std::input_iterator_tag)
                {
                    while(n > 0 && it != end)
                    {
                        ++it;
                        --n;
                    }
                    return n;
                }
                template<typename RandomAccessIterator, typename Diff, typename Sentinel>
                static Diff fwd(RandomAccessIterator & it, Diff n, Sentinel end,
                    std::random_access_iterator_tag)
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
                template<typename InputIterator, typename Diff, typename Sentinel>
                static Diff back(InputIterator &, Diff n, Sentinel,
                    std::input_iterator_tag)
                {
                    RANGES_ASSERT(false);
                    return n;
                }
                template<typename BidirectionalIterator, typename Diff>
                static Diff back(BidirectionalIterator & it, Diff n, BidirectionalIterator begin,
                    std::bidirectional_iterator_tag)
                {
                    while(n < 0 && it != begin)
                    {
                        --it;
                        ++n;
                    }
                    return n;
                }
                template<typename RandomAccessIterator, typename Diff>
                static Diff back(RandomAccessIterator & it, Diff n, RandomAccessIterator begin,
                    std::random_access_iterator_tag)
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
                template<typename InputIterator, typename Diff, typename Sentinel>
                Diff operator()(InputIterator & it, Diff n, Sentinel bound) const
                {
                    using impl = advance_bounded_;
                    if(0 <= n)
                        return impl::fwd(it, n, std::move(bound),
                            iterator_category_t<InputIterator>{});
                    else
                        return impl::back(it, n, std::move(bound),
                            iterator_category_t<InputIterator>{});
                }
            };

            RANGES_CONSTEXPR advance_bounded_ advance_bounded {};
        }
    }
}

#endif
