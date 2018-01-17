/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_INTERFACE_HPP
#define RANGES_V3_VIEW_INTERFACE_HPP

#include <iosfwd>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator.hpp>

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
                CONCEPT_template(typename F, typename T)(
                    requires ConvertibleTo<F, From>() && ConvertibleTo<T, To>())
                slice_bounds(F from, T to)
                  : from(from), to(to)
                {}
            };

            template<typename Int>
            struct from_end_
            {
                Int dist_;

                CONCEPT_template(typename Other)(
                    requires Integral<Other>() && ExplicitlyConvertibleTo<Other, Int>())
                operator from_end_<Other> () const
                {
                    return {static_cast<Other>(dist_)};
                }
            };
        }
        /// \endcond

        /// \addtogroup group-core
        /// @{
        template<typename Derived, cardinality Cardinality /* = finite*/>
        struct view_interface
          : basic_view<Cardinality>
        {
        protected:
            RANGES_CXX14_CONSTEXPR Derived &derived() noexcept
            {
                CONCEPT_ASSERT(DerivedFrom<Derived, view_interface>());
                return static_cast<Derived &>(*this);
            }
            /// \overload
            constexpr Derived const &derived() const noexcept
            {
                CONCEPT_ASSERT(DerivedFrom<Derived, view_interface>());
                return static_cast<Derived const &>(*this);
            }
            ~view_interface() = default;
        public:
            view_interface() = default;
            view_interface(view_interface &&) = default;
            view_interface(view_interface const &) = default;
            view_interface &operator=(view_interface &&) = default;
            view_interface &operator=(view_interface const &) = default;
            // A few ways of testing whether a range can be empty:
            CONCEPT_REQUIRES(Cardinality >= 0)
            constexpr bool empty() const noexcept
            {
                return Cardinality == 0;
            }
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && Cardinality < 0 && ForwardRange<D const>())
            constexpr bool empty() const
                noexcept(noexcept(bool(ranges::begin(std::declval<D const &>()) ==
                    ranges::end(std::declval<D const &>()))))
            {
                return ranges::begin(derived()) == ranges::end(derived());
            }
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>())
            constexpr auto operator!() const
            RANGES_DECLTYPE_NOEXCEPT(ranges::empty(std::declval<D const &>()))
            {
                return ranges::empty(derived());
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>()),
                typename = decltype(ranges::empty(std::declval<D const &>()))>
            constexpr explicit operator bool() const
                noexcept(noexcept(ranges::empty(std::declval<D const &>())))
            {
                return !ranges::empty(derived());
            }
            /// Access the size of the range, if it can be determined:
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && Cardinality >= 0)
            constexpr range_size_type_t<D> size() const noexcept
            {
                return (range_size_type_t<D>)Cardinality;
            }
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && Cardinality < 0 &&
                    SizedSentinel<sentinel_t<D const>, iterator_t<D const>>() &&
                    ForwardRange<D const>())
            constexpr range_size_type_t<D> size() const
            {
                return iter_size(derived().begin(), derived().end());
            }
            /// Access the first element in a range:
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && ForwardRange<D>())
            range_reference_t<D> front()
            {
                return *derived().begin();
            }
            /// \overload
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && ForwardRange<D const>())
            range_reference_t<D const> front() const
            {
                return *derived().begin();
            }
            /// Access the last element in a range:
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && BoundedRange<D>() && BidirectionalRange<D>())
            range_reference_t<D> back()
            {
                return *prev(derived().end());
            }
            /// \overload
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && BoundedRange<D const>() && BidirectionalRange<D const>())
            range_reference_t<D const> back() const
            {
                return *prev(derived().end());
            }
            /// Simple indexing:
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && RandomAccessRange<D>())
            auto operator[](range_difference_type_t<D> n) ->
                decltype(std::declval<D &>().begin()[n])
            {
                return derived().begin()[n];
            }
            /// \overload
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && RandomAccessRange<D const>())
            auto operator[](range_difference_type_t<D> n) const ->
                decltype(std::declval<D const &>().begin()[n])
            {
                return derived().begin()[n];
            }
            /// Python-ic slicing:
            //      rng[{4,6}]
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<range_difference_type_t<D>> offs) & ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<range_difference_type_t<D>> offs) const & ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<range_difference_type_t<D>> offs) && ->
                decltype(std::declval<Slice>()(std::declval<D>(), offs.from, offs.to))
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{4,end-2}]
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<range_difference_type_t<D>,
                detail::from_end_<range_difference_type_t<D>>> offs) & ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<range_difference_type_t<D>,
                detail::from_end_<range_difference_type_t<D>>> offs) const & ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<range_difference_type_t<D>,
                detail::from_end_<range_difference_type_t<D>>> offs) && ->
                decltype(std::declval<Slice>()(std::declval<D>(), offs.from, offs.to))
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{end-4,end-2}]
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_type_t<D>>,
                detail::from_end_<range_difference_type_t<D>>> offs) & ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_type_t<D>>,
                detail::from_end_<range_difference_type_t<D>>> offs) const & ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_type_t<D>>,
                detail::from_end_<range_difference_type_t<D>>> offs) && ->
                decltype(std::declval<Slice>()(std::declval<D>(), offs.from, offs.to))
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{4,end}]
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<range_difference_type_t<D>, end_fn> offs) & ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<range_difference_type_t<D>, end_fn> offs) const & ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<range_difference_type_t<D>, end_fn> offs) && ->
                decltype(std::declval<Slice>()(std::declval<D>(), offs.from, offs.to))
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{end-4,end}]
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_type_t<D>>, end_fn> offs) & ->
                decltype(std::declval<Slice>()(std::declval<D &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_type_t<D>>, end_fn> offs) const & ->
                decltype(std::declval<Slice>()(std::declval<D const &>(), offs.from, offs.to))
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived>())
            auto operator[](detail::slice_bounds<detail::from_end_<range_difference_type_t<D>>, end_fn> offs) && ->
                decltype(std::declval<Slice>()(std::declval<D>(), offs.from, offs.to))
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            /// Returns a reference to the element at specified location pos, with bounds checking.
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && RandomAccessRange<D>() && SizedRange<D>())
            auto at(range_difference_type_t<D> n) ->
                decltype(std::declval<D &>().begin()[n])
            {
                using size_type = range_size_type_t<Derived>;
                if (n < 0 || size_type(n) >= ranges::size(derived()))
                {
                    throw std::out_of_range("view_interface::at");
                }
                return derived().begin()[n];
            }
            /// \overload
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && RandomAccessRange<D const>() && SizedRange<D const>())
            auto at(range_difference_type_t<D> n) const  ->
                decltype(std::declval<D const &>().begin()[n])
            {
                using size_type = range_size_type_t<Derived>;
                if (n < 0 || size_type(n) >= ranges::size(derived()))
                {
                    throw std::out_of_range("view_interface::at");
                }
                return derived().begin()[n];
            }
            /// Implicit conversion to something that looks like a container.
            CONCEPT_template(typename Container, typename D = Derived,
                typename = typename Container::allocator_type)( // HACKHACK
                requires detail::ConvertibleToContainer<D, Container>())
            operator Container ()
            {
                return ranges::to_<Container>(derived());
            }
            /// \overload
            CONCEPT_template(typename Container, typename D = Derived,
                typename = typename Container::allocator_type)( // HACKHACK
                requires detail::ConvertibleToContainer<D const, Container>())
            operator Container () const
            {
                return ranges::to_<Container>(derived());
            }
            /// \brief Print a range to an ostream
        private:
            CONCEPT_template(typename Stream, typename Rng)(
                requires Same<Derived, meta::_t<std::remove_cv<Rng>>>())
            static Stream &print_(Stream &sout, Rng &rng)
            {
                sout << '[';
                auto it = ranges::begin(rng);
                auto const e = ranges::end(rng);
                if(it != e)
                {
                    for(;;)
                    {
                        sout << *it;
                        if(++it == e) break;
                        sout << ',';
                    }
                }
                sout << ']';
                return sout;
            }

            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && InputRange<D const>())
            friend std::ostream &operator<<(std::ostream &sout, Derived const &rng)
            {
                return view_interface::print_(sout, rng);
            }
            /// \overload
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && !Range<D const>() && InputRange<D>())
            friend std::ostream &operator<<(std::ostream &sout, Derived &rng)
            {
                return view_interface::print_(sout, rng);
            }
            /// \overload
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived>() && !Range<D const>() && InputRange<D>())
            friend std::ostream &operator<<(std::ostream &sout, Derived &&rng)
            {
                return view_interface::print_(sout, rng);
            }
        };
        /// @}
    }
}

#endif
