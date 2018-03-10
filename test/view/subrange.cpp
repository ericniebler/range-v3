// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <list>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/utility/unreachable.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

// https://bugs.llvm.org/show_bug.cgi?id=34776
RANGES_DIAGNOSTIC_IGNORE_UNDEFINED_FUNC_TEMPLATE

using namespace ranges;

template<typename Rng>
safe_subrange_t<Rng> algorithm(Rng &&rng);

int main()
{
    std::vector<int> vi{1,2,3,4};

    ////////////////////////////////////////////////////////////////////////////
    // safe_subrange_t tests:

    // lvalues are ReferenceableRanges and do not dangle:
    CONCEPT_ASSERT(Same<subrange<int*>,
        decltype(::algorithm(std::declval<int(&)[42]>()))>());
    CONCEPT_ASSERT(Same<subrange<std::vector<int>::iterator>,
        decltype(::algorithm(vi))>());

    // subrange and ref_view are ReferenceableRanges and do not dangle:
    CONCEPT_ASSERT(Same<subrange<int*>,
        decltype(::algorithm(std::declval<subrange<int*>>()))>());
    CONCEPT_ASSERT(Same<subrange<int*>,
        decltype(::algorithm(std::declval<ref_view<int[42]>>()))>());

    // non-ReferenceableRange rvalue ranges dangle:
    CONCEPT_ASSERT(Same<dangling<subrange<int*>>,
        decltype(::algorithm(std::declval<iterator_range<int*>>()))>());
    CONCEPT_ASSERT(Same<dangling<subrange<std::vector<int>::iterator>>,
        decltype(::algorithm(std::move(vi)))>());

    subrange<std::vector<int>::iterator> r0 {vi.begin(), vi.end()};
    CONCEPT_ASSERT(std::tuple_size<decltype(r0)>::value == 2);
    CONCEPT_ASSERT(Same<std::vector<int>::iterator,
        std::tuple_element<0, decltype(r0)>::type>());
    CONCEPT_ASSERT(Same<std::vector<int>::iterator,
        std::tuple_element<1, decltype(r0)>::type>());
    ::models<concepts::SizedView>(aux::copy(r0));
    CHECK(r0.size() == 4);
    CHECK(r0.begin() == vi.begin());
    CHECK(get<0>(r0) == vi.begin());
    CHECK(r0.end() == vi.end());
    CHECK(get<1>(r0) == vi.end());
    r0 = r0.next();
    CHECK(r0.size() == 3);

    {
        subrange<std::vector<int>::iterator> rng {vi.begin(), vi.end(), ranges::distance(vi)};
        CHECK(rng.size() == 4);
        CHECK(rng.begin() == vi.begin());
        CHECK(rng.end() == vi.end());
    }

    std::pair<std::vector<int>::iterator, std::vector<int>::iterator> p0 = r0;
    CHECK(p0.first == vi.begin()+1);
    CHECK(p0.second == vi.end());

    subrange<std::vector<int>::iterator, unreachable> r1 { r0.begin(), {} };
    CONCEPT_ASSERT(std::tuple_size<decltype(r1)>::value == 2);
    CONCEPT_ASSERT(Same<std::vector<int>::iterator,
        std::tuple_element<0, decltype(r1)>::type>());
    CONCEPT_ASSERT(Same<unreachable,
        std::tuple_element<1, decltype(r1)>::type>());
    ::models<concepts::View>(aux::copy(r1));
    ::models_not<concepts::SizedRange>(r1);
    CHECK(r1.begin() == vi.begin()+1);
    r1.end() = unreachable{};

    r0 = r0.next();
    ++r0.begin();
    CHECK(r0.begin() == vi.begin()+2);
    CHECK(r0.size() == 2);
    r0 = {r0.begin(), --r0.end()}; // --r0.end();
    CHECK(r0.end() == vi.end()-1);
    CHECK(r0.size() == 1);
    CHECK(r0.front() == 3);
    CHECK(r0.back() == 3);

    std::pair<std::vector<int>::iterator, unreachable> p1 = r1;
    CHECK(p1.first == vi.begin()+1);

    subrange<std::vector<int>::iterator, unreachable> r2 { p1 };
    CHECK(r1.begin() == vi.begin()+1);

    std::list<int> li{1,2,3,4};
    sized_subrange<std::list<int>::iterator> l0 {li.begin(), li.end(),
        static_cast<std::ptrdiff_t>(li.size())};
    ::models<concepts::SizedView>(aux::copy(l0));
    CHECK(l0.begin() == li.begin());
    CHECK(l0.end() == li.end());
    CHECK(l0.size() == static_cast<std::ptrdiff_t>(li.size()));
    l0 = l0.next();
    CHECK(l0.begin() == next(li.begin()));
    CHECK(l0.end() == li.end());
    CHECK(l0.size() == static_cast<std::ptrdiff_t>(li.size()) - 1);

    l0 = view::all(li);

    subrange<std::list<int>::iterator> l1 = l0;
    ::models_not<concepts::SizedRange>(l1);
    CHECK(l1.begin() == li.begin());
    CHECK(l1.end() == li.end());

    {
#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        {
            subrange s0{vi.begin(), vi.end()};
            subrange s1{li.begin(), li.end()};
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l1), decltype(s1)>());
        }
#endif
        {
            auto s0 = make_subrange(vi.begin(), vi.end());
            auto s1 = make_subrange(li.begin(), li.end());
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l1), decltype(s1)>());
        }
    }
    {
#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        {
            subrange s0{vi.begin(), vi.end(), ranges::distance(vi)};
            subrange s1{li.begin(), li.end(), ranges::distance(li)};
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
        }
#endif
        {
            auto s0 = make_subrange(vi.begin(), vi.end(), ranges::distance(vi));
            auto s1 = make_subrange(li.begin(), li.end(), ranges::distance(li));
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
        }
    }
    {
#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        {
            subrange s0{vi};
            subrange s1{li};
            subrange s2{view::all(vi)};
            subrange s3{view::all(li)};
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s2)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s3)>());
        }
#endif
        {
            auto s0 = make_subrange(vi);
            auto s1 = make_subrange(li);
            auto s2 = make_subrange(view::all(vi));
            auto s3 = make_subrange(view::all(li));
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s2)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s3)>());
        }
    }
    {
#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        {
            subrange s0{r0};
            subrange s1{l0};
            subrange s2{l1};
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(l1), decltype(s2)>());
        }
#endif
        {
            auto s0 = make_subrange(r0);
            auto s1 = make_subrange(l0);
            auto s2 = make_subrange(l1);
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(l1), decltype(s2)>());
        }
    }
    {
#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        {
            subrange s0{vi, ranges::distance(vi)};
            subrange s1{li, ranges::distance(li)};
            subrange s2{view::all(vi), ranges::distance(vi)};
            subrange s3{view::all(li), ranges::distance(li)};
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s2)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s3)>());
        }
#endif
        {
            auto s0 = make_subrange(vi, ranges::distance(vi));
            auto s1 = make_subrange(li, ranges::distance(li));
            auto s2 = make_subrange(view::all(vi), ranges::distance(vi));
            auto s3 = make_subrange(view::all(li), ranges::distance(li));
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s2)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s3)>());
        }
    }
    {
#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        {
            subrange s0{r0, size(r0)};
            subrange s1{l0, size(l0)};
            subrange s2{l1, size(l0)};
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s2)>());
        }
#endif
        {
            auto s0 = make_subrange(r0, size(r0));
            auto s1 = make_subrange(l0, size(l0));
            auto s2 = make_subrange(l1, size(l0));
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s2)>());
        }
    }
    {
#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        {
            subrange s0{p1};
            subrange s1{std::make_pair(vi.begin(), vi.end())};
            subrange s2{std::make_pair(li.begin(), li.end())};
            CONCEPT_ASSERT(Same<decltype(r2), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(l1), decltype(s2)>());
        }
#endif
        {
            auto s0 = make_subrange(p1);
            auto s1 = make_subrange(std::make_pair(vi.begin(), vi.end()));
            auto s2 = make_subrange(std::make_pair(li.begin(), li.end()));
            CONCEPT_ASSERT(Same<decltype(r2), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s1)>());
            CONCEPT_ASSERT(Same<decltype(l1), decltype(s2)>());
        }
    }
    {
#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        {
            subrange s0{std::make_pair(vi.begin(), vi.end()), distance(vi)};
            subrange s1{std::make_pair(li.begin(), li.end()), distance(li)};
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
        }
#endif
        {
            auto s0 = make_subrange(std::make_pair(vi.begin(), vi.end()), distance(vi));
            auto s1 = make_subrange(std::make_pair(li.begin(), li.end()), distance(li));
            CONCEPT_ASSERT(Same<decltype(r0), decltype(s0)>());
            CONCEPT_ASSERT(Same<decltype(l0), decltype(s1)>());
        }
    }

    return ::test_result();
}
