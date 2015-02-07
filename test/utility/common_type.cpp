#include <utility>
#include <type_traits>
#include <range/v3/utility/common_type.hpp>

struct B {};
struct D : B {};

int main()
{
    using namespace ranges;
    using namespace detail;
    static_assert(std::is_same<common_reference_t<B &, D &>, B &>::value, "");
    static_assert(std::is_same<common_reference_t<B &, D const &>, B const &>::value, "");
    static_assert(std::is_same<common_reference_t<B const &, D &>, B const &>::value, "");

    static_assert(std::is_same<common_reference_t<B &&, D &&>, B &&>::value, "");
    static_assert(std::is_same<common_reference_t<B const &&, D &&>, B const &&>::value, "");
    static_assert(std::is_same<common_reference_t<B &&, D const &&>, B const &&>::value, "");

    static_assert(std::is_same<common_reference_t<B &, D &&>, B const &>::value, "");
    static_assert(std::is_same<common_reference_t<B &, D const &&>, B const &>::value, "");
    static_assert(std::is_same<common_reference_t<B const &, D &&>, B const &>::value, "");

    static_assert(std::is_same<common_reference_t<B &&, D &>, B const &>::value, "");
    static_assert(std::is_same<common_reference_t<B &&, D const &>, B const &>::value, "");
    static_assert(std::is_same<common_reference_t<B const &&, D &>, B const &>::value, "");

    static_assert(std::is_same<common_reference_t<int, short>, int>::value, "");
    static_assert(std::is_same<common_reference_t<int, short &>, int>::value, "");
    static_assert(std::is_same<common_reference_t<int &, short &>, int>::value, "");
    static_assert(std::is_same<common_reference_t<int &, short>, int>::value, "");
}
