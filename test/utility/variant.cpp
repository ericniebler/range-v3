#include <range/v3/utility/variant.hpp>
#include "../simple_test.hpp"

int main() {

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        constexpr ranges::tagged_variant<int, double> v0(meta::size_t<0>{}, 2);
        constexpr ranges::tagged_variant<int, double> v1(meta::size_t<1>{}, 2.0);
        constexpr ranges::tagged_variant<int, double> v2(meta::size_t<0>{}, 2);
        static_assert(v0 != v1, "");
        static_assert(v0 == v2, "");
    }
#endif

  return test_result();
}
