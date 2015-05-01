// Range v3 library
//
//  Copyright Eric Niebler 2013-2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Usage:
//     calendar 2015
//
// Output:
/*
        January              February                March
              1  2  3   1  2  3  4  5  6  7   1  2  3  4  5  6  7
  4  5  6  7  8  9 10   8  9 10 11 12 13 14   8  9 10 11 12 13 14
 11 12 13 14 15 16 17  15 16 17 18 19 20 21  15 16 17 18 19 20 21
 18 19 20 21 22 23 24  22 23 24 25 26 27 28  22 23 24 25 26 27 28
 25 26 27 28 29 30 31                        29 30 31

         April                  May                  June
           1  2  3  4                  1  2      1  2  3  4  5  6
  5  6  7  8  9 10 11   3  4  5  6  7  8  9   7  8  9 10 11 12 13
 12 13 14 15 16 17 18  10 11 12 13 14 15 16  14 15 16 17 18 19 20
 19 20 21 22 23 24 25  17 18 19 20 21 22 23  21 22 23 24 25 26 27
 26 27 28 29 30        24 25 26 27 28 29 30  28 29 30
                       31
         July                 August               September
           1  2  3  4                     1         1  2  3  4  5
  5  6  7  8  9 10 11   2  3  4  5  6  7  8   6  7  8  9 10 11 12
 12 13 14 15 16 17 18   9 10 11 12 13 14 15  13 14 15 16 17 18 19
 19 20 21 22 23 24 25  16 17 18 19 20 21 22  20 21 22 23 24 25 26
 26 27 28 29 30 31     23 24 25 26 27 28 29  27 28 29 30
                       30 31
        October              November              December
              1  2  3   1  2  3  4  5  6  7         1  2  3  4  5
  4  5  6  7  8  9 10   8  9 10 11 12 13 14   6  7  8  9 10 11 12
 11 12 13 14 15 16 17  15 16 17 18 19 20 21  13 14 15 16 17 18 19
 18 19 20 21 22 23 24  22 23 24 25 26 27 28  20 21 22 23 24 25 26
 25 26 27 28 29 30 31  29 30                 27 28 29 30 31
// */

// Credits:
//   Thanks to H. S. Teoh for the article that served as the
//     inspiration for this example:
//     <http://wiki.dlang.org/Component_programming_with_ranges>
//   Thanks to github's Arzar for bringing date::week_number
//     to my attention.

#include <cstddef>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/group_by.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/action/join.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/mismatch.hpp>

namespace greg = boost::gregorian;
using date = greg::date;
using day = greg::date_duration;
using namespace ranges;
using std::cout;

namespace boost { namespace gregorian {
    date &operator++(date &d) { return d = d + day(1); }
    date operator++(date &d, int) { return ++d - day(1); }
}}
namespace ranges {
    template<> struct difference_type<date> {
        using type = date::duration_type::duration_rep::int_type;
    };
}
CONCEPT_ASSERT(Incrementable<date>());

auto dates_in_year(int year) {
    return view::iota(date{year,greg::Jan,1},
                      date{year+1,greg::Jan,1});
}

auto by_month() {
    return view::group_by([](date a, date b) {
        return a.month() == b.month();
    });
}

auto by_week() {
    return view::group_by([](date a, date b) {
        // ++a because week_numer is Mon-Sun and we want Sun-Sat
        return (++a).week_number() == (++b).week_number();
    });
}

std::string format_day(date d) {
    return boost::str(boost::format("%|3|") % d.day());
}

// In:  Range<Range<date>>: month grouped by weeks.
// Out: Range<std::string>: month with formatted weeks.
auto format_weeks() {
    return view::transform([](/*Range<date>*/ auto week) {
        return boost::str(boost::format("%1%%2%%|22t|")
            % std::string((int)front(week).day_of_week() * 3, ' ')
            % (week | view::transform(format_day) | action::join));
    });
}

// Return a formatted string with the title of the month
// corresponding to a date.
std::string month_title(date d) {
    return boost::str(boost::format("%|=22|")
        % d.month().as_long_string());
}

// In:  Range<Range<date>>: year of months of days
// Out: Range<Range<std::string>>: year of months of formatted wks
auto layout_months() {
    return view::transform([](/*Range<date>*/ auto month) {
        int week_count = distance(month | by_week());
        return view::concat(
            view::single(month_title(front(month))),
            month | by_week() | format_weeks(),
            view::repeat_n(std::string(22,' '),6-week_count));
    });
}

// In:  Range<T>
// Out: Range<Range<T>>, where each inner range has $n$ elements.
//                       The last range may have fewer.
template<class Rng>
class chunk_view : public range_adaptor<chunk_view<Rng>, Rng> {
    CONCEPT_ASSERT(ForwardIterable<Rng>());
    std::size_t n_;
    friend range_access;
    class adaptor;
    adaptor begin_adaptor() {
        return adaptor{n_, ranges::end(this->base())};
    }
public:
    chunk_view() = default;
    chunk_view(Rng rng, std::size_t n)
      : range_adaptor_t<chunk_view>(std::move(rng)), n_(n)
    {}
};

template<class Rng>
class chunk_view<Rng>::adaptor : public adaptor_base {
    std::size_t n_;
    range_sentinel_t<Rng> end_;
    using adaptor_base::prev;
public:
    adaptor() = default;
    adaptor(std::size_t n, range_sentinel_t<Rng> end)
      : n_(n), end_(end)
    {}
    auto current(range_iterator_t<Rng> it) const {
        return view::take(make_range(std::move(it), end_), n_);
    }
    void next(range_iterator_t<Rng> &it) {
        ranges::advance(it, n_, end_);
    }
};

// In:  Range<T>
// Out: Range<Range<T>>, where each inner range has $n$ elements.
//                       The last range may have fewer.
auto chunk(std::size_t n) {
    return make_pipeable([=](auto&& rng) {
        using Rng = decltype(rng);
        return chunk_view<view::all_t<Rng>>{
            view::all(std::forward<Rng>(rng)), n};
    });
}

// Flattens a range of ranges by iterating the inner
// ranges in round-robin fashion.
template<class Rngs>
class interleave_view : public range_facade<interleave_view<Rngs>> {
    friend range_access;
    std::vector<range_value_t<Rngs>> rngs_;
    struct cursor;
    cursor begin_cursor() {
        return {0, &rngs_, view::transform(rngs_, ranges::begin)};
    }
public:
    interleave_view() = default;
    explicit interleave_view(Rngs rngs)
      : rngs_(std::move(rngs))
    {}
};

template<class Rngs>
struct interleave_view<Rngs>::cursor  {
    std::size_t n_;
    std::vector<range_value_t<Rngs>> *rngs_;
    std::vector<range_iterator_t<range_value_t<Rngs>>> its_;
    decltype(auto) current() const {
        return *its_[n_];
    }
    void next() {
        if(0 == ((++n_) %= its_.size()))
            for_each(its_, [](auto& it){ ++it; });
    }
    bool done() const {
        return n_ == 0 && its_.end() != mismatch(its_,
            view::transform(*rngs_, ranges::end), std::not_equal_to<>()).first;
    }
    CONCEPT_REQUIRES(ForwardIterable<range_value_t<Rngs>>())
    bool equal(cursor const& that) const {
        return n_ == that.n_ && its_ == that.its_;
    }
};

// In:  Range<Range<T>>
// Out: Range<T>, flattened by walking the ranges
//                round-robin fashion.
auto interleave() {
    return make_pipeable([](auto&& rngs) {
        using Rngs = decltype(rngs);
        return interleave_view<view::all_t<Rngs>>(
            view::all(std::forward<Rngs>(rngs)));
    });
}

// In:  Range<Range<T>>
// Out: Range<Range<T>>, transposing the rows and columns.
auto transpose() {
    return make_pipeable([](auto&& rngs) {
        using Rngs = decltype(rngs);
        CONCEPT_ASSERT(ForwardIterable<Rngs>());
        return std::forward<Rngs>(rngs)
            | interleave()
            | chunk(distance(rngs));
    });
}

// In:  Range<Range<Range<string>>>
// Out: Range<Range<Range<string>>>, transposing months.
auto transpose_months() {
    return view::transform([](/*Range<Range<string>>*/ auto rng) {
        return rng | transpose();
    });
}

// In:  Range<Range<string>>
// Out: Range<string>, joining the strings of the inner ranges
auto join_months() {
    return view::transform([](/*Range<string>*/ auto rng) {
        return action::join(rng);
    });
}

int main(int argc, char *argv[]) try {
    if(argc < 2) {
        std::cerr << "Please enter the year to format.\n";
        std::cerr << boost::format("  Usage: %1% <year>\n") % argv[0];
        return 1;
    }

    int year = boost::lexical_cast<int>(argv[1]);
    int months_per_line = 3;

    auto calendar =
        // Make a range of all the dates in a year:
        dates_in_year(year)
            // Group the dates by month:
          | by_month()
            // Format the month into a range of strings:
          | layout_months()
            // Group the months that belong side-by-side:
          | chunk(months_per_line)
            // Transpose the rows and columns of the size-by-side months:
          | transpose_months()
            // Ungroup the side-by-side months:
          | view::join
            // Join the strings of the transposed months:
          | join_months();

    // Write the result to stdout:
    copy(calendar, ostream_iterator<>(std::cout, "\n"));
}
catch(std::exception &e) {
    std::cerr << "ERROR: Unhandled exception\n";
    std::cerr << "  what(): " << e.what();
    return 1;
}
