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

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/range/v3/istream_range.hpp>
#include <boost/range/v3/adaptor/filter.hpp>
#include <boost/range/v3/adaptor/transform.hpp>

int main()
{
    std::istringstream sin{"this is his face"};
    boost::range::istream_range<std::string> lines{sin};
    for(auto line : boost::range::filter(lines, [](std::string s){return s.length()>2;}))
        std::cout << "> " << line << '\n';

    auto lines2 = std::vector<std::string>{"this","is","his","face"}
                    | boost::range::filter([](std::string s){return s.length()>2;})
                    | boost::range::filter([](std::string s){return s.length()<4;})
                    | boost::range::transform([](std::string s){return s + " or her";})
                    ;
    for(std::string const & line : lines2)
    {
        //line += " or her";
        std::cout << "> " << line << '\n';
    }

    for(auto const &line : lines2.base().base())//.base())
        std::cout << "> " << line << '\n';

    auto b = lines2.begin();
    decltype(lines2)::const_iterator bc = b;
}
