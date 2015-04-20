#include <range/v3/size.hpp>
#include <array>

int main() {

  constexpr int ca1[4] = {1, 2, 3, 4};
  static_assert(ranges::size(ca1) == 4, "");

#ifdef RANGES_CXX_GREATER_THAN_11
  constexpr std::array<int, 4> a1{{1, 2, 3, 4}};
  static_assert(ranges::size(a1) == 4, "");

  static_assert(ranges::size(std::initializer_list<int>{1, 2, 3, 4}) == 4, "");
#endif

  return 0;
}
