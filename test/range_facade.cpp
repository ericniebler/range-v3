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

#include <vector>
#include <range/v3/core.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

struct MyRange
  : ranges::range_facade<MyRange>
{
private:
    friend struct ranges::range_core_access;
    std::vector<int> ints_;
    struct cursor
    {
    private:
        std::vector<int>::const_iterator iter;
    public:
        cursor() = default;
        cursor(std::vector<int>::const_iterator it)
          : iter(it)
        {}
        int const & current() const
        {
            return *iter;
        }
        void next()
        {
            ++iter;
        }
        bool equal(cursor const &that) const
        {
            return iter == that.iter;
        }
    };
    cursor begin_cursor() const
    {
        return {ints_.begin()};
    }
    cursor end_cursor() const
    {
        return {ints_.end()};
    }
public:
    MyRange()
      : ints_{1, 2, 3, 4, 5, 6, 7}
    {}
};

int main()
{
    using namespace ranges;
    auto r = MyRange{};
    ::models<concepts::BoundedRange>(r);
    ::models<concepts::ForwardIterator>(r.begin());
    ::check_equal(r, {1, 2, 3, 4, 5, 6, 7});

    return test_result();
}
