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

#include <range/v3/utility/is_constexpr_default_cosntructible.hpp>
#include <range/v3/detail/config.hpp>
#include "../simple_test.hpp"

using namespace ranges;

struct default_constructible_t
{
    default_constructible_t() = default;
};

struct empty_ctr_t
{
    empty_ctr_t(){}
};

struct non_default_constructible_t
{
    non_default_constructible_t(int) noexcept {}
};

struct constexpr_default_constructible_t
{
    constexpr constexpr_default_constructible_t() {}
};

struct constexpr_non_default_constructible_t
{
    constexpr constexpr_non_default_constructible_t(int) {}
};

#if __cplusplus >= RANGES_CXX_STD_14
int l = 100;
struct false_constexpr_default_constructible_t
{
    constexpr false_constexpr_default_constructible_t()
    {
        ++l;
    }
};
#endif

int main()
{
    CHECK( is_constexpr_default_cosntructible<default_constructible_t>::value );
    CHECK( !is_constexpr_default_cosntructible<empty_ctr_t>::value );
    CHECK( !is_constexpr_default_cosntructible<non_default_constructible_t>::value );
    CHECK( is_constexpr_default_cosntructible<constexpr_default_constructible_t>::value );
    CHECK( !is_constexpr_default_cosntructible<constexpr_non_default_constructible_t>::value );

    #if __cplusplus >= RANGES_CXX_STD_14
    CHECK( !is_constexpr_default_cosntructible<false_constexpr_default_constructible_t>::value );
    #endif

    return ::test_result();
}
