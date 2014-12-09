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
#ifndef RANGES_V3_RANGE_INTERFACE_HPP
#define RANGES_V3_RANGE_INTERFACE_HPP

#include <iosfwd>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/to_container.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename From, typename To = From>
            struct slice_bounds
            {
                From from;
                To to;
                template<typename F, typename T,
                    CONCEPT_REQUIRES_(Convertible<F, From>() && Convertible<T, To>())>
                slice_bounds(F from, T to)
                  : from(from), to(to)
                {}
            };

            template<typename Int>
            struct from_end_
            {
                Int dist_;

                template<typename Other,
                    CONCEPT_REQUIRES_(Integral<Other>() && Convertible<Other, Int>())>
                operator from_end_<Other> () const
                {
                    return {static_cast<Other>(dist_)};
                }
            };
        }
        /// \endcond

        /// \addtogroup group-core
        /// @{
        template<typename Derived, bool Inf /* = false*/>
        struct range_interface
          : private basic_range<Inf>
        {
        protected:
            Derived & derived()
            {
                return static_cast<Derived &>(*this);
            }
            /// \overload
            Derived const & derived() const
            {
                return static_cast<Derived const &>(*this);
            }
        public:
            // A few ways of testing whether a range can be empty:
            bool empty() const
            {
                return derived().begin() == derived().end();
            }
            bool operator!() const
            {
                return empty();
            }
            explicit operator bool() const
            {
                return !empty();
            }
            /// Access the size of the range, if it can be determined:
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() &&
                    SizedIteratorRange<range_iterator_t<D>, range_sentinel_t<D>>())>
            range_size_t<D> size() const
            {
                return iter_size(derived().begin(), derived().end());
            }
            /// Access the first element in a range:
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            range_reference_t<D> front()
            {
                return *derived().begin();
            }
            /// \overload
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            range_reference_t<D const> front() const
            {
                return *derived().begin();
            }
            /// Access the last element in a range:
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && BoundedRange<D>() && BidirectionalRange<D>())>
            range_reference_t<D> back()
            {
                return *prev(derived().end());
            }
            /// \overload
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && BoundedRange<D const>() && BidirectionalRange<D const>())>
            range_reference_t<D const> back() const
            {
                return *prev(derived().end());
            }
            /// Simple indexing:
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && RandomAccessRange<D>())>
            auto operator[](range_difference_t<D> n) ->
                decltype(std::declval<D &>().begin()[n])
            {
                return derived().begin()[n];
            }
            /// \overload
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && RandomAccessRange<D const>())>
            auto operator[](range_difference_t<D> n) const ->
                decltype(std::declval<D const &>().begin()[n])
            {
                return derived().begin()[n];
            }
            /// Python-ic slicing:
            //      rng[{4,6}]
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<range_difference_t<D>> offs) ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<range_difference_t<D>> offs) const ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            //      rng[{4,end-2}]
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<range_difference_t<D>,
                detail::from_end_<range_difference_t<D>>> offs) ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<range_difference_t<D>,
                detail::from_end_<range_difference_t<D>>> offs) const ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            //      rng[{end-4,end-2}]
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_t<D>>,
                detail::from_end_<range_difference_t<D>>> offs) ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_t<D>>,
                detail::from_end_<range_difference_t<D>>> offs) const ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            //      rng[{4,end}]
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<range_difference_t<D>, end_fn> offs) ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<range_difference_t<D>, end_fn> offs) const ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            //      rng[{end-4,end}]
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_t<D>>, end_fn> offs) ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_t<D>>, end_fn> offs) const ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// Implicit conversion to something that looks like a container.
            template<typename Container, typename D = Derived,
                typename Alloc = typename Container::allocator_type, // HACKHACK
                CONCEPT_REQUIRES_(detail::ConvertibleToContainer<D, Container>())>
            operator Container ()
            {
                return ranges::to_<Container>(derived());
            }
            /// \overload
            template<typename Container, typename D = Derived,
                typename Alloc = typename Container::allocator_type, // HACKHACK
                CONCEPT_REQUIRES_(detail::ConvertibleToContainer<D const, Container>())>
            operator Container () const
            {
                return ranges::to_<Container>(derived());
            }
            /// \brief Print a range to an ostream
            template<bool B = true, typename Stream = enable_if_t<B, std::ostream>>
            friend Stream &operator<<(Stream &sout, Derived &rng)
            {
                auto it = ranges::begin(rng);
                auto const e = ranges::end(rng);
                if(it == e)
                    return sout << "[]";
                sout << '[' << *it;
                while(++it != e)
                    sout << ',' << *it;
                return sout << ']';
            }
            /// \overload
            template<bool B = true, typename Stream = enable_if_t<B, std::ostream>>
            friend Stream &operator<<(Stream &sout, Derived const &rng)
            {
                auto it = ranges::begin(rng);
                auto const e = ranges::end(rng);
                if(it == e)
                    return sout << "[]";
                sout << '[' << *it;
                while(++it != e)
                    sout << ',' << *it;
                return sout << ']';
            }
        };
        /// @}
    }
}

#endif
