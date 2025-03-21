#if defined(_MSC_VER)
// Enable level 4 "unreachable code" warning
#pragma warning(error : 4702)
#elif defined(__clang__)
#pragma clang diagnostic error "-Wunreachable-code-aggressive"
#elif defined(__GNUC__)
// Unsupported and ignored in gcc >= 5
#pragma GCC diagnostic error "-Wunreachable-code"
#endif

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/generate_n.hpp>

int main()
{
    ranges::views::generate_n(rand, 5) | ranges::to<std::vector>();
}
