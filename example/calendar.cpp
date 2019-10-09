// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <range/v3/detail/config.hpp>

#if RANGES_CXX_RETURN_TYPE_DEDUCTION >= RANGES_CXX_RETURN_TYPE_DEDUCTION_14 && \
    RANGES_CXX_GENERIC_LAMBDAS >= RANGES_CXX_GENERIC_LAMBDAS_14

///[calendar]

// Usage:
//     calendar 2015
//
// Output:
/*
     January 2015         February 2015          March 2015
              1  2  3   1  2  3  4  5  6  7   1  2  3  4  5  6  7
  4  5  6  7  8  9 10   8  9 10 11 12 13 14   8  9 10 11 12 13 14
 11 12 13 14 15 16 17  15 16 17 18 19 20 21  15 16 17 18 19 20 21
 18 19 20 21 22 23 24  22 23 24 25 26 27 28  22 23 24 25 26 27 28
 25 26 27 28 29 30 31                        29 30 31

      April 2015             May 2015             June 2015
           1  2  3  4                  1  2      1  2  3  4  5  6
  5  6  7  8  9 10 11   3  4  5  6  7  8  9   7  8  9 10 11 12 13
 12 13 14 15 16 17 18  10 11 12 13 14 15 16  14 15 16 17 18 19 20
 19 20 21 22 23 24 25  17 18 19 20 21 22 23  21 22 23 24 25 26 27
 26 27 28 29 30        24 25 26 27 28 29 30  28 29 30
                       31
      July 2015           August 2015         September 2015
           1  2  3  4                     1         1  2  3  4  5
  5  6  7  8  9 10 11   2  3  4  5  6  7  8   6  7  8  9 10 11 12
 12 13 14 15 16 17 18   9 10 11 12 13 14 15  13 14 15 16 17 18 19
 19 20 21 22 23 24 25  16 17 18 19 20 21 22  20 21 22 23 24 25 26
 26 27 28 29 30 31     23 24 25 26 27 28 29  27 28 29 30
                       30 31
     October 2015         November 2015        December 2015
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


#include <algorithm>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <range/v3/action/join.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/mismatch.hpp>
#include <range/v3/core.hpp>
#include <range/v3/iterator/stream_iterators.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/group_by.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "date.h"

using namespace ranges;

// Characters per week line.
const int chars_per_week = 22;

// Spaces per day
const int spaces_per_day = 3;


// This template wrapper class provides the ability to act both as an
// "incrementable" class which can be used with ranges and also a
// "date" class which can be used to calculate year/month/day and other
// values.
template< class T >
class CalDateWrapper
{
  public:

    using value_type = T;

    using difference_type = T;

    CalDateWrapper( ) : m_Date( )
    {

    }

    CalDateWrapper( const date::year_month_day &d_ )
      : m_Date( d_ )
    {
    }

    operator T( ) const
    {
      return date::sys_days( m_Date ).time_since_epoch( ).count( );
    }

    // helpers

    int week_day_count( )
    {
      date::year_month_weekday wd( m_Date );
      date::days days = wd.weekday( ) - date::Sunday;
      return days.count( );
    }

    int week_number_in_month( ) const
    {
      // Get the first Sunday before the current date.
      date::year_month_day date1 = m_Date.year( ) / m_Date.month( ) / 1;
      date::weekday weekday = date::sys_days( date1 );
      date1 = date::sys_days( date1 ) - ( weekday - date::Sunday );
      auto d1 = date::sys_days( date1 );
      auto d2 = date::sys_days( m_Date );
      int wn = std::chrono::duration_cast< date::weeks >( d2 - d1 ).count( ) + 1;
      return wn;
    }

    // modifiers

    CalDateWrapper &operator++( )
    {
      date::sys_days d( m_Date );
      d = d + date::days{ 1 };
      m_Date = d;
      return *this;
    }

    CalDateWrapper operator++( int )
    {
      CalDateWrapper< T > d = *this;
      ++( *this );
      return d;
    }

    // accessors

    date::year_month_day ymd( )
    {
      return m_Date;
    }

    date::year year( )
    {
      return m_Date.year( );
    }

    date::month month( )
    {
      return m_Date.month( );
    }

    date::day day( )
    {
      return m_Date.day( );
    }

  private:

    date::year_month_day m_Date;
};


using CalDate = CalDateWrapper< date::sys_days::rep >;


namespace ranges
{
    template< >
    struct incrementable_traits< CalDate >
    {
        using difference_type = CalDate::difference_type;
    };
}

CPP_assert( incrementable< CalDate > );

auto
dates(unsigned short start, unsigned short stop)
{
    const CalDate startDate( CalDate{ date::year( start ) / date::jan / 1 } );
    const CalDate stopDate(  CalDate{ date::year( stop  ) / date::jan / 1 } );
    return views::iota( startDate, stopDate  );
}

auto
dates_from(unsigned short startYear)
{
    const CalDate startDate{ date::year( startYear ) / date::jan / 1 };
    return views::iota( startDate );
}

auto
by_month()
{
    return views::group_by(
        []( CalDate a, CalDate b )
    {
      return a.month( ) == b.month( );
    });
}

auto
by_week()
{
    return views::group_by([](CalDate a, CalDate b) {
        int weekA = a.week_number_in_month( );
        int weekB = b.week_number_in_month( );
        return weekA == weekB;
    });
}

std::string
format_day(CalDate d)
{
    // 3 spaces for each day.
    std::stringstream ss;
    ss << std::setw( spaces_per_day ) << std::setfill( ' ' ) << (int) (unsigned) d.day( );
    return ss.str( );
}

// In:  range<range<CalDate>>: month grouped by weeks.
// Out: range<std::string>: month with formatted weeks.
auto
format_weeks()
{
    return views::transform([](/*range<CalDate>*/ auto week) {
        std::stringstream ss;
        ss << std::string( front( week ).week_day_count( ) * spaces_per_day, ' ' );
        size_t len = ss.str( ).length( );
        ss << (week | views::transform( format_day ) | actions::join );
        ss << std::string( chars_per_week - len, ' ' );
        return ss.str( ).substr( 0, chars_per_week );
    });
}

// Return a formatted string with the title of the month
// corresponding to a date.
std::string
month_title(CalDate d)
{
    std::stringstream ss;
    ss << date::format( "%B", d.ymd( ) ) << " " << d.year( );
    std::string longMonth = ss.str( );
    ss.str( "" );
    const size_t totalSize = chars_per_week;
    ss << std::string( ( totalSize - longMonth.length( ) ) / 2, ' ' );
    ss << longMonth;
    ss << std::string( ( totalSize - longMonth.length( ) ) / 2, ' ' );
    return ss.str( );
}

// In:  range<range<CalDate>>: year of months of days
// Out: range<range<std::string>>: year of months of formatted wks
auto
layout_months()
{
    return views::transform([](/*range<CalDate>*/ auto month) {
        auto week_count =
            static_cast<std::ptrdiff_t>(distance(month | by_week()));
        return views::concat(
            views::single(month_title(front(month))),
            month | by_week() | format_weeks(),
            views::repeat_n(std::string( chars_per_week, ' '), 6 - week_count));
    });
}

// Flattens a range of ranges by iterating the inner
// ranges in round-robin fashion.
template<class Rngs>
class interleave_view : public view_facade<interleave_view<Rngs>>
{
    friend range_access;
    std::vector<range_value_t<Rngs>> rngs_;
    struct cursor;
    cursor begin_cursor()
    {
        return {0, &rngs_, views::transform(rngs_, ranges::begin) | to<std::vector>};
    }

public:
    interleave_view() = default;
    explicit interleave_view(Rngs rngs)
      : rngs_(std::move(rngs) | to<std::vector>)
    {}
};

template<class Rngs>
struct interleave_view<Rngs>::cursor
{
    std::size_t n_;
    std::vector<range_value_t<Rngs>> *rngs_;
    std::vector<iterator_t<range_value_t<Rngs>>> its_;
    decltype(auto) read() const
    {
        return *its_[n_];
    }
    void next()
    {
        if(0 == ((++n_) %= its_.size()))
            for_each(its_, [](auto &it) { ++it; });
    }
    bool equal(default_sentinel_t) const
    {
        if(n_ != 0)
            return false;
        auto ends = *rngs_ | views::transform(ranges::end);
        return its_.end() != std::mismatch(
            its_.begin(), its_.end(), ends.begin(), std::not_equal_to<>{}).first;
    }
    CPP_member
    auto equal(cursor const& that) const -> CPP_ret(bool)(
        requires forward_range<range_value_t<Rngs>>)
    {
        return n_ == that.n_ && its_ == that.its_;
    }
};

// In:  range<range<T>>
// Out: range<T>, flattened by walking the ranges
//                round-robin fashion.
auto
interleave()
{
    return make_view_closure([](auto &&rngs) {
        using Rngs = decltype(rngs);
        return interleave_view<views::all_t<Rngs>>(
            views::all(std::forward<Rngs>(rngs)));
    });
}

// In:  range<range<T>>
// Out: range<range<T>>, transposing the rows and columns.
auto
transpose()
{
    return make_view_closure([](auto &&rngs) {
        using Rngs = decltype(rngs);
        CPP_assert(forward_range<Rngs>);
        return std::forward<Rngs>(rngs)
            | interleave()
            | views::chunk(static_cast<std::size_t>(distance(rngs)));
    });
}

// In:  range<range<range<string>>>
// Out: range<range<range<string>>>, transposing months.
auto
transpose_months()
{
    return views::transform(
        [](/*range<range<string>>*/ auto rng) { return rng | transpose(); });
}

// In:  range<range<string>>
// Out: range<string>, joining the strings of the inner ranges
auto
join_months()
{
    return views::transform(
        [](/*range<string>*/ auto rng) { return actions::join(rng); });
}

// In:  range<CalDate>
// Out: range<string>, lines of formatted output
auto
format_calendar(std::size_t months_per_line)
{
    return
        // Group the dates by month:
        by_month()
        // Format the month into a range of strings:
      | layout_months()
        // Group the months that belong side-by-side:
      | views::chunk(months_per_line)
        // Transpose the rows and columns of the size-by-side months:
      | transpose_months()
        // Ungroup the side-by-side months:
      | views::join
        // Join the strings of the transposed months:
      | join_months();
}


void usage( )
{
  std::cout << "Allowed options:"                              << std::endl;
  std::cout << "  --help               produce help message"   << std::endl;
  std::cout << "  --start arg          Year to start"          << std::endl;
  std::cout << "  --stop arg           Year to stop"           << std::endl;
  std::cout << "  --per-line arg (=3)  Nbr of months per line" << std::endl;
}


int
main(int argc, char *argv[]) try
{
    // Configuration.
    bool help            = false;
    auto start           = 0;
    auto stop            = 0;
    auto months_per_line = 3;

    for ( int i = 1; i < argc; ++i )
    {
      std::string arg = argv[ i ];

      if ( arg == "--help" )
      {
        help = true;
        continue;
      }

      if ( arg == "--start" )
      {
        arg = argv[ ++i ];
        start = atoi( arg.c_str( ) );
        continue;
      }

      if ( arg == "--stop" )
      {
        arg = argv[ ++i ];
        stop = atoi( arg.c_str( ) );
        continue;
      }

      if ( arg == "--per-line" )
      {
        arg = argv[ ++i ];
        months_per_line = atoi( arg.c_str( ) );
        continue;
      }

      if ( !arg.empty( ) )
      {
        start = atoi( arg.c_str( ) );
        stop  = start + 1;
      }
    }

    if ( help || ( start == 0 && stop == 0 ) )
    {
      usage( );
      return 0;
    }

    if(stop != (unsigned short)-1 && stop <= start)
    {
        std::cerr << "ERROR: The stop year must be larger than the start"
                  << '\n';
        return 1;
    }

    if((unsigned short)-1 != stop)
    {
        copy(dates(start, stop) | format_calendar(months_per_line),
             ostream_iterator<>(std::cout, "\n"));
    }
    else
    {
        copy(dates_from(start) | format_calendar(months_per_line),
             ostream_iterator<>(std::cout, "\n"));
    }
}
catch(std::exception &e)
{
    std::cerr << "ERROR: Unhandled exception\n";
    std::cerr << "  what(): " << e.what();
    return 1;
}
///[calendar]

#else
#pragma message( \
    "calendar requires C++14 return type deduction and generic lambdas")
int
main()
{}
#endif
