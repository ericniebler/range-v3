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

            using difference_t = range_difference_type_t<Rng>;
            using size_t = range_size_type_t<Rng>;

            Rng rng;
            difference_t n;
            detail::non_propagating_cache<sentinel_t<Rng>> sentinel;

            template<class T>
            size_t get_size(T&& rng) const
            {
                const size_t initial_size = ranges::size(rng);
                const size_t n = static_cast<size_t>(this->n);

                return initial_size > n
                    ? initial_size - n
                    : 0;
            }
        public:
            drop_last_view() = default;
            drop_last_view(Rng rng, difference_t n)
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
                CONCEPT_REQUIRES_(RandomAccessRange<CRng>())>
            iterator_t<CRng> begin() const
            {
                return ranges::begin(rng);
            }
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(RandomAccessRange<CRng>())>
            sentinel_t<CRng> end() const
            {
                return ranges::prev(ranges::end(rng), n, ranges::begin(rng));
            }

            CONCEPT_REQUIRES(SizedRange<Rng>())
            size_t size()
            {
                return get_size(rng);
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            size_t size() const
            {
                return get_size(rng);
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

            using difference_t = range_difference_type_t<Rng>;
            using size_t = range_size_type_t<Rng>;
            difference_t n;

            detail::non_propagating_cache<iterator_t<Rng>> probe_begin;

            struct adaptor : adaptor_base
            {
                iterator_t<Rng> probe;

                adaptor() = default;

                adaptor(iterator_t<Rng> probe_begin)
                  : probe(std::move(probe_begin))
                {}

                void next(iterator_t<Rng> &it)
                {
                    ++it;
                    ++probe;
                }
            };

            struct sentinel_adaptor :  adaptor_base
            {
                template<typename I, typename S>
                bool empty(I const &, adaptor const &ia, S const &s) const
                {
                    return ia.probe == s;
                }
            };

            adaptor begin_adaptor() {
                if (!probe_begin)
                {
                    probe_begin = ranges::next(ranges::begin(this->base()), n, ranges::end(this->base()));
                }
                return {*probe_begin};
            }
            sentinel_adaptor end_adaptor() { return {}; }

            template<class T>
            size_t get_size(T &&rng) const
            {
                const size_t initial_size = ranges::size(rng);
                const size_t n = static_cast<size_t>(this->n);

                return initial_size > n
                    ? initial_size - n
                    : 0;
            }
        public:
            drop_last_view() = default;
            drop_last_view(Rng rng, difference_t n)
              : drop_last_view::view_adaptor(std::move(rng))
              , n(n)
            {}

            CONCEPT_REQUIRES(SizedRange<Rng>())
            size_t size()
            {
                return get_size(this->base());
            }
            // XXX: should we leave this?
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            size_t size() const
            {
                return get_size(this->base());
            }
        };

        namespace view
        {
            struct drop_last_fn
            {
            private:
                friend view_access;

                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto bind(drop_last_fn drop_last, Int n)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(drop_last, std::placeholders::_1, n))
                )
            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(ForwardRange<Rng>())>
                drop_last_view<all_t<Rng>> operator()(Rng &&rng, range_difference_type_t<Rng> n) const
                {
                    return {all(static_cast<Rng&&>(rng)), n};
                }
            };

            RANGES_INLINE_VARIABLE(view<drop_last_fn>, drop_last)
        }
    }
}

#endif
