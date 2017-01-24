// Range v3 library
//
//  Copyright Filip Matzner 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <list>
#include <memory>
#include <tuple>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/shared.hpp>
#include <range/v3/view/take.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

template<typename T>
std::tuple<std::shared_ptr<T>, shared_view<T>, shared_view<T>>
    make_from_shared_ptr()
{
    // build two instances sharing the same base
    auto base_ptr = std::make_shared<T>();
    *base_ptr = {1, 1, 1, 2, 3, 4, 4};
    auto view1 = view::shared(base_ptr);
    auto view2 = view1;
    CHECK(base_ptr.use_count() == 3);
    return std::make_tuple(base_ptr, view1, view2);
}

template<typename T>
std::tuple<shared_view<T>, shared_view<T>>
    make_from_rvalue()
{
    // build two instances sharing the same base
    auto view1 = view::shared(T{1, 1, 1, 2, 3, 4, 4});
    auto view2 = view1;
    CHECK(view1.get_shared().use_count() == 2);
    return std::make_tuple(view1, view2);
}

template<typename T>
void check_shared_contents_(shared_view<T> view1, shared_view<T> view2)
{
    // check the length of the views
    CHECK(view1.size() == 7u);
    CHECK(view2.size() == 7u);

    // try to mangle with the sharing objects and check shared_ptr use count
    auto base = view1.get_shared();
    CHECK(base.use_count() == 3);
    view1 = view2;
    CHECK(base.use_count() == 3);
    view2 = view1;
    CHECK(base.use_count() == 3);
    view2 = shared_view<T>();
    CHECK(base.use_count() == 2);
    base = std::shared_ptr<T>();
    CHECK(view1.get_shared().use_count() == 1);
    view2 = view1;
    CHECK(view1.get_shared().use_count() == 2);

    // check the stored numbers
    auto check_values = [](shared_view<T> & rng) {
      ::check_equal(view::cycle(rng) | view::take(10), {1, 1, 1, 2, 3, 4, 4, 1, 1, 1});
      ::check_equal(view::all(rng) | view::take(5), {1, 1, 1, 2, 3});
      ::check_equal(rng | view::take(5), {1, 1, 1, 2, 3});
      ::check_equal(rng, {1, 1, 1, 2, 3, 4, 4});
    };
    check_values(view1);
    check_values(view2);
}

template<typename T>
void check_shared_contents()
{
    std::shared_ptr<T> base;
    shared_view<T> view1, view2;

    // build two views from a shared_ptr in an indepenent function
    std::tie(base, view1, view2) = make_from_shared_ptr<T>();
    CHECK(base.use_count() == 3);
    base = std::shared_ptr<T>();
    // check in an independent function
    check_shared_contents_(std::move(view1), std::move(view2));

    // build two views from an rvalue in an indepenent function
    std::tie(view1, view2) = make_from_rvalue<T>();
    // check in an independent function
    check_shared_contents_(std::move(view1), std::move(view2));
}

int main()
{
    check_shared_contents<std::vector<int>>();
    check_shared_contents<std::list<int>>();

    {
        //check whether it is possible to write through into the shared storage
        auto base_vec_ptr = std::make_shared<std::vector<int>>();
        *base_vec_ptr = {2, 1, 5, 3, 9};
        auto vec_view = view::shared(base_vec_ptr) | view::remove_if([](int i){ return i > 4; });
        RANGES_FOR(int &a, vec_view) { a = 0; } // zero out all the elements <= 4
        ::check_equal(*base_vec_ptr, {0, 0, 5, 0, 9});
    }

    {
        // check the piped construction from an rvalue
        std::vector<int> base_vec = {1, 2, 2, 8, 2, 7};
        auto vec_view = std::move(base_vec) | view::shared;
        CHECK(vec_view.size() == 6u);
        CHECK(vec_view.get_shared().use_count() == 1);
        ::check_equal(vec_view, {1, 2, 2, 8, 2, 7});
    }

    {
        // check the piped construction from a shared_ptr
        auto base_vec_ptr = std::make_shared<std::vector<int>>();
        *base_vec_ptr = {1, 2, 8, 8, 8};
        auto vec_view = base_vec_ptr | view::shared;
        CHECK(vec_view.size() == 5u);
        CHECK(vec_view.get_shared().use_count() == 2);
        ::check_equal(vec_view, {1, 2, 8, 8, 8});
    }

    {
        // check the piped construction from an rvalue of a shared_ptr
        auto base_vec_ptr = std::make_shared<std::vector<int>>();
        *base_vec_ptr = {1, 2, 8, 8, 8};
        auto vec_view = std::move(base_vec_ptr) | view::shared;
        CHECK(vec_view.size() == 5u);
        CHECK(vec_view.get_shared().use_count() == 1);
        ::check_equal(vec_view, {1, 2, 8, 8, 8});
    }

    {
        // test bidirectional range
        auto list_view = std::list<int>{1, 2, 3} | view::shared;

        CHECK(list_view.size() == 3u);
        has_type<int &>(*begin(list_view));
        ::models<concepts::SizedView>(list_view);
        ::models<concepts::BidirectionalView>(list_view);
        ::models<concepts::BidirectionalIterator>(begin(list_view));
        ::models_not<concepts::RandomAccessView>(list_view);
        ::models_not<concepts::RandomAccessIterator>(begin(list_view));

        // test bidirectional range iterator
        auto list_iter = begin(list_view);
        CHECK(*list_iter == 1);
        CHECK(*(++list_iter) == 2);
        auto nxt = next(list_iter, 1);
        auto prev = next(list_iter, -1);
        CHECK(*nxt == 3);
        CHECK(*prev == 1);
        CHECK(*(--nxt) == *list_iter);
        CHECK(*(++prev) == *list_iter);
        CHECK(*(--list_iter) == 1);
    }

    {
        // test bidirectional range, which does not know its size
        auto base_list = std::list<int>{1, 2, 3};
        auto iter_range = make_iterator_range(begin(base_list), end(base_list));
        auto list_view = std::move(iter_range) | view::shared;

        has_type<int &>(*begin(list_view));
        ::models_not<concepts::SizedView>(list_view);
        ::models<concepts::BidirectionalView>(list_view);
        ::models<concepts::BidirectionalIterator>(begin(list_view));

        // test its iterator
        auto list_iter = begin(list_view);
        CHECK(*list_iter == 1);
        CHECK(*(++list_iter) == 2);
        CHECK(*(--list_iter) == 1);
    }

    {
        // test random access range
        auto vec_view = std::vector<int>{1, 2, 3} | view::shared;

        CHECK(vec_view.size() == 3u);
        has_type<int &>(*begin(vec_view));
        ::models<concepts::SizedView>(vec_view);
        ::models<concepts::RandomAccessView>(vec_view);
        ::models<concepts::RandomAccessIterator>(begin(vec_view));
        CHECK(vec_view[0] == 1);
        CHECK(vec_view[1] == 2);
        CHECK(vec_view[2] == 3);

        // test random access range iterator
        auto vec_iter = begin(vec_view);
        CHECK(*vec_iter == 1);
        CHECK(vec_iter[0] == 1);
        CHECK(vec_iter[1] == 2);
        CHECK(vec_iter[2] == 3);
    }

    {
        // test const random access range
        const std::vector<int> cvec{1, 2, 3};
        const auto cvec_ptr = std::make_shared<const std::vector<int>>(cvec);
        auto vec_view = cvec_ptr | view::shared;

        CHECK(vec_view.size() == 3u);
        has_type<const int &>(*begin(vec_view));
        ::models<concepts::SizedView>(vec_view);
        ::models<concepts::RandomAccessView>(vec_view);
        ::models<concepts::RandomAccessIterator>(begin(vec_view));
        CHECK(vec_view[0] == 1);
        CHECK(vec_view[1] == 2);
        CHECK(vec_view[2] == 3);

        // test const random access range iterator
        auto vec_iter = begin(vec_view);
        CHECK(*vec_iter == 1);
        CHECK(vec_iter[0] == 1);
        CHECK(vec_iter[1] == 2);
        CHECK(vec_iter[2] == 3);
    }

    {
        // check ranges::for_each
        auto vec_view = std::vector<int>{1, 2, 3} | view::shared;
        std::vector<int> vec_view_copy;
        for_each(vec_view, [&](int a) { vec_view_copy.push_back(a); });
        ::check_equal(vec_view, vec_view_copy);
    }

    {
        // check RANGES_FOR
        auto vec_view = std::vector<int>{1, 2, 3} | view::shared;
        std::vector<int> vec_view_copy;
        RANGES_FOR(int a, vec_view) {
            vec_view_copy.push_back(a);
        }
        ::check_equal(vec_view, vec_view_copy);
    }

    {
        // check temporary value in view::transform
        auto f = [](unsigned a){ return std::vector<unsigned>(a, a); };

        auto vec_view =
            view::iota(1u)
          | view::transform(f)
          | view::transform(view::shared)
          | view::join
          | view::take(10);

        ::check_equal(vec_view, {1u, 2u, 2u, 3u, 3u, 3u, 4u, 4u, 4u, 4u});
    }

    {
        // check temporary value in view::for_each
        std::vector<int> base_vec{1, 2, 3};
        auto vec_view =
            view::repeat(base_vec)
          | view::for_each([](std::vector<int> tmp) {
                return yield_from(std::move(tmp) | view::shared | view::reverse);
            })
          | view::take(7);
        ::check_equal(vec_view, {3, 2, 1, 3, 2, 1, 3});
    }

    {
        // check temporary value in view::for_each without the yield_from
        std::vector<int> base_vec{1, 2, 3};
        auto vec_view =
            view::repeat(base_vec)
          | view::for_each([](std::vector<int> tmp) {
                return std::move(tmp) | view::shared | view::reverse;
            })
          | view::take(7);
        ::check_equal(vec_view, {3, 2, 1, 3, 2, 1, 3});
    }

    return test_result();
}
