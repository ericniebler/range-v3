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
#include <range/v3/utility/counted_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace aux{ namespace drop_last_view {
            template<class Rng, class I, typename size_t = range_size_type_t<Rng>>
            size_t get_size(Rng&& rng, I i)
            {
                RANGES_EXPECT(i >= 0);
                const size_t initial_size = ranges::size(rng);
                const size_t n = static_cast<size_t>(i);
                RANGES_EXPECT(initial_size >= n);

                return initial_size > n
                       ? initial_size - n
                       : 0;
            }

            enum class mode{bidi, forward, sized, invalid};

            template<class Rng>
            constexpr mode get_mode(){
                // keep range bound
                return (RandomAccessView<Rng>::value  && SizedView<Rng>::value) ||
                       (BidirectionalView<Rng>::value && BoundedView<Rng>::value)
                       ? aux::drop_last_view::mode::bidi
                       : SizedView<Rng>::value
                         ? aux::drop_last_view::mode::sized
                         : ForwardView<Rng>::value
                           ? aux::drop_last_view::mode::forward
                           : aux::drop_last_view::mode::invalid;

                // max performance
                // Sized Bidi use mode::sized instead of mode::bidi - thus become unbound.
                /*return (RandomAccessView<Rng>::value && SizedView<Rng>::value) ||
                       (BidirectionalView<Rng>::value && BoundedView<Rng>::value)
                       ? aux::drop_last_view::mode::bidi
                       : SizedView<Rng>::value
                         ? aux::drop_last_view::mode::sized
                         : BidirectionalView<Rng>::value && BoundedView<Rng>::value
                           ? aux::drop_last_view::mode::bidi
                           : ForwardView<Rng>::value
                             ? aux::drop_last_view::mode::forward
                             : aux::drop_last_view::mode::invalid;*/
            }
        }}

        template<typename Rng, aux::drop_last_view::mode mode = aux::drop_last_view::get_mode<Rng>()>
        struct drop_last_view{};

        template<typename Rng>
        struct drop_last_view<Rng, aux::drop_last_view::mode::bidi>
          : view_interface<
              drop_last_view<Rng, aux::drop_last_view::mode::bidi>,
              is_finite<Rng>::value ? finite : range_cardinality<Rng>::value   // finite at best
          >
        {
            CONCEPT_ASSERT(
                (RandomAccessView<Rng>()  && SizedView<Rng>()) ||
                (BidirectionalView<Rng>() && BoundedView<Rng>()));
        private:
            friend range_access;

            using difference_t = range_difference_type_t<Rng>;
            using size_t = range_size_type_t<Rng>;

            Rng rng;
            difference_t n;
            detail::non_propagating_cache<sentinel_t<Rng>> sentinel;

            template<typename R>
            using RAS = meta::strict_and<RandomAccessRange<R>, SizedRange<R>>;

            CONCEPT_REQUIRES(RAS<Rng>())
            sentinel_t<Rng> get_sentinel(){
                return ranges::begin(rng) + static_cast<difference_t>(size());
            }
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(RAS<CRng>())>
            sentinel_t<CRng> get_sentinel() const {
                return ranges::begin(rng) + static_cast<difference_t>(size());
            }

            CONCEPT_REQUIRES(!RAS<Rng>() && BoundedRange<Rng>())
            sentinel_t<Rng> get_sentinel(){
                return ranges::prev(ranges::end(rng), n, ranges::begin(rng));
            }
            template<typename CRng = Rng const,
                CONCEPT_REQUIRES_(!RAS<CRng>() && BoundedRange<CRng>())>
            sentinel_t<CRng> get_sentinel() const {
                return ranges::prev(ranges::end(rng), n, ranges::begin(rng));
            }

        public:
            drop_last_view() = default;
            drop_last_view(Rng rng, difference_t n)
              : rng(std::move(rng)), n(n)
            {
                RANGES_EXPECT(n >= 0);
            }

            iterator_t<Rng> begin()
            {
                return ranges::begin(rng);
            }
            sentinel_t<Rng> end()
            {
                if (!sentinel)
                {
                    sentinel = get_sentinel();
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
                return get_sentinel();
            }

            CONCEPT_REQUIRES(SizedRange<Rng>())
            size_t size()
            {
                return aux::drop_last_view::get_size(rng, n);
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            size_t size() const
            {
                return aux::drop_last_view::get_size(rng, n);
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
        struct drop_last_view<Rng, aux::drop_last_view::mode::forward>
          : view_adaptor<
              drop_last_view<Rng, aux::drop_last_view::mode::forward>,
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

        public:
            drop_last_view() = default;
            drop_last_view(Rng rng, difference_t n)
              : drop_last_view::view_adaptor(std::move(rng))
              , n(n)
            {
                RANGES_EXPECT(n >= 0);
            }

            CONCEPT_REQUIRES(SizedRange<Rng>())
            size_t size()
            {
                return aux::drop_last_view::get_size(this->base(), n);
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            size_t size() const
            {
                return aux::drop_last_view::get_size(this->base(), n);
            }
        };

        template<typename Rng>
        struct drop_last_view<Rng, aux::drop_last_view::mode::sized>
           : view_interface<
               drop_last_view<Rng, aux::drop_last_view::mode::sized>,
               finite
           >
        {
            CONCEPT_ASSERT(ranges::SizedView<Rng>());
        private:
            friend range_access;

            using difference_t = range_difference_type_t<Rng>;
            using size_t = range_size_type_t<Rng>;
            Rng rng;
            difference_t n;

        public:
            drop_last_view() = default;
            drop_last_view(Rng rng, difference_t n)
               : rng(std::move(rng)), n(n)
            {
                RANGES_EXPECT(n >= 0);
            }

            counted_iterator<iterator_t<Rng>> begin()
            {
                return {ranges::begin(rng), static_cast<difference_t>(size())};
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            counted_iterator<iterator_t<Rng>> begin() const
            {
                return {ranges::begin(rng), static_cast<difference_t>(size())};
            }
            default_sentinel end() const
            {
                return {};
            }
            size_t size()
            {
                return aux::drop_last_view::get_size(this->base(), n);
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            size_t size() const
            {
                return aux::drop_last_view::get_size(this->base(), n);
            }

            // TODO: fix view_interface #https://github.com/ericniebler/range-v3/issues/1147. This should be auto-generated.
            bool empty()
            {
                return this->size() == 0;
            }
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            bool empty() const
            {
                return this->size() == 0;
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
                    CONCEPT_REQUIRES_(SizedRange<Rng>() || ForwardRange<Rng>())>
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
