//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_UTILITY_RETURNS_HPP
#define RANGES_V3_UTILITY_RETURNS_HPP

/// RETURNS() is used to avoid writing boilerplate
/// "->decltype(x) { return x; }" phrases.
//
/// USAGE: auto function(<arguments>) RETURNS(<some-expression>);
///
/// Note: we end with a unique typedef so the function can be followed
/// by a semicolon.  If we omit the semicolon, editors get confused and
/// think we haven't completed the function declaration.
///
/// Author: Dave Abrahams, see
/// https://groups.google.com/forum/#!msg/boost-devel-archive/OzJ5Ft3pSnU\
/// /b_Ter9bgNqAJ .
///
/// \todo Eric Niebler discusses how to improve errors messages when combining
/// the RETURNS macro with SFINAE for expressions here:
/// https://groups.google.com/forum/#!topic/boost-developers-archive\
/// /Ipn1bF24STc%5B1-25-false%5D .
///
#define RANGE_RETURNS(...)                                                \
  noexcept(noexcept(decltype(__VA_ARGS__)(std::move(__VA_ARGS__)))) \
   ->decltype(__VA_ARGS__) {                                        \
    return (__VA_ARGS__);                                           \
  }                                                                 \
  using RANGE_RETURNS_CAT(RANGE_RETURNS_, __LINE__) = int
// Standard PP concatenation formula
#define RANGE_RETURNS_CAT_0(x, y) x##y
#define RANGE_RETURNS_CAT(x, y) RANGE_RETURNS_CAT_0(x, y)

#endif
