#include <range/v3/view/sample.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <numeric>
#include <vector>
#include <random>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

int main ()
{
    std::vector<int> pop(100);
    std::iota(std::begin(pop), std::end(pop), 0);
    {
        constexpr std::size_t N = 32;
        std::array<int, N> tmp;
        std::mt19937 engine;
        auto rng = pop | view::sample(N, engine);
        CONCEPT_ASSERT(InputRange<decltype(rng)>());
        CONCEPT_ASSERT(View<decltype(rng)>());
        ranges::copy(rng, tmp.begin());
        rng = pop | view::sample(N, engine);
        CHECK(!ranges::equal(rng, tmp));
        engine = decltype(engine){};
        rng = pop | view::sample(N, engine);
        CHECK(ranges::equal(rng, tmp));
    }

    return ::test_result();
}
