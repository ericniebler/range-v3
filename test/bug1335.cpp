#include <vector>
#include <range/v3/action/sort.hpp>

template <class A, class B>
constexpr auto operator-(A a, B) {
  return a;
}

int main() {
  std::vector<int> data;
  data |= ranges::actions::sort;
}
