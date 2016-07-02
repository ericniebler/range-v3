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

#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/indirect.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

struct Int
{
    using difference_type = int;
    int i = 0;
    Int() = default;
    explicit Int(int i) : i(i) {}
    Int & operator++() {++i; CHECK(i <= 10); return *this;}
    Int operator++(int) {auto tmp = *this; ++*this; return tmp;}
    bool operator==(Int j) const { return i == j.i; }
    bool operator!=(Int j) const { return i != j.i; }
};

template <typename Integral>
void test_iota_minus() {
  using namespace ranges;
  using D = detail::iota_difference_t<Integral>;
  using I = Integral;
  Integral max = std::numeric_limits<Integral>::max();

  CHECK(detail::iota_minus(I(0), I(0)) == D(0));
  CHECK(detail::iota_minus(I(0), I(1)) == D(-1));
  CHECK(detail::iota_minus(I(1), I(0)) ==  D(1));
  CHECK(detail::iota_minus(I(1), I(1)) == D(0));

  CHECK(detail::iota_minus(I(max - I(1)), I(max - I(1))) == D(0));
  CHECK(detail::iota_minus(I(max - I(1)), I(max)) == D(-1));
  CHECK(detail::iota_minus(I(max), I(max - I(1))) == D(1));
  CHECK(detail::iota_minus(I(max), I(max)) == D(0));
}

int main()
{
    using namespace ranges;

    char const *sz = "hello world";
    ::check_equal(view::iota(forward_iterator<char const*>(sz)) | view::take(10) | view::indirect,
        {'h','e','l','l','o',' ','w','o','r','l'});

    {
        auto ints = view::closed_iota(Int{0}, Int{10});
        ::check_equal(ints, {Int{0},Int{1},Int{2},Int{3},Int{4},Int{5},Int{6},Int{7},Int{8},Int{9},Int{10}});
        models_not<concepts::BoundedView>(ints);
    }

    {  // iota minus tests
      test_iota_minus<int8_t>();
      test_iota_minus<int16_t>();
      test_iota_minus<int32_t>();
      test_iota_minus<int64_t>();

      test_iota_minus<uint8_t>();
      test_iota_minus<uint16_t>();
      test_iota_minus<uint32_t>();
      test_iota_minus<uint64_t>();
    }

    return ::test_result();
}
