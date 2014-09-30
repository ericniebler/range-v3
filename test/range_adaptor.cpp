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

template<typename BidiRange>
struct my_reverse_view
  : ranges::range_adaptor<my_reverse_view<BidiRange>, BidiRange>
{
private:
    CONCEPT_ASSERT(ranges::BidirectionalIterable<BidiRange>());
    CONCEPT_ASSERT(ranges::BoundedIterable<BidiRange>());
    friend ranges::range_access;
    using base_cursor_t = ranges::base_cursor_t<my_reverse_view>;

    struct adaptor : ranges::default_adaptor
    {
        // Cross-wire begin and end.
        base_cursor_t begin(my_reverse_view const &rng) const
        {
            return default_adaptor::end(rng);
        }
        base_cursor_t end(my_reverse_view const &rng) const
        {
            return default_adaptor::begin(rng);
        }
        void next(base_cursor_t &pos)
        {
            pos.prev();
        }
        void prev(base_cursor_t &pos)
        {
            pos.next();
        }
        auto current(base_cursor_t tmp) const -> decltype(tmp.current())
        {
            tmp.prev();
            return tmp.current();
        }
        CONCEPT_REQUIRES(ranges::RandomAccessIterable<BidiRange>())
        void advance(base_cursor_t &pos, ranges::range_difference_t<BidiRange> n)
        {
            pos.advance(-n);
        }
        CONCEPT_REQUIRES(ranges::RandomAccessIterable<BidiRange>())
        ranges::range_difference_t<BidiRange>
        distance_to(base_cursor_t const &here, base_cursor_t const &there)
        {
            return there.distance_to(here);
        }
    };
    adaptor begin_adaptor() const
    {
        return {};
    }
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
        ranges::delimited_view<ranges::istream_range<int>, int>>
{
    using range_adaptor_t::range_adaptor_t;
};

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

    return ::test_result();
}
