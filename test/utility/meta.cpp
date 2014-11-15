#include <tuple>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/typelist.hpp>
#include "../simple_test.hpp"

using namespace ranges;

// An implementation of tuple_cat gives Range v3's meta-programming and typelist
// utilities a good workout. It's a good compiler stress test, too.

namespace __detail
{
    template<typename Ret, typename...Is, typename ...Ks, typename Tuples>
    Ret tuple_cat_(typelist<Is...>, typelist<Ks...>, Tuples tpls)
    {
        return Ret{std::get<Ks::value>(std::get<Is::value>(tpls))...};
    }
}

template<typename...Tuples,
    typename Res =
        typelist_apply_t<
            meta_quote<std::tuple>,
            typelist_cat_t<typelist<as_typelist_t<Tuples>...> > > >
Res tuple_cat(Tuples &&... tpls)
{
    static constexpr std::size_t N = sizeof...(Tuples);
    // E.g. [0,0,0,2,2,2,3,3]
    using inner =
        typelist_cat_t<
            typelist_transform_t<
                typelist<as_typelist_t<Tuples>...>,
                typelist_transform_t<
                    as_typelist_t<make_index_sequence<N> >,
                    meta_quote<meta_always> >,
                meta_quote<typelist_transform_t> > >;
    // E.g. [0,1,2,0,1,2,0,1]
    using outer =
        typelist_cat_t<
            typelist_transform_t<
                typelist<as_typelist_t<Tuples>...>,
                meta_compose<
                    meta_quote<as_typelist_t>,
                    meta_quote_i<std::size_t, make_index_sequence>,
                    meta_quote<typelist_size_t> > > >;
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
static_assert(std::is_same<typelist_reverse_t<typelist<int, short, double>>,
                                              typelist<double, short, int>>::value, "");

static_assert(typelist_all_of<typelist<int, short, long>, meta_quote<std::is_integral>>::value, "");
static_assert(typelist_none_of<typelist<int, short, long>, meta_quote<std::is_floating_point>>::value, "");
static_assert(!typelist_any_of<typelist<int, short, long>, meta_quote<std::is_floating_point>>::value, "");
static_assert(typelist_any_of<typelist<int, short, long, float>, meta_quote<std::is_floating_point>>::value, "");

static_assert(std::is_same<typelist_zip_t<typelist<typelist<int, short, double>, typelist<int*, short*, double*>>>,
                           typelist<typelist<int, int*>, typelist<short, short*>, typelist<double, double*>>>::value, "");
static_assert(std::is_same<typelist_zip_with_t<meta_quote<std::tuple>, typelist<typelist<int, short, double>, typelist<int*, short*, double*>>>,
                           typelist<std::tuple<int, int*>, std::tuple<short, short*>, std::tuple<double, double*>>>::value, "");


int main()
{
    test_tuple_cat();
    return ::test_result();
}
