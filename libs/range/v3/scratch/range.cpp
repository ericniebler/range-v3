// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#include <vector>
#include <iostream>
#include <range/v3/range.hpp>

struct S
{
    S() { std::cout << "S::S()\n"; }
    S(S &&) { std::cout << "S::S(S&&)\n"; }
    S(S const &) { std::cout << "S::S(S const &)\n"; }
    S &operator=(S &&) { std::cout << "S::operator=(S&&)\n"; return *this; }
    S &operator=(S const &) { std::cout << "S::operator=(S const &)\n"; return *this; }
    ~S() { std::cout << "S::~S()\n"; }
    friend std::ostream& operator<<(std::ostream& sout, S const &)
    {
        return sout << "S";
    }
};

struct disp
{
    template<typename T>
    void operator()(T const & t) const
    {
        std::cout << typeid(T).name() << " : " << t << '\n';
    }
};

struct my_visitor
{
    int& operator()(int& i) const
    {
        std::cout << "my_visitor::operator()(int) " << i << "\n";
        return i;
    }
    float operator()(float i) const
    {
        std::cout << "my_visitor::operator()(float) " << i << "\n";
        return i;
    }
    template<typename T>
    void operator()(T) const
    {
        std::cout << "my_visitor::operator()(T)\n";
    }
};

void test_tagged_variant()
{
    using namespace ranges;
    using x = typelist_unique_t<typelist<int, int, int>>;
    static_assert(std::is_same<x, typelist<int>>::value, "");

    using y = typelist_unique_t<typelist<int, short, int>>;
    static_assert(std::is_same<y, typelist<int, short>>::value, "");

    std::cout << "\nTesting tagged_variant\n";
    tagged_variant<int, char const *, short, char, S, wchar_t, long, float> v(size_t<4>{}, S{});

    auto v2 = std::move(v);

    v.apply(disp{});
    v.set<7>(3.14);
    v.apply(disp{});
    tagged_variant<int&, void_t, void_t, void_t, void_t, void_t, void_t, float> vv = v.apply(my_visitor{});
    RANGES_ASSERT(7 == vv.which());
    v.set<4>(S{});
    v.apply(disp{});
    v.set<0>(42);
    v.apply(disp{});
    vv = v.apply(my_visitor{});
    RANGES_ASSERT(0 == vv.which());
    RANGES_ASSERT((&get<0>(v) == &get<0>(vv)));
    auto const & cvv = vv;
    int & iref = get<0>(cvv);
    vv.set<7>(2.56);
    float const & f = get<7>(cvv);
}

int main()
{
    using namespace ranges;
    std::vector<int> v {1,2,3,4,5};

    for_each(v, [](int i){ std::cout << i << ' '; });
    std::cout << '\n';

    auto t = view::transform(v, [](int i){return i * i;});
    for_each(t, [](int i){ std::cout << i << ' '; });
    std::cout << '\n';
}
