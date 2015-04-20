#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/back.hpp>
#include <range/v3/front.hpp>
#include <range/v3/at.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view/iota.hpp>

#ifdef RANGES_CXX_GREATER_THAN_11
template<class R>
RANGES_RELAXED_CONSTEXPR auto get_end(R const& r) {
    auto b = ranges::begin(r);
    ranges::advance(b, ranges::end(r));
    return b;
}

template<class R>
RANGES_RELAXED_CONSTEXPR auto test_it(R&& r) {
   auto beg = ranges::begin(r);
   auto end = ranges::end(r);
   --end;
   return (*beg) + (*end);
}

template<class R>
RANGES_RELAXED_CONSTEXPR auto test_cit(R&& r) {
    auto beg = ranges::cbegin(r);
    auto end = ranges::cend(r);
    --end;
    return (*beg) + (*end);
}

template<class R>
RANGES_RELAXED_CONSTEXPR auto test_rit(R&& rng) {
    auto beg = ranges::rbegin(rng);
    auto end = ranges::rend(rng);
    --end;
    return (*beg) + (*end);
}

template<class R>
RANGES_RELAXED_CONSTEXPR auto test_crit(R&& rng) {
    auto beg = ranges::crbegin(rng);
    auto end = ranges::crend(rng);
    --end;
    return (*beg) + (*end);
}

#endif

int main() {

#ifdef RANGES_CXX_GREATER_THAN_11
    {  // iota range
        constexpr auto a = ranges::view::iota(1,4);
        constexpr auto beg = ranges::begin(a);
        static_assert(*beg == 1, "");
        constexpr auto end_m1 = --get_end(a);
        static_assert(*end_m1 == 4, "");
        static_assert(test_it(a) == 5, "");
        static_assert(test_cit(a) == 5, "");
        static_assert(!ranges::empty(a), "");
        static_assert(ranges::front(a) == 1, "");
        static_assert(ranges::back(a) == 4, "");
        static_assert(ranges::at(a, 2) == 3, "");
        static_assert(ranges::size(a) == 4, "");
    }
    {  // C-Array
        constexpr int a[4]{1, 2, 3, 4};
        static_assert(test_it(a) == 5, "");
        static_assert(test_cit(a) == 5, "");
        static_assert(test_rit(a) == 5, "");
        static_assert(test_crit(a) == 5, "");
        static_assert(!ranges::empty(a), "");
        static_assert(ranges::front(a) == 1, "");
        static_assert(ranges::back(a) == 4, "");
        static_assert(ranges::at(a, 2) == 3, "");
        static_assert(ranges::size(a) == 4, "");

    }
    { // initializer_list
        static_assert(test_it(std::initializer_list<int>{1, 2, 3, 4}) == 5, "");
        static_assert(test_cit(std::initializer_list<int>{1, 2, 3, 4}) == 5, "");
        static_assert(test_rit(std::initializer_list<int>{1, 2, 3, 4}) == 5, "");
        static_assert(test_crit(std::initializer_list<int>{1, 2, 3, 4}) == 5, "");
        static_assert(ranges::size(std::initializer_list<int>{1, 2, 3, 4}) == 4, "");

        static_assert(ranges::front(std::initializer_list<int>{1, 2, 3, 4}) == 1, "");
        static_assert(ranges::back(std::initializer_list<int>{1, 2, 3, 4}) == 4, "");
        static_assert(ranges::at(std::initializer_list<int>{1, 2, 3, 4}, 2) == 3, "");

        // empty init list:
        static_assert(ranges::size(std::initializer_list<int>{}) == 0, "");
        static_assert(ranges::empty(std::initializer_list<int>{}), "");
    }
#endif

    return 0;
}
