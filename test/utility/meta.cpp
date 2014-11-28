#include <tuple>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/integer_sequence.hpp>
#include "../simple_test.hpp"

using namespace ranges;
using namespace meta;

// An implementation of tuple_cat gives Range v3's meta-programming and list
// utilities a good workout. It's a good compiler stress test, too.

namespace tc_detail
{
    template<typename Ret, typename...Is, typename ...Ks, typename Tuples>
    Ret tuple_cat_(list<Is...>, list<Ks...>, Tuples tpls)
    {
        return Ret{std::get<Ks::value>(std::get<Is::value>(tpls))...};
    }
}

template<typename...Tuples,
    typename Res = apply_list<quote<std::tuple>, list_cat<list<as_list<Tuples>...>>>>
Res tuple_cat(Tuples &&... tpls)
{
    static constexpr std::size_t N = sizeof...(Tuples);
    // E.g. [0,0,0,2,2,2,3,3]
    using inner =
        list_cat<
            transform<
                list<as_list<Tuples>...>,
                transform<as_list<make_index_sequence<N>>, quote<always>>,
                quote<transform>>>;
    // E.g. [0,1,2,0,1,2,0,1]
    using outer =
        list_cat<
            transform<
                list<as_list<Tuples>...>,
                compose<quote<as_list>, quote_i<std::size_t, make_index_sequence>, quote<size>>>>;
    return tc_detail::tuple_cat_<Res>(
        inner{},
        outer{},
        std::forward_as_tuple(std::forward<Tuples>(tpls)...));
}

void test_tuple_cat()
{
    std::tuple<int, short, long> t1;
    std::tuple<> t2;
    std::tuple<float, double, long double> t3;
    std::tuple<void*, char*> t4;

    auto x = ::tuple_cat(t1, t2, t3, t4);
    using expected_type = std::tuple<int, short, long, float, double, long double, void*, char*>;
    static_assert(std::is_same<decltype(x), expected_type>::value, "");
}

// Other misc tests
static_assert(std::is_same<reverse<list<int, short, double>>,
                                              list<double, short, int>>::value, "");

static_assert(all_of<list<int, short, long>, quote<std::is_integral>>::value, "");
static_assert(none_of<list<int, short, long>, quote<std::is_floating_point>>::value, "");
static_assert(!any_of<list<int, short, long>, quote<std::is_floating_point>>::value, "");
static_assert(any_of<list<int, short, long, float>, quote<std::is_floating_point>>::value, "");

static_assert(std::is_same<apply<uncurry<curry<quote_trait<id>>>, std::tuple<int, short, double>>, list<int, short, double>>::value, "");

int main()
{
    test_tuple_cat();
    return ::test_result();
}
