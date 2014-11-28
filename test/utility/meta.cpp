#include <tuple>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/integer_sequence.hpp>
#include "../simple_test.hpp"

using namespace ranges;

// An implementation of tuple_cat gives Range v3's meta-programming and meta::list
// utilities a good workout. It's a good compiler stress test, too.

namespace __detail
{
    template<typename Ret, typename...Is, typename ...Ks, typename Tuples>
    Ret tuple_cat_(meta::list<Is...>, meta::list<Ks...>, Tuples tpls)
    {
        return Ret{std::get<Ks::value>(std::get<Is::value>(tpls))...};
    }
}

template<typename...Tuples,
    typename Res =
        meta::apply_list<
            meta::quote<std::tuple>,
            meta::list_cat<meta::list<meta::as_list<Tuples>...> > > >
Res tuple_cat(Tuples &&... tpls)
{
    static constexpr std::size_t N = sizeof...(Tuples);
    // E.g. [0,0,0,2,2,2,3,3]
    using inner =
        meta::list_cat<
            meta::transform<
                meta::list<meta::as_list<Tuples>...>,
                meta::transform<
                    meta::as_list<make_index_sequence<N> >,
                    meta::quote<meta::always> >,
                meta::quote<meta::transform> > >;
    // E.g. [0,1,2,0,1,2,0,1]
    using outer =
        meta::list_cat<
            meta::transform<
                meta::list<meta::as_list<Tuples>...>,
                meta::compose<
                    meta::quote<meta::as_list>,
                    meta::quote_i<std::size_t, make_index_sequence>,
                    meta::quote<meta::size> > > >;
    return __detail::tuple_cat_<Res>(
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
static_assert(std::is_same<meta::reverse<meta::list<int, short, double>>,
                                              meta::list<double, short, int>>::value, "");

static_assert(meta::all_of<meta::list<int, short, long>, meta::quote<std::is_integral>>::value, "");
static_assert(meta::none_of<meta::list<int, short, long>, meta::quote<std::is_floating_point>>::value, "");
static_assert(!meta::any_of<meta::list<int, short, long>, meta::quote<std::is_floating_point>>::value, "");
static_assert(meta::any_of<meta::list<int, short, long, float>, meta::quote<std::is_floating_point>>::value, "");

static_assert(std::is_same<meta::apply<meta::uncurry<meta::curry<meta::quote_trait<meta::id>>>, std::tuple<int, short, double>>, meta::list<int, short, double>>::value, "");

int main()
{
    test_tuple_cat();
    return ::test_result();
}
