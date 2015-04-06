#include <range/v3/begin_end.hpp>
#include <range/v3/view/iota.hpp>
#include <array>

int main() {
    constexpr auto a = ranges::view::iota(1,4);

    constexpr auto b = ranges::begin(a);

    return 0;
}
