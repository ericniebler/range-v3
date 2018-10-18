/// \file
// Range v3 library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_SAMPLE_HPP
#define RANGES_V3_VIEW_SAMPLE_HPP

#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/algorithm/shuffle.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Rng, bool = SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>::value>
            class size_tracker
            {
                range_difference_type_t<Rng> size_;
            public:
                CONCEPT_ASSERT(ForwardRange<Rng>() || SizedRange<Rng>());
                size_tracker() = default;
                size_tracker(Rng &rng)
                  : size_(ranges::distance(rng))
                {}
                void decrement()
                {
                    --size_;
                }
                range_difference_type_t<Rng> get(Rng &, iterator_t<Rng> &) const
                {
                    return size_;
                }
            };

            // Impl for SizedSentinel (no need to store anything)
            template<typename Rng>
            class size_tracker<Rng, true>
            {
            public:
                size_tracker() = default;
                size_tracker(Rng &)
                {}
                void decrement()
                {}
                range_difference_type_t<Rng> get(Rng &rng, iterator_t<Rng> const &it) const
                {
                    return ranges::end(rng) - it;
                }
            };
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{

        // Take a random sampling from another view
        template<typename Rng, typename URNG>
        class sample_view
          : public view_facade<sample_view<Rng, URNG>, finite>
          , tagged_compressed_tuple<
                tag::range(Rng),
                tag::size(mutable_<range_difference_type_t<Rng>>),
                tag::engine(reference_wrapper<URNG>)>
        {
            friend range_access;
            using D = range_difference_type_t<Rng>;
            using base_t = tagged_compressed_tuple<
                tag::range(Rng), tag::size(mutable_<D>), tag::engine(reference_wrapper<URNG>)>;
            using base_t::engine;
            using base_t::range;
            using base_t::size;

            template<bool IsConst>
            class cursor
              : tagged_compressed_tuple<
                    tag::range(meta::const_if_c<IsConst, sample_view> *),
                    tag::current(iterator_t<meta::const_if_c<IsConst, Rng>>),
                    tag::size(detail::size_tracker<meta::const_if_c<IsConst, Rng>>)>
            {
                friend class cursor<!IsConst>;
                using base_t = tagged_compressed_tuple<
                    tag::range(meta::const_if_c<IsConst, sample_view> *),
                    tag::current(iterator_t<meta::const_if_c<IsConst, Rng>>),
                    tag::size(detail::size_tracker<meta::const_if_c<IsConst, Rng>>)>;
                using base_t::current;
                using base_t::range;
                using base_t::size;

                D pop_size()
                {
                    RANGES_EXPECT(range());
                    return size().get(range()->range(), current());
                }
                void advance()
                {
                    RANGES_EXPECT(range());
                    if(range()->size() > 0)
                    {
                        using Dist = std::uniform_int_distribution<D>;
                        using Param_t = typename Dist::param_type;
                        Dist dist{};
                        URNG& engine = range()->engine().get();

                        for(; ; ++current(), size().decrement())
                        {
                            RANGES_ASSERT(current() != ranges::end(range()->range()));
                            auto n = pop_size();
                            RANGES_EXPECT(n > 0);
                            const Param_t interval{ 0, n - 1 };
                            if(dist(engine, interval) < range()->size())
                                break;
                        }
                    }
                }
            public:
                using value_type = range_value_type_t<Rng>;
                using difference_type = D;

                cursor() = default;
                explicit cursor(meta::const_if_c<IsConst, sample_view> &rng)
                  : base_t{&rng, ranges::begin(rng.range()), rng.range()}
                {
                    auto n = pop_size();
                    if(rng.size() > n)
                        rng.size() = n;
                    advance();
                }
                template<bool Other,
                    CONCEPT_REQUIRES_(IsConst && !Other)>
                cursor(cursor<Other> that)
                  : base_t(static_cast<typename cursor<Other>::base_t &&>(
                      static_cast<typename cursor<Other>::base_t &>(that)))
                {}
                range_reference_t<Rng> read() const
                {
                    return *current();
                }
                bool equal(default_sentinel) const
                {
                    RANGES_EXPECT(range());
                    return range()->size() <= 0;
                }
                void next()
                {
                    RANGES_EXPECT(range());
                    RANGES_EXPECT(range()->size() > 0);
                    --range()->size();
                    RANGES_ASSERT(current() != ranges::end(range()->range()));
                    ++current();
                    size().decrement();
                    advance();
                }
            };

            cursor<false> begin_cursor()
            {
                return cursor<false>{*this};
            }
            template<class CRng = Rng const,
                CONCEPT_REQUIRES_(meta::or_<
                    SizedRange<CRng>,
                    SizedSentinel<sentinel_t<CRng>, iterator_t<CRng>>,
                    ForwardRange<CRng>>::value)>
            cursor<true> begin_cursor() const
            {
                return cursor<true>{*this};
            }

        public:
            sample_view() = default;

            explicit sample_view(Rng rng, D sample_size, URNG& generator)
            : base_t{std::move(rng), sample_size, generator}
            {
                RANGES_EXPECT(sample_size >= 0);
            }
        };

        namespace view
        {

            /// Returns a random sample of a range of length `size(range)`.
            class sample_fn
            {
                template<typename Rng, typename URNG>
                using Constraint = meta::and_<
                    InputRange<Rng>, UniformRandomNumberGenerator<URNG>,
                    ConvertibleTo<
                        concepts::UniformRandomNumberGenerator::result_t<URNG>,
                        range_difference_type_t<Rng>>,
                    meta::or_<
                        SizedRange<Rng>,
                        SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>,
                        ForwardRange<Rng>>>;

                friend view_access;
                template<typename Size, typename URNG = detail::default_random_engine,
                    CONCEPT_REQUIRES_(Integral<Size>(), UniformRandomNumberGenerator<URNG>())>
                static auto bind(sample_fn fn, Size n, URNG &urng = detail::get_random_engine())
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(fn, std::placeholders::_1, n, bind_forward<URNG &>(urng)))
                )

            public:
                template<typename Rng, typename URNG = detail::default_random_engine,
                    CONCEPT_REQUIRES_(Constraint<Rng, URNG>())>
                sample_view<all_t<Rng>, URNG> operator()(
                    Rng && rng, range_difference_type_t<Rng> sample_size,
                    URNG &generator = detail::get_random_engine()) const
                {
                    return sample_view<all_t<Rng>, URNG>{
                        all(static_cast<Rng&&>(rng)), sample_size, generator
                    };
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename URNG = detail::default_random_engine,
                    CONCEPT_REQUIRES_(!Constraint<Rng, URNG>())>
                void operator()(Rng &&, URNG && = URNG{}) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::sample operates must satisfy the InputRange "
                        "concept.");
                    CONCEPT_ASSERT_MSG(UniformRandomNumberGenerator<URNG>(),
                        "The generator passed to view::sample must satisfy the "
                        "UniformRandomNumberGenerator concept.");
                    CONCEPT_ASSERT_MSG(meta::or_<
                        SizedRange<Rng>,
                        SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>,
                        ForwardRange<Rng>>(),
                        "The underlying range for view::sample must either satisfy the SizedRange"
                        "concept, have iterator and sentinel types that satisfy the "
                        "SizedSentinel concept, or be a forward range.");
                    CONCEPT_ASSERT_MSG(ConvertibleTo<
                        concepts::UniformRandomNumberGenerator::result_t<URNG>,
                        range_difference_type_t<Rng>>(),
                        "The random generator passed to view::sample has to have a return type "
                        "convertible to the base iterator difference type.");
                }
            #endif
            };

            /// \relates sample_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<sample_fn>, sample)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::sample_view)

#endif
