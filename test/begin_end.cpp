#include <range/v3/begin_end.hpp>
#include <range/v3/view/iota.hpp>
#include <array>

int main() {

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        constexpr auto a = ranges::view::iota(1,4);
        constexpr auto b = ranges::begin(a);
    }
#endif

    return 0;
}
