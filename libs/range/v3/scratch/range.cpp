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

#include <iomanip>
#include <utility>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>
#include <initializer_list>
#include <range/v3/range.hpp>

template<std::size_t>
struct undef_i;

template<typename T>
struct undef_t;

struct noncopyable
{
    noncopyable(noncopyable const &) = delete;
};

struct nondefaultconstructible
{
    nondefaultconstructible(int) {};
};

static_assert(ranges::CopyAssignable<int>(), "");
static_assert(!ranges::CopyAssignable<int const>(), "");

static_assert(ranges::CopyConstructible<int>(), "");
static_assert(!ranges::CopyConstructible<noncopyable>(), "");

static_assert(ranges::DefaultConstructible<int>(), "");
static_assert(!ranges::DefaultConstructible<nondefaultconstructible>(), "");

static_assert(ranges::InputIterator<int*>(), "");
static_assert(!ranges::InputIterator<int>(), "");

static_assert(ranges::ForwardIterator<int*>(), "");
static_assert(!ranges::ForwardIterator<int>(), "");

static_assert(ranges::BidirectionalIterator<int*>(), "");
static_assert(!ranges::BidirectionalIterator<int>(), "");

static_assert(ranges::RandomAccessIterator<int*>(), "");
static_assert(!ranges::RandomAccessIterator<int>(), "");

static_assert(ranges::InputRange<ranges::istream_range<int>>(), "");
static_assert(!ranges::InputRange<int>(), "");

static_assert(ranges::RandomAccessRange<std::vector<int> const &>(), "");
static_assert(!ranges::RandomAccessRange<ranges::istream_range<int>>(), "");

static_assert(ranges::BinaryPredicate<std::less<int>, int, int>(), "");
static_assert(!ranges::BinaryPredicate<std::less<int>, char*, int>(), "");

static_assert(ranges::OutputIterator<int *, int>(), "");
static_assert(!ranges::OutputIterator<int const *, int>(), "");

struct NotDestructible
{
    ~NotDestructible() = delete;
};

static_assert(ranges::Destructible<int>(), "");
static_assert(!ranges::Destructible<NotDestructible>(), "");

struct IntComparable
{
    friend bool operator<(int, IntComparable);
};

static_assert(ranges::LessThanComparable<int>(), "");
static_assert(ranges::LessThanComparable<int, IntComparable>(), "");
static_assert(!ranges::LessThanComparable<IntComparable, int>(), "");

static_assert(
    std::is_same<
        ranges::range_concept_t<std::vector<int>>,
        ranges::concepts::RandomAccessRange
    >::value, "");

static_assert(
    std::is_same<
        ranges::range_concept_t<ranges::istream_range<int>>,
        ranges::concepts::InputRange
    >::value, "");

struct Abstract { virtual ~Abstract() = 0; };
static_assert(std::is_same<ranges::typelist_element_t<0, ranges::typelist<int, Abstract, float(), int(&&)[]>>, int>::value, "");
static_assert(std::is_same<ranges::typelist_element_t<1, ranges::typelist<int, Abstract, float(), int(&&)[]>>, Abstract>::value, "");
static_assert(std::is_same<ranges::typelist_element_t<2, ranges::typelist<int, Abstract, float(), int(&&)[]>>, float()>::value, "");
static_assert(std::is_same<ranges::typelist_element_t<3, ranges::typelist<int, Abstract, float(), int(&&)[]>>, int(&&)[]>::value, "");

static_assert(std::is_same<ranges::typelist_back_t<ranges::typelist<int, Abstract, float(), int(&&)[]>>, int(&&)[]>::value, "");

static_assert(std::is_same<ranges::typelist_drop_t<0, ranges::typelist<int, Abstract, float(), int(&&)[]>>, ranges::typelist<int, Abstract, float(), int(&&)[]>>::value, "");
static_assert(std::is_same<ranges::typelist_drop_t<1, ranges::typelist<int, Abstract, float(), int(&&)[]>>, ranges::typelist<Abstract, float(), int(&&)[]>>::value, "");
static_assert(std::is_same<ranges::typelist_drop_t<2, ranges::typelist<int, Abstract, float(), int(&&)[]>>, ranges::typelist<float(), int(&&)[]>>::value, "");
static_assert(std::is_same<ranges::typelist_drop_t<3, ranges::typelist<int, Abstract, float(), int(&&)[]>>, ranges::typelist<int(&&)[]>>::value, "");
static_assert(std::is_same<ranges::typelist_drop_t<4, ranges::typelist<int, Abstract, float(), int(&&)[]>>, ranges::typelist<>>::value, "");

struct move_only
{
    move_only() = default;
    move_only(move_only &&) = default;
    move_only(move_only const &) = delete;
    int operator()(std::string const &str) const { return str.length(); }
};

struct identity
{
    template<typename T>
    T operator()(T && t) const
    {
        return std::forward<T>(t);
    }
} ident {};

void test_indirect_view()
{
    using namespace ranges;
    std::vector<std::shared_ptr<int>> vp;
    for(int i = 0; i < 10; ++i)
        vp.push_back(std::make_shared<int>(i));
    std::cout << "\nindirected:\n";
    for(int & i : vp | view::indirect)
        std::cout << i << ' ';
    std::cout << '\n';
}

void test_keys_values_view()
{
    using namespace ranges;
    std::map<std::string, int> m ={
        {"this",0},
        {"that",1},
        {"other",2}};
    std::cout << "\nkeys:\n";
    for(std::string const & s : m | view::keys)
        std::cout << s << ' ';
    std::cout << '\n';
    std::cout << "\nvalues:\n";
    for(int & i : m | view::values)
        std::cout << i << ' ';
    std::cout << '\n';
}

void test_view_replace()
{
    using namespace ranges;
    using namespace std::placeholders;

    std::string str{"1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 "};
    std::stringstream sin{str};

    for(auto & i : istream<int>(sin) | view::replace(1, 42))
        std::cout << i << ' ';
    std::cout << '\n';
}

void test_view_replace_if()
{
    using namespace ranges;
    using namespace std::placeholders;

    std::cout << "\nreplace_if\n";
    std::string str{"1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 "};
    std::stringstream sin{str};

    for(auto & i : istream<int>(sin) | view::replace_if([](int i){return i==1;}, 42))
        std::cout << i << ' ';
    std::cout << '\n';
}

void test_slicer()
{
    using namespace ranges;
    using namespace std::placeholders;

    int rgi[] = {0,1,2,3,4,5,6,7,8,9,10};
    auto sl = rgi | view::slice(3,9);
    for(int& i : rgi | view::slice(3,9))
        std::cout << i << ' ';
    std::cout << '\n';
    for(int& i : sl | view::reverse)
        std::cout << i << ' ';
    std::cout << '\n';
    for(int i : std::vector<int>{0,1,2,3,4,5,6,7,8,9,10} | view::slice(3,9) | view::reverse)
        std::cout << i << ' ';
    std::cout << '\n';

    std::stringstream sin("0 1 2 3 4 5 6 7 8 9 10");
    for(int i : istream<int>(sin) | view::slice(3,9))
        std::cout << i << ' ';
    std::cout << '\n';

    std::list<int> li {0,1,2,3,4,5,6,7,8,9,10};
    for(int i : li | view::slice(3,9) | view::reverse)
        std::cout << i << ' ';
    std::cout << '\n';
}

void test_stride_view()
{
    using namespace ranges;
    std::cout << "\nstride\n";

    std::vector<int> v(50);
    iota(v, 0);
    static_assert(
        sizeof((v|view::stride(3)).begin()) ==
        sizeof(void*)+sizeof(v.begin())+sizeof(std::ptrdiff_t),"");
    for(int i : v | view::stride(3) | view::reverse)
        std::cout << i << ' ';
    std::cout << '\n';

    std::stringstream str;
    copy(v, std::ostream_iterator<int>{str, " "});
    static_assert(
        sizeof((istream<int>(str)|view::stride(3)).begin()) ==
        sizeof(void*)+sizeof(istream<int>(str).begin()),"");
    for(int i : istream<int>(str) | view::stride(3))
        std::cout << i << ' ';
    std::cout << '\n';

    std::list<int> li;
    copy(v, std::back_inserter(li));
    static_assert(
        sizeof((li|view::stride(3)).begin()) ==
        sizeof(void*)+sizeof(li.begin())+sizeof(int)+sizeof(std::ptrdiff_t),"");
    for(int i : li | view::stride(3))
        std::cout << i << ' ';
    std::cout << '\n';
    for(int i : li | view::stride(3) | view::reverse)
        std::cout << i << ' ';
    std::cout << '\n';

    auto x = v | view::stride(3);
    std::cout << ranges::distance(x) << '\n';
    std::cout << '\n';
}

void test_adjacent_filter()
{
    using namespace ranges;
    using namespace std::placeholders;

    int rgi[] = {1,1,1,2,3,4,4};
    rgi | view::adjacent_filter(std::not_equal_to<int>{})
        | copy(std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}

void test_unique_view()
{
    using namespace ranges;
    using namespace std::placeholders;

    int const rgi[] = {1,1,1,2,3,4,4};
    rgi | view::unique
        | copy(std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}

void test_partial_sort_copy()
{
    using namespace ranges;
    using namespace std::placeholders;

    std::string str{"1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 "};
    std::stringstream sin{str};
    partial_sort_copy_result<std::vector<int>> x
        = istream<int>(sin)
            | partial_sort_copy(std::vector<int>(20,0));
    for(int i : x)
        std::cout << i << ' ';
    std::cout << '\n';

    std::stringstream sin2{str};
    std::vector<int> vi(20,0);
    partial_sort_copy_result<std::vector<int> &> y =
        istream<int>(sin2)
            | partial_sort_copy(vi);
    for(int i : y)
        std::cout << i << ' ';
    std::cout << '\n';

    for(int i : vi)
        std::cout << i << ' ';
    std::cout << '\n';
}

void test_tokenize_view()
{
    // Drat, can't test this because libstdc++ doesn't have <regex> yet. :(
    //
    //using namespace ranges;
    //
    //for(auto & sub : std::string("this is his face")
    //               | view::tokenize(std::regex("\\w+"), {1,2,3,4}))
    //{
    //    std::cout << sub << '*';
    //}
    //std::cout << '\n';
}

void test_zip_view()
{
    using namespace ranges;
    std::cout << "\nzip view:\n";

    std::vector<int> vi{0,1,2,3,4,5,6,7,8,9,10};
    std::vector<std::string> vs{
        "hello"
      , "goodbye"
      , "hello"
      , "goodbye"
    };
    std::stringstream str{"john paul george ringo"};
    for(auto t : view::zip(vi, vs, istream<std::string>(str)))
    {
        std::cout << "{"
            << std::get<0>(t) << ", "
            << std::get<1>(t) << ", "
            << std::get<2>(t) << "}\n";
    }

    auto rnd_rng = view::zip(vi, vs);
    auto rnd_it = rnd_rng.begin();
    auto tmp = rnd_it + 3;
    std::cout << "\n{"
        << std::get<0>(*tmp) << ", "
        << std::get<1>(*tmp) << "}\n";

    std::cout << rnd_rng.end() - rnd_rng.begin() << "\n";
    std::cout << rnd_rng.begin() - rnd_rng.end() << "\n";
}

void test_move_view()
{
    using namespace ranges;
    std::cout << "\nmove view:\n";
    std::vector<std::string> vs {
        "'allo",
        "'allo",
        "???"
    };
    move_range_view<std::vector<std::string>&> x = vs | view::move;
    if(0) std::cout << x.begin()->c_str() << std::endl;
    std::vector<std::string> vs2(x.begin(), x.end());
    static_assert(std::is_same<std::string&&, decltype(*x.begin())>::value, "");
    std::cout << "target:\n";
    for(std::string &s : vs2)
        std::cout << '"' << s << "\" ";
    std::cout << "\nsource:\n";
    for(std::string &s : vs)
        std::cout << '"' << s << "\" ";
}

//*
int main()
{
    using namespace ranges;
    using namespace std::placeholders;

    test_adjacent_filter();
    test_unique_view();

    // Pipeable algorithms
    std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 ");
    istream<int>(sinx)
        | partial_sort_copy(std::vector<int>(20,0))
        | view::transform([](int i){return -i;})
        | for_each([](int i)
          {
              std::cout << i << ' ';
          });

    std::vector<int> vi{ 1, 2, 2, 3, 4 };
    std::cout << '\n';
    std::cout << (vi | count(2)) << std::endl;

    // Range placeholder expressions.
    std::cout << "\n";
    for (int i : vi | range(adjacent_find(_1), prev(end(_1))))
        std::cout << "> " << i << '\n';

    // A pipeline where some algorithms return iterators into rvalue ranges. It's ok!
    std::cout << "\n";
    vi | view::filter([](int) {return true;})
       | range(adjacent_find(_1), prev(end(_1)))
       | range(begin(_1), next(end(_1)))
       | for_each([](int i)
         {
             std::cout << "> " << i << '\n';
         });

    std::cout << "\n";
    for( int i : vi | view::transform(_1, [](int i){return i*2;}))
        std::cout << "> " << i << '\n';

    // Mutate in-place
    transform(vi, vi.begin(), [](int i){return -i;});
    std::cout << "\n";
    for( int i : vi )
        std::cout << "> " << i << '\n';

    std::cout << "\n";
    std::istringstream sin{"this is his face"};
    istream_range<std::string> lines{sin};
    for(auto line : view::filter(lines, [](std::string s){return s.length()>2;}))
        std::cout << "> " << line << '\n';

    std::cout << "\n";
    auto lines2 = std::vector<std::string>{"this","is","his","face"}
                    | view::filter([](std::string s){return s.length()>2;})
                    | view::filter(_1, [](std::string s){return s.length()<4;})
                    | view::transform([](std::string s){return s + " or her";})
                    ;
    //undef_i<sizeof(lines2)> ttt;
    for(std::string const & line : lines2)
    {
        //line += " or her";
        std::cout << "> " << line << '\n';
    }
    auto b = lines2.begin();
    decltype(lines2)::const_iterator bc = b;

    std::cout << "\n";
    for(auto const &line : lines2.base().base())//.base())
        std::cout << "> " << line << '\n';

    std::cout << "\n";
    auto sizes = std::vector<std::string>{"this","is","his","face"}
                    //| view::transform([](std::string const &str){return &str; })
                    | view::transform(_1, &std::string::length);
                    //| view::transform(move_only{});
    for(std::size_t size : sizes)
        std::cout << "> " << size << '\n';

    std::cout << "\n";
    //std::istringstream sin2{"this is his face"};
    auto joined = view::join(std::vector<std::string>{"this","is","his","face"},
                             std::vector<std::string>{"another","fine","mess"});
    for(std::string & s : joined | view::reverse)
        std::cout << "> " << s << '\n';

    auto revjoin = joined | view::reverse;
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

    std::cout << '\n';
    test_slicer();
    test_indirect_view();
    test_keys_values_view();
    test_view_replace_if();
    test_tokenize_view();
    test_stride_view();
    test_zip_view();
    test_move_view();
}
//*/
