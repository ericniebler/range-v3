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
#include <range/v3/range.hpp>

struct noncopyable
{
    noncopyable(noncopyable const &) = delete;
};

struct nondefaultconstructible
{
    nondefaultconstructible(int) {};
};

static_assert(range::Assignable<int>(), "");
static_assert(!range::Assignable<int const>(), "");

static_assert(range::CopyConstructible<int>(), "");
static_assert(!range::CopyConstructible<noncopyable>(), "");

static_assert(range::DefaultConstructible<int>(), "");
static_assert(!range::DefaultConstructible<nondefaultconstructible>(), "");

static_assert(range::InputIterator<int*>(), "");
static_assert(!range::InputIterator<int>(), "");

static_assert(range::ForwardIterator<int*>(), "");
static_assert(!range::ForwardIterator<int>(), "");

static_assert(range::BidirectionalIterator<int*>(), "");
static_assert(!range::BidirectionalIterator<int>(), "");

static_assert(range::RandomAccessIterator<int*>(), "");
static_assert(!range::RandomAccessIterator<int>(), "");

static_assert(range::InputRange<range::istream_range<int>>(), "");
static_assert(!range::InputRange<int>(), "");

static_assert(range::RandomAccessRange<std::vector<int> const &>(), "");
static_assert(!range::RandomAccessRange<range::istream_range<int>>(), "");

int main()
{
    std::istringstream sin{"this is his face"};
    range::istream_range<std::string> lines{sin};
    for(auto line : range::filter(lines, [](std::string s){return s.length()>2;}))
        std::cout << "> " << line << '\n';

    auto lines2 = std::vector<std::string>{"this","is","his","face"}
                    | range::filter([](std::string s){return s.length()>2;})
                    | range::filter([](std::string s){return s.length()<4;})
                    | range::transform([](std::string s){return s + " or her";})
                    ;
    //undef<sizeof(lines2)> t;
    for(std::string const & line : lines2)
    {
        //line += " or her";
        std::cout << "> " << line << '\n';
    }
    auto b = lines2.begin();
    decltype(lines2)::const_iterator bc = b;

    for(auto const &line : lines2.base().base())//.base())
        std::cout << "> " << line << '\n';

    std::cout << "\n";
    auto sizes = std::vector<std::string>{"this","is","his","face"}
                    | range::transform(&std::string::length);
    for(std::size_t size : sizes)
        std::cout << "> " << size << '\n';

    std::cout << "\n";
    //std::istringstream sin2{"this is his face"};
    auto joined = range::join(std::vector<std::string>{"this","is","his","face"},
                                     std::vector<std::string>{"another","fine","mess"});
    for(std::string & s : joined | range::reverse)
        std::cout << "> " << s << '\n';

    auto revjoin = joined | range::reverse;
    std::cout << "*** " << (revjoin.end() - revjoin.begin()) << std::endl;

    std::cout << '\n';
    auto begin = joined.begin();
    std::cout << *(begin+0) << "\n";
    std::cout << *(begin+1) << "\n";
    std::cout << *(begin+2) << "\n";
    std::cout << *(begin+3) << "\n";
    std::cout << *(begin+4) << "\n";
    std::cout << *(begin+5) << "\n";
    std::cout << *(begin+6) << "\n";

    std::cout << '\n';
    std::cout << *(begin) << "\n";
    std::cout << *(begin+=1) << "\n";
    std::cout << *(begin+=1) << "\n";
    std::cout << *(begin+=1) << "\n";
    std::cout << *(begin+=1) << "\n";
    std::cout << *(begin+=1) << "\n";
    std::cout << *(begin+=1) << "\n";

    std::cout << '\n';
    auto end = joined.end();
    std::cout << *(end-1) << "\n";
    std::cout << *(end-2) << "\n";
    std::cout << *(end-3) << "\n";
    std::cout << *(end-4) << "\n";
    std::cout << *(end-5) << "\n";
    std::cout << *(end-6) << "\n";
    std::cout << *(end-7) << "\n";

    std::cout << '\n';
    std::cout << *(end-=1) << "\n";
    std::cout << *(end-=1) << "\n";
    std::cout << *(end-=1) << "\n";
    std::cout << *(end-=1) << "\n";
    std::cout << *(end-=1) << "\n";
    std::cout << *(end-=1) << "\n";
    std::cout << *(end-=1) << "\n";

    std::cout << '\n';
    std::cout << (joined.end() - joined.begin()) << std::endl;
}
