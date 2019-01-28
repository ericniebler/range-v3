// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <vector>
#include <iterator>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/generate_n.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/filter.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS

namespace
{
    template<typename T, std::size_t N>
    struct input_array
    {
        T elements_[N];

        input_iterator<T*> begin() { return input_iterator<T*>{elements_ + 0}; }
        input_iterator<T*> end() { return input_iterator<T*>{elements_ + N}; }
        constexpr std::size_t size() const { return N; }
    };

    static int N = 0;
    auto make_input_rng()
    {
        using ranges::view::generate_n;
        return generate_n([](){
            return generate_n([](){
                return N++;
            },3);
        },3);
    }

    template<typename T>
    constexpr auto twice(T t)
    {
        return ranges::view::concat(
            ranges::view::single(t),
            ranges::view::single(t));
    }

#ifdef __clang__
RANGES_DIAGNOSTIC_IGNORE_PRAGMAS
RANGES_DIAGNOSTIC_IGNORE("-Wunneeded-member-function")
RANGES_DIAGNOSTIC_IGNORE("-Wunused-member-function")
#endif

    // https://github.com/ericniebler/range-v3/issues/283
    void test_issue_283()
    {
        const std::vector<std::vector<int>> nums =
        {
            { 1, 2, 3 },
            { 4, 5, 6 }
        };
        const std::vector<int> flat_nums = ranges::view::join( nums );
        ::check_equal(flat_nums, {1,2,3,4,5,6});
    }
}

int main()
{
    using namespace ranges;

    // Test that we can join an input range of input ranges:
    auto rng0 = make_input_rng() | view::join;
    static_assert(range_cardinality<decltype(rng0)>::value == ranges::finite, "");
    models<InputRangeConcept>(rng0);
    models_not<ForwardRangeConcept>(rng0);
    models_not<BoundedRangeConcept>(rng0);
    models_not<SizedRangeConcept>(rng0);
    check_equal(rng0, {0,1,2,3,4,5,6,7,8});

    // Joining with a value
    N = 0;
    auto rng1 = make_input_rng() | view::join(42);
    static_assert(range_cardinality<decltype(rng1)>::value == ranges::finite, "");
    models<InputRangeConcept>(rng1);
    models_not<ForwardRangeConcept>(rng1);
    models_not<BoundedRangeConcept>(rng1);
    models_not<SizedRangeConcept>(rng1);
    check_equal(rng1, {0,1,2,42,3,4,5,42,6,7,8});

    // Joining with a range
    N = 0;
    int rgi[] = {42,43};
    auto rng2 = make_input_rng() | view::join(rgi);
    static_assert(range_cardinality<decltype(rng2)>::value == ranges::finite, "");
    models<InputRangeConcept>(rng2);
    models_not<ForwardRangeConcept>(rng2);
    models_not<BoundedRangeConcept>(rng2);
    models_not<SizedRangeConcept>(rng2);
    check_equal(rng2, {0,1,2,42,43,3,4,5,42,43,6,7,8});

    // Just for fun:
    std::string str = "Now,is,the,time,for,all,good,men,to,come,to,the,aid,of,their,country";
    std::string res = str | view::split(',') | view::join(' ');
    CHECK(res == "Now is the time for all good men to come to the aid of their country");
    static_assert(range_cardinality<decltype(res)>::value == ranges::finite, "");

    std::vector<std::string> vs{"This","is","his","face"};
    auto rng3 = view::join(vs);
    static_assert(range_cardinality<decltype(rng3)>::value == ranges::finite, "");
    models_not<SizedRangeConcept>(rng3);
    CPP_assert(!SizedSentinel<decltype(end(rng3)), decltype(begin(rng3))>);
    CHECK(to<std::string>(rng3) == "Thisishisface");

    auto rng4 = view::join(vs, ' ');
    static_assert(range_cardinality<decltype(rng3)>::value == ranges::finite, "");
    models_not<SizedRangeConcept>(rng4);
    CPP_assert(!SizedSentinel<decltype(end(rng4)), decltype(begin(rng4))>);
    CHECK(to<std::string>(rng4) == "This is his face");

    auto rng5 = view::join(twice(twice(42)));
    static_assert(range_cardinality<decltype(rng5)>::value == 4, "");
    models<SizedRangeConcept>(rng5);
    CHECK(rng5.size() == 4u);
    check_equal(rng5, {42,42,42,42});

    auto rng6 = view::join(twice(view::repeat_n(42, 2)));
    static_assert(range_cardinality<decltype(rng6)>::value == ranges::finite, "");
    models<SizedRangeConcept>(rng6);
    CHECK(rng6.size() == 4u);
    check_equal(rng6, {42,42,42,42});

    test_issue_283();

    {
        input_array<std::string, 4> some_strings = {{"This","is","his","face"}};
        models<InputRangeConcept>(some_strings);
        models<SizedRangeConcept>(some_strings);
        models_not<SizedRangeConcept>(some_strings | view::join);
    }

    {
        int const some_int_pairs[3][2] = {{0,1},{2,3},{4,5}};
        auto rng = debug_input_view<int const[2]>{some_int_pairs} | view::join;
        check_equal(rng, {0,1,2,3,4,5});
    }

	{
		std::vector<std::string> vs{"this","is","his","face"};
		join_view<ref_view<std::vector<std::string>>> jv{vs};
		check_equal(jv, {'t','h','i','s','i','s','h','i','s','f','a','c','e'});
		CPP_assert(BidirectionalRange<decltype(jv)>);
		CPP_assert(BidirectionalRange<const decltype(jv)>);
		CPP_assert(CommonRange<decltype(jv)>);
		CPP_assert(CommonRange<const decltype(jv)>);
	}

	{
		auto rng = view::iota(0,4)
			| view::transform([](int i) {return view::iota(0,i);})
			| view::join;
		check_equal(rng, {0,0,1,0,1,2});
		CPP_assert(InputRange<decltype(rng)>);
		CPP_assert(!Range<const decltype(rng)>);
		CPP_assert(!ForwardRange<decltype(rng)>);
		CPP_assert(!CommonRange<decltype(rng)>);
	}

	{
		auto rng = view::iota(0,4)
			| view::transform([](int i) {return view::iota(0,i);})
			| view::filter([](auto){ return true; })
			| view::join;
		check_equal(rng, {0,0,1,0,1,2});
		CPP_assert(InputRange<decltype(rng)>);
		CPP_assert(!Range<const decltype(rng)>);
		CPP_assert(!ForwardRange<decltype(rng)>);
		CPP_assert(!CommonRange<decltype(rng)>);
	}

    return ::test_result();
}
