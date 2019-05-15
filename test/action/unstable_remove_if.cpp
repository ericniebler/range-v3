// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>

#include <range/v3/action/unstable_remove_if.hpp>
#include <range/v3/action/remove_if.hpp>
#include <range/v3/action/sort.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"


void logic_test()
{
    using namespace ranges;

    using List = std::vector<int>;

    const auto make_list = []() -> List {
        return {1,2,3,4,5};
    };

    // empty
    {
        List list;
        list |= action::unstable_remove_if([](int){ return true; });
        CHECK(list.empty());
    }

    // all stay
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int){ return false; });
        check_equal(list, {1,2,3,4,5});
    }

    // all remove
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int){ return true; });
        CHECK(list.empty());
    }

    // remove one in the middle
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int i){ return i==2; });
        check_equal(list, {1,5,3,4});
    }

    // remove first
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int i){ return i==1; });
        check_equal(list, {5,2,3,4});
    }

    // remove last
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int i){ return i==5; });
        check_equal(list, {1,2,3,4});
    }

    // remove group in the middle
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int i){ return i==2 || i==3 || i==4; });
        check_equal(list, {1,5});
    }

    // remove group in the begin
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int i){ return i==1 || i==2 || i==3; });
        check_equal(list, {5,4});
    }

    // remove group in the end
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int i){ return i==3 || i==4 || i==5; });
        check_equal(list, {1,2});
    }

    // remains one in the middle
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int i){ return i!=3; });
        check_equal(list, {3});
    }
    // remains group in the middle
    {
        List list = make_list();
        list |= action::unstable_remove_if([](int i){ return (i!=3) && (i!=4); });
        check_equal(list, {4,3});
    }
}

class fuzzy_test_fn
{
    int size;
#if defined(__GLIBCXX__) && defined(RANGES_WORKAROUND_VALGRIND_RDRAND)
    std::random_device rd{"/dev/urandom"};
#else
    std::random_device rd;
#endif
    std::mt19937 eng{rd()};
    std::uniform_int_distribution<int> distr;

public:
    explicit fuzzy_test_fn(int size)
        : size(size)
        , distr{0, size}
    {}

    void operator()()
    {
        struct Int
        {
            int value;

            explicit Int(int value) : value(value) {}
            Int(const Int&) = default;
            Int(Int&& other) noexcept : value(0)
            {
                *this = std::move(other);
            }

            Int& operator=(const Int&) = default;
            Int& operator=(Int&& other) noexcept
            {
                const int sentinel = -1;
                CHECK(other.value != sentinel);

                value = other.value;
                other.value = sentinel;
                return *this;
            }

            RANGES_DIAGNOSTIC_PUSH
            RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER
            bool operator==(const Int& other) const
            {
                return value == other.value;
            }
            bool operator!=(const Int& other) const
            {
                return value != other.value;
            }
            bool operator<(const Int& other) const
            {
                return value < other.value;
            }
            bool operator>(const Int& other) const
            {
                return value > other.value;
            }
            bool operator<=(const Int& other) const
            {
                return value <= other.value;
            }
            bool operator>=(const Int& other) const
            {
                return value >= other.value;
            }
            RANGES_DIAGNOSTIC_POP
        };

        using namespace ranges;
        using List = std::vector<Int>;
        List ordered_list;
        List unordered_list;

        //fill
        for(int i=0;i<size;++i)
        {
            ordered_list.emplace_back(i);
            unordered_list.emplace_back(i);
        }

        //erase
        const int erase_count = distr(eng);
        for(int i=0; i<erase_count; ++i)
        {
            const int value = distr(eng);
            const auto pred = [value](Int i){ return i.value==value; };
            unordered_list |= action::unstable_remove_if(pred);
            ordered_list   |= action::remove_if(pred);
        }

        // compare
        unordered_list |= action::sort;
        CHECK(ordered_list == unordered_list);
    }
};



int main()
{
    logic_test();

    {
        const int size    = 100;
        const int repeats = 1000;
        fuzzy_test_fn fuzzy_test(size);
        for(int i=0; i<repeats; ++i) fuzzy_test();
    }

    return ::test_result();
}
