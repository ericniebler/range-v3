#include <vector>
#include <list>
#include <forward_list>
#include <range/v3/core.hpp>
#include <range/v3/range_concepts.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

int main()
{
    using namespace ranges;

    auto vec = std::vector<int>{1, 2, 3, 4, 5};
    auto lst = std::list<int>{1, 2, 3, 4};

    static_assert(
        concepts::models<concepts::RandomAccessIterator, decltype(vec.begin())>{}, "??");

    CHECK(size(vec) == 5);
    CHECK(size(lst) == 4); // std::list offers .size() member function

    auto &&vec_rng = range(begin(vec), end(vec));
    auto &&lst_rng = range(begin(lst), end(lst));
    auto &&lst_rng2 = range(lst);

    CHECK(size(vec_rng) == 5);
    CHECK(size(lst_rng2) == 4);
    static_assert(
        !concepts::models<concepts::Invokable, decltype(size), decltype(lst_rng)>(),
        "no O(1) size for bidirectional ranges");

    auto &&lst_sized_rng = range(begin(lst), end(lst), lst.size());
    CHECK(size(lst_sized_rng) == 4);

    auto f_lst = std::forward_list<int>{1, 2, 3};
    static_assert(
        !concepts::models<concepts::Invokable, decltype(size), decltype(f_lst)>(),
        "no O(1) size member function in std::forward_list");

    return ::test_result();
}
