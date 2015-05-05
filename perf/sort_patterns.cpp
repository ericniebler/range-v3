/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2015
//  Copyright Gonzalo Brito Gadeschi 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <functional>
#include <climits>
#include <chrono>
#include <algorithm>
#include <range/v3/all.hpp>

#ifdef RANGES_CXX_GREATER_THAN_11

/// Creates an geometric infinite sequence starting at 1 where the
/// successor is multiplied by \p V
auto geometric_sequence(std::size_t V) {
  std::size_t N = 0;
  return ranges::view::generate([N, V]() mutable {
    if (N == 0) {
      N = 1;
      return N;
    }
    N *= V;
    return N;
  });
}

/// Creates an geometric infinite sequence starting at 1 where the
/// successor is multiplied by \p V
auto geometric_sequence_n(std::size_t V, std::size_t limit) {
  return geometric_sequence(V) |
         ranges::view::take_while([limit](auto n) { return n <= limit; });
}

/// Random uniform integer sequence
struct random_uniform_integer_sequence {
  std::default_random_engine gen;
  std::uniform_int_distribution<> dist;
  random_uniform_integer_sequence() : dist(INT_MIN, INT_MAX) {}
  auto operator()(std::size_t) {
    return ranges::view::generate([&]() mutable { return dist(gen); });
  }
  static std::string name() { return "random_uniform_integer_sequence"; }
};

struct ascending_integer_sequence {
  auto operator()(std::size_t) { return ranges::view::ints(1); }
  static std::string name() { return "ascending_integer_sequence"; }
};

struct descending_integer_sequence {
  auto operator()(std::size_t) {
    return ranges::view::iota(0ll, std::numeric_limits<long long>::max()) |
           ranges::view::reverse;
  }
  static std::string name() { return "descending_integer_sequence"; }
};

bool even(int i) { return i % 2 == 0; }
bool odd(int i) { return !even(i); }

struct even_odd_integer_sequence {
  static std::string name() { return "even_odd_integer_sequence"; }
  auto operator()(std::size_t n) {
    return ranges::view::concat(ranges::view::ints(std::size_t{0}, n) | ranges::view::filter(even),
                                ranges::view::ints(std::size_t{0}, n) | ranges::view::filter(odd));
  }
};

struct organ_pipe_integer_sequence {
  static std::string name() { return "organ_pipe_integer_sequence"; }
  auto operator()(std::size_t n) {
    return ranges::view::concat(ranges::view::ints(std::size_t{0}, n/2),
                                ranges::view::ints(std::size_t{0}, n/2 + 1)
                                | ranges::view::reverse);
  }
};

template<typename Seq>
void print(Seq seq, std::size_t n) {
  std::cout << "sequence: " << seq.name() << "\n";
  RANGES_FOR(auto i , seq(n) | ranges::view::take(n)) {
    std::cout << i << "\n";
  }
}

/// Returns the duration of a computation
template <typename Computation, typename Duration = std::chrono::milliseconds>
auto duration(Computation &&c) {
  auto time = []() { return std::chrono::high_resolution_clock::now(); };
  const auto start = time();
  c();
  const auto end = time();
  return std::chrono::duration_cast<Duration>(end - start);
}

template <typename Durations> auto compute_mean(Durations &&durations) {
  auto mean = 0ll;
  for (auto &&i : durations) {
    mean += i.count();
  }
  return mean / static_cast<long long>(durations.size());
}

template <typename Durations> auto compute_max(Durations &&durations) {
  return ranges::accumulate(
      durations, std::numeric_limits<long long>::min(),
      [](auto acc, auto i) { return std::max(acc, (long long)i.count()); });
}

template <typename Durations> auto compute_min(Durations &&durations) {
  return ranges::accumulate(
      durations, std::numeric_limits<long long>::max(),
      [](auto acc, auto i) { return std::min(acc, (long long)i.count()); });
}

template <typename Durations> auto compute_stddev(Durations &&durations) {
  auto mean = compute_mean(durations);
  auto stddev = 0ll;
  for (auto &&i : durations) {
    stddev += std::pow(i.count() - mean, 2);
  }
  return std::sqrt(stddev / durations.size());
}

struct benchmark {
  using duration_t = std::chrono::milliseconds;
  struct result_t {
    long long mean_t;
    long long max_t;
    long long min_t;
    std::size_t size;
    double deviation;
  };
  std::vector<result_t> results;

  template <typename Computation, typename Sizes>
  benchmark(Computation &&c, Sizes &&sizes, double target_deviation = 0.25,
            std::size_t max_iters = 100, std::size_t min_iters = 5) {

    RANGES_FOR(auto size, sizes) {
      std::vector<duration_t> durations;
      double deviation = std::numeric_limits<double>::max();
      long long mean_duration = -1;
      std::size_t iter = 0;

      do {
        c.init(size);
        durations.emplace_back(duration(c));
        mean_duration = compute_mean(durations);
        if (++iter == max_iters) {
          break;
        }
        if (durations.size() < min_iters) {
          continue;
        }
        deviation = compute_stddev(durations);
      } while (deviation > target_deviation * mean_duration);
      auto max_duration = compute_max(durations);
      auto min_duration = compute_min(durations);
      results.emplace_back(
          result_t{mean_duration, max_duration, min_duration, size, deviation});
      std::cerr << "size: " << size << " iter: " << iter
                << " dev: " << deviation << " mean: " << mean_duration
                << " max: " << max_duration << " min: " << min_duration << "\n";
    }
  }
};

template <typename Seq, typename Comp> struct computation_on_sequence {
  Seq seq;
  Comp comp;
  std::vector<ranges::range_value_t<decltype(seq(std::size_t{}))>> data;
  computation_on_sequence(Seq s, Comp c, std::size_t max_size)
      : seq(std::move(s)), comp(std::move(c)) {
    data.reserve(max_size);
  }
  void init(std::size_t size) {
    data.resize(size);
    ranges::copy(seq(size) | ranges::view::take(size), ranges::begin(data));
  }
  void operator()() { comp(data); }
};

template <typename Seq, typename Comp>
auto make_computation_on_sequence(Seq s, Comp c, std::size_t max_size) {
  return computation_on_sequence<Seq, Comp>(std::move(s), std::move(c),
                                            max_size);
}

template <typename Seq> void benchmark_sort(Seq &&seq, std::size_t max_size) {
  auto ranges_sort_comp =
      make_computation_on_sequence(seq, ranges::sort, max_size);

  auto std_sort_comp = make_computation_on_sequence(
      seq, [](auto &&v) { std::sort(std::begin(v), std::end(v)); }, max_size);

  auto ranges_sort_benchmark =
      benchmark(ranges_sort_comp, geometric_sequence_n(2, max_size));

  auto std_sort_benchmark =
      benchmark(std_sort_comp, geometric_sequence_n(2, max_size));
  using std::setw;
  std::cout << "#"
            << "pattern: " << seq.name() << "\n";
  std::cout << "#" << setw(19) << "N" << setw(20) << "ranges::sort" << setw(20)
            << "std::sort"
            << "\n";
  RANGES_FOR(auto p, ranges::view::zip(ranges_sort_benchmark.results,
                                       std_sort_benchmark.results)) {
    auto rs = p.first;
    auto ss = p.second;

    std::cout << setw(20) << rs.size << setw(20) << rs.mean_t << setw(20)
              << ss.mean_t << "\n";
  }
}

int main() {
  constexpr std::size_t max_size = 2000000;

  print(random_uniform_integer_sequence(), 20);
  print(ascending_integer_sequence(), 20);
  print(descending_integer_sequence(), 20);
  print(even_odd_integer_sequence(), 20);
  print(organ_pipe_integer_sequence(), 20);


  benchmark_sort(random_uniform_integer_sequence(), max_size);
  benchmark_sort(ascending_integer_sequence(), max_size);
  benchmark_sort(descending_integer_sequence(), max_size);
  benchmark_sort(organ_pipe_integer_sequence(), max_size);

  return 0;
}


#else

#pragma message("sort_patterns requires C++ 14 or greater")

int main() { return 0; }

#endif
