// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// These tests of reverse_iterator have been adapted from libc++
// (http://libcxx.llvm.org).
//
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <range/v3/begin_end.hpp>
#include <range/v3/view/iota.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

template<class It> void test() { ranges::reverse_iterator<It>{}; }

template<class It> void test2(It i) {
  ranges::reverse_iterator<It> r(i);
  CHECK(r.base() == i);
}

template<class It, class U> void test3(U u) {
  const ranges::reverse_iterator<U> r2(u);
  ranges::reverse_iterator<It> r1 = r2;
  CHECK(r1.base() == u);
}

struct Base {};
struct Derived : Base {};

template<class It> void test4(It i) {
  const ranges::reverse_iterator<It> r = ranges::make_reverse_iterator(i);
  CHECK(r.base() == i);
}

template<class It> void test5(It l, It r, bool x) {
  const ranges::reverse_iterator<It> r1(l);
  const ranges::reverse_iterator<It> r2(r);
  CHECK((r1 != r2) == x);
}

template<class It> void test6(It i, It x) {
  ranges::reverse_iterator<It> r(i);
  ranges::reverse_iterator<It> rr = r++;
  CHECK(r.base() == x);
  CHECK(rr.base() == i);
}

template<class It> void test7(It i, It x) {
  ranges::reverse_iterator<It> r(i);
  ranges::reverse_iterator<It> &rr = ++r;
  CHECK(r.base() == x);
  CHECK(&rr == &r);
}

template<class It>
void test8(It i, ranges::difference_type_t<It> n, It x) {
  const ranges::reverse_iterator<It> r(i);
  ranges::reverse_iterator<It> rr = r + n;
  CHECK(rr.base() == x);
}

template<class It>
void test9(It i, ranges::difference_type_t<It> n, It x) {
  ranges::reverse_iterator<It> r(i);
  ranges::reverse_iterator<It> &rr = r += n;
  CHECK(r.base() == x);
  CHECK(&rr == &r);
}

template<class It> void test10(It i, It x) {
  ranges::reverse_iterator<It> r(i);
  ranges::reverse_iterator<It> rr = r--;
  CHECK(r.base() == x);
  CHECK(rr.base() == i);
}
template<class It> void test11(It i, It x) {
  ranges::reverse_iterator<It> r(i);
  ranges::reverse_iterator<It> &rr = --r;
  CHECK(r.base() == x);
  CHECK(&rr == &r);
}
template<class It>
void test12(It i, ranges::difference_type_t<It> n, It x) {
  const ranges::reverse_iterator<It> r(i);
  ranges::reverse_iterator<It> rr = r - n;
  CHECK(rr.base() == x);
}

template<class It>
void test13(It i, ranges::difference_type_t<It> n, It x) {
  ranges::reverse_iterator<It> r(i);
  ranges::reverse_iterator<It> &rr = r -= n;
  CHECK(r.base() == x);
  CHECK(&rr == &r);
}

class A {
  int data_ = 1;

public:
  A() = default;

  friend bool operator==(const A &x, const A &y) { return x.data_ == y.data_; }
};

template<class It> void test14(It i, ranges::value_type_t<It> x) {
  ranges::reverse_iterator<It> r(i);
  CHECK(*r == x);
}

template<class It, class U> void test15(U u) {
  const ranges::reverse_iterator<U> r2(u);
  ranges::reverse_iterator<It> r1;
  ranges::reverse_iterator<It> &rr = r1 = r2;
  CHECK(r1.base() == u);
  CHECK(&rr == &r1);
}
template<class It> void test16(It l, It r, bool x) {
  const ranges::reverse_iterator<It> r1(l);
  const ranges::reverse_iterator<It> r2(r);
  CHECK((r1 == r2) == x);
}

template<class It1, class It2> void test17(It1 l, It2 r, std::ptrdiff_t x) {
   const ranges::reverse_iterator<It1> r1(l);
   const ranges::reverse_iterator<It2> r2(r);
   CHECK((r1 - r2) == x);
}

template<class It> void test18(It l, It r, bool x) {
  const ranges::reverse_iterator<It> r1(l);
  const ranges::reverse_iterator<It> r2(r);
  CHECK((r1 > r2) == x);
}

template<class It> void test19(It l, It r, bool x) {
  const ranges::reverse_iterator<It> r1(l);
  const ranges::reverse_iterator<It> r2(r);
  CHECK((r1 >= r2) == x);
}

template<class It>
void test20(It i, ranges::difference_type_t<It> n,
            ranges::value_type_t<It> x) {
  const ranges::reverse_iterator<It> r(i);
  ranges::value_type_t<It> rr = r[n];
  CHECK(rr == x);
}

template<class It> void test21(It l, It r, bool x) {
  const ranges::reverse_iterator<It> r1(l);
  const ranges::reverse_iterator<It> r2(r);
  CHECK((r1 < r2) == x);
}

template<class It>
void
test22(It l, It r, bool x)
{
    const ranges::reverse_iterator<It> r1(l);
    const ranges::reverse_iterator<It> r2(r);
    CHECK((r1 < r2) == x);
}

template<class It>
void
test23(It l, It r, bool x)
{
    const ranges::reverse_iterator<It> r1(l);
    const ranges::reverse_iterator<It> r2(r);
    CHECK((r1 <= r2) == x);
}


class B
{
    int data_ = 1;
  public:
    B() = default;

    int get() const {return data_;}

    friend bool operator==(const B& x, const B& y)
    {return x.data_ == y.data_;}
};

template<class It>
void
test24(It i, ranges::value_type_t<It> x)
{
    ranges::reverse_iterator<It> r(i);
    CHECK((*r).get() == x.get());
}


class C
{
    int data_ = 1;
  public:
    C(int d) : data_(d) {}
    C() = default;

    int get() const {return data_;}

    friend bool operator==(const C& x, const C& y)
    {return x.data_ == y.data_;}
    const C *operator&() const { return nullptr; }
    C       *operator&()       { return nullptr; }
};

template<class It>
void
test25(It i, ranges::difference_type_t<It> n, It x)
{
    const ranges::reverse_iterator<It> r(i);
    ranges::reverse_iterator<It> rr = n + r;
    CHECK(rr.base() == x);
}

int main() {
  {

    static_assert(
        ranges::detail::BidirectionalCursor<
        ranges::detail::reverse_cursor<bidirectional_iterator<const char *>>>{},
        "");
    static_assert(
        ranges::detail::BidirectionalCursor<
        ranges::detail::reverse_cursor<random_access_iterator<const char *>>>{},
        "");
    static_assert(
        ranges::detail::RandomAccessCursor<
        ranges::detail::reverse_cursor<random_access_iterator<const char *>>>{},
        "");
    static_assert(
        ranges::BidirectionalIterator<
            ranges::reverse_iterator<bidirectional_iterator<const char *>>>{},
        "");
    static_assert(
        ranges::RandomAccessIterator<
            ranges::reverse_iterator<random_access_iterator<const char *>>>{},
        "");
  }
  { // test
    test<bidirectional_iterator<const char *>>();
    test<random_access_iterator<char *>>();
    test<char *>();
    test<const char *>();
  }
  { // test 2
    const char s[] = "123";
    test2(bidirectional_iterator<const char *>(s));
    test2(random_access_iterator<const char *>(s));
  }
  { // test3
    Derived d;
    test3<bidirectional_iterator<Base *>>(
        bidirectional_iterator<Derived *>(&d));
    test3<random_access_iterator<const Base *>>(
        random_access_iterator<Derived *>(&d));
  }
  { // test4
    const char *s = "1234567890";
    random_access_iterator<const char *> b(s);
    random_access_iterator<const char *> e(s + 10);
    while (b != e)
      test4(b++);
  }
  { // test5
    const char *s = "1234567890";
    test5(bidirectional_iterator<const char *>(s),
          bidirectional_iterator<const char *>(s), false);
    test5(bidirectional_iterator<const char *>(s),
          bidirectional_iterator<const char *>(s + 1), true);
    test5(random_access_iterator<const char *>(s),
          random_access_iterator<const char *>(s), false);
    test5(random_access_iterator<const char *>(s),
          random_access_iterator<const char *>(s + 1), true);
    test5(s, s, false);
    test5(s, s + 1, true);
  }
  {
    const char *s = "123";
    test6(bidirectional_iterator<const char *>(s + 1),
          bidirectional_iterator<const char *>(s));
    test6(random_access_iterator<const char *>(s + 1),
          random_access_iterator<const char *>(s));
    test6(s + 1, s);
  }
  {
    const char *s = "123";
    test7(bidirectional_iterator<const char *>(s + 1),
          bidirectional_iterator<const char *>(s));
    test7(random_access_iterator<const char *>(s + 1),
          random_access_iterator<const char *>(s));
    test7(s + 1, s);
  }
  {
    const char *s = "1234567890";
    test8(random_access_iterator<const char *>(s + 5), 5,
          random_access_iterator<const char *>(s));
    test8(s + 5, 5, s);
  }
  {
    const char *s = "1234567890";
    test9(random_access_iterator<const char *>(s + 5), 5,
          random_access_iterator<const char *>(s));
    test9(s + 5, 5, s);
  }
  {
    const char *s = "123";
    test10(bidirectional_iterator<const char *>(s + 1),
           bidirectional_iterator<const char *>(s + 2));
    test10(random_access_iterator<const char *>(s + 1),
           random_access_iterator<const char *>(s + 2));
    test10(s + 1, s + 2);
  }
  {
    const char *s = "123";
    test11(bidirectional_iterator<const char *>(s + 1),
           bidirectional_iterator<const char *>(s + 2));
    test11(random_access_iterator<const char *>(s + 1),
           random_access_iterator<const char *>(s + 2));
    test11(s + 1, s + 2);
  }
  {
    const char *s = "1234567890";
    test12(random_access_iterator<const char *>(s + 5), 5,
           random_access_iterator<const char *>(s + 10));
    test12(s + 5, 5, s + 10);
  }
  {
    const char *s = "1234567890";
    test13(random_access_iterator<const char *>(s + 5), 5,
           random_access_iterator<const char *>(s + 10));
    test13(s + 5, 5, s + 10);
  }
  {
    A a;
    test14(&a + 1, A());
  }
  {
    Derived d;

    test15<bidirectional_iterator<Base *>>(
        bidirectional_iterator<Derived *>(&d));
    test15<random_access_iterator<const Base *>>(
        random_access_iterator<Derived *>(&d));
    test15<Base *>(&d);
  }
  {
    const char *s = "1234567890";
    test16(bidirectional_iterator<const char *>(s),
           bidirectional_iterator<const char *>(s), true);
    test16(bidirectional_iterator<const char *>(s),
           bidirectional_iterator<const char *>(s + 1), false);
    test16(random_access_iterator<const char *>(s),
           random_access_iterator<const char *>(s), true);
    test16(random_access_iterator<const char *>(s),
           random_access_iterator<const char *>(s + 1), false);
    test16(s, s, true);
    test16(s, s + 1, false);
  }
  {
    char s[3] = {0};
    test17(random_access_iterator<const char *>(s),
           random_access_iterator<char *>(s), 0);
    random_access_iterator<char *> inp1(s);
    test17(random_access_iterator<char *>(s),
           random_access_iterator<const char *>(s + 1), 1);
    test17(random_access_iterator<const char *>(s + 1),
           random_access_iterator<char *>(s), -1);
    test17(s, s, 0);
    test17(s, s + 1, 1);
    test17(s + 1, s, -1);
  }
  {
    const char *s = "1234567890";
    test18(random_access_iterator<const char *>(s),
           random_access_iterator<const char *>(s), false);
    test18(random_access_iterator<const char *>(s),
           random_access_iterator<const char *>(s + 1), true);
    test18(random_access_iterator<const char *>(s + 1),
           random_access_iterator<const char *>(s), false);
    test18(s, s, false);
    test18(s, s + 1, true);
    test18(s + 1, s, false);
  }
  {
    const char *s = "1234567890";
    test19(random_access_iterator<const char *>(s),
           random_access_iterator<const char *>(s), true);
    test19(random_access_iterator<const char *>(s),
           random_access_iterator<const char *>(s + 1), true);
    test19(random_access_iterator<const char *>(s + 1),
           random_access_iterator<const char *>(s), false);
    test19(s, s, true);
    test19(s, s + 1, true);
    test19(s + 1, s, false);
  }
  {
    const char *s = "1234567890";
    test20(random_access_iterator<const char *>(s + 5), 4, '1');
    test20(s + 5, 4, '1');
  }
  {
    const char *s = "1234567890";
    test21(random_access_iterator<const char *>(s),
         random_access_iterator<const char *>(s), false);
    test21(random_access_iterator<const char *>(s),
         random_access_iterator<const char *>(s + 1), false);
    test21(random_access_iterator<const char *>(s + 1),
         random_access_iterator<const char *>(s), true);
    test21(s, s, false);
    test21(s, s + 1, false);
    test21(s + 1, s, true);
  }
  {
      const char* s = "1234567890";
      test22(random_access_iterator<const char*>(s), random_access_iterator<const char*>(s), false);
      test22(random_access_iterator<const char*>(s), random_access_iterator<const char*>(s+1), false);
      test22(random_access_iterator<const char*>(s+1), random_access_iterator<const char*>(s), true);
      test22(s, s, false);
      test22(s, s+1, false);
      test22(s+1, s, true);
  }
  {
      const char* s = "1234567890";
      test23(random_access_iterator<const char*>(s), random_access_iterator<const char*>(s), true);
      test23(random_access_iterator<const char*>(s), random_access_iterator<const char*>(s+1), false);
      test23(random_access_iterator<const char*>(s+1), random_access_iterator<const char*>(s), true);
      test23(s, s, true);
      test23(s, s+1, false);
      test23(s+1, s, true);
  }
  {
      B a;
      test24(&a+1, B());
  }
  {
      C l[3] = {C(0), C(1), C(2)};

      auto ri = ranges::rbegin(l);
      CHECK ( ri->get() == 2 );
      CHECK ( (*ri).get() == 2 );
      CHECK ( ri.operator->() == ranges::prev(ri.base()) );
      ++ri;
      CHECK ( ri->get() == 1 );
      CHECK ( (*ri).get() == 1 );
      CHECK ( ri.operator->() == ranges::prev(ri.base()) );
      ++ri;
      CHECK ( ri->get() == 0 );
      CHECK ( (*ri).get() == 0 );
      CHECK ( ri.operator->() == ranges::prev(ri.base()) );
      ++ri;
      CHECK ( ri == ranges::rend(l));
  }
  {
      const char* s = "1234567890";
      test25(random_access_iterator<const char*>(s+5), 5, random_access_iterator<const char*>(s));
      test25(s+5, 5, s);
  }

  return test_result();
}
