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
        template<typename Rng>
        struct drop_last_bidirectional_view
          : view_interface<drop_last_bidirectional_view<Rng>, range_cardinality<Rng>::value>
        {
            CONCEPT_ASSERT(ranges::BidirectionalRange<Rng>());
        private:
            friend range_access;
            Rng rng;
            std::size_t n;
            detail::non_propagating_cache<sentinel_t<Rng>> sentinel;
            void update_sentinel()
            {
                if (!sentinel)
                {
                    sentinel = ranges::prev(ranges::end(rng), n, ranges::begin(rng));
                }
            }
        public:
            drop_last_bidirectional_view() = default;
            drop_last_bidirectional_view(Rng&& rng, std::size_t n)
              : rng(std::move(rng)), n(n)
            {}

            iterator_t<Rng> begin()
            {
                return ranges::begin(rng);
            }
            sentinel_t<Rng> end()
            {
                update_sentinel();
                return *sentinel;
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
        struct drop_last_forward_view
          : view_adaptor<drop_last_forward_view<Rng>, Rng>
        {
            CONCEPT_ASSERT(ranges::ForwardRange<Rng>());
        private:
            friend range_access;
            std::size_t n;

            struct sentinel_adaptor;
            class adaptor : public adaptor_base
            {
                friend sentinel_adaptor;
                iterator_t<Rng> probe;
            public:
                adaptor() = default;
                adaptor(const Rng& rng, std::size_t n)
                {
                    probe = ranges::next(ranges::begin(rng), n, ranges::end(rng));
                }

                void next(iterator_t<Rng>& it)
                {
                    ++it;
                    ++probe;
                }
            };

            struct sentinel_adaptor :  adaptor_base
            {
                bool empty(const iterator_t<Rng>&, const adaptor& ia, const sentinel_t<Rng>& s) const
                {
                    return ia.probe == s;
                }
            };

            adaptor begin_adaptor() const { return {this->base(), n}; }
            sentinel_adaptor end_adaptor() const { return {}; }
        public:
            drop_last_forward_view() = default;
            drop_last_forward_view(Rng&& rng, std::size_t n)
              : drop_last_forward_view::view_adaptor(std::move(rng))
              , n(n)
            {}
        };

        template<class Rng>
        using drop_last_view =
            meta::if_<BidirectionalRange<Rng>
                , drop_last_bidirectional_view<Rng>
                , drop_last_forward_view<Rng>>;

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
                invoke_(Rng && rng, std::size_t n)
                {
                    return {all(static_cast<Rng&&>(rng)), n};
                }
            public:
                template<typename Rng,
                    CONCEPT_REQUIRES_(ForwardRange<Rng>())>
                auto operator()(Rng && rng, std::size_t n) const
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
