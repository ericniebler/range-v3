#include <utility>
#include <type_traits>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/common_tuple.hpp>

struct B {};
struct D : B {};

struct noncopyable
{
    noncopyable() = default;
    noncopyable(noncopyable const &) = delete;
    noncopyable(noncopyable &&) = default;
    noncopyable &operator=(noncopyable const &) = delete;
    noncopyable &operator=(noncopyable &&) = default;
};

int main()
{
    using namespace ranges;
    using namespace detail;
    static_assert(std::is_same<common_reference_t<B &, D &>, B &>::value, "");
    static_assert(std::is_same<common_reference_t<B &, D const &>, B const &>::value, "");
    static_assert(std::is_same<common_reference_t<B &, D const &, D &>, B const &>::value, "");
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

    // Some tests for common_pair with common_reference
    static_assert(std::is_same<
        common_reference_t<std::pair<int &, int &>, common_pair<int,int> const &>,
        common_pair<int const &, int const &>
    >::value, "");

    static_assert(std::is_same<
        common_reference_t<common_pair<int const &, int const &>, std::pair<int, int>>,
        std::pair<int, int>
    >::value, "");

    static_assert(std::is_same<
        detail::builtin_common_t<common_pair<int, int> const &, std::pair<int, int> &>,
        std::pair<int, int> const &
    >::value, "");

    static_assert(std::is_same<
        detail::builtin_common_t<noncopyable const &, noncopyable>,
        noncopyable
    >::value, "");
}
