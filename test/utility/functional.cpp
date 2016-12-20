// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <range/v3/utility/functional.hpp>
#include <range/v3/view/filter.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

// GCC 4.8 is extremely confused about && and const&& qualifiers. Luckily they
// are rare - we'll simply break them.
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5 && __GNUC_MINOR__ < 9
#define GCC_4_8_WORKAROUND 1
#endif

namespace
{
    struct Integer
    {
        int i;
        operator int() const { return i; }
        bool odd() const { return (i % 2) != 0; }
    };

    enum class kind { lvalue, const_lvalue, rvalue, const_rvalue };

    std::ostream &operator<<(std::ostream &os, kind k)
    {
        const char* message = nullptr;
        switch (k) {
            case kind::lvalue:
                message = "lvalue";
                break;
            case kind::const_lvalue:
                message = "const_lvalue";
                break;
            case kind::rvalue:
                message = "rvalue";
                break;
            case kind::const_rvalue:
                message = "const_rvalue";
                break;
        }
        return os << message;
    }

    kind last_call;

    template <kind DisableKind>
    struct fn
    {
        bool operator()() &
        {
            last_call = kind::lvalue;
            return DisableKind != kind::lvalue;
        }
        bool operator()() const &
        {
            last_call = kind::const_lvalue;
            return DisableKind != kind::const_lvalue;
        }
        bool operator()() &&
        {
            last_call = kind::rvalue;
            return DisableKind != kind::rvalue;
        }
        bool operator()() const &&
        {
            last_call = kind::const_rvalue;
            return DisableKind != kind::const_rvalue;
        }
    };
} // unnamed namespace

int main()
{
    {
        // Check that not_fn works with callables
        Integer some_ints[] = {{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}};
        ::check_equal(some_ints | ranges::view::filter(ranges::not_fn(&Integer::odd)),
                      {0,2,4,6});
    }

    // Check that not_fn forwards value category
    {
        constexpr auto k = kind::lvalue;
        using F = fn<k>;
        auto f = ranges::not_fn(F{});
        CHECK(f() == true);
        CHECK(last_call == k);
    }
    {
        constexpr auto k = kind::const_lvalue;
        using F = fn<k>;
        auto const f = ranges::not_fn(F{});
        CHECK(f() == true);
        CHECK(last_call == k);
    }
    {
#ifdef GCC_4_8_WORKAROUND
        constexpr auto k = kind::lvalue;
#else
        constexpr auto k = kind::rvalue;
#endif
        using F = fn<k>;
        auto f = ranges::not_fn(F{});
        CHECK(std::move(f)() == true); // xvalue
        CHECK(last_call == k);

        CHECK(decltype(f){}() == true); // prvalue
        CHECK(last_call == k);
    }

    {
#ifdef GCC_4_8_WORKAROUND
        constexpr auto k = kind::const_lvalue;
#else
        constexpr auto k = kind::const_rvalue;
#endif
        using F = fn<k>;
        auto const f = ranges::not_fn(F{});
        CHECK(std::move(f)() == true); // xvalue
        CHECK(last_call == k);

        CHECK(decltype(f){}() == true); // prvalue
        CHECK(last_call == k);
    }

#ifdef _WIN32
    {
        // Ensure that Callable accepts pointers to functions with non-default calling conventions.
        CONCEPT_ASSERT(ranges::Callable<void(__cdecl*)()>());
        CONCEPT_ASSERT(ranges::Callable<void(__stdcall*)()>());
        CONCEPT_ASSERT(ranges::Callable<void(__fastcall*)()>());
        CONCEPT_ASSERT(ranges::Callable<void(__thiscall*)()>());
        CONCEPT_ASSERT(ranges::Callable<void(__vectorcall*)()>());
    }
#endif // _WIN32

    return ::test_result();
}
