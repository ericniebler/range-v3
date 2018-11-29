// Range v3 library
//
//  Copyright Gonzalo Brito Gadeschi 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <range/v3/detail/config.hpp>

#if RANGES_CXX_CONSTEXPR >= RANGES_CXX_CONSTEXPR_14

#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/back.hpp>
#include <range/v3/front.hpp>
#include <range/v3/at.hpp>
#include <range/v3/size.hpp>
#include "array.hpp"
#include "test_iterators.hpp"

// Test sequence 1,2,3,4
template<typename It>
RANGES_CXX14_CONSTEXPR auto test_it_back(It, It end,
    ranges::concepts::BidirectionalIterator*) -> bool
{
    auto end_m1_2 = It{ranges::prev(end, 1)};
    if (*end_m1_2 != 4) { return false; }
    return true;
}

template<typename It, typename Concept>
RANGES_CXX14_CONSTEXPR auto test_it_back(It, It, Concept) -> bool
{
    return true;
}

template<typename It>
RANGES_CXX14_CONSTEXPR auto test_it_(It beg, It end) -> bool {
    if (*beg != 1) { return false; }
    if (ranges::distance(beg, end) != 4) { return false; }
    if (ranges::next(beg, 4) != end) { return false; }
    auto end_m1 = It{ranges::next(beg, 3)};
    if (*end_m1 != 4) { return false; }

    if (!test_it_back(beg, end, ranges::iterator_concept<It>{})) { return false; }
    auto end2 = beg;
    ranges::advance(end2, end);
    if (end2 != end) { return false; }
    auto end3 = beg;
    ranges::advance(end3, 4);
    if (end3 != end) { return false; }
    if (ranges::iter_enumerate(beg, end) != std::pair<std::ptrdiff_t, It>{4, end})
    {
        return false;
    }
    if (ranges::iter_distance(beg, end) != 4) { return false; }
    if (ranges::iter_distance_compare(beg, end, 4) != 0) { return false; }
    if (ranges::iter_distance_compare(beg, end, 3) != 1) { return false; }
    if (ranges::iter_distance_compare(beg, end, 5) != -1) { return false; }
    return true;
}

// Test sequence 4,3,2,1 (for reverse iterators)
template<typename It>
RANGES_CXX14_CONSTEXPR auto test_rit_(It beg, It end) -> bool {
    if (ranges::distance(beg, end) != 4) { return false; }
    if (ranges::next(beg, 4) != end) { return false; }
    auto end_m1 = It{ranges::next(beg, 3)};
    if (*end_m1 != 1) { return false; }
    if (ranges::detail::is_convertible<ranges::iterator_concept<It>,
                                       ranges::concepts::BidirectionalIterator*>{}) {
        auto end_m1_2 = It{ranges::prev(end, 1)};
        if (*end_m1_2 != 1) { return false; }
    }
    auto end2 = beg;
    ranges::advance(end2, end);
    if (end2 != end) { return false; }
    auto end3 = beg;
    ranges::advance(end3, 4);
    if (end3 != end) { return false; }
    if (ranges::iter_enumerate(beg, end) != std::pair<std::ptrdiff_t, It>{4, end})
    {
        return false;
    }
    if (ranges::iter_distance(beg, end) != 4) { return false; }
    if (ranges::iter_distance_compare(beg, end, 4) != 0) { return false; }
    if (ranges::iter_distance_compare(beg, end, 3) != 1) { return false; }
    if (ranges::iter_distance_compare(beg, end, 5) != -1) { return false; }
    return true;
}

template<typename It, typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_it(Sequence1234&& a) -> bool {
    auto beg = It{ranges::begin(a)};
    auto end = It{ranges::end(a)};
    return test_it_(beg, end);
}

template<typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_its_c(Sequence1234&& a) -> bool {
    return     test_it<input_iterator<int const *>>(a)
            && test_it<forward_iterator<int const *>>(a)
            && test_it<bidirectional_iterator<int const *>>(a)
            && test_it<random_access_iterator<int const *>>(a);

}

template<typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_its(Sequence1234&& a) -> bool {
    return     test_it<input_iterator<int *>>(a)
            && test_it<forward_iterator<int *>>(a)
            && test_it<bidirectional_iterator<int *>>(a)
            && test_it<random_access_iterator<int *>>(a)
            && test_its_c(a);

}

template<typename It, typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_rit(Sequence1234&& a) -> bool {
    auto beg = It{ranges::rbegin(a)};
    auto end = It{ranges::rend(a)};
    return test_rit_(beg, end);
}

template<typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_rits(Sequence1234&& a) -> bool {
    using rit = decltype(ranges::rbegin(a));
    return     test_rit<bidirectional_iterator<rit>>(a)
            && test_rit<random_access_iterator<rit>>(a);
}

template<typename It, typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_cit(Sequence1234&& a) -> bool {
    auto beg = It{ranges::cbegin(a)};
    auto end = It{ranges::cend(a)};
    return test_it_(beg, end);
}

template<typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_cits(Sequence1234&& a) -> bool {
    return     test_cit<input_iterator<int const *>>(a)
            && test_cit<forward_iterator<int const *>>(a)
            && test_cit<bidirectional_iterator<int const *>>(a)
            && test_cit<random_access_iterator<int const *>>(a);
}


template<typename It, typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_crit(Sequence1234&& a) -> bool {
    auto beg = It{ranges::crbegin(a)};
    auto end = It{ranges::crend(a)};
    return test_rit_(beg, end);
}

template<typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_crits(Sequence1234&& a) -> bool {
    using rit = decltype(ranges::crbegin(a));
    return     test_crit<bidirectional_iterator<rit>>(a)
            && test_crit<random_access_iterator<rit>>(a);
}

template<typename Sequence1234>
RANGES_CXX14_CONSTEXPR auto test_non_member_f(Sequence1234&& a) -> bool {
    if (ranges::empty(a)) { return false; }
    if (ranges::front(a) != 1) { return false; }
    if (ranges::back(a) != 4) { return false; }
    if (ranges::index(a, 2) != 3) { return false; }
    if (ranges::at(a, 2) != 3) { return false; }
    if (ranges::size(a) != 4) { return false; }
    return true;
}

RANGES_CXX14_CONSTEXPR auto test_array() -> bool {
    test::array<int, 4> a{{1, 2, 3, 4}};

    auto beg = ranges::begin(a);
    auto three = ranges::next(beg, 2);

    if ((false)) {
      ranges::iter_swap(beg, three);
      if (*beg != 3) { return false; }
      if (*three != 1) { return false; }
      ranges::iter_swap(beg, three);
    }

    if (!test_its(a)) { return false; }
    if (!test_cits(a)) { return false; }
    if (!test_rits(a)) { return false; }
    if (!test_crits(a)) { return false; }
    if (!test_non_member_f(a)) { return false; }

    // This can be workedaround but is just bad:
    test::array<int, 4> b{{5, 6, 7, 8}};
    ranges::swap(a, b);
    if (a[0] != 5 || b[0] != 1 || a[3] != 8 || b[3] != 4) { return false; }

    return true;
}

RANGES_CXX14_CONSTEXPR auto test_c_array() -> bool {
    int a[4]{1, 2, 3, 4};
    if (!test_its(a)) { return false; }
    if (!test_cits(a)) { return false; }
    if (!test_rits(a)) { return false; }
    if (!test_crits(a)) { return false; }
    if (!test_non_member_f(a)) { return false; }

    // C-arrays have no associated namespace, so this can't work:
    // int b[4]{5, 6, 7, 8};
    // ranges::swap(a, b);
    // if (a[0] != 5 || b[0] != 1 || a[3] != 8 || b[3] != 4) { return false; }

    return true;
}

RANGES_CXX14_CONSTEXPR auto test_init_list() -> bool {
    std::initializer_list<int> a{1, 2, 3, 4};
    if (!test_its_c(a)) { return false; }
    if (!test_cits(a)) { return false; }
    if (!test_rits(a)) { return false; }
    if (!test_crits(a)) { return false; }
    if (!test_non_member_f(a)) { return false; }

    std::initializer_list<int> b{5, 6, 7, 8};
    ranges::swap(a, b);
    if (ranges::at(a, 0) != 5 || ranges::at(b, 0) != 1
        || ranges::at(a, 3) != 8 || ranges::at(b, 3) != 4) {
        return false;
    }

    return true;
}

int main() {
    static_assert(test_array(), "");
    static_assert(test_c_array(), "");
    static_assert(test_init_list(), "");
}

#else
int main() {}
#endif
