/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ITERATOR_STREAM_ITERATORS_HPP
#define RANGES_V3_ITERATOR_STREAM_ITERATORS_HPP

#include <cstddef>
#include <iosfwd>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/iterator/concepts.hpp>

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{
    template<typename T = void, typename Char = char,
             typename Traits = std::char_traits<Char>>
    struct ostream_iterator
    {
        using difference_type = std::ptrdiff_t;
        using char_type = Char;
        using traits_type = Traits;
        using ostream_type = std::basic_ostream<Char, Traits>;

        constexpr ostream_iterator() = default;
        ostream_iterator(ostream_type & s, Char const * d = nullptr) noexcept
          : sout_(&s)
          , delim_(d)
        {}
        template<typename U>
        auto operator=(U && value) -> CPP_ret(ostream_iterator &)( //
            requires convertible_to<
                U, detail::if_then_t<std::is_void<T>::value, U, T> const &>)
        {
            RANGES_EXPECT(sout_);
            *sout_ << value;
            if(delim_)
                *sout_ << delim_;
            return *this;
        }
        ostream_iterator & operator*()
        {
            return *this;
        }
        ostream_iterator & operator++()
        {
            return *this;
        }
        ostream_iterator & operator++(int)
        {
            return *this;
        }

    private:
        ostream_type * sout_;
        Char const * delim_;
    };

    template<typename Delim, typename Char = char,
             typename Traits = std::char_traits<Char>>
    struct ostream_joiner
    {
        CPP_assert(semiregular<Delim>);
        using difference_type = std::ptrdiff_t;
        using char_type = Char;
        using traits_type = Traits;
        using ostream_type = std::basic_ostream<Char, Traits>;

        constexpr ostream_joiner() = default;
        ostream_joiner(ostream_type & s, Delim const & d)
          : delim_(d)
          , sout_(std::addressof(s))
          , first_(true)
        {}
        ostream_joiner(ostream_type & s, Delim && d)
          : delim_(std::move(d))
          , sout_(std::addressof(s))
          , first_(true)
        {}
        template<typename T>
        ostream_joiner & operator=(T const & value)
        {
            RANGES_EXPECT(sout_);
            if(!first_)
                *sout_ << delim_;
            first_ = false;
            *sout_ << value;
            return *this;
        }
        ostream_joiner & operator*() noexcept
        {
            return *this;
        }
        ostream_joiner & operator++() noexcept
        {
            return *this;
        }
        ostream_joiner & operator++(int) noexcept
        {
            return *this;
        }

    private:
        Delim delim_;
        ostream_type * sout_;
        bool first_;
    };

    struct make_ostream_joiner_fn
    {
        template<typename Delim, typename Char, typename Traits>
        auto operator()(std::basic_ostream<Char, Traits> & s, Delim && d) const
            -> CPP_ret(ostream_joiner<detail::decay_t<Delim>, Char, Traits>)( //
                requires semiregular<detail::decay_t<Delim>>)
        {
            return {s, std::forward<Delim>(d)};
        }
    };

    /// \sa `make_ostream_joiner_fn`
    RANGES_INLINE_VARIABLE(make_ostream_joiner_fn, make_ostream_joiner)

    template<typename Char, typename Traits = std::char_traits<Char>>
    struct ostreambuf_iterator
    {
    public:
        typedef ptrdiff_t difference_type;
        typedef Char char_type;
        typedef Traits traits_type;
        typedef std::basic_streambuf<Char, Traits> streambuf_type;
        typedef std::basic_ostream<Char, Traits> ostream_type;

        constexpr ostreambuf_iterator() = default;
        ostreambuf_iterator(ostream_type & s) noexcept
          : ostreambuf_iterator(s.rdbuf())
        {}
        ostreambuf_iterator(streambuf_type * s) noexcept
          : sbuf_(s)
        {
            RANGES_ASSERT(s != nullptr);
        }
        ostreambuf_iterator & operator=(Char c)
        {
            RANGES_ASSERT(sbuf_ != nullptr);
            if(!failed_)
                failed_ = (sbuf_->sputc(c) == Traits::eof());
            return *this;
        }
        ostreambuf_iterator & operator*()
        {
            return *this;
        }
        ostreambuf_iterator & operator++()
        {
            return *this;
        }
        ostreambuf_iterator & operator++(int)
        {
            return *this;
        }
        bool failed() const noexcept
        {
            return failed_;
        }

    private:
        streambuf_type * sbuf_ = nullptr;
        bool failed_ = false;
    };

    namespace cpp20
    {
        template<typename T, typename Char = char,
                 typename Traits = std::char_traits<Char>>
        using ostream_iterator = ranges::ostream_iterator<T, Char, Traits>;

        using ranges::ostreambuf_iterator;
    } // namespace cpp20
    /// @}
} // namespace ranges

/// \cond
namespace std
{
    template<typename T, typename Char, typename Traits>
    struct iterator_traits<::ranges::ostream_iterator<T, Char, Traits>>
      : ::ranges::detail::std_output_iterator_traits<>
    {};

    template<typename Char, typename Traits>
    struct iterator_traits<::ranges::ostreambuf_iterator<Char, Traits>>
      : ::ranges::detail::std_output_iterator_traits<>
    {};
} // namespace std
/// \endcond

#endif // RANGES_V3_ITERATOR_STREAM_ITERATORS_HPP
