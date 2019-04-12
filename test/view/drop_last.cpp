/// \file
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
//

#include "../simple_test.hpp"
#include "../test_utils.hpp"

#include <vector>
#include <list>
#include <forward_list>

#include <range/v3/view/drop_last.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/transform.hpp>

using namespace ranges;

template<class Rng>
class view_non_const_only
    : public view_adaptor<view_non_const_only<Rng>, Rng>
{
    friend ranges::range_access;

    ranges::adaptor_base begin_adaptor() { return {}; }
    ranges::adaptor_base end_adaptor()   { return {}; }

    // ???
    ranges::adaptor_base begin_adaptor() const = delete;
    ranges::adaptor_base end_adaptor()   const = delete;
public:
    using view_non_const_only::view_adaptor::view_adaptor;
};

template<class Rng>
view_non_const_only<view::all_t<Rng>> non_const_only(Rng &&rng){
    return view_non_const_only<view::all_t<Rng>>{view::all(static_cast<Rng&&>(rng))};
}


template<class Rng>
void test_range(Rng&& src)
{
    {
        auto list = src | view::drop_last(2);
        ::check_equal(list, {1,2});
    }
    {
        auto list = src | view::drop_last(0);
        ::check_equal(list, {1,2,3,4});
    }
    {
        auto list = src | view::drop_last(4);
        CHECK(list.empty());
    }
}

template<class Rng>
void test_size(Rng&& src)
{
    CHECK( (src | view::drop_last(2)).size() == std::size_t(2) );
}

void random_acccess_test()
{
    using Src = std::vector<int>;
    static_assert(
        ranges::RandomAccessRange<Src>().value
        , "Must be exactly RA.");
    static_assert(
        std::is_same<
            drop_last_view<view::all_t<Src>>, drop_last_view<view::all_t<Src>, true>
        >::value
        , "Must have correct view.");

    Src src = {1,2,3,4};

    test_range(src);
    test_range(non_const_only(src));
    test_size(src);

    // test non-convertible const<=>non-const range
    test_range(std::move(src | view::transform([](const int& i) -> const int& {return i;})));
}

void bidirectional_test()
{
    using Src = std::list<int>;
    static_assert(
        !ranges::RandomAccessRange<Src>().value &&
        ranges::BidirectionalRange<Src>().value
        , "Must be exactly bidirectional.");
    static_assert(
        std::is_same<
            drop_last_view<view::all_t<Src>>, drop_last_view<view::all_t<Src>, true>
        >::value
        , "Must have correct view.");

    Src src = {1,2,3,4};

    test_range(src);
    test_range(non_const_only(src));
    test_size(src);

    // test non-convertible const<=>non-const range
    test_range(std::move(src | view::transform([](const int& i) -> const int& {return i;})));
}

void forward_test()
{
    using Src = std::forward_list<int>;
    static_assert(
        !ranges::BidirectionalRange<Src>().value &&
        ranges::ForwardRange<Src>().value
        , "Must be exactly forward.");
    static_assert(
        std::is_same<
            drop_last_view<view::all_t<Src>>, drop_last_view<view::all_t<Src>, false>
        >::value
        , "Must have correct view.");

    Src src = {1,2,3,4};

    test_range(src);
    test_range(non_const_only(src));
    test_size(src | view::take_exactly(4));

    // test non-convertible const<=>non-const range
    test_range(std::move(src | view::transform([](const int& i) -> const int& {return i;})));
}

int main()
{
    random_acccess_test();
    bidirectional_test();
    forward_test();

    return test_result();
}