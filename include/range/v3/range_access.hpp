// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_RANGE_ACCESS_HPP
#define RANGES_V3_RANGE_ACCESS_HPP

#include <cstddef>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{
        struct range_access
        {
            /// \cond

            //
            // Concepts that the range cursor must model
            // BUGBUG this doesn't handle weak cursors.
            //
            struct InputCursorConcept
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        //t.done(),
                        t.current(),
                        (t.next(), concepts::void_)
                    ));
            };
            struct ForwardCursorConcept
              : concepts::refines<InputCursorConcept>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t.equal(t))
                    ));
            };
            struct BidirectionalCursorConcept
              : concepts::refines<ForwardCursorConcept>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        (t.prev(), concepts::void_)
                    ));
            };
            struct RandomAccessCursorConcept
              : concepts::refines<BidirectionalCursorConcept>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SignedIntegral>(t.distance_to(t)),
                        (t.advance(t.distance_to(t)), concepts::void_)
                    ));
            };
            struct InfiniteCursorConcept
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(typename T::is_infinite{})
                    ));
            };

            template<typename Rng>
            static auto begin_cursor(Rng & rng) -> decltype(rng.begin_cursor())
            {
                return rng.begin_cursor();
            }
            template<typename Rng>
            static auto end_cursor(Rng & rng) -> decltype(rng.end_cursor())
            {
                return rng.end_cursor();
            }

            template<typename Rng>
            static auto begin_adaptor(Rng & rng) -> decltype(rng.begin_adaptor())
            {
                return rng.begin_adaptor();
            }
            template<typename Rng>
            static auto end_adaptor(Rng & rng) -> decltype(rng.end_adaptor())
            {
                return rng.end_adaptor();
            }

            template<typename Cur>
            static auto current(Cur const &pos) -> decltype(pos.current())
            {
                return pos.current();
            }
            template<typename Cur>
            static auto next(Cur & pos) -> decltype(pos.next())
            {
                pos.next();
            }
            template<typename Cur>
            static constexpr auto done(Cur const & pos) -> decltype(pos.done())
            {
                return pos.done();
            }
            template<typename Cur>
            static auto equal(Cur const &pos0, Cur const &pos1) ->
                decltype(pos0.equal(pos1))
            {
                return pos0.equal(pos1);
            }
            template<typename Cur, typename S>
            static constexpr auto empty(Cur const &pos, S const &end) ->
                decltype(end.equal(pos))
            {
                return end.equal(pos);
            }
            template<typename Cur>
            static auto prev(Cur & pos) -> decltype(pos.prev())
            {
                pos.prev();
            }
            template<typename Cur, typename D>
            static auto advance(Cur & pos, D n) ->
                decltype(pos.advance(n))
            {
                pos.advance(n);
            }
            template<typename Cur>
            static auto distance_to(Cur const &pos0, Cur const &pos1) ->
                decltype(pos0.distance_to(pos1))
            {
                return pos0.distance_to(pos1);
            }

        private:
            template<typename Cur>
            using random_access_cursor_difference_t =
                decltype(range_access::distance_to(std::declval<Cur>(), std::declval<Cur>()));

            template<typename Cur, typename Enable = void>
            struct cursor_difference2
            {
                using type = std::ptrdiff_t;
            };

            template<typename Cur>
            struct cursor_difference2<Cur, detail::void_t<random_access_cursor_difference_t<Cur>>>
            {
                using type = random_access_cursor_difference_t<Cur>;
            };

            template<typename Cur, typename Enable = void>
            struct cursor_difference
              : cursor_difference2<Cur>
            {};

            template<typename Cur>
            struct cursor_difference<Cur, detail::void_t<typename Cur::difference_type>>
            {
                using type = typename Cur::difference_type;
            };

            template<typename Cur, typename Enable = void>
            struct cursor_value
            {
                using type = uncvref_t<decltype(std::declval<Cur const &>().current())>;
            };

            template<typename Cur>
            struct cursor_value<Cur, detail::void_t<typename Cur::value_type>>
            {
                using type = typename Cur::value_type;
            };

            template<typename T, typename Enable = void>
            struct single_pass
            {
                using type = std::false_type;
            };

            template<typename T>
            struct single_pass<T, detail::void_t<typename T::single_pass>>
            {
                using type = typename T::single_pass;
            };
        public:
            template<typename Cur>
            using cursor_difference_t = typename cursor_difference<Cur>::type;

            template<typename Cur>
            using cursor_value_t = typename cursor_value<Cur>::type;

            template<typename Cur>
            using single_pass_t = typename single_pass<Cur>::type;

            template<typename Cur, typename S>
            static Cur cursor(basic_iterator<Cur, S> it)
            {
                return std::move(it.pos());
            }
            template<typename S>
            static S sentinel(basic_sentinel<S> s)
            {
                return std::move(s.end());
            }

            template<typename RangeAdaptor>
            struct base_range
            {
                using type = typename RangeAdaptor::base_range_t;
            };
            template<typename RangeAdaptor>
            struct base_range<RangeAdaptor const>
            {
                using type = typename RangeAdaptor::base_range_t const;
            };
            template<typename RangeFacade>
            struct range_facade
            {
                using type = typename RangeFacade::range_facade_t;
            };
            template<typename RangeAdaptor>
            struct range_adaptor
            {
                using type = typename RangeAdaptor::range_adaptor_t;
            };
            /// endcond
        };
        /// @}

        /// \cond
        namespace detail
        {
            template<typename T>
            using InputCursor =
                concepts::models<range_access::InputCursorConcept, T>;

            template<typename T>
            using ForwardCursor =
                concepts::models<range_access::ForwardCursorConcept, T>;

            template<typename T>
            using BidirectionalCursor =
                concepts::models<range_access::BidirectionalCursorConcept, T>;

            template<typename T>
            using RandomAccessCursor =
                concepts::models<range_access::RandomAccessCursorConcept, T>;

            template<typename T>
            using InfiniteCursor =
                concepts::models<range_access::InfiniteCursorConcept, T>;

            template<typename T>
            using cursor_concept =
                concepts::most_refined<
                    meta::list<
                        range_access::RandomAccessCursorConcept,
                        range_access::BidirectionalCursorConcept,
                        range_access::ForwardCursorConcept,
                        range_access::InputCursorConcept>, T>;

            template<typename T>
            using cursor_concept_t = meta::eval<cursor_concept<T>>;
        }
        /// \endcond
    }
}

#endif
