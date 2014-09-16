#include <type_traits>
#include <utility>

struct empty
{};

template<typename T, typename U = void>
struct first_base
{
    T first;
};

template<typename T>
struct first_base<T, typename std::enable_if<std::is_empty<T>::value>::type>
{
    static T first; // Can there be a data race "mutating" an empty object?
    first_base() = default;
    constexpr explicit first_base(T const &)
    {}
};

template<typename T>
T first_base<T, typename std::enable_if<std::is_empty<T>::value>::type>::first{};

template<typename T, typename U = void>
struct second_base
{
    T second;
};

template<typename T>
struct second_base<T, typename std::enable_if<std::is_empty<T>::value>::type>
{
    static T second;
    second_base() = default;
    constexpr explicit second_base(T const &)
    {}
};

template<typename T>
T second_base<T, typename std::enable_if<std::is_empty<T>::value>::type>::second{};

template<typename First, typename Second>
struct compressed_pair
  : private first_base<First>
  , private second_base<Second>
{
    using first_base<First>::first;
    using second_base<Second>::second;

    compressed_pair() = default;

    constexpr compressed_pair(First f, Second s)
      : first_base<First>{(First &&) f}
      , second_base<Second>{(Second &&) s}
    {}

    template<typename F, typename S,
        typename std::enable_if<std::is_constructible<First, F &&>::value &&
                                std::is_constructible<Second, S &&>::value, int>::type = 0>
    constexpr compressed_pair(F && f, S && s)
      : first_base<First>{(F &&) f}
      , second_base<Second>{(S &&) s}
    {}
};

int main()
{
    static_assert(sizeof(compressed_pair<int, empty>) == sizeof(int), "");
    static_assert(sizeof(compressed_pair<empty, int>) == sizeof(int), "");
    static_assert(sizeof(compressed_pair<empty, empty>) == 1, "");
    static_assert(sizeof(compressed_pair<empty, compressed_pair<empty, empty>>) == 1, "");
    static_assert(sizeof(std::pair<int, empty>) > sizeof(compressed_pair<int, empty>), "");
    compressed_pair<int, empty> y{42,{}};
    compressed_pair<int, empty> x;
    empty e = x.second;
    x.second = e;
}


//// Boost.Range library
////
////  Copyright Eric Niebler 2013.
////
////  Use, modification and distribution is subject to the
////  Boost Software License, Version 1.0. (See accompanying
////  file LICENSE_1_0.txt or copy at
////  http://www.boost.org/LICENSE_1_0.txt)
////
//// For more information, see http://www.boost.org/libs/range/
////
//
//#include <chrono>
//#include <iostream>
//#include <range/v3/all.hpp>
//
//using namespace ranges;
//
//auto const intsFrom = view::iota;
//auto const ints = [](int i, int j){ return view::take(intsFrom(i), j-i+1); };
//
//class timer
//{
//private:
//    std::chrono::high_resolution_clock::time_point start_;
//public:
//    timer()
//    {
//        reset();
//    }
//    void reset()
//    {
//        start_ = std::chrono::high_resolution_clock::now();
//    }
//    std::chrono::milliseconds elapsed() const
//    {
//        return std::chrono::duration_cast<std::chrono::milliseconds>(
//            std::chrono::high_resolution_clock::now() - start_);
//    }
//    friend std::ostream &operator<<(std::ostream &sout, timer const &t)
//    {
//        return sout << t.elapsed().count() << "ms";
//    }
//};
//
//void benchmark()
//{
//    // Define an infinite range containing all the Pythagorean triples:
//    auto all_triples =
//        view::for_each(intsFrom(1), [](int z)
//        {
//            return view::for_each(ints(1, z), [=](int x)
//            {
//                return view::for_each(ints(x, z), [=](int y)
//                {
//                    return yield_if(x*x + y*y == z*z, std::make_tuple(x, y, z));
//                });
//            });
//        });
//
//    static constexpr int max_triples = 3000;
//    auto triples = view::take(all_triples, max_triples);
//
//    timer t;
//    int result = 0;
//    for_each(triples, [&](std::tuple<int, int, int> triple){
//        int i, j, k;
//        std::tie(i, j, k) = triple;
//        result += (i + j + k);
//    });
//    std::cout << t << '\n';
//    std::cout << result << '\n';
//
//    result = 0;
//    int found = 0;
//    t.reset();
//    for(int z = 1;; ++z)
//    {
//        for(int x = 1; x <= z; ++x)
//        {
//            for(int y = x; y <= z; ++y)
//            {
//                if(x*x + y*y == z*z)
//                {
//                    result += (x + y + z);
//                    ++found;
//                    if(found == max_triples)
//                        goto done;
//                }
//            }
//        }
//    }
//done:
//    std::cout << t << '\n';
//    std::cout << result << '\n';
//}
//
//int main()
//{
//    // Define an infinite range containing all the Pythagorean triples:
//    auto triples =
//        view::for_each(intsFrom(1), [](int z)
//        {
//            return view::for_each(ints(1, z), [=](int x)
//            {
//                return view::for_each(ints(x, z), [=](int y)
//                {
//                    return yield_if(x*x + y*y == z*z,
//                        std::make_tuple(x, y, z));
//                });
//            });
//        });
//
//    // Display the first 100 triples
//    for(auto triple : triples | view::take(100))
//    {
//        std::cout << '('
//            << std::get<0>(triple) << ','
//            << std::get<1>(triple) << ','
//            << std::get<2>(triple) << ')' << '\n';
//    }
//}
