#include <range/v3/utility/variant.hpp>


int main() {
    constexpr ranges::tagged_variant<int, double> v0(meta::size_t<0>{}, 2);
    constexpr ranges::tagged_variant<int, double> v1(meta::size_t<1>{}, 2.0);
    constexpr ranges::tagged_variant<int, double> v2(meta::size_t<0>{}, 2);
    static_assert(v0 != v1, "");
    static_assert(v0 == v2, "");
    return 0;
}
