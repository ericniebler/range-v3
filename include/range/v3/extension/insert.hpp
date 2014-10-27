///////////////////////////////////////////////////////////////////////////////
/// \file insert.hpp
///   Contains range-based non-member function version of the STL containers'
///   insert member function.
//
// Copyright 2004 Eric Niebler.
// Copyright 2014 Gonzalo Brito Gadeschi.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef RANGES_V3_EXTENSION_INSERT_HPP
#define RANGES_V3_EXTENSION_INSERT_HPP

#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace concepts
        {
            /// An object of type O models Insertable if:
            ///
            ///    - O models the Iterable concept and provides an insert member
            ///      function:
            ///
            ///         std::declval<O>().insert(before, begin, end)
            ///
            ///      where
            ///
            ///      - [begin, end) is an IteratorRange whose elements are
            ///        convertible to the value_type of O, and
            ///      - before denotes a position within an Iterable of type O.
            ///
            ///
            ///  \warning if before does not denote a position within the
            ///  Iterable of type O the behavior is undefined.
            ///
            ///  \todo assert that before points to a
            ///  position within the Iterable of type O when it is cheap (O(1))
            ///  to do so.
            ///
            struct Insertable
            {
                template <typename O, typename P, typename B, typename E>
                auto requires_(O &&to, P &&p, B &&b, E &&e) -> decltype(
                    concepts::model_of<Iterable>(to),
                    concepts::model_of<IteratorRange>(b, e),
                    concepts::is_true(std::is_convertible<
                        iterator_value_t<B>, iterator_value_t<decltype(begin(to))>>()),
                    concepts::valid_expr(to.insert(p, b, e)));

                template <typename O, typename P, typename I>
                auto requires_(O &&to, P &&p, I &&from) -> decltype(
                    concepts::model_of<Iterable>(to), concepts::model_of<Iterable>(from),
                    concepts::is_true(
                        std::is_convertible<iterator_value_t<decltype(begin(from))>,
                                            iterator_value_t<decltype(begin(to))>>()),
                    concepts::valid_expr(to.insert(p, begin(from), end(from))));
            };

        } // namespace concepts

        template <typename... Args>
        using Insertable = concepts::models<concepts::Insertable, Args...>;

        struct insert_fn
        {
            template <typename I, typename S, typename O, typename OIt,
                      CONCEPT_REQUIRES_(Insertable<O, OIt, I, S>())>
            auto operator()(O &to, OIt &&before, I &&from_begin, S &&from_end) const
                RANGES_DECLTYPE_AUTO_RETURN(to.insert(std::forward<OIt>(before),
                                        std::forward<I>(from_begin),
                                        std::forward<S>(from_end)));

            template <typename I, typename O, typename OIt,
                      CONCEPT_REQUIRES_(Insertable<O, OIt, I>())>
            auto operator()(O &to, OIt &&before, I &&from) const
                RANGES_DECLTYPE_AUTO_RETURN((*this)(to, std::forward<OIt>(before),
                                      begin(std::forward<I>(from)),
                                      end(std::forward<I>(from))));
        };

        RANGES_CONSTEXPR insert_fn insert{};
    }
}

#endif
