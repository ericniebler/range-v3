// Range v3 library
//
// Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_RANGE_ADAPTOR_HPP
#define RANGES_V3_RANGE_ADAPTOR_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Derived>
            using begin_adaptor_t =
                decltype(range_access::begin_adaptor(std::declval<Derived &>()));

            template<typename Derived>
            using end_adaptor_t =
                decltype(range_access::end_adaptor(std::declval<Derived &>()));

            template<typename Derived>
            using adapted_iterator_t =
                decltype(std::declval<begin_adaptor_t<Derived>>().begin(std::declval<Derived &>()));

            template<typename Derived>
            using adapted_sentinel_t =
                decltype(std::declval<end_adaptor_t<Derived>>().end(std::declval<Derived &>()));
        }

        template<typename Derived>
        using base_range_t = typename range_access::base_range<Derived>::type;

        template<typename Derived>
        using range_adaptor_t = meta_apply<range_access::range_adaptor, Derived>;

        template<typename BaseIt, typename Adapt>
        struct adaptor_cursor;

        template<typename BaseSent, typename Adapt>
        struct adaptor_sentinel;

        struct sentinel_adaptor_base
        {
            template<typename Rng>
            range_sentinel_t<base_range_t<Rng>> end(Rng &rng) const
            {
                return ranges::end(rng.base());
            }
            template<typename I, typename IA, typename S, typename SA, CONCEPT_REQUIRES_(IteratorRange<I, S>())>
            static bool empty(adaptor_cursor<I, IA> const &pos0, adaptor_sentinel<S, SA> const &pos1)
            {
                return pos0.first == pos1.first;
            }
        };

        struct iterator_adaptor_base
          : sentinel_adaptor_base
        {
            template<typename Rng>
            range_iterator_t<base_range_t<Rng>> begin(Rng &rng) const
            {
                return ranges::begin(rng.base());
            }
            template<typename I, typename IA, CONCEPT_REQUIRES_(Iterator<I>())>
            static bool equal(adaptor_cursor<I, IA> const &pos0, adaptor_cursor<I, IA> const &pos1)
            {
                return pos0.first == pos1.first;
            }
            template<typename I, typename IA>
            static iterator_reference_t<I> current(adaptor_cursor<I, IA> const &pos)
            {
                return *pos.first;
            }
            template<typename I, typename IA>
            static void next(adaptor_cursor<I, IA> &pos)
            {
                ++pos.first;
            }
            template<typename I, typename IA, CONCEPT_REQUIRES_(BidirectionalIterator<I>())>
            static void prev(adaptor_cursor<I, IA> &pos)
            {
                --pos.first;
            }
            template<typename I, typename IA, CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            static void advance(adaptor_cursor<I, IA> &pos, iterator_difference_t<I> n)
            {
                pos.first += n;
            }
            template<typename I, typename IA, CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            static iterator_difference_t<I> distance_to(adaptor_cursor<I, IA> const &pos0,
                adaptor_cursor<I, IA> const &pos1)
            {
                return pos1.first - pos0.first;
            }
        };

        template<typename BaseIter, typename Adapt>
        struct adaptor_cursor
          : compressed_pair<BaseIter, Adapt>
        {
            using single_pass = detail::or_t<
                range_access::single_pass_t<Adapt>,
                SinglePass<BaseIter>>;
            adaptor_cursor() = default;
            using compressed_pair<BaseIter, Adapt>::compressed_pair;

            operator BaseIter & ()
            {
                return this->first;
            }
            operator BaseIter const & () const
            {
                return this->first;
            }
            template<typename A = Adapt,
                     typename R = decltype(std::declval<A>().current(std::declval<adaptor_cursor const &>()))>
            R current() const
            {
                return this->second.current(*this);
            }
            template<typename A = Adapt,
                     typename R = decltype(std::declval<A>().next(std::declval<adaptor_cursor &>()))>
            void next()
            {
                this->second.next(*this);
            }
            template<typename A = Adapt,
                     typename R = decltype(std::declval<A>().equal(std::declval<adaptor_cursor const &>(), std::declval<adaptor_cursor const &>()))>
            bool equal(adaptor_cursor const &that) const
            {
                return this->second.equal(*this, that);
            }
            template<typename A = Adapt,
                     typename R = decltype(std::declval<A>().prev(std::declval<adaptor_cursor &>()))>
            void prev()
            {
                this->second.prev(*this);
            }
            template<typename A = Adapt,
                     typename R = decltype(std::declval<A>().advance(std::declval<adaptor_cursor &>(), 0))>
            void advance(iterator_difference_t<BaseIter> n)
            {
                this->second.advance(*this, n);
            }
            template<typename A = Adapt,
                     typename R = decltype(std::declval<A>().distance_to(std::declval<adaptor_cursor const &>(), std::declval<adaptor_cursor const &>()))>
            iterator_difference_t<BaseIter> distance_to(adaptor_cursor const &that) const
            {
                return this->second.distance_to(*this, that);
            }
        };

        template<typename BaseSent, typename Adapt>
        struct adaptor_sentinel
          : compressed_pair<BaseSent, Adapt>
        {
            using single_pass = range_access::single_pass_t<Adapt>;
            adaptor_sentinel() = default;
            using compressed_pair<BaseSent, Adapt>::compressed_pair;

            operator BaseSent & ()
            {
                return this->first;
            }
            operator BaseSent const & () const
            {
                return this->first;
            }
            template<typename C2, typename A2>
            bool equal(adaptor_cursor<C2, A2> const &that) const
            {
                return this->second.empty(that, *this);
            }
        };

        template<typename D>
        using adaptor_cursor_t =
            adaptor_cursor<detail::adapted_iterator_t<D>, detail::begin_adaptor_t<D>>;

        template<typename D>
        using adaptor_sentinel_t =
            detail::conditional_t<
                Same<detail::adapted_iterator_t<D>, detail::adapted_sentinel_t<D>>() &&
                    Same<detail::begin_adaptor_t<D>, detail::end_adaptor_t<D>>(),
                adaptor_cursor<detail::adapted_iterator_t<D>, detail::begin_adaptor_t<D>>,
                adaptor_sentinel<detail::adapted_sentinel_t<D>, detail::end_adaptor_t<D>>>;

        template<typename Derived, typename BaseRng, bool Inf /*= is_infinite<BaseRng>::value*/>
        struct range_adaptor
          : range_facade<Derived, Inf>
        {
        private:
            friend Derived;
            friend range_access;
            friend iterator_adaptor_base;
            friend sentinel_adaptor_base;
            using range_adaptor_t = range_adaptor;
            using base_range_t = view::all_t<BaseRng>;
            using range_facade<Derived, Inf>::derived;
            using begin_adaptor_t =
                iterator_adaptor_base;
            using end_adaptor_t =
                detail::conditional_t<
                    (bool) BoundedIterable<BaseRng>(),
                    iterator_adaptor_base,
                    sentinel_adaptor_base>;
            // Mutable here. Const-correctness is enforced below by disabling
            // begin_cursor/end_cursor if "BaseRng const" does not model
            // the Range concept.
            mutable base_range_t rng_;

            base_range_t & base() const
            {
                return rng_;
            }
            begin_adaptor_t begin_adaptor() const
            {
                return {};
            }
            end_adaptor_t end_adaptor() const
            {
                return {};
            }

            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            adaptor_cursor_t<D> begin_cursor()
            {
                auto adapt = range_access::begin_adaptor(derived());
                auto pos = adapt.begin(derived());
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            adaptor_sentinel_t<D> end_cursor()
            {
                auto adapt = range_access::end_adaptor(derived());
                auto pos = adapt.end(derived());
                return {std::move(pos), std::move(adapt)};
            }
            // Const-correctness is enforced here by only allowing these if the base range
            // has const begin/end accessors. That disables the const begin()/end() accessors
            // in range_facade, meaning the derived range type only has mutable iterators.
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && Range<base_range_t const>())>
            adaptor_cursor_t<D const> begin_cursor() const
            {
                auto adapt = range_access::begin_adaptor(derived());
                auto pos = adapt.begin(derived());
                return {std::move(pos), std::move(adapt)};
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && Range<base_range_t const>())>
            adaptor_sentinel_t<D const> end_cursor() const
            {
                auto adapt = range_access::end_adaptor(derived());
                auto pos = adapt.end(derived());
                return {std::move(pos), std::move(adapt)};
            }
        public:
            range_adaptor() = default;
            constexpr range_adaptor(BaseRng && rng)
              : rng_(view::all(detail::forward<BaseRng>(rng)))
            {}
        };
    }
}

#endif
