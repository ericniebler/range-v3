#include <tuple>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/typelist.hpp>

using namespace ranges;

// An implementation of tuple_cat gives Range v3's meta-programming and typelist
// utilities a good workout. It's a good compiler stress test, too.

namespace __detail
{
    template<typename T>
    struct tuple_types
    {};

    template<typename...Ts>
    struct tuple_types<std::tuple<Ts...> >
    {
        using type = typelist<Ts...>;
    };

    template<typename Ret, typename...Is, typename ...Ks, typename Tuples>
    Ret tuple_cat_(typelist<Is...>, typelist<Ks...>, Tuples tpls)
    {
        return Ret{std::get<Ks::value>(std::get<Is::value>(tpls))...};
    }
}

template<typename T>
struct tuple_types
  : __detail::tuple_types<meta_quote_apply<std::decay, T> >
{};

template<typename T>
using tuple_types_t = meta_quote_apply<tuple_types, T>;

template<typename...Tuples,
    typename Res =
        typelist_expand_t<
            typelist_cat_t<tuple_types_t<Tuples>...>,
            std::tuple> >
Res tuple_cat(Tuples &&... tpls)
{
    static constexpr std::size_t N = sizeof...(Tuples);
    using inner =
        typelist_expand_t<
            typelist_transform2_t<
                typelist<tuple_types_t<Tuples>...>,
                typelist_transform_t<
                    as_typelist_t<make_index_sequence_t<N> >,
                    meta_quote_alias<meta_always> >,
                meta_quote<typelist_transform> >,
            typelist_cat_t>;
    using outer =
        typelist_expand_t<
            typelist_transform_t<
                typelist<tuple_types_t<Tuples>...>,
                meta_compose<
                    meta_quote<as_typelist>,
                    meta_quote_i<std::size_t, make_index_sequence>,
                    meta_quote<typelist_size> > >,
            typelist_cat_t>;
    return __detail::tuple_cat_<Res>(
        inner{},
        outer{},
        std::forward_as_tuple(std::forward<Tuples>(tpls)...));
}

int main()
{
    std::tuple<int, short, long> t1;
    std::tuple<> t2;
    std::tuple<float, double, long double> t3;
    std::tuple<void*, char*> t4;

    auto x = ::tuple_cat(t1, t2, t3, t4);
    using expected_type = std::tuple<int, short, long, float, double, long double, void*, char*>;
    static_assert(std::is_same<decltype(x), expected_type>::value, "");
}
