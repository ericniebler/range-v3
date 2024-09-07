#include <range/v3/core.hpp>
#include <range/v3/view/tokenize.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

template <class>
void test_forward(std::false_type)
{}

template <class R>
void test_forward(std::true_type)
{
    CPP_assert(ranges::forward_range<R>);
}

int main()
{
    using namespace ranges;

    // GCC 4.8 doesn't do regex
#if !defined(__GNUC__) || defined(__clang__) || __GNUC__ > 4 || __GNUC_MINOR__ > 8
    std::string txt{"abc\ndef\tghi"};
    const std::regex rx{R"delim(([\w]+))delim"};
    auto rng = txt | views::tokenize(rx,1);
    const auto crng = txt | views::tokenize(rx,1);

    ::check_equal(rng, {"abc","def","ghi"});
    ::check_equal(crng, {"abc","def","ghi"});

    ::has_type<const std::sub_match<std::string::iterator>&>(*ranges::begin(rng));
    ::has_type<const std::sub_match<std::string::iterator>&>(*ranges::begin(crng));

    CPP_assert(common_range<decltype(rng)>);
    const bool is_bidi = bidirectional_iterator<std::regex_token_iterator<std::string::iterator>>;
    test_forward<decltype(rng)>(std::integral_constant<bool, is_bidi>{});
    CPP_assert(!bidirectional_range<decltype(rng)>);
    CPP_assert(!sized_range<decltype(rng)>);

    CPP_assert(common_range<decltype(crng)>);
    const bool const_is_bidi = bidirectional_iterator<std::regex_token_iterator<std::string::const_iterator>>;
    test_forward<decltype(crng)>(std::integral_constant<bool, const_is_bidi>{});
    CPP_assert(!bidirectional_range<decltype(crng)>);
    CPP_assert(!sized_range<decltype(crng)>);

    CPP_assert(view_<decltype(rng)>);
    CPP_assert(!view_<decltype(crng)>);
#endif

    return test_result();
}
