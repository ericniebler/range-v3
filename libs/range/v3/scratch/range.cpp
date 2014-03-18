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

#include <map>
#include <list>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <utility>
#include <sstream>
#include <iostream>
#include <functional>
#include <initializer_list>
#include <range/v3/range.hpp>
#include <range/v3/utility/safe_int.hpp>

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

static_assert(ranges::Iterable<ranges::istream_iterable<int>>(), "");
static_assert(ranges::InputIterator<ranges::range_iterator_t<ranges::istream_iterable<int>>>(), "");
static_assert(!ranges::Iterable<int>(), "");

static_assert(ranges::Range<std::vector<int> const &>(), "");
static_assert(ranges::RandomAccessIterator<ranges::range_iterator_t<std::vector<int> const &>>(), "");
static_assert(!ranges::Range<ranges::istream_iterable<int>>(), "");

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
        ranges::concepts::SizedRange
    >::value, "");

static_assert(
    std::is_same<
        ranges::range_concept_t<ranges::istream_iterable<int>>,
        ranges::concepts::Iterable
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

    for(auto & i : istream<int>(sin) | view::replace(1, 42) | view::as_range)
        std::cout << i << ' ';
    std::cout << '\n';
}

void test_view_replace_if()
{
    using namespace ranges;

    std::cout << "\nreplace_if\n";
    std::string str{"1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 "};
    std::stringstream sin{str};

    for(auto & i : istream<int>(sin) | view::replace_if([](int i){return i==1;}, 42) | view::as_range)
        std::cout << i << ' ';
    std::cout << '\n';
}

void test_slicer()
{
    using namespace ranges;
    std::cout << "\ntest slice\n";

    int rgi[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for(int& i : rgi | view::slice(3,9))
        std::cout << i << ' ';
    std::cout << '\n';

    auto sl = rgi | view::slice(3, 9);
    for(int& i : sl | view::reverse)
        std::cout << i << ' ';
    std::cout << '\n';

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for(int i : v | view::slice(3, 9) | view::reverse)
        std::cout << i << ' ';
    std::cout << '\n';

    std::list<int> li{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for(int i : li | view::slice(3, 9) | view::reverse)
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
    for(int i : istream<int>(str) | view::as_range | view::stride(3))
        std::cout << i << ' ';
    std::cout << '\n';

    std::list<int> li;
    copy(v, std::back_inserter(li));
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

    std::cout << "\nTesting adjacent_filter:\n";
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
        = istream<int>(sin) | view::as_range
            | partial_sort_copy(std::vector<int>(20,0));
    for(int i : x)
        std::cout << i << ' ';
    std::cout << '\n';

    std::stringstream sin2{str};
    std::vector<int> vi(20,0);
    partial_sort_copy_result<std::vector<int> &> y =
        istream<int>(sin2) | view::as_range
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
    for(auto t : view::zip(vi, vs, istream<std::string>(str)|view::as_range))
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
    auto x = vs | view::move;
    CONCEPT_ASSERT(Same<range_concept_t<decltype(x)>, concepts::SizedRange>());
    CONCEPT_ASSERT(Same<iterator_concept_t<decltype(x.begin())>, concepts::InputIterator>());
    CONCEPT_ASSERT(Same<iterator_category_t<decltype(x.begin())>, std::input_iterator_tag>());
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

void test_slice_iota()
{
    using namespace ranges;
    std::cout << "\n\nslice an infinite range:\n";
    for (int i : view::iota(10) | view::slice(10, 20))
        std::cout << i << ' ';
    (view::iota(10) | view::slice(2, 4)).begin() + 1;
    CONCEPT_ASSERT(Range<decltype(view::iota(10) | view::slice(2, 4))>());
    static_assert(!ranges::is_infinite<decltype(view::iota(10) | view::slice(2, 4))>::value, "");

    std::cout << '\n';
}

void test_delimit_iota()
{
    using namespace ranges;
    std::cout << "\n\ndelimit an infinite range:\n";
    view::iota(10) | view::delimit(50) | for_each([](int i) {
        std::cout << i << ' ';
    });
    CONCEPT_ASSERT(Iterable<delimit_iterable_view<std::vector<int>, int>>());
    CONCEPT_ASSERT(RandomAccessIterator<range_iterator_t<delimit_iterable_view<std::vector<int>, int>>>());
    std::cout << '\n';
}

void test_delimit_iota_finite()
{
    using namespace ranges;
    std::cout << "\n\ndelimit a finite range:\n";
    std::vector<int>(10) | iota(10) | view::delimit(50) | for_each([](int i) {
        std::cout << i << ' ' << std::flush;
    });
    std::cout << '\n';
}

void test_take_repeat()
{
    using namespace ranges;
    std::cout << "\n\nslice a repeat infinite range:\n";
    for (int i : view::repeat(9) | view::take(10))
        std::cout << i << ' ';
    std::cout << '\n';
}

void test_safe_int()
{
    using namespace ranges;
    constexpr safe_int<int> i = 42;
    constexpr safe_int<int> pos_inf = safe_int<int>::inf();
    constexpr safe_int<int> neg_inf = -safe_int<int>::inf();
    constexpr safe_int<int> NaN = safe_int<int>::NaN();
    constexpr safe_int<int> zero = 0;
    constexpr safe_int<int> zero2{};
    static_assert(zero == zero2, "");
    static_assert(i + 1 == 43, "");
    static_assert(i + std::numeric_limits<int>::max() == pos_inf, "");
    static_assert(-i - std::numeric_limits<int>::max() == neg_inf, "");
    static_assert(pos_inf == - neg_inf, "");
    static_assert(pos_inf - 1 == pos_inf, "");
    static_assert(1 - pos_inf == neg_inf, "");
    static_assert(pos_inf != NaN, "");
    static_assert(neg_inf != NaN, "");
    static_assert(!(pos_inf == NaN), "");
    static_assert(!(neg_inf == NaN), "");
    static_assert(!(NaN == NaN), "");
    static_assert(NaN != NaN, "");
    static_assert(neg_inf != pos_inf, "");
    static_assert(!(neg_inf == pos_inf), "");
    static_assert((pos_inf - pos_inf).is_NaN(), "");
    static_assert((pos_inf + neg_inf).is_NaN(), "");
    static_assert(pos_inf + pos_inf == pos_inf, "");
    static_assert(neg_inf - pos_inf == neg_inf, "");
    static_assert(zero / pos_inf == 0, "");
    static_assert(zero / -pos_inf == 0, "");
    static_assert(i / pos_inf == 0, "");
    static_assert(i / -pos_inf == 0, "");
    static_assert(i / zero == pos_inf, "");
    static_assert((zero / zero).is_NaN(), "");
    static_assert(pos_inf / zero == pos_inf, "");
    static_assert(neg_inf / zero == neg_inf, "");
    static_assert(pos_inf / i == pos_inf, "");
    static_assert(neg_inf / i == neg_inf, "");
    static_assert(pos_inf / -i == neg_inf, "");
    static_assert(neg_inf / -i == pos_inf, "");
    static_assert((pos_inf / pos_inf).is_NaN(), "");
    static_assert((pos_inf / neg_inf).is_NaN(), "");
    static_assert((neg_inf / pos_inf).is_NaN(), "");
    static_assert((neg_inf / neg_inf).is_NaN(), "");
    static_assert(i / 2 == 21, "");
    static_assert(i * 2 == 84, "");
    static_assert(i * pos_inf == pos_inf, "");
    static_assert(-i * pos_inf == neg_inf, "");
    static_assert(i * neg_inf == neg_inf, "");
    static_assert(-i * neg_inf == pos_inf, "");
    static_assert((0 * pos_inf).is_NaN(), "");
    static_assert((0 * neg_inf).is_NaN(), "");
    static_assert(pos_inf * pos_inf == pos_inf, "");
    static_assert(neg_inf * neg_inf == pos_inf, "");
    static_assert(pos_inf * neg_inf == neg_inf, "");
    static_assert(neg_inf * pos_inf == neg_inf, "");
    static_assert(safe_int<int>{0x8000} * safe_int<int>{0x10000} == pos_inf, "");
    static_assert(safe_int<int>{0x8000} * safe_int<int>{-(int)0x10000} == neg_inf, "");
    static_assert(safe_int<int>{-(int)0x8000} * safe_int<int>{-(int)0x10000} == pos_inf, "");

//    RANGES_ASSERT((view::iota(10) | distance) == safe_int<int>::inf());
}

void test_find_end_iterable()
{
    using namespace ranges;
    std::cout << "\nfind_end on a delimited range:\n";
    iterator_range<char const *> input = {"now is the time for all good men to come to the aid of their country", nullptr};
    iterator_range<char const *> pattern = {"to", nullptr};
    auto result = find_end(view::delimit(input, '\0'), view::delimit(pattern, '\0'));
    std::cout << &*result << '\n';
}

void test_sentinel()
{
    using namespace ranges;
    using It = range_iterator_t<iota_iterable_view<int>>;
    using S = range_sentinel_t<iota_iterable_view<int>>;
    static_assert(is_infinite<iota_iterable_view<int>>::value, "");
    static_assert(!is_infinite<std::vector<int>>::value, "");
}

static_assert(!ranges::is_infinite<std::vector<int>>::value, "");
static_assert(!ranges::is_infinite<ranges::istream_iterable<int>>::value, "");
static_assert(ranges::is_infinite<ranges::iota_iterable_view<int>>::value, "");

struct MyRange
  : ranges::range_facade<MyRange>
{
private:
    friend struct ranges::range_core_access;
    std::vector<int> ints_;
    struct cursor
    {
        cursor(std::vector<int>::const_iterator it) : iter(it) {}
        std::vector<int>::const_iterator iter;
        int const & current() const { return *iter; }
        void next() { ++iter; }
        bool equal(cursor const &that) const { return iter == that.iter; }
    };
    cursor get_begin() const
    {
        return {ints_.begin()};
    }
    cursor get_end() const
    {
        return {ints_.end()};
    }
public:
    MyRange()
      : ints_{1,2,3,4,5,6,7}
    {}
};

void test_range_facade()
{
    using namespace ranges;
    std::cout << "\nTesting range facade:\n";
    if(auto r = MyRange{})
        for(auto &i : r)
            std::cout << i << ' ';
    std::cout << std::endl;
}

template<typename BidiRange>
struct my_reverse_view
  : ranges::range_adaptor<my_reverse_view<BidiRange>, BidiRange>
{
private:
    CONCEPT_ASSERT(ranges::Range<BidiRange>());
    CONCEPT_ASSERT(ranges::BidirectionalIterator<ranges::range_iterator_t<BidiRange>>());
    friend ranges::range_core_access;
    using base_cursor_t = ranges::base_cursor_t<my_reverse_view>;

    struct adaptor : ranges::adaptor_defaults
    {
        // Cross-wire begin and end.
        base_cursor_t begin(my_reverse_view const &rng) const
        {
            return adaptor_defaults::end(rng);
        }
        base_cursor_t end(my_reverse_view const &rng) const
        {
            return adaptor_defaults::begin(rng);
        }
        void next(base_cursor_t &pos)
        {
            pos.prev();
        }
        void prev(base_cursor_t &pos)
        {
            pos.next();
        }
        auto current(base_cursor_t tmp) const -> decltype(tmp.current())
        {
            tmp.prev();
            return tmp.current();
        }
        CONCEPT_REQUIRES(ranges::RandomAccessIterator<ranges::range_iterator_t<BidiRange>>())
        void advance(base_cursor_t &pos, ranges::range_difference_t<BidiRange> n)
        {
            pos.advance(-n);
        }
        CONCEPT_REQUIRES(ranges::RandomAccessIterator<ranges::range_iterator_t<BidiRange>>())
        ranges::range_difference_t<BidiRange>
        distance_to(base_cursor_t const &here, base_cursor_t const &there)
        {
            return there.distance_to(here);
        }
    };
    adaptor get_adaptor(ranges::begin_end_tag) const
    {
        return {};
    }
public:
    using ranges::range_adaptor_t<my_reverse_view>::range_adaptor_t;
};

struct my_delimited_range
  : ranges::range_adaptor<
        my_delimited_range
      , ranges::delimit_iterable_view<ranges::istream_iterable<int>, int>>
{
    using range_adaptor_t::range_adaptor_t;
};

void test_range_adaptor()
{
    using namespace ranges;
    std::cout << "\nTesting range adaptor:\n";

    std::vector<int> v {1,2,3,4};
    my_reverse_view<std::vector<int>& > retro{v};
    for(int i : retro)
        std::cout << i << ' ';
    std::cout << std::endl;

    std::list<int> l { 1,2,3,4 };
    my_reverse_view<std::list<int>& > retro2{l};
    for(int i : retro2)
        std::cout << i << ' ';
    std::cout << std::endl;

    std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 42 6 7 8 9 ");
    my_delimited_range r{view::delimit(istream<int>(sinx), 42)};
    r | for_each([](int i){
        std::cout << i << ' ';
    });
    std::cout << std::endl;
}

void test_as_range()
{
    using namespace ranges;
    std::cout << "\nTesting as_range\n";

    std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 42 6 7 8 9 ");
    for(int i : istream<int>(sinx) | view::delimit(42) | view::as_range)
        std::cout << i << ' ';
    std::cout << '\n';

    auto x = std::vector<int>{} | view::delimit(42) | view::as_range;
    CONCEPT_ASSERT(Range<decltype(x)>());
    CONCEPT_ASSERT(RandomAccessIterator<range_iterator_t<decltype(x)>>());
    auto const & y = x;
    auto i = x.begin(); // non-const
    auto j = y.begin(); // const
    j = i;
}

void test_counted_range()
{
    using namespace ranges;
    std::cout << "\nTesting counted\n";
    int rgi[] = {1,2,3,4,5,6,7,8,9,10};
    auto rng = view::counted(rgi, 10);
    CONCEPT_ASSERT(SizedIterable<decltype(rng)>());
    auto i = rng.begin();
    auto b = i.base();
    auto c = i.count();
    decltype(i) j{b,c};
    for_each(rng, [](int i){
        std::cout << i << ' ';
    });
    static_assert(std::is_same<decltype(i),counted_iterator<int*>>::value,"");
}

void test_unbounded()
{
    using namespace ranges;
    std::cout << "\nTesting unbounded\n";
    static constexpr int rgi[] = {1,2,3,4,5,6,7,8,9,10};
    constexpr unbounded_iterable_view<int const*> rng{&rgi[0]};
    constexpr auto i = rng.begin();
    constexpr auto e = rng.end();
    constexpr bool b = i == e;
    static_assert(!b,"");
    CONCEPT_ASSERT(Iterable<unbounded_iterable_view<int const*>>());
    CONCEPT_ASSERT(RandomAccessIterator<range_iterator_t<unbounded_iterable_view<int const*>>>());
    static_assert(is_infinite<unbounded_iterable_view<int const*>>::value, "");
}

void test_view_all()
{
    using namespace ranges;

    std::cout << "\nTesting view::all\n";
    std::list<int> li{1,2,3,4};
    RANGES_ASSERT(ranges::size(li)==4);
    auto liter = view::all(li);
    CONCEPT_ASSERT(Same<decltype(liter), sized_iterator_range<std::list<int>::iterator>>());
    RANGES_ASSERT(ranges::size(liter) == 4);

    auto cli = view::counted(li.begin(), 4);
    static_assert(is_iterable<decltype(cli)>::value, "");
    auto cliter = view::all(cli);
    CONCEPT_ASSERT(Same<decltype(cliter),
                            counted_iterable_view<std::list<int>::iterator>>());

    // This triggers a static assert. It's unsafe to get a view of a temporary container.
    //view::all(std::vector<int>{});
}

void test_common_range_iterator()
{
    using namespace ranges;
    int rgi[] = {1,2,3,4,5};
    common_range_iterator<int*, unreachable> it{rgi};
    common_range_iterator<int*, unreachable> end{unreachable{}};

    std::cout << "\nTesting common_range_iterator\n";
    if(it != end)
        std::cout << *it << ' ';
    if(++it != end)
        std::cout << *it << ' ';
    if(++it != end)
        std::cout << *it << ' ';
    if(++it != end)
        std::cout << *it << ' ';
    if(++it != end)
        std::cout << *it << ' ';
    std::cout << '\n';

    auto rng = view::counted(rgi, 5);
    using it_t = range_iterator_t<decltype(rng)>;
    using se_t = range_sentinel_t<decltype(rng)>;
    static_assert(!std::is_same<it_t, se_t>::value, "");
    using cit_t = common_type_t<it_t, se_t>;
    CONCEPT_ASSERT(Common<it_t, se_t>());
    static_assert(std::is_same<cit_t,
        common_range_iterator<counted_iterator<int*>, counted_sentinel<int*>>>::value, "");
}

void test_filter()
{
    using namespace ranges;
    std::cout << "\ntest filter\n";
    std::stringstream sin("1 2 3 4 5 6 7 8 9");
    auto ints = istream<int>(sin);
    auto evens = ints | view::filter([](int i){return i % 2 == 0; });
    using evens_t = decltype(evens);
    range_iterator_t<evens_t> i = evens.begin();
    range_iterator_t<evens_t const> j = evens.begin();
    j = i;
    range_sentinel_t<evens_t> k = evens.end();
    range_sentinel_t<evens_t const> l = evens.end();
    l = k;
    for (; j != evens.end(); ++j)
        std::cout << *j << ' ';
    std::cout << '\n';
}

int main()
{
    using namespace ranges;
    using namespace std::placeholders;

    test_adjacent_filter();
    test_unique_view();
    test_filter();

    // Pipeable algorithms
    std::cout << '\n';
    std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 ");
    istream<int>(sinx)
        | view::as_range
        | partial_sort_copy(std::vector<int>(20,0))
        | view::transform([](int i){return -i;})
        | for_each([](int i)
          {
              std::cout << i << ' ';
          });

    std::vector<int> vi{ 1, 2, 2, 3, 4 };
    ranges::size(vi);
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
    istream_iterable<std::string> lines{sin};
    for (auto line : view::filter(lines, [](std::string s){return s.length()>2; }) | view::as_range)
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
    range_iterator_t<decltype(lines2) const> bc = b;

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

    test_slicer();
    test_indirect_view();
    test_keys_values_view();
    test_view_replace_if();
    test_tokenize_view();
    test_stride_view();
    test_zip_view();
    test_move_view();
    test_slice_iota();
    test_delimit_iota();
    test_delimit_iota_finite();
    test_take_repeat();
    test_safe_int();
    test_find_end_iterable();
    test_range_facade();
    test_range_adaptor();
    test_as_range();
    test_counted_range();
    test_common_range_iterator();
}
//*/


//#include <typeinfo>
//#include <iostream>
//#include <range/v3/utility/typelist.hpp>
//#include <range/v3/utility/concepts.hpp>
//#include <range/v3/utility/variant.hpp>
//
//struct S
//{
//    S() {}
//    ~S() {std::cout << "S::~S()\n";}
//    friend std::ostream& operator<<(std::ostream& sout, S)
//    {
//        return sout << "S";
//    }
//};
//
//template<std::size_t N>
//struct undef;
//
//struct disp
//{
//    template<typename T>
//    void operator()(T const & t) const
//    {
//        std::cout << typeid(T).name() << " : " << t << '\n';
//    }
//};
//
//struct my_visitor
//{
//    int operator()(int i) const { return i; }
//    float operator()(float i) const { return i; }
//    template<typename T>
//    void operator()(T) const {}
//};
//
//int main()
//{
//    using namespace ranges;
//    using x = typelist_unique_t<typelist<int, int, int>>;
//    static_assert(std::is_same<x, typelist<int>>::value, "");
//
//    using y = typelist_unique_t<typelist<int, short, int>>;
//    static_assert(std::is_same<y, typelist<int, short>>::value, "");
//
//    std::cout << "here1\n";
//    variant<int, char const *, short, char, S, wchar_t, long, float> v(S{});
//    v.apply(disp{});
//    v = 3.14f;
//    v.apply(disp{});
//    variant<int, void_, float> vv = v.apply(my_visitor{});
//    assert(typeid(float)==vv.type());
//    v = S{};
//    v.apply(disp{});
//    std::cout << "here2\n";
//}
