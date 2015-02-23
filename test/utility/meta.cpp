// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Acknowledgements: Thanks for Paul Fultz for the suggestions that
//                   concepts can be ordinary Boolean metafunctions.
//
// Project home: https://github.com/ericniebler/range-v3
//
#include <tuple>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/integer_sequence.hpp>
#include <range/v3/range_fwd.hpp>
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
    typename Res = apply_list<quote<std::tuple>, concat<as_list<Tuples>...>>>
Res tuple_cat(Tuples &&... tpls)
{
    static constexpr std::size_t N = sizeof...(Tuples);
    // E.g. [0,0,0,2,2,2,3,3]
    using inner =
        join<
            transform<
                list<as_list<Tuples>...>,
                transform<as_list<make_index_sequence<N>>, quote<always>>,
                quote<transform>>>;
    // E.g. [0,1,2,0,1,2,0,1]
    using outer =
        join<
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

template<typename, typename, typename = void>
struct can_apply_
  : std::false_type
{};

template<typename F, typename...As>
struct can_apply_<F, meta::list<As...>, meta::void_<meta::apply<F, As...>>>
  : std::true_type
{};

template<typename F, typename...As>
struct can_apply
  : can_apply_<F, meta::list<As...>>
{};

static_assert(can_apply<meta::quote<std::pair>, int, int>::value, "");
// I'm guessing this failure is due to GCC #64970
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64970
#if !defined(__GNUC__) || defined(__clang__)
static_assert(!can_apply<meta::quote<std::pair>, int, int, int>::value, "");
#endif

// Sanity-check meta::lambda
using Lambda0 = lambda<_a, _b, std::pair<_a, _b>>;
using Lambda1 = lambda<_a, _b, std::pair<_b, _a>>;
using Lambda2 = lambda<_a, _b, std::pair<_b, std::pair<_a, _a>>>;
using Pair0 = apply<Lambda0, int, short>;
using Pair1 = apply<Lambda1, int, short>;
using Pair2 = apply<Lambda2, int, short>;
static_assert(std::is_same<Pair0, std::pair<int, short>>::value, "");
static_assert(std::is_same<Pair1, std::pair<short, int>>::value, "");
static_assert(std::is_same<Pair2, std::pair<short, std::pair<int, int>>>::value, "");

// Not saying you should do it this way, but it's a good test.
namespace l = meta::lazy;
template<class L>
using cart_prod=foldr<L,list<list<>>,
    lambda<_a,_b,l::join<l::transform<_b,
        lambda<_c,l::join<l::transform<_a,
            lambda<_d,list<l::push_front<_d,_c>>>>>>>>>>;

using CartProd = cart_prod<meta::list<meta::list<int, short>, meta::list<float, double>>>;
static_assert(std::is_same<CartProd,
    meta::list<
        meta::list<int, float>,
        meta::list<int, double>,
        meta::list<short, float>,
        meta::list<short, double> >
>::value, "");

template<typename List>
using rev = foldr<List, list<>, lambda<_a, _b, defer<push_back, _a, _b> > >;
static_assert(std::is_same<rev<list<int, short, double>>,
                           list<double, short, int>>::value, "");

int main()
{
    test_tuple_cat();
    return ::test_result();
}
