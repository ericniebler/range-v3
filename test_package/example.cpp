// Range v3 library
//
//  Copyright Luis Martinez de Bartolome Izquierdo 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

// This example shows how to define a range that is implemented
// in terms of itself. The example is generating the Fibonacci
// sequence using self-reference and zip_with.
//
// Note: don't use recursive_range_fn in performance sensitive
// code. Self-reference comes with indirection and dynamic
// allocation overhead.

#include <range/v3/all.hpp>
#include <iostream>

using namespace ranges;
// A range that iterates over all the characters in a
// null-terminated string.
class c_string_range
  : public view_facade<c_string_range>
{
    friend range_access;
    char const * sz_;
    char const & get() const { return *sz_; }
    bool done() const { return *sz_ == '\0'; }
    void next() { ++sz_; }
public:
    c_string_range() = default;
    explicit c_string_range(char const *sz) : sz_(sz)
    {
        assert(sz != nullptr);
    }
};



int main()
{
    c_string_range r("hello world");
    // Iterate over all the characters and print them out
    ranges::for_each(r, [](char ch){
        std::cout << ch << ' ';
    });
    // prints: h e l l o   w o r l d
}
