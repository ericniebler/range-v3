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

#include <vector>
#include <iostream>
#include <iomanip>
#include <range/v3/range.hpp>

using namespace ranges;

template<typename Rng, typename F>
auto mbind(Rng rng, F f)
RANGES_DECLTYPE_AUTO_RETURN(
    std::move(rng) | view::transform(f) | view::flatten
)

template<typename V>
auto mreturn(V v)
RANGES_DECLTYPE_AUTO_RETURN(
    view::repeat(v) | view::take(1)
)

template<typename F>
auto mguard(bool b, F f)
RANGES_DECLTYPE_AUTO_RETURN(
    view::generate(std::move(f)) | view::take(b ? 1 : 0)
)

auto const intsFrom = view::iota;
auto const ints = [](int i, int j){ return view::take(intsFrom(i), j); };

int main()
{
    auto all_triples = mbind(intsFrom(1), [](int z)
    {
        return mbind(ints(1, z), [z](int x)
        {
            return mbind(ints(x, z), [x, z](int y)
            {
                return mguard(x*x + y*y == z*z, [x, y, z] {
                    return std::make_tuple(x, y, z);
                });
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
