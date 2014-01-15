// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_SENTINEL_FACADE_HPP
#define RANGES_V3_UTILITY_SENTINEL_FACADE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_traits.hpp>

#define REQUIRES(ITER, CAT)                                         \
    typename std::enable_if<                                        \
        std::is_convertible<typename ITER::iterator_category,       \
                            std::CAT ## _iterator_tag>::value,      \
        int                                                         \
    >::type = 0                                                     \
    /**/

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            //
            // enable if for use in operator implementation.
            //
            template<typename Facade1, typename Facade2, typename Return>
            using enable_if_interoperable_t =
                typename std::enable_if<
                    std::is_convertible<Facade1, Facade2>::value ||
                    std::is_convertible<Facade2, Facade1>::value
                  , Return
                >::type;

            template<typename I1, typename I2>
            using choose_difference_type_t =
                detail::conditional_t<
                   std::is_convertible<I2, I1>::value
                 , iterator_difference_t<I1>
                 , iterator_difference_t<I2>
               >;
        }

        //
        // Helper typename for granting access to the iterator core interface.
        //
        // The simple core interface is used by iterator_facade. The core
        // interface of a user/library defined iterator type should not be made public
        // so that it does not clutter the public interface. Instead iterator_core_access
        // should be made friend so that iterator_facade can access the core
        // interface through iterator_core_access.
        //
        struct iterator_core_access
        {
            // objects of this typename are useless
            iterator_core_access() = delete;

            template<typename Facade>
            static typename Facade::reference dereference(Facade const& f)
            {
                return f.dereference();
            }
            template<typename Facade>
            static typename Facade::pointer arrow(Facade const& f)
            {
                return f.arrow();
            }
            template<typename Facade>
            static void increment(Facade& f)
            {
                f.increment();
            }
            template<typename Facade1, typename Facade2>
            static bool equal(Facade1 const& f1, Facade2 const& f2, std::true_type)
            {
                return f1.equal(f2);
            }
            template<typename Facade1, typename Facade2>
            static bool equal(Facade1 const& f1, Facade2 const& f2, std::false_type)
            {
                return f2.equal(f1);
            }
            template<typename Facade, typename Sentinel>
            static constexpr bool done(Facade const& f, Sentinel const& s)
            {
                return f.equal(s);
            }
            template<typename Facade>
            static void decrement(Facade& f)
            {
                f.decrement();
            }
            template<typename Facade>
            static void advance(Facade& f, typename Facade::difference_type n)
            {
                f.advance(n);
            }
            template<typename Facade1, typename Facade2>
            static auto distance_from(Facade1 const& f1, Facade2 const& f2, std::true_type)
                -> typename Facade1::difference_type
            {
                return -f1.distance_to(f2);
            }
            template<typename Facade1, typename Facade2>
            static auto distance_from(Facade1 const& f1, Facade2 const& f2, std::false_type)
                -> typename Facade2::difference_type
            {
                return f2.distance_to(f1);
            }
            template<typename Facade, typename Sentinel>
            static constexpr auto distance_sentinel(Facade const& f, Sentinel const& s)
                -> typename Facade::difference_type
            {
                return f.distance_to(s);
            }
        };

        template<typename Derived, typename It>
        struct sentinel_facade
        {
            sentinel_facade() = default;
        private:
            friend struct iterator_core_access;
            //
            // Curiously Recurring Template interface.
            //
            Derived& derived()
            {
                return *static_cast<Derived*>(this);
            }
            constexpr Derived const& derived() const
            {
                return *static_cast<Derived const*>(this);
            }
        public:
            // operator==
            template<typename I, typename V, typename TC, typename R, typename D, typename P>
            friend constexpr detail::enable_if_interoperable_t<It, I, bool>
            operator==(iterator_facade<I, V, TC, R, D, P> const& f, sentinel_facade const &s)
            {
                return iterator_core_access::done(static_cast<I const &>(f), s.derived());
            }
            friend constexpr bool operator==(sentinel_facade const &, sentinel_facade const &)
            {
                return true;
            }
            // operator!=
            template<typename I, typename V, typename TC, typename R, typename D, typename P>
            friend constexpr detail::enable_if_interoperable_t<It, I, bool>
            operator!=(iterator_facade<I, V, TC, R, D, P> const& f, sentinel_facade const &s)
            {
                return !iterator_core_access::done(static_cast<I const &>(f), s.derived());
            }
            friend constexpr bool operator!=(sentinel_facade const &, sentinel_facade const &)
            {
                return false;
            }

            // operator-
            template<typename I, typename V, typename TC, typename R, typename D, typename P,
                REQUIRES(I, random_access)>
            friend constexpr detail::enable_if_interoperable_t<
                It, I
              , detail::choose_difference_type_t<It, I>
            >
            operator-(iterator_facade<I, V, TC, R, D, P> const& f, sentinel_facade const &s)
            {
                return -iterator_core_access::distance_sentinel(
                    static_cast<I const &>(f)
                  , s.derived()
                );
            }
            template<typename I, typename V, typename TC, typename R, typename D, typename P,
                REQUIRES(I, random_access)>
            friend constexpr detail::enable_if_interoperable_t<
                It, I
              , detail::choose_difference_type_t<It, I>
            >
            operator-(sentinel_facade const &s, iterator_facade<I, V, TC, R, D, P> const& f)
            {
                return iterator_core_access::distance_sentinel(
                    static_cast<I const &>(f)
                  , s.derived()
                );
            }
            friend constexpr iterator_difference_t<It>
            operator-(sentinel_facade const &, sentinel_facade const &)
            {
                return iterator_difference_t<It>{};
            }
        };
    }
}

#undef REQUIRES

#endif
