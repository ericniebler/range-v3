// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#include <iostream>
#include <range/v3/range.hpp>

using namespace ranges;

auto const intsFrom = view::iota;
auto const ints = [](int i, int j){ return view::take(intsFrom(i), j); };

int main()
{
    // Define an infinite range containing all the Pythagorean triples:
    auto all_triples =
        view::for_each(intsFrom(1), [](int z)
        {
            return view::for_each(ints(1, z), [=](int x)
            {
                return view::for_each(ints(x, z), [=](int y)
                {
                    return yield_if(x*x + y*y == z*z, std::make_tuple(x, y, z));
                });
            });
        });

    auto triples = view::take(all_triples, 100);

    // Display the first 100 triples
    for_each(triples, [](std::tuple<int, int, int> triple){
        std::cout << '('
            << std::get<0>(triple) << ','
            << std::get<1>(triple) << ','
            << std::get<2>(triple) << ')' << '\n';
    });
}
