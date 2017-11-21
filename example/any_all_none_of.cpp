// Range v3 library
//
//  Copyright Jeff Garland 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
// 

//[any_all_none_of]]
// Demonstrates any_of, all_of, none_of
// output
// vector: 6 2 3 4 5 6 
// vector any_of  is_six: true
// vector all_of  is_six: false
// vector none_of is_six: false

#include <range/v3/algorithm/all_of.hpp>  
#include <range/v3/algorithm/any_of.hpp>  
#include <range/v3/algorithm/none_of.hpp>  
#include <range/v3/algorithm/for_each.hpp> 

#include <vector>
#include <iostream>
using std::cout;

auto print  = [](int i)         { cout << i << " "; };
auto is_six = [](int i) -> bool { return i == 6; };

int main() {

  std::vector<int> v { 6, 2, 3, 4, 5, 6 };
  cout << "vector: ";
  ranges::for_each( v, print ); 
  cout << "\n";
  
  if (ranges::any_of( v, is_six ))   {
    cout << "vector any_of  is_six: true" << "\n"; 
  };
  if (!ranges::all_of( v, is_six )) { 
    cout << "vector all_of  is_six: false" << "\n"; 
  };
  if (!ranges::none_of( v, is_six )) { 
    cout << "vector none_of is_six: false" << "\n"; 
  };
  
}
//[any_all_none_of]]
