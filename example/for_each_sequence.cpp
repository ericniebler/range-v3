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

///[for_each_sequence]
//Use the for_each to print from various containers
//output
//vector:   1 2 3 4 5 6 
//array:    1 2 3 4 5 6 
//list:     1 2 3 4 5 6 
//fwd_list: 1 2 3 4 5 6 
//deque:    1 2 3 4 5 6 

#include <range/v3/algorithm/for_each.hpp>   //specific includes
#include <vector>
#include <deque>
#include <list>
#include <forward_list>
#include <stack>
#include <queue>
#include <array>
#include <iostream>
using std::cout;

auto print = [] (int i) { cout << i << " "; };

int main() {

  cout  << "vector:   ";
  std::vector<int> v { 1, 2, 3, 4, 5, 6 };
  ranges::for_each( v, print ); //1 2 3 4 5 6

  cout << "\n" << "array:    ";
  std::array<int, 6> a { 1, 2, 3, 4, 5, 6 };
  ranges::for_each( a, print );
 
  cout << "\n" << "list:     ";  
  std::list<int> ll { 1, 2, 3, 4, 5, 6 };
  ranges::for_each( ll, print );

  cout << "\n" << "fwd_list: ";
  std::forward_list<int> fl { 1, 2, 3, 4, 5, 6 };
  ranges::for_each( fl, print ); 
  
  cout << "\n" << "deque:    ";
  std::deque<int> d { 1, 2, 3, 4, 5, 6 };
  ranges::for_each( d, print ); 
  cout << "\n";
}
///[for_each_sequence]
