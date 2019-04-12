/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_DROP_LAST_HPP
#define RANGES_V3_VIEW_DROP_LAST_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng, bool is_bidirectional = BidirectionalView<Rng>::value >
        struct drop_last_view{};

        template<typename Rng>
        struct drop_last_view<Rng, true>
          : view_interface<
              drop_last_view<Rng, true>,
              is_finite<Rng>::value ? finite : range_cardinality<Rng>::value   // finite at best
          >
        {
            CONCEPT_ASSERT(BidirectionalView<Rng>());
        private:
            friend range_access;
            Rng rng;
            std::size_t n;

            // TODO: optimise for convertible const<=>non-const sentinel?
            detail::non_propagating_cache<sentinel_t<Rng>> sentinel;

            // TODO: optimise with ebo.
            template<bool may_be_const = Range<Rng const>::value, class dummy = void>
            struct const_sentinel_{};

            template<class dummy>
            struct const_sentinel_<true, dummy>{
                using type = detail::non_propagating_cache<sentinel_t<Rng const>>;
            };

            template<class dummy>
            struct const_sentinel_<false, dummy>{
                struct type{};
            };

            using   const_sentinel_t = typename const_sentinel_<>::type;
            mutable const_sentinel_t const_sentinel;

        public:
            drop_last_view() = default;
            drop_last_view(Rng rng, std::size_t n)
              : rng(std::move(rng)), n(n)
            {}

            iterator_t<Rng> begin()
            {
                return ranges::begin(rng);
            }
            sentinel_t<Rng> end()
            {
                if (!sentinel)
                {
                    sentinel = ranges::prev(ranges::end(rng), n, ranges::begin(rng));
                }
                return *sentinel;
            }
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(Range<CRng>())>
            iterator_t<CRng> begin() const
            {
                return ranges::begin(rng);
            }
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(Range<CRng>())>
            sentinel_t<CRng> end() const
            {
                if (!const_sentinel)
                {
                    const_sentinel = ranges::prev(ranges::end(rng), n, ranges::begin(rng));
                }
                return *const_sentinel;
            }

            template<typename CRng = Rng const,
                    CONCEPT_REQUIRES_(SizedRange<CRng>())>
            range_size_type_t<CRng> size() const
            {
                const auto initial_size = ranges::size(rng);
                return initial_size > n
                    ? initial_size - static_cast<range_size_type_t<CRng>>(n)
                    : 0;
            }

            Rng & base()
            {
                return rng;
            }
            Rng const & base() const
            {
                return rng;
            }
        };

        template<typename Rng>
        struct drop_last_view<Rng, false>
          : view_adaptor<
              drop_last_view<Rng, false>,
              Rng,
              is_finite<Rng>::value ? finite : range_cardinality<Rng>::value   // finite at best (but unknown is expected)
          >
        {
            CONCEPT_ASSERT(ranges::ForwardView<Rng>());
        private:
            friend range_access;
            std::size_t n;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
                using CRng = meta::const_if_c<IsConst, Rng>;
                iterator_t<CRng> probe;

                adaptor() = default;

                template<bool Other,
                    CONCEPT_REQUIRES_(IsConst && !Other)>
                adaptor(adaptor<Other> other)
                    : probe(other.probe)
                {}

                adaptor(CRng &rng, std::size_t n)
                {
                    probe = ranges::next(ranges::begin(rng), n, ranges::end(rng));
                }

                void next(iterator_t<CRng>& it)
                {
                    ++it;
                    ++probe;
                }
            };

            struct sentinel_adaptor :  adaptor_base
            {
                template<typename I, typename IA, typename S>
                bool empty(I const &, IA const &ia, S const &s) const
                {
                    return ia.probe == s;
                }
            };

            adaptor<false> begin_adaptor() { return {this->base(), n}; }
            sentinel_adaptor end_adaptor() { return {}; }

            template<typename CRng = Rng const, CONCEPT_REQUIRES_(Range<CRng>())>
            adaptor<true>  begin_adaptor() const { return {this->base(), n}; }

            template<typename CRng = Rng const, CONCEPT_REQUIRES_(Range<CRng>())>
            sentinel_adaptor end_adaptor() const { return {}; }
        public:
            drop_last_view() = default;
            drop_last_view(Rng rng, std::size_t n)
              : drop_last_view::view_adaptor(std::move(rng))
              , n(n)
            {}

            // forward list can be size too,
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(SizedRange<CRng>())>
            range_size_type_t<CRng> size() const
            {
                const auto initial_size = ranges::size(this->base());
                return initial_size > n
                       ? initial_size - static_cast<range_size_type_t<CRng>>(n)
                       : 0;
            }
        };

        namespace view
        {
            struct drop_last_fn
            {
            private:
                friend view_access;

                template<class dummy = void>
                static auto bind(drop_last_fn drop_last, std::size_t n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(drop_last, std::placeholders::_1, n))
                )

                template<typename Rng>
                static drop_last_view<all_t<Rng>>
                invoke_(Rng &&rng, std::size_t n)
                {
                    return {all(static_cast<Rng&&>(rng)), n};
                }
            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(ForwardRange<Rng>())>
                auto operator()(Rng &&rng, std::size_t n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    drop_last_fn::invoke_(static_cast<Rng&&>(rng), n)
                )
            };

            RANGES_INLINE_VARIABLE(view<drop_last_fn>, drop_last)
        }
    }
}

#endif
