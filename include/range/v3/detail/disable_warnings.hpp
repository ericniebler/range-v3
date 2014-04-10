/// \file \brief This file disables some warnings produced by the library
///
/// \warning This file has no include guards (it is supposed to be included
/// multiple times) and should always be paired with a:
///
/// #include <boost/v3/detail/re_enable_warnings.hpp>
///
/// The following warnings are disabled by this file:
/// -Wshadow
///

/// \note Works with GCC and Clang
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wshadow"
