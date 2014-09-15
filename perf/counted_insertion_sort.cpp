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

#include <chrono>
#include <iostream>
#include <range/v3/range.hpp>

class timer
{
private:
    std::chrono::high_resolution_clock::time_point start_;
public:
    timer()
    {
        reset();
    }
    void reset()
    {
        start_ = std::chrono::high_resolution_clock::now();
    }
    std::chrono::milliseconds elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start_);
    }
    friend std::ostream &operator<<(std::ostream &sout, timer const &t)
    {
        return sout << t.elapsed().count() << "ms";
    }
};

template<typename It>
struct forward_iterator
{
    It it_;

    template<typename U>
    friend struct forward_iterator;
public:
    typedef          std::forward_iterator_tag                 iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    forward_iterator() : it_() {}
    explicit forward_iterator(It it) : it_(it) {}

    reference operator*() const {return *it_;}
    pointer operator->() const {return it_;}

    forward_iterator& operator++() {++it_; return *this;}
    forward_iterator operator++(int)
        {forward_iterator tmp(*this); ++(*this); return tmp;}

    friend bool operator==(const forward_iterator& x, const forward_iterator& y)
        {return x.it_ == y.it_;}
    friend bool operator!=(const forward_iterator& x, const forward_iterator& y)
        {return !(x == y);}
};

template<typename I, typename V2>
I upper_bound_n(I begin, typename std::iterator_traits<I>::difference_type d, V2 const &val)
{
    while(0 != d)
    {
        auto half = d / 2;
        auto middle = std::next(begin, half);
        if(val < *middle)
            d = half;
        else
        {
            begin = ++middle;
            d -= half + 1;
        }
    }
    return begin;
}

template<typename I>
void insertion_sort_n(I begin, typename std::iterator_traits<I>::difference_type n)
{
    auto m = 0;
    for(auto it = begin; m != n; ++it, ++m)
    {
        auto insertion = upper_bound_n(begin, m, *it);
        ranges::rotate(insertion, it, std::next(it));
    }
}

template<typename I, typename S>
void insertion_sort(I begin, S end)
{
    for(auto it = begin; it != end; ++it)
    {
        auto insertion = ranges::upper_bound(begin, it, *it);
        ranges::rotate(insertion, it, std::next(it));
    }
}

template<typename Rng>
void insertion_sort(Rng && rng)
{
    ::insertion_sort(std::begin(rng), std::end(rng));
}

std::unique_ptr<int> data(int i)
{
    std::unique_ptr<int> a(new int[i]);
    auto rng = ranges::view::counted(a.get(), i);
    ranges::iota(rng, 0);
    return a;
}

void shuffle(int *a, int i)
{
    auto rng = ranges::view::counted(a, i);
    ranges::random_shuffle(rng);
}

constexpr int cloops = 3;

template<typename I>
void benchmark_n(int i)
{
    auto a = data(i);
    long ms = 0;
    for(int j = 0; j < cloops; ++j)
    {
        ::shuffle(a.get(), i);
        timer t;
        insertion_sort_n(I{a.get()}, i);
        ms += t.elapsed().count();
    }
    std::cout << (int)((double)ms/cloops) << std::endl;
}

template<typename I>
void benchmark_counted(int i)
{
    auto a = data(i);
    long ms = 0;
    for(int j = 0; j < cloops; ++j)
    {
        ::shuffle(a.get(), i);
        timer t;
        insertion_sort(ranges::view::counted(I{a.get()}, i));
        ms += t.elapsed().count();
    }
    std::cout << (int)((double)ms/cloops) << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
        return -1;

    int i = std::atoi(argv[1]);
    std::cout << "insertion_sort_n (random-access) : ";
    benchmark_n<int*>(i);
    std::cout << "insertion_sort   (random-access) : ";
    benchmark_counted<int*>(i);
    std::cout << "\n";
    std::cout << "insertion_sort_n (forward)       : ";
    benchmark_n<forward_iterator<int*>>(i);
    std::cout << "insertion_sort   (forward)       : ";
    benchmark_counted<forward_iterator<int*>>(i);
}
