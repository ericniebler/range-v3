// Range v3 library
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <vector>
#include <range/v3/view/any_view.hpp>
#include <range/v3/algorithm/for_each.hpp>

#if META_HAS_STATIC_RTTI
struct Foo {
    Foo() = default;
    Foo(Foo const&) = default;
    virtual ~Foo() = default;
    virtual void foo() = 0;
};

struct Bar : public Foo {
    virtual void foo() override {}
};
#endif

int main()
{
#if META_HAS_STATIC_RTTI
    std::vector<Bar> bars { Bar() };
    ranges::any_view<Foo &> foos = bars;
    ranges::for_each(foos, [] (Foo & foo) {
        foo.foo();
    });
#endif
}
