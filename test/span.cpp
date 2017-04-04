// Range v3 library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#include <range/v3/span.hpp>

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <range/v3/view/empty.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

using namespace ranges;

namespace
{
    using byte = unsigned char;

    struct BaseClass { };
    struct DerivedClass : BaseClass { };

    template<typename T, typename U>
    inline auto narrow_cast(U && u)
    RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
        static_cast<T>((U &&) u)
    )

    void test_default_constructor()
    {
        {
            span<int> s;
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int> cs;
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }

        {
            span<int, 0> s;
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int, 0> cs;
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            span<int, 1> s;
            CHECK(s.length() == 1);
            CHECK(s.data() == nullptr); // explains why it can't compile
#endif
        }

        {
            span<int> s{};
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int> cs{};
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }
    }

    void test_size_optimization()
    {
        {
            span<int> s;
            CHECK(sizeof(s) == sizeof(int*) + sizeof(std::ptrdiff_t));
        }

        {
            span<int, 0> s;
            CHECK(sizeof(s) == sizeof(int*));
        }
    }

    void test_from_nullptr_constructor()
    {
        {
            span<int> s = nullptr;
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int> cs = nullptr;
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }

        {
            span<int, 0> s = nullptr;
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int, 0> cs = nullptr;
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            span<int, 1> s = nullptr;
            CHECK(s.length() == 1);
            CHECK(s.data() == nullptr); // explains why it can't compile
#endif
        }

        {
            span<int> s{nullptr};
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int> cs{nullptr};
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }

        {
            span<int*> s{nullptr};
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int*> cs{nullptr};
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }
    }

    void test_from_nullptr_length_constructor()
    {
        {
            span<int> s{nullptr, 0};
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int> cs{nullptr, 0};
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }

        {
            span<int, 0> s{nullptr, 0};
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int, 0> cs{nullptr, 0};
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            span<int, 1> s{nullptr, 0};
            CHECK(s.length() == 1);
            CHECK(s.data() == nullptr); // explains why it can't compile
#endif
        }

        {
            span<int*> s{nullptr, 0};
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);

            span<const int*> cs{nullptr, 0};
            CHECK(cs.length() == 0);
            CHECK(cs.data() == nullptr);
        }
    }

    void test_from_pointer_length_constructor()
    {
        int arr[4] = {1, 2, 3, 4};

        {
            span<int> s{&arr[0], 2};
            CHECK(s.length() == 2);
            CHECK(s.data() == &arr[0]);
            CHECK(s[0] == 1);
            CHECK(s[1] == 2);
        }

        {
            span<int, 2> s{&arr[0], 2};
            CHECK(s.length() == 2);
            CHECK(s.data() == &arr[0]);
            CHECK(s[0] == 1);
            CHECK(s[1] == 2);
        }

        {
            int* p = nullptr;
            span<int> s{p, 0};
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);
        }
    }

    void test_from_pointer_pointer_constructor()
    {
        int arr[4] = {1, 2, 3, 4};

        {
            span<int> s{&arr[0], &arr[2]};
            CHECK(s.length() == 2);
            CHECK(s.data() == &arr[0]);
            CHECK(s[0] == 1);
            CHECK(s[1] == 2);
        }

        {
            span<int, 2> s{&arr[0], &arr[2]};
            CHECK(s.length() == 2);
            CHECK(s.data() == &arr[0]);
            CHECK(s[0] == 1);
            CHECK(s[1] == 2);
        }

        {
            span<int> s{&arr[0], &arr[0]};
            CHECK(s.length() == 0);
            CHECK(s.data() == &arr[0]);
        }

        {
            span<int, 0> s{&arr[0], &arr[0]};
            CHECK(s.length() == 0);
            CHECK(s.data() == &arr[0]);
        }

        {
            int* p = nullptr;
            span<int> s{p, p};
            CHECK(s.length() == 0);
            CHECK(s.data() == nullptr);
        }
    }

    void test_from_array_constructor()
    {
        int arr[5] = {1, 2, 3, 4, 5};

        {
            span<int> s{arr};
            CHECK(s.length() == 5);
            CHECK(s.data() == &arr[0]);
        }

        {
            span<int, 5> s{arr};
            CHECK(s.length() == 5);
            CHECK(s.data() == &arr[0]);
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            span<int, 6> s{arr};
#endif
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            span<int, 0> s{arr};
            CHECK(s.length() == 0);
            CHECK(s.data() == &arr[0]);
#endif
        }
    }

    void test_from_std_array_constructor()
    {
        std::array<int, 4> arr = {{1, 2, 3, 4}};

        {
            span<int> s{arr};
            CHECK(s.size() == narrow_cast<ptrdiff_t>(arr.size()));
            CHECK(s.data() == arr.data());

            span<const int> cs{arr};
            CHECK(cs.size() == narrow_cast<ptrdiff_t>(arr.size()));
            CHECK(cs.data() == arr.data());
        }

        {
            span<int, 4> s{arr};
            CHECK(s.size() == narrow_cast<ptrdiff_t>(arr.size()));
            CHECK(s.data() == arr.data());

            span<const int, 4> cs{arr};
            CHECK(cs.size() == narrow_cast<ptrdiff_t>(arr.size()));
            CHECK(cs.data() == arr.data());
        }

#ifdef CONFIRM_COMPILATION_ERRORS
        {
            span<int, 2> s{arr};
            CHECK(s.size() == 2);
            CHECK(s.data() == arr.data());

            span<const int, 2> cs{arr};
            CHECK(cs.size() == 2);
            CHECK(cs.data() == arr.data());
        }

        {
            span<int, 0> s{arr};
            CHECK(s.size() == 0);
            CHECK(s.data() == arr.data());

            span<const int, 0> cs{arr};
            CHECK(cs.size() == 0);
            CHECK(cs.data() == arr.data());
        }
#endif

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            span<int, 5> s{arr};
#endif
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            auto get_an_array = []() { return std::array<int, 4>{{1, 2, 3, 4}}; };
            auto take_a_span = [](span<int>) {};
            // try to take a temporary std::array
            take_a_span(get_an_array());
#endif
        }
    }

    void test_from_const_std_array_constructor()
    {
        const std::array<int, 4> arr = {{1, 2, 3, 4}};

        {
            span<const int> s{arr};
            CHECK(s.size() == narrow_cast<ptrdiff_t>(arr.size()));
            CHECK(s.data() == arr.data());
        }

        {
            span<const int, 4> s{arr};
            CHECK(s.size() == narrow_cast<ptrdiff_t>(arr.size()));
            CHECK(s.data() == arr.data());
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            span<const int, 5> s{arr};
#endif
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            auto get_an_array = []() -> const std::array<int, 4> { return {{1, 2, 3, 4}}; };
            auto take_a_span = [](span<const int>) {};
            // try to take a temporary std::array
            take_a_span(get_an_array());
#endif
        }
    }

    void test_from_container_constructor()
    {
        std::vector<int> v = {1, 2, 3};
        const std::vector<int> cv = v;

        {
            span<int> s{v};
            CHECK(s.size() == narrow_cast<std::ptrdiff_t>(v.size()));
            CHECK(s.data() == v.data());

            span<const int> cs{v};
            CHECK(cs.size() == narrow_cast<std::ptrdiff_t>(v.size()));
            CHECK(cs.data() == v.data());
        }

        std::string str = "hello";
        const std::string cstr = "hello";

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            span<char> s{str};
            CHECK(s.size() == narrow_cast<std::ptrdiff_t>(str.size()));
            CHECK(s.data() == str.data());
#endif
            span<const char> cs{str};
            CHECK(cs.size() == narrow_cast<std::ptrdiff_t>(str.size()));
            CHECK(cs.data() == str.data());
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            span<char> s{cstr};
#endif
            span<const char> cs{cstr};
            CHECK(cs.size() == narrow_cast<std::ptrdiff_t>(cstr.size()));
            CHECK(cs.data() == cstr.data());
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            auto get_temp_vector = []() -> std::vector<int> { return {}; };
            auto use_span = [](span<int>) {};
            use_span(get_temp_vector());
#endif
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            auto get_temp_string = []() -> std::string { return {}; };
            auto use_span = [](span<char>) {};
            use_span(get_temp_string());
#endif
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            auto get_temp_vector = []() -> const std::vector<int> { return {}; };
            auto use_span = [](span<const char>) {};
            use_span(get_temp_vector());
#endif
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            auto get_temp_string = []() -> const std::string { return {}; };
            auto use_span = [](span<const char>) {};
            use_span(get_temp_string());
#endif
        }

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            std::map<int, int> m;
            span<int> s{m};
#endif
        }
    }

    void test_from_convertible_span_constructor()
    {
        span<DerivedClass> avd;
#ifdef CONFIRM_COMPILATION_ERRORS
        span<BaseClass> avb = avd;
#endif
        span<const DerivedClass> avcd = avd;
        (void) avcd;
    }

    void test_copy_move_and_assignment()
    {
        span<int> s1;
        CHECK(s1.empty());

        int arr[] = {3, 4, 5};

        span<const int> s2 = arr;
        CHECK(s2.length() == 3);
        CHECK(s2.data() == &arr[0]);

        s2 = s1;
        CHECK(s2.empty());

        auto get_temp_span = [&]() -> span<int> { return {&arr[1], 2}; };
        auto use_span = [&](span<const int> s) { CHECK(s.length() == 2);
        CHECK(s.data() == &arr[1]); };
        use_span(get_temp_span());

        s1 = get_temp_span();
        CHECK(s1.length() == 2);
        CHECK(s1.data() == &arr[1]);
    }

    template<class Bounds>
    void fn(const Bounds&)
    {
        static_assert(Bounds::static_size == 60, "static bounds is wrong size");
    }

    void test_first()
    {
        int arr[5] = {1, 2, 3, 4, 5};

        {
            span<int, 5> av = arr;
            CHECK(av.first<2>().length() == 2);
            CHECK(av.first(2).length() == 2);
        }

        {
            span<int, 5> av = arr;
            CHECK(av.first<0>().length() == 0);
            CHECK(av.first(0).length() == 0);
        }

        {
            span<int, 5> av = arr;
            CHECK(av.first<5>().length() == 5);
            CHECK(av.first(5).length() == 5);
        }

        {
            span<int, 5> av = arr;
#ifdef CONFIRM_COMPILATION_ERRORS
            CHECK(av.first<6>().length() == 6);
            CHECK(av.first<-1>().length() == -1);
#endif
            (void)av;
        }

        {
            span<int, dynamic_extent> av;
            CHECK(av.first<0>().length() == 0);
            CHECK(av.first(0).length() == 0);
        }
    }

    void test_last()
    {
        int arr[5] = {1, 2, 3, 4, 5};

        {
            span<int, 5> av = arr;
            CHECK(av.last<2>().length() == 2);
            CHECK(av.last(2).length() == 2);
        }

        {
            span<int, 5> av = arr;
            CHECK(av.last<0>().length() == 0);
            CHECK(av.last(0).length() == 0);
        }

        {
            span<int, 5> av = arr;
            CHECK(av.last<5>().length() == 5);
            CHECK(av.last(5).length() == 5);
        }

        {
            span<int, 5> av = arr;
#ifdef CONFIRM_COMPILATION_ERRORS
            CHECK((av.last<6>().bounds() == static_bounds<6>()));
            CHECK(av.last<6>().length() == 6);
#endif
            (void)av;
        }

        {
            span<int, dynamic_extent> av;
            CHECK(av.last<0>().length() == 0);
            CHECK(av.last(0).length() == 0);
        }
    }

    void test_subspan()
    {
        int arr[5] = {1, 2, 3, 4, 5};

        {
            span<int, 5> av = arr;
            CHECK((av.subspan<2, 2>().length() == 2));
            CHECK(av.subspan(2, 2).length() == 2);
            CHECK(av.subspan(2, 3).length() == 3);
        }

        {
            span<int, 5> av = arr;
            CHECK((av.subspan<0, 0>().length() == 0));
            CHECK(av.subspan(0, 0).length() == 0);
        }

        {
            span<int, 5> av = arr;
            CHECK((av.subspan<0, 5>().length() == 5));
            CHECK(av.subspan(0, 5).length() == 5);
        }

        {
            span<int, 5> av = arr;
            CHECK((av.subspan<5, 0>().length() == 0));
            CHECK(av.subspan(5, 0).length() == 0);
        }

        {
            span<int, dynamic_extent> av;
            CHECK((av.subspan<0, 0>().length() == 0));
            CHECK(av.subspan(0, 0).length() == 0);
        }

        {
            span<int> av;
            CHECK(av.subspan(0).length() == 0);
        }

        {
            span<int> av = arr;
            CHECK(av.subspan(0).length() == 5);
            CHECK(av.subspan(1).length() == 4);
            CHECK(av.subspan(4).length() == 1);
            CHECK(av.subspan(5).length() == 0);
            auto av2 = av.subspan(1);
            for (int i = 0; i < 4; ++i) CHECK(av2[i] == i + 2);
        }

        {
            span<int, 5> av = arr;
            CHECK(av.subspan(0).length() == 5);
            CHECK(av.subspan(1).length() == 4);
            CHECK(av.subspan(4).length() == 1);
            CHECK(av.subspan(5).length() == 0);
            auto av2 = av.subspan(1);
            for (int i = 0; i < 4; ++i) CHECK(av2[i] == i + 2);
        }
    }

    void test_extent()
    {
        {
            span<int> s;
            CHECK(s.extent == dynamic_extent);
        }

        {
            span<int, 0> s;
            CHECK(s.extent == 0);
        }
    }

    void test_operator_function_call()
    {
        int arr[4] = {1, 2, 3, 4};

        {
            span<int> s = arr;
            CHECK(s(0) == 1);
        }
    }

    void test_comparison_operators()
    {
        {
            int arr[] = {2, 1}; // bigger

            span<int> s1 = nullptr;
            span<int> s2 = arr;

            CHECK(s1 != s2);
            CHECK(s2 != s1);
            CHECK(!(s1 == s2));
            CHECK(!(s2 == s1));
            CHECK(s1 < s2);
            CHECK(!(s2 < s1));
            CHECK(s1 <= s2);
            CHECK(!(s2 <= s1));
            CHECK(s2 > s1);
            CHECK(!(s1 > s2));
            CHECK(s2 >= s1);
            CHECK(!(s1 >= s2));
        }

        {
            int arr1[] = {1, 2};
            int arr2[] = {1, 2};
            span<int> s1 = arr1;
            span<int> s2 = arr2;

            CHECK(s1 == s2);
            CHECK(!(s1 != s2));
            CHECK(!(s1 < s2));
            CHECK(s1 <= s2);
            CHECK(!(s1 > s2));
            CHECK(s1 >= s2);
            CHECK(s2 == s1);
            CHECK(!(s2 != s1));
            CHECK(!(s2 < s1));
            CHECK(s2 <= s1);
            CHECK(!(s2 > s1));
            CHECK(s2 >= s1);
        }

        {
            int arr[] = {1, 2, 3};

            span<int> s1 = {&arr[0], 2}; // shorter
            span<int> s2 = arr; // longer

            CHECK(s1 != s2);
            CHECK(s2 != s1);
            CHECK(!(s1 == s2));
            CHECK(!(s2 == s1));
            CHECK(s1 < s2);
            CHECK(!(s2 < s1));
            CHECK(s1 <= s2);
            CHECK(!(s2 <= s1));
            CHECK(s2 > s1);
            CHECK(!(s1 > s2));
            CHECK(s2 >= s1);
            CHECK(!(s1 >= s2));
        }

        {
            int arr1[] = {1, 2}; // smaller
            int arr2[] = {2, 1}; // bigger

            span<int> s1 = arr1;
            span<int> s2 = arr2;

            CHECK(s1 != s2);
            CHECK(s2 != s1);
            CHECK(!(s1 == s2));
            CHECK(!(s2 == s1));
            CHECK(s1 < s2);
            CHECK(!(s2 < s1));
            CHECK(s1 <= s2);
            CHECK(!(s2 <= s1));
            CHECK(s2 > s1);
            CHECK(!(s1 > s2));
            CHECK(s2 >= s1);
            CHECK(!(s1 >= s2));
        }
    }

    void test_basics()
    {
        auto ptr = span<int>(new int[10], 10);
        std::fill(ptr.begin(), ptr.end(), 99);
        for (int num : ptr) {
            CHECK(num == 99);
        }

        delete[] ptr.data();
    }

    void overloaded_func(span<const int, dynamic_extent> exp, int expected_value)
    {
        for (auto val : exp) {
            CHECK(val == expected_value);
        }
    }

    void overloaded_func(span<const char, dynamic_extent>, char)
    {}

    void fixed_func(span<int, 4> exp, int expected_value)
    {
        for (auto val : exp) {
            CHECK(val == expected_value);
        }
    }

    void test_span_parameter_test()
    {
        auto data = new int[4];

        auto av = span<int>(data, 4);

        std::fill(av.begin(), av.end(), 34);

        int count = 0;
        std::for_each(av.rbegin(), av.rend(), [&](int val) { count += val; });
        CHECK(count == 34 * 4);
        overloaded_func(av, 34);

        fixed_func(av, 34);
        delete[] data;
        (void)static_cast<void(*)(span<const char>, char)>(&overloaded_func);
    }

    void test_empty_spans()
    {
        {
            span<int, 0> empty_av(nullptr);
            for (auto& v : empty_av) {
                (void) v;
                CHECK(false);
            }
        }

        {
            span<int> empty_av = {};
            for (auto& v : empty_av) {
                (void) v;
                CHECK(false);
            }
        }
    }

    void test_fixed_size_conversions()
    {
        int arr[] = {1, 2, 3, 4};

        // converting to an span from an equal size array is ok
        span<int, 4> av4 = arr;
        CHECK(av4.length() == 4);

        // converting to dynamic_extent a_v is always ok
        {
            span<int, dynamic_extent> av = av4;
            (void) av;
        }
        {
            span<int, dynamic_extent> av = arr;
            (void) av;
        }

// initialization or assignment to static span that REDUCES size is NOT ok
#ifdef CONFIRM_COMPILATION_ERRORS
        {
            span<int, 2> av2 = arr;
        }
        {
            span<int, 2> av2 = av4;
        }
#endif

        {
            span<int, dynamic_extent> av = arr;
            span<int, 2> av2 = av.first<2>();
            (void) av2;
        }

#ifdef CONFIRM_COMPILATION_ERRORS
        {
            span<int, dynamic_extent> av = arr;
            span<int, 2, 1> av2 = av.as_span(dim<2>(), dim<2>());
        }
#endif

        {
            span<int, dynamic_extent> av = arr;
#if 0
            span<int, 2, 1> av2 = as_span(av, dim<>(2), dim<>(2));
            auto workaround_macro = [&]() { return av2[{1, 0}] == 2; };
            CHECK(workaround_macro());
#else
            (void)av;
#endif
        }

        // but doing so explicitly is ok

        // you can convert statically
        {
            span<int, 2> av2 = {arr, 2};
            (void) av2;
        }
        {
            span<int, 1> av2 = av4.first<1>();
            (void) av2;
        }

        // ...or dynamically
        {
            // NB: implicit conversion to span<int,2> from span<int,dynamic_extent>
            span<int, 1> av2 = av4.first(1);
            (void) av2;
        }


#ifdef CONFIRM_COMPILATION_ERRORS
        // initialization or assignment to static span that requires size INCREASE is not ok.
        int arr2[2] = {1, 2};
        {
            span<int, 4> av4 = arr2;
        }
        {
            span<int, 2> av2 = arr2;
            span<int, 4> av4 = av2;
        }
#endif
    }

    void test_as_writeable_bytes()
    {
        int a[] = {1, 2, 3, 4};

        {
#ifdef CONFIRM_COMPILATION_ERRORS
            // you should not be able to get writeable bytes for const objects
            span<const int, dynamic_extent> av = a;
            auto wav = av.as_writeable_bytes();
#endif
        }

        {
            span<int, dynamic_extent> av;
            auto wav = as_writeable_bytes(av);
            CHECK(wav.length() == av.length());
            CHECK(wav.length() == 0);
            CHECK(wav.size_bytes() == 0);
        }

        {
            span<int, dynamic_extent> av = a;
            auto wav = as_writeable_bytes(av);
            CHECK(wav.data() == (byte*) &a[0]);
            CHECK(wav.length() == static_cast<int>(sizeof(a)));
        }
    }

    void test_iterator()
    {
        int a[] = {1, 2, 3, 4};

        {
            span<int, dynamic_extent> av = a;
            auto wav = as_writeable_bytes(av);
            for (auto& b : wav) {
                b = byte(0);
            }
            for (size_t i = 0; i < 4; ++i) {
                CHECK(a[i] == 0);
            }
        }

        {
            span<int, dynamic_extent> av = a;
            for (auto& n : av) {
                n = 1;
            }
            for (size_t i = 0; i < 4; ++i) {
                CHECK(a[i] == 1);
            }
        }
    }
}

int main()
{
    {
        int arr[5] = {0,1,2,3,4};
        CONCEPT_ASSERT(SizedRange<decltype(arr)>());
        CONCEPT_ASSERT(ContiguousRange<decltype(arr)>());
        CONCEPT_ASSERT(ContiguousRange<decltype((arr))>());
        CONCEPT_ASSERT(ContiguousRange<int[5]>());
        CONCEPT_ASSERT(ContiguousRange<int(&&)[5]>());
        CHECK(static_cast<int>(range_cardinality<decltype(arr)>::value) == 5);
    }

    CONCEPT_ASSERT(range_cardinality<span<int, 4>>::value == 4u);
    {
        span<int> s{};
        CHECK(ranges::data(s) == nullptr);
        CONCEPT_ASSERT(RandomAccessRange<span<int>>());
        CONCEPT_ASSERT(SizedRange<span<int>>());
        CONCEPT_ASSERT(View<span<int>>());
        CONCEPT_ASSERT(ContiguousRange<span<char>>());
        CONCEPT_ASSERT(ContiguousRange<span<std::string>>());
        CONCEPT_ASSERT(ContiguousRange<span<int>>());
        CONCEPT_ASSERT(View<span<int, 4>>());
        CONCEPT_ASSERT(ContiguousRange<span<int, 4>>());
    }

    {
        struct S {};
        S arr[] = {{}, {}};
        span<S> s{arr};
        span<S const>{arr};
        span<S const> cs = s;
        (void)cs;
    }

    {
        span<int const> s{ranges::view::empty<int>()};
        CHECK(s.empty());
    }

    test_default_constructor();
    test_size_optimization();
    test_from_nullptr_constructor();
    test_from_nullptr_length_constructor();
    test_from_pointer_length_constructor();
    test_from_pointer_pointer_constructor();
    test_from_array_constructor();
    test_from_std_array_constructor();
    test_from_const_std_array_constructor();
    test_from_container_constructor();
    test_from_convertible_span_constructor();
    test_copy_move_and_assignment();
    test_first();
    test_last();
    test_subspan();
    test_extent();
    test_operator_function_call();
    test_comparison_operators();
    test_basics();
    test_span_parameter_test();
    test_empty_spans();
    test_fixed_size_conversions();
    test_as_writeable_bytes();
    test_iterator();

    return ::test_result();
}
