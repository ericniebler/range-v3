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
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <random>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/shuffle.hpp>
#include "../safe_int_swap.hpp"
#include <range/v3/numeric/iota.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template <unsigned long long a, unsigned long long c,
          unsigned long long m, unsigned long long Mp,
          bool _MightOverflow = (a != 0 && m != 0 && m-1 > (Mp-c)/a)>
struct lce_ta;

// 64

template <unsigned long long a, unsigned long long c, unsigned long long m>
struct lce_ta<a, c, m, (unsigned long long)(~0), true>
{
    typedef unsigned long long result_type;
    static RANGES_CXX14_CONSTEXPR result_type next(result_type x)
    {
        // Schrage's algorithm
        const result_type q = m / a;
        const result_type r = m % a;
        const result_type t0 = a * (x % q);
        const result_type t1 = r * (x / q);
        x = t0 + (t0 < t1) * m - t1;
        x += c - (x >= m - c) * m;
        return x;
    }
};

template <unsigned long long a, unsigned long long m>
struct lce_ta<a, 0, m, (unsigned long long)(~0), true>
{
    typedef unsigned long long result_type;

    static RANGES_CXX14_CONSTEXPR result_type next(result_type x)
    {
        // Schrage's algorithm
        const result_type q = m / a;
        const result_type r = m % a;
        const result_type t0 = a * (x % q);
        const result_type t1 = r * (x / q);
        x = t0 + (t0 < t1) * m - t1;
        return x;
    }
};

template <unsigned long long a, unsigned long long c, unsigned long long m>
struct lce_ta<a, c, m, (unsigned long long)(~0), false>
{
    typedef unsigned long long result_type;

    static RANGES_CXX14_CONSTEXPR result_type next(result_type x)
    {
        return (a * x + c) % m;
    }
};

template <unsigned long long a, unsigned long long c>
struct lce_ta<a, c, 0, (unsigned long long)(~0), false>
{
    typedef unsigned long long result_type;

    static RANGES_CXX14_CONSTEXPR result_type next(result_type x)
    {
        return a * x + c;
    }
};

// 32

template <unsigned long long Ap, unsigned long long Cp, unsigned long long Mp>
struct lce_ta<Ap, Cp, Mp, unsigned(~0), true>
{
    typedef unsigned result_type;

    static RANGES_CXX14_CONSTEXPR result_type next(result_type x)
    {
        const result_type a = static_cast<result_type>(Ap);
        const result_type c = static_cast<result_type>(Cp);
        const result_type m = static_cast<result_type>(Mp);
        // Schrage's algorithm
        const result_type q = m / a;
        const result_type r = m % a;
        const result_type t0 = a * (x % q);
        const result_type t1 = r * (x / q);
        x = t0 + (t0 < t1) * m - t1;
        x += c - (x >= m - c) * m;
        return x;
    }
};

template <unsigned long long Ap, unsigned long long Mp>
struct lce_ta<Ap, 0, Mp, unsigned(~0), true>
{
    typedef unsigned result_type;

    static RANGES_CXX14_CONSTEXPR result_type next(result_type x)
    {
        const result_type a = static_cast<result_type>(Ap);
        const result_type m = static_cast<result_type>(Mp);
        // Schrage's algorithm
        const result_type q = m / a;
        const result_type r = m % a;
        const result_type t0 = a * (x % q);
        const result_type t1 = r * (x / q);
        x = t0 + (t0 < t1) * m - t1;
        return x;
    }
};

template <unsigned long long Ap, unsigned long long Cp, unsigned long long Mp>
struct lce_ta<Ap, Cp, Mp, unsigned(~0), false>
{
    typedef unsigned result_type;

    static RANGES_CXX14_CONSTEXPR  result_type next(result_type x)
    {
        const result_type a = static_cast<result_type>(Ap);
        const result_type c = static_cast<result_type>(Cp);
        const result_type m = static_cast<result_type>(Mp);
        return (a * x + c) % m;
    }
};

template <unsigned long long Ap, unsigned long long Cp>
struct lce_ta<Ap, Cp, 0, unsigned(~0), false>
{
    typedef unsigned result_type;

    static RANGES_CXX14_CONSTEXPR result_type next(result_type x)
    {
        const result_type a = static_cast<result_type>(Ap);
        const result_type c = static_cast<result_type>(Cp);
        return a * x + c;
    }
};

// 16

template <unsigned long long a, unsigned long long c, unsigned long long m, bool b>
struct lce_ta<a, c, m, (unsigned short)(~0), b>
{
    typedef unsigned short result_type;

    static RANGES_CXX14_CONSTEXPR result_type next(result_type x)
    {
        return static_cast<result_type>(lce_ta<a, c, m, unsigned(~0)>::next(x));
    }
};


template <class Sseq, class Engine>
struct is_seed_sequence
{
        static constexpr const bool value =
                        !std::is_convertible<Sseq, typename Engine::result_type>::value &&
                        !std::is_same<typename std::remove_cv<Sseq>::type, Engine>::value;
};

template <class UInt, UInt a, UInt c, UInt m>
class linear_congruential_engine
{
public:
    // types
    typedef UInt result_type;

private:
    result_type x_;

    static constexpr const result_type Mp = result_type(~0);

    static_assert(m == 0 || a < m, "linear_congruential_engine invalid parameters");
    static_assert(m == 0 || c < m, "linear_congruential_engine invalid parameters");
public:
    static constexpr const result_type Min = c == 0u ? 1u: 0u;
    static constexpr const result_type Max = m - 1u;
    static_assert(Min < Max,           "linear_congruential_engine invalid parameters");

    // engine characteristics
    static constexpr const result_type multiplier = a;
    static constexpr const result_type increment = c;
    static constexpr const result_type modulus = m;

    static constexpr result_type min() {return Min;}

    static constexpr result_type max() {return Max;}
    static constexpr const result_type default_seed = 1u;

    // constructors and seeding functions
    RANGES_CXX14_CONSTEXPR
    explicit linear_congruential_engine(result_type s = default_seed) : x_(0)
    { seed(s); }

    template<class Sseq>
    RANGES_CXX14_CONSTEXPR
    explicit linear_congruential_engine(Sseq& q,
    typename std::enable_if<is_seed_sequence<Sseq, linear_congruential_engine>::value>::type* = 0)
    {seed(q);}

    RANGES_CXX14_CONSTEXPR
    void seed(result_type s = default_seed)
        {seed(std::integral_constant<bool, m == 0>(),
              std::integral_constant<bool, c == 0>(), s);}
    template<class Sseq>
    typename std::enable_if
    <
        is_seed_sequence<Sseq, linear_congruential_engine>::value,
                         void
    >::type
    RANGES_CXX14_CONSTEXPR
    seed(Sseq& q)
    {seed(q, std::integral_constant<unsigned,
                1 + (m == 0 ? (sizeof(result_type) * CHAR_BIT - 1)/32
                             :  (m > 0x100000000ull))>());}

    RANGES_CXX14_CONSTEXPR
    result_type operator()()
        {return x_ = static_cast<result_type>(lce_ta<a, c, m, Mp>::next(x_));}

    RANGES_CXX14_CONSTEXPR
    void discard(unsigned long long __z) {for (; __z; --__z) operator()();}

    friend RANGES_CXX14_CONSTEXPR
    bool operator==(const linear_congruential_engine& x,
                    const linear_congruential_engine& __y)
        {return x.x_ == __y.x_;}
    friend RANGES_CXX14_CONSTEXPR
    bool operator!=(const linear_congruential_engine& x,
                    const linear_congruential_engine& __y)
        {return !(x == __y);}

private:

    RANGES_CXX14_CONSTEXPR
    void seed(std::true_type, std::true_type, result_type s) {x_ = s == 0 ? 1 : s;}
    RANGES_CXX14_CONSTEXPR
    void seed(std::true_type, std::false_type, result_type s) {x_ = s;}
    RANGES_CXX14_CONSTEXPR
    void seed(std::false_type, std::true_type, result_type s) {x_ = s % m == 0 ?
                                                                 1 : s % m;}
    RANGES_CXX14_CONSTEXPR
    void seed(std::false_type, std::false_type, result_type s) {x_ = s % m;}

    template<class Sseq>
    RANGES_CXX14_CONSTEXPR
    void seed(Sseq& q, std::integral_constant<unsigned, 1>);
    template<class Sseq>
    RANGES_CXX14_CONSTEXPR
    void seed(Sseq& q, std::integral_constant<unsigned, 2>);
};

template <class UInt, UInt a, UInt c, UInt m>
constexpr const typename linear_congruential_engine<UInt, a, c, m>::result_type
linear_congruential_engine<UInt, a, c, m>::multiplier;

template <class UInt, UInt a, UInt c, UInt m>
constexpr const typename linear_congruential_engine<UInt, a, c, m>::result_type
linear_congruential_engine<UInt, a, c, m>::increment;

template <class UInt, UInt a, UInt c, UInt m>
constexpr const typename linear_congruential_engine<UInt, a, c, m>::result_type
linear_congruential_engine<UInt, a, c, m>::modulus;

template <class UInt, UInt a, UInt c, UInt m>
constexpr const typename linear_congruential_engine<UInt, a, c, m>::result_type
linear_congruential_engine<UInt, a, c, m>::default_seed;

template <class UInt, UInt a, UInt c, UInt m>
template<class Sseq>
RANGES_CXX14_CONSTEXPR
void
linear_congruential_engine<UInt, a, c, m>::seed(Sseq& q,
                                                 std::integral_constant<unsigned, 1>)
{
    const unsigned k = 1;
    uint32_t ar[k+3] = {0};
    q.generate(ar, ar + k + 3);
    result_type s = static_cast<result_type>(ar[3] % m);
    x_ = c == 0 && s == 0 ? result_type(1) : s;
}

template <class UInt, UInt a, UInt c, UInt m>
template<class Sseq>
RANGES_CXX14_CONSTEXPR
void
linear_congruential_engine<UInt, a, c, m>::seed(Sseq& q,
                                                 std::integral_constant<unsigned, 2>)
{
    const unsigned k = 2;
    uint32_t ar[k+3] = {0};
    q.generate(ar, ar + k + 3);
    result_type s = static_cast<result_type>((ar[3] +
                                              ((uint64_t)ar[4] << 32)) % m);
    x_ = c == 0 && s == 0 ? result_type(1) : s;
}

typedef linear_congruential_engine<uint_fast32_t, 48271, 0, 2147483647>
                                                                    minstd_rand;

#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_CXX14_CONSTEXPR bool test_constexpr()
{
        using namespace ranges;
        safe_int<int> ia[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        safe_int<int> ia1[] = {2, 7, 1, 4, 3, 6, 5, 10, 9, 8};
        safe_int<int> ia2[] = {1, 8, 3, 4, 6, 9, 5, 7, 2, 10};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        minstd_rand g;
        shuffle(ia, ia+sa, g);
        if(!equal(ia, ia+sa, ia1)) { return false; }
        shuffle(ia, ia+sa, g);
        if(!equal(ia, ia+sa, ia2)) { return false; }
        return true;
}
#endif

int main()
{
    {
        int ia[100];
        int ib[100];
        int orig[100];
        ranges::iota(ia, 0);
        ranges::iota(ib, 0);
        ranges::iota(orig, 0);
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        std::minstd_rand g;
        ranges::shuffle(random_access_iterator<int*>(ia), sentinel<int*>(ia+sa), g);
        CHECK(!ranges::equal(ia, orig));
        CHECK(ranges::shuffle(ib, ib+sa, g) == ib+sa);
        CHECK(!ranges::equal(ia, ib));
    }

    {
        int ia[100];
        int ib[100];
        int orig[100];
        ranges::iota(ia, 0);
        ranges::iota(ib, 0);
        ranges::iota(orig, 0);
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        std::minstd_rand g;
        auto rng = ranges::make_range(random_access_iterator<int*>(ia), sentinel<int*>(ia+sa));
        ranges::shuffle(rng, g);
        CHECK(!ranges::equal(ia, orig));
        CHECK(ranges::shuffle(ib, g) == ranges::end(ib));
        CHECK(!ranges::equal(ia, ib));

        ranges::iota(ia, 0);
        CHECK(ranges::shuffle(std::move(rng), g).get_unsafe().base() == ranges::end(ia));
        CHECK(!ranges::equal(ia, orig));
    }

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(test_constexpr(), "");
    }
#endif

    return ::test_result();
}
