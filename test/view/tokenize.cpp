#include <range/v3/core.hpp>
#include <range/v3/view/tokenize.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
	using namespace ranges;

	std::string txt{"abc\ndef\tghi"};
	const std::regex rx{R"delim(([\w]+))delim"};
	auto&& rng = txt | view::tokenize(rx,1);
	const auto& crng = txt | view::tokenize(rx,1);

	::check_equal(rng, {"abc","def","ghi"});
	::check_equal(crng, {"abc","def","ghi"});

	::has_type<const std::sub_match<std::string::iterator>&>(*ranges::begin(rng));
	::has_type<const std::sub_match<std::string::iterator>&>(*ranges::begin(crng));

	::models<concepts::BoundedIterable>(rng);
	::models<concepts::ForwardIterable>(rng);
	::models_not<concepts::BidirectionalIterable>(rng);
	::models_not<concepts::SizedIterable>(rng);
	::models_not<concepts::OutputIterable>(rng);

	::models<concepts::BoundedIterable>(crng);
	::models<concepts::ForwardIterable>(crng);
	::models_not<concepts::BidirectionalIterable>(crng);
	::models_not<concepts::SizedIterable>(crng);
	::models_not<concepts::OutputIterable>(crng);

	// ::models<concepts::Range>(rng);
	// ::models<concepts::Range>(crng);

	return test_result();
}
