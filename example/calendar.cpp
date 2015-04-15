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

#include <string>
#include <cstdio>
#include <iostream>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <range/v3/all.hpp>

namespace greg = boost::gregorian;
using date = greg::date;
using day = greg::date_duration;
using namespace ranges;

namespace boost {
namespace gregorian {
    // Bit of a hack:
    date &operator++(date &d) { return d = d + day(1); }
    date operator++(date &d, int) { return ++d - day(1); }
}}
namespace ranges {
    template<> struct difference_type<date> {
        using type = date::duration_type::duration_rep::int_type;
    };
}
CONCEPT_ASSERT(Incrementable<date>());

/// The number of columns per day in the formatted output.
constexpr int cols_per_day = 3;
 
/// The number of columns per week in the formatted output.
constexpr int cols_per_week = 7 * cols_per_day;

auto dates_in_year(int year) {
    return view::iota(date{year,greg::Jan,1},
                      date{year+1,greg::Jan,1}-day(1));
}

auto by_month() {
    return view::group_by([](date a, date b) {
        return a.month() == b.month();
    });
}

auto by_week() {
    return view::group_by([](date a, date b) {
        return b.day_of_week() != greg::Sunday || a == b;
    });
}

// Return a string containing exactly n spaces.
std::string spaces(size_t n) {
    return std::string(n, ' ');
}

std::string format_day(date d) {
    char buf[4];
    using namespace std;
    snprintf(buf, 4, "%3d", (int)d.day());
    return buf;
}

// Given a range of ranges of dates, where each inner range represents
// one week in a month, return a range of strings where each string
// is a formatted week.
auto format_week() {
    return view::transform([](auto week) {
        static_assert(21 == cols_per_week, "");
        return boost::str(
            boost::format("%1%%2%%|22t|")
              % spaces((int)front(week).day_of_week() * cols_per_day)
              % (week | view::transform(format_day) | action::join)
        );
    });
}

// Return a formatted string with the title of the month
// corresponding to a date.
std::string month_title(date d) {
    static constexpr char const *const s_months[] = {
        "January", "February", "March", "April",
        "May", "June", "July", "August", "September",
        "October", "November", "December"
    };
    return boost::str(boost::format("%|=22|")
        % s_months[(int)d.month()-1]);
}

// Turns a range of months (where each month is a range of days)
// into a range of ranges of strings, where each string is a
// formatted week. The inner range has exactly 7 lines.
auto format_month() {
    return view::transform([](auto month) {
        return view::concat(
            // One line for month title
            view::single(month_title(front(month))),
            // 4 to 6 lines of formatted weeks
            month | by_week() | format_week(),
            // 0 to 2 empty lines
            view::repeat_n(spaces(22), 6-distance(month|by_week()))
        );
    });
}

// Turn a range into a range of ranges where each inner range
// has $n$ elements. The last range may have fewer.
template<class Rng>
class chunk_view
  : public range_adaptor<chunk_view<Rng>, Rng> {
    CONCEPT_ASSERT(ForwardIterable<Rng>());
    std::size_t n_;
    friend range_access;
    class adaptor : public adaptor_base {
        chunk_view *rng_;
        using adaptor_base::prev;
    public:
        adaptor(chunk_view *p = nullptr) : rng_(p) {}
        auto current(range_iterator_t<Rng> it) const {
            return make_range(it,
                ranges::next(it, rng_->n_, ranges::end(rng_->base())));
        }
        void next(range_iterator_t<Rng> &it) {
            ranges::advance(it, rng_->n_, ranges::end(rng_->base()));
        }
    };
    adaptor begin_adaptor() { return adaptor{this}; }
public:
    chunk_view() = default;
    chunk_view(Rng rng, std::size_t n)
      : range_adaptor_t<chunk_view>(std::move(rng)), n_(n)
    {}
};

// A pipeable adaptor that chunks a range into a range of ranges
// each of $n$ elements.
auto chunk(std::size_t n) {
    return make_pipeable([=](auto&& rng) {
        using Rng = decltype(rng);
        return chunk_view<view::all_t<Rng>>{
            view::all(std::forward<Rng>(rng)), n};
    });
}

// Given a range of ranges, flatten by iterating in
// round-robin fashion.
template<class Rngs>
class interleave_view
  : public range_facade<interleave_view<Rngs>>
{
    friend range_access;
    std::vector<range_value_t<Rngs>> rngs_;
    struct cursor {
        using iterator_ = range_iterator_t<range_value_t<Rngs>>;
        std::vector<iterator_> its_;
        std::size_t n_;
        std::vector<range_value_t<Rngs>> const *rngs_;
        iterator_reference_t<iterator_> current() const {
            return *its_[n_];
        }
        void next() {
            if(0 == ((++n_) %= its_.size()))
                for_each(its_, [](auto& it) { ++it; });
        }
        bool done() const {
            return n_ == 0 &&
                any_of(view::zip(its_, view::transform(*rngs_, ranges::end)),
                       [](auto p) {return p.first==p.second;});
        }
        CONCEPT_REQUIRES(ForwardIterator<iterator_>())
        bool equal(cursor const& that) const {
            return n_ == that.n_ && its_ == that.its_;
        }
    };
    cursor begin_cursor() {
        return cursor{view::transform(rngs_, ranges::begin), 0, &rngs_};
    }
public:
    interleave_view() = default;
    explicit interleave_view(Rngs rngs)
      : rngs_(std::move(rngs))
    {}
};

auto interleave() {
    return make_pipeable([](auto&& rngs) {
        using Rngs = decltype(rngs);
        return interleave_view<view::all_t<Rngs>>(
            view::all(std::forward<Rngs>(rngs)));
    });
}

// Turn a range of ranges of ranges of dates, where the middle
// set of ranges represents a cluster of $n$ months, into a
// range of ranges of strings by formatting each month (turning
// it into a range of strings), and flattening the clusters of
// months in round-robin fashion, interleaving the weeks.
auto interleave_months() {
    return view::transform([=](auto months) {
        return months | format_month() | interleave();
    });
}

// Given a range of ranges of strings, return a range of strings
// by concatenating all the strings of each inner range.
auto join_lines() {
    return view::transform([](auto parts) {
        return action::join(parts);
    });
}

// Given a range of ranges of strings, where each inner range
// represents $n$ months that have been interleaved together,
// turn it into a range of range of strings where each inner
// range contains properly formatted lines for the $n$ months.
auto assemble_lines(int months_per_line) {
    return view::transform([=](auto hunk) {
        return hunk | chunk(months_per_line) | join_lines();
    });
}

int main(int argc, char *argv[]) try {
    if(argc < 2) {
        std::cerr << "Please enter the year to format.\n";
        return 1;
    }

    int year = boost::lexical_cast<int>(argv[1]);
    int months_per_line = 3;

    copy(
        // A range of dates:
        dates_in_year(year)
            // ... grouped into ranges of dates by month:
          | by_month()
            // ... further grouped into chunks of $n$ months:
          | chunk(months_per_line)
            // ... with their formatted weeks interleaved:
          | interleave_months()
            // ... and reassembled into lines:
          | assemble_lines(months_per_line)
            // ... and finally joined into one big range of lines.
          | view::join,
        // ... and written to stdout
        ostream_iterator<>(std::cout, "\n")
    );
}
catch(std::exception &e) {
    std::cerr << "ERROR: Unhandled exception\n";
    std::cerr << "  what(): " << e.what();
    return 1;
}
