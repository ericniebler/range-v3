// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <vector>
#include <range/v3/action/remove.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

struct Data{
    int i;

    Data() = default;
    explicit Data(int i) : i(i) {}

    Data(const Data&) = delete;

    Data(Data&&) = default;
    Data& operator=(Data&&) = default;

    bool operator==(const Data& other) const {
        return other.i == i;
    }
    bool operator!=(const Data& other) const {
        return other.i != i;
    }
};


int main()
{
    using namespace ranges;

    std::vector<Data> list;
    list.emplace_back(Data{1});
    list.emplace_back(Data{2});
    list.emplace_back(Data{3});
    list.emplace_back(Data{4});


    list |= action::remove(Data{2});
    check_equal(list, {Data{1}, Data{3}, Data{4}});

    list |= action::remove(3, &Data::i);
    check_equal(list, {Data{1}, Data{4}});


    return ::test_result();
}
