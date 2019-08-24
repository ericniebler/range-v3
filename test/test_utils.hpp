// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_TEST_UTILS_HPP
#define RANGES_TEST_UTILS_HPP

#include <algorithm>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <ostream>
#include <meta/meta.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/view/subrange.hpp>
#include "./debug_view.hpp"
#include "./simple_test.hpp"
#include "./test_iterators.hpp"

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_THIS_CAPTURE

CPP_def
(
    template(typename T, typename U)
    concept both_ranges,
        ranges::input_range<T> && ranges::input_range<U>
);

struct check_equal_fn
{
    CPP_template(typename T, typename U)(
        requires (!both_ranges<T, U>))
    void operator()(T &&actual, U &&expected) const
    {
        CHECK((T &&) actual == (U &&) expected);
    }

    CPP_template(typename Rng1, typename Rng2)(
        requires both_ranges<Rng1, Rng2>)
    void operator()(Rng1 &&actual, Rng2 &&expected) const
    {
        auto begin0 = ranges::begin(actual);
        auto end0 = ranges::end(actual);
        auto begin1 = ranges::begin(expected);
        auto end1 = ranges::end(expected);
        for(; begin0 != end0 && begin1 != end1; ++begin0, ++begin1)
            (*this)(*begin0, *begin1);
        CHECK(begin0 == end0);
        CHECK(begin1 == end1);
    }

    CPP_template(typename Rng, typename Val)(
        requires ranges::input_range<Rng>)
    void operator()(Rng &&actual, std::initializer_list<Val> && expected) const
    {
        (*this)(actual, expected);
    }
};

inline namespace function_objects
{
    RANGES_INLINE_VARIABLE(check_equal_fn, check_equal)
}

template<typename Expected, typename Actual>
void has_type(Actual &&)
{
    static_assert(std::is_same<Expected, Actual>::value, "Not the same");
}

template<typename T>
T & as_lvalue(T &&t)
{
    return t;
}

// A simple, light-weight, non-owning reference to a type-erased function.
template<typename Sig>
struct function_ref;

template<typename Ret, typename...Args>
struct function_ref<Ret(Args...)>
{
private:
    void const *data_{nullptr};
    Ret (*pfun_)(void const *, Args...){nullptr};
    template<typename Fun>
    static Ret apply_(void const *data, Args... args)
    {
        return (*static_cast<Fun const *>(data))(args...);
    }
public:
    function_ref() = default;
    template<typename T>
    function_ref(T const &t)
      : data_(&t)
      , pfun_(&apply_<T>)
    {}
    Ret operator()(Args... args) const
    {
        return (*pfun_)(data_, args...);
    }
};

template<typename T>
struct checker
{
private:
    std::function<void(function_ref<void(T)>)> algo_;
public:
    explicit checker(std::function<void(function_ref<void(T)>)> algo)
      : algo_(std::move(algo))
    {}
    void check(function_ref<void(T)> const & check) const
    {
        algo_(check);
    }
};

template<bool B, typename T>
meta::if_c<B, T, T const &> rvalue_if(T const &t)
{
    return t;
}

template<typename Algo, bool RvalueOK = false>
struct test_range_algo_1
{
private:
    Algo algo_;
public:
    explicit test_range_algo_1(Algo algo)
      : algo_(algo)
    {}
    template<typename I, typename...Rest>
    auto operator()(I first, I last, Rest &&... rest) const ->
        ::checker<decltype(algo_(first, last, rest...))>
    {
        using S = meta::_t<sentinel_type<I>>;
        using R = decltype(algo_(first, last, rest...));
        auto check_algo = [algo = algo_, first, last, rest...](function_ref<void(R)> const & check)
        {
            check(algo(first, last, rest...));
            check(algo(first, S{base(last)}, rest...));
            check(algo(::rvalue_if<RvalueOK>(ranges::make_subrange(first, last)), rest...));
            check(algo(::rvalue_if<RvalueOK>(ranges::make_subrange(first, S{base(last)})), rest...));
        };
        return ::checker<R>{check_algo};
    }
};

template<bool RvalueOK = false, typename Algo>
test_range_algo_1<Algo, RvalueOK> make_testable_1(Algo algo)
{
    return test_range_algo_1<Algo, RvalueOK>{algo};
}

template<typename Algo, bool RvalueOK1 = false, bool RvalueOK2 = false>
struct test_range_algo_2
{
private:
    Algo algo_;
public:
    explicit test_range_algo_2(Algo algo)
      : algo_(algo)
    {}
    template<typename I1, typename I2, typename...Rest>
    auto operator()(I1 begin1, I1 end1, I2 begin2, I2 end2, Rest &&... rest) const ->
        checker<decltype(algo_(begin1, end1, begin2, end2, rest...))>
    {
        using S1 = meta::_t<sentinel_type<I1>>;
        using S2 = meta::_t<sentinel_type<I2>>;
        using R = decltype(algo_(begin1, end1, begin2, end2, rest...));
        return checker<R>{[algo = algo_, begin1, end1, begin2, end2, rest...](function_ref<void(R)> const & check)
        {
            check(algo(begin1, end1, begin2, end2, rest...));
            check(algo(begin1, S1{base(end1)}, begin2, S2{base(end2)}, rest...));
            check(algo(::rvalue_if<RvalueOK1>(ranges::make_subrange(begin1, end1)),
                       ::rvalue_if<RvalueOK2>(ranges::make_subrange(begin2, end2)),
                       rest...));
            check(algo(::rvalue_if<RvalueOK1>(ranges::make_subrange(begin1, S1{base(end1)})),
                       ::rvalue_if<RvalueOK2>(ranges::make_subrange(begin2, S2{base(end2)})),
                       rest...));
        }};
    }
};

template<bool RvalueOK1 = false, bool RvalueOK2 = false, typename Algo>
test_range_algo_2<Algo, RvalueOK1, RvalueOK2> make_testable_2(Algo algo)
{
    return test_range_algo_2<Algo, RvalueOK1, RvalueOK2>{algo};
}

// a simple type to test move semantics
struct MoveOnlyString
{
    char const *sz_;

    MoveOnlyString(char const *sz = "")
      : sz_(sz)
    {}
    MoveOnlyString(MoveOnlyString &&that)
      : sz_(that.sz_)
    {
        that.sz_ = "";
    }
    MoveOnlyString(MoveOnlyString const &) = delete;
    MoveOnlyString &operator=(MoveOnlyString &&that)
    {
        sz_ = that.sz_;
        that.sz_ = "";
        return *this;
    }
    MoveOnlyString &operator=(MoveOnlyString const &) = delete;
    bool operator==(MoveOnlyString const &that) const
    {
        return 0 == std::strcmp(sz_, that.sz_);
    }
    bool operator<(const MoveOnlyString &that) const
    {
        return std::strcmp(sz_, that.sz_) < 0;
    }
    bool operator!=(MoveOnlyString const &that) const
    {
        return !(*this == that);
    }
    friend std::ostream & operator<< (std::ostream &sout, MoveOnlyString const &str)
    {
        return sout << '"' << str.sz_ << '"';
    }
};

RANGES_DIAGNOSTIC_POP

#endif
