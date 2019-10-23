#include <vector>
#include <transform_join.hpp>
#include <iostream>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main() {
	{
		std::vector<int> rng = { 1,3,2,0 };
		//*

		auto tr = rng | ranges::views::transform_join([&](int a) {
			return ranges::views::generate_n([=]() {
				return a;
				}, a);
			});

		::check_equal(tr,{1,3,3,3,2,2});
	}

	{
		std::vector<int> rng;

		auto y = rng | ranges::views::transform_join([](int a) {
			return std::vector<int>(a, a);
			});

		CPP_assert(ranges::input_range<decltype(y)>);

		rng = { 1,2,3,4 };
		::check_equal(y, { 1,2,2,3,3,3,4,4,4,4 });
	}
	{
		std::vector<int> rng = { 1,2,3,4 };
		auto tr = rng | ranges::views::transform_join([](int a) {
			return ranges::views::iota(0, a);
			});
		CPP_assert(ranges::input_range<decltype(tr)>);


		::check_equal(tr, { 0,0,1,0,1,2,0,1,2,3 });
	}
	{
		std::vector<std::vector<int>> data = { {1,1,1,1},{2,2,2},{3,3},{4} };
		std::vector<int> rng = { 1,3,2,0 };
		auto tr = rng | ranges::views::transform_join([&](int a) ->auto& {
			return data[a];
		});

		CPP_assert(ranges::input_range<decltype(tr)>);


		::check_equal(tr, { 2,2,2,4,3,3,1,1,1,1 });
	}

	{
		std::vector<int> rng = { 1,3,2,0,3,4,5 };
		auto tr = rng | ranges::views::transform_join([&](int a) {
			return std::array<int,2>{a,0};
		});

		CPP_assert(ranges::input_range<decltype(tr)>);


		::check_equal(tr, { 1,0,3,0,2,0,0,0,3,0,4,0,5,0 });
	}
	{
		
		std::vector<int> rng = { 1,3,2,0,3,4,5 };
		auto tr = rng | ranges::views::transform_join([&](int a) {
			return std::to_string(a);
		});

		CPP_assert(ranges::input_range<decltype(tr)>);


		::check_equal(tr, std::string("1320345"));
	}

	{

		std::vector<int> rng = { 1,3,2,0,3,4,5 };
		auto tr = rng | ranges::views::transform_join([&](int a) {
			return std::string_view("");
			});

		CPP_assert(ranges::input_range<decltype(tr)>);
		
		auto b = ranges::begin(tr);
		auto e = ranges::end(tr);
		CHECK(b == e);
		//can't use empty since tr isn't forward range
	}
	
	std::cin.get();
}
