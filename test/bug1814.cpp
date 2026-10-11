#if defined(_MSC_VER)
// Enable level 4 "unreachable code" warning
#pragma warning(error : 4702)
#elif defined(__clang__)
#pragma clang diagnostic error "-Wunreachable-code-aggressive"
#elif defined(__GNUC__)
// Unsupported and ignored in gcc >= 5
#pragma GCC diagnostic error "-Wunreachable-code"
#endif

#include <vector>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

int main()
{
    const std::vector<std::vector<int>> v{{{1, 2, 3}}, {{4, 5, 6}}};
    v | ranges::views::transform([](const auto & sub_v) { return sub_v; }) |
        ranges::views::join | ranges::to_vector;
}
