// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <list>
#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/delimit.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"
#include "./array.hpp"

template<typename BidiRange>
struct my_reverse_view
  : ranges::range_adaptor<my_reverse_view<BidiRange>, BidiRange>
{
private:
    CONCEPT_ASSERT(ranges::BidirectionalIterable<BidiRange>());
    CONCEPT_ASSERT(ranges::BoundedIterable<BidiRange>());
    friend ranges::range_access;
    using base_iterator_t = ranges::range_iterator_t<BidiRange>;

    struct adaptor : ranges::adaptor_base
    {
        // Cross-wire begin and end.
        RANGES_CXX14_CONSTEXPR
        base_iterator_t begin(my_reverse_view const &rng) const
        {
            return ranges::end(rng.base());
        }
        RANGES_CXX14_CONSTEXPR
        base_iterator_t end(my_reverse_view const &rng) const
        {
            return ranges::begin(rng.base());
        }
        RANGES_CXX14_CONSTEXPR
        void next(base_iterator_t &it)
        {
            --it;
        }
        RANGES_CXX14_CONSTEXPR
        void prev(base_iterator_t &it)
        {
            ++it;
        }
        RANGES_CXX14_CONSTEXPR
        ranges::range_reference_t<BidiRange> current(base_iterator_t it) const
        {
            return *ranges::prev(it);
        }
        CONCEPT_REQUIRES(ranges::RandomAccessIterable<BidiRange>())
        RANGES_CXX14_CONSTEXPR
        void advance(base_iterator_t &it, ranges::range_difference_t<BidiRange> n)
        {
            it -= n;
        }
        CONCEPT_REQUIRES(ranges::RandomAccessIterable<BidiRange>())
        RANGES_CXX14_CONSTEXPR
        ranges::range_difference_t<BidiRange>
        distance_to(base_iterator_t const &here, base_iterator_t const &there)
        {
            return here - there;
        }
    };
    RANGES_CXX14_CONSTEXPR
    adaptor begin_adaptor() const
    {
        return {};
    }
    RANGES_CXX14_CONSTEXPR
    adaptor end_adaptor() const
    {
        return {};
    }
public:
    using ranges::range_adaptor_t<my_reverse_view>::range_adaptor_t;
};

struct my_delimited_range
  : ranges::range_adaptor<
        my_delimited_range,
        ranges::delimit_view<ranges::istream_range<int>, int>>
{
    using range_adaptor_t::range_adaptor_t;
};

#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_CXX14_CONSTEXPR bool test_constexpr()
{
    using namespace ranges;
    // TODO: constexpr
    // requires view::all to be constexpr
    // array<int, 4> a{{1,2,3,4}};
    // my_reverse_view<array<int, 4>& > retro{a};
    // ::models<concepts::BoundedRange>(retro);
    // ::models<concepts::RandomAccessIterator>(retro.begin());
    // ::check_equal(retro, {4, 3, 2, 1});

    return true;
}
#endif

int main()
{
    using namespace ranges;
    std::vector<int> v{1, 2, 3, 4};
    my_reverse_view<std::vector<int>& > retro{v};
    ::models<concepts::BoundedRange>(retro);
    ::models<concepts::RandomAccessIterator>(retro.begin());
    ::check_equal(retro, {4, 3, 2, 1});

    std::list<int> l{1, 2, 3, 4};
    my_reverse_view<std::list<int>& > retro2{l};
    ::models<concepts::BoundedRange>(retro2);
    ::models<concepts::BidirectionalIterator>(retro2.begin());
    ::models_not<concepts::RandomAccessIterator>(retro2.begin());
    ::check_equal(retro2, {4, 3, 2, 1});

    std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 42 6 7 8 9 ");
    my_delimited_range r{view::delimit(istream<int>(sinx), 42)};
    ::models<concepts::Range>(r);
    ::models_not<concepts::BoundedRange>(r);
    ::models<concepts::InputIterator>(r.begin());
    ::models_not<concepts::ForwardIterator>(r.begin());
    ::check_equal(r, {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4});

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(test_constexpr(), "");
    }
#endif

    return ::test_result();
}
