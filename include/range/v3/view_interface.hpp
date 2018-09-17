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
                    requires ConvertibleTo<F, From> && ConvertibleTo<T, To>)
                constexpr slice_bounds(F from, T to)
                  : from(from), to(to)
                {}
            };

            template<typename Int>
            struct from_end_
            {
                Int dist_;

                CONCEPT_template(typename Other)(
                    requires Integral<Other> && ExplicitlyConvertibleTo<Other, Int>)
                constexpr operator from_end_<Other> () const
                {
                    return {static_cast<Other>(dist_)};
                }
            };

            CONCEPT_def
            (
                template(typename Rng)
                concept CanEmpty,
                    requires (Rng &&rng)
                    (
                        ranges::empty((Rng &&) rng)
                    )
            );
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
                CONCEPT_assert(DerivedFrom<Derived, view_interface>);
                return static_cast<Derived &>(*this);
            }
            /// \overload
            constexpr Derived const &derived() const noexcept
            {
                CONCEPT_assert(DerivedFrom<Derived, view_interface>);
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
            CONCEPT_requires(Cardinality >= 0 || Cardinality == infinite)
            constexpr bool empty() const noexcept
            {
                return Cardinality == 0;
            }
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> &&
                    Cardinality < 0 && Cardinality != infinite &&
                    Range<D> && ForwardIterator<iterator_t<D>>)
            RANGES_CXX14_CONSTEXPR bool empty()
                noexcept(noexcept(bool(ranges::begin(std::declval<D &>()) ==
                    ranges::end(std::declval<D &>()))))
            {
                return bool(ranges::begin(derived()) == ranges::end(derived()));
            }
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> &&
                    Cardinality < 0 && Cardinality != infinite &&
                    Range<D const> && ForwardIterator<iterator_t<D const>>)
            constexpr bool empty() const
                noexcept(noexcept(bool(ranges::begin(std::declval<D const &>()) ==
                    ranges::end(std::declval<D const &>()))))
            {
                return bool(ranges::begin(derived()) == ranges::end(derived()));
            }
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && detail::CanEmpty<D &>)
            RANGES_CXX14_CONSTEXPR explicit operator bool()
                noexcept(noexcept(ranges::empty(std::declval<D &>())))
            {
                return !ranges::empty(derived());
            }
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && detail::CanEmpty<D const &>)
            constexpr explicit operator bool() const
                noexcept(noexcept(ranges::empty(std::declval<D const &>())))
            {
                return !ranges::empty(derived());
            }
            /// Access the size of the range, if it can be determined:
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && Cardinality >= 0)
            static constexpr range_size_type_t<D> size() noexcept
            {
                return static_cast<range_size_type_t<D>>(Cardinality);
            }
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && Cardinality < 0 &&
                    SizedSentinel<sentinel_t<D>, iterator_t<D>> &&
                    ForwardRange<D>)
            RANGES_CXX14_CONSTEXPR range_size_type_t<D> size()
            {
                return iter_size(derived().begin(), derived().end());
            }
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && Cardinality < 0 &&
                    SizedSentinel<sentinel_t<D const>, iterator_t<D const>> &&
                    ForwardRange<D const>)
            constexpr range_size_type_t<D> size() const
            {
                return iter_size(derived().begin(), derived().end());
            }
            /// Access the first element in a range:
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && ForwardRange<D>)
            RANGES_CXX14_CONSTEXPR range_reference_t<D> front()
            {
                return *derived().begin();
            }
            /// \overload
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && ForwardRange<D const>)
            constexpr range_reference_t<D const> front() const
            {
                return *derived().begin();
            }
            /// Access the last element in a range:
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> &&
                    BoundedRange<D> &&
                    BidirectionalRange<D>)
            RANGES_CXX14_CONSTEXPR range_reference_t<D> back()
            {
                return *prev(derived().end());
            }
            /// \overload
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> &&
                    BoundedRange<D const> &&
                    BidirectionalRange<D const>)
            constexpr range_reference_t<D const> back() const
            {
                return *prev(derived().end());
            }
            /// Simple indexing:
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && RandomAccessRange<D>)
            RANGES_CXX14_CONSTEXPR range_reference_t<D>
            operator[](range_difference_type_t<D> n)
            {
                return derived().begin()[n];
            }
            /// \overload
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && RandomAccessRange<D const>)
            constexpr range_reference_t<D const>
            operator[](range_difference_type_t<D> n) const
            {
                return derived().begin()[n];
            }
            /// Returns a reference to the element at specified location pos, with bounds checking.
            CONCEPT_template(typename D = Derived)(
                requires Same<D, Derived> && RandomAccessRange<D> && SizedRange<D>)
            RANGES_CXX14_CONSTEXPR range_reference_t<D>
            at(range_difference_type_t<D> n)
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
                requires Same<D, Derived> &&
                    RandomAccessRange<D const> &&
                    SizedRange<D const>)
            RANGES_CXX14_CONSTEXPR range_reference_t<D const>
            at(range_difference_type_t<D> n) const
            {
                using size_type = range_size_type_t<Derived>;
                if (n < 0 || size_type(n) >= ranges::size(derived()))
                {
                    throw std::out_of_range("view_interface::at");
                }
                return derived().begin()[n];
            }
            /// Python-ic slicing:
            //      rng[{4,6}]
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D &,
                        range_difference_type_t<D>,
                        range_difference_type_t<D>>)
            RANGES_CXX14_CONSTEXPR
            invoke_result_t<
                Slice,
                D &,
                range_difference_type_t<D>,
                range_difference_type_t<D>>
            operator[](detail::slice_bounds<range_difference_type_t<D>> offs) &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D const &,
                        range_difference_type_t<D>,
                        range_difference_type_t<D>>)
            constexpr
            invoke_result_t<
                Slice,
                D const &,
                range_difference_type_t<D>,
                range_difference_type_t<D>>
            operator[](detail::slice_bounds<range_difference_type_t<D>> offs) const &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D,
                        range_difference_type_t<D>,
                        range_difference_type_t<D>>)
            RANGES_CXX14_CONSTEXPR
            invoke_result_t<
                Slice,
                D,
                range_difference_type_t<D>,
                range_difference_type_t<D>>
            operator[](detail::slice_bounds<range_difference_type_t<D>> offs) &&
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{4,end-2}]
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D &,
                        range_difference_type_t<D>,
                        detail::from_end_<range_difference_type_t<D>>>)
            RANGES_CXX14_CONSTEXPR
            invoke_result_t<
                Slice,
                D &,
                range_difference_type_t<D>,
                detail::from_end_<range_difference_type_t<D>>>
            operator[](
                detail::slice_bounds<
                    range_difference_type_t<D>,
                    detail::from_end_<range_difference_type_t<D>>> offs) &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D const &,
                        range_difference_type_t<D>,
                        detail::from_end_<range_difference_type_t<D>>>)
            constexpr
            invoke_result_t<
                Slice,
                D const &,
                range_difference_type_t<D>,
                detail::from_end_<range_difference_type_t<D>>>
            operator[](
                detail::slice_bounds<
                    range_difference_type_t<D>,
                    detail::from_end_<range_difference_type_t<D>>> offs) const &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D,
                        range_difference_type_t<D>,
                        detail::from_end_<range_difference_type_t<D>>>)
            RANGES_CXX14_CONSTEXPR
            invoke_result_t<
                Slice,
                D,
                range_difference_type_t<D>,
                detail::from_end_<range_difference_type_t<D>>>
            operator[](
                detail::slice_bounds<
                    range_difference_type_t<D>,
                    detail::from_end_<range_difference_type_t<D>>> offs) &&
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{end-4,end-2}]
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D &,
                        detail::from_end_<range_difference_type_t<D>>,
                        detail::from_end_<range_difference_type_t<D>>>)
            RANGES_CXX14_CONSTEXPR
            invoke_result_t<
                Slice,
                D &,
                detail::from_end_<range_difference_type_t<D>>,
                detail::from_end_<range_difference_type_t<D>>>
            operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    detail::from_end_<range_difference_type_t<D>>> offs) &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D const &,
                        detail::from_end_<range_difference_type_t<D>>,
                        detail::from_end_<range_difference_type_t<D>>>)
            constexpr
            invoke_result_t<
                Slice,
                D const &,
                detail::from_end_<range_difference_type_t<D>>,
                detail::from_end_<range_difference_type_t<D>>>
            operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    detail::from_end_<range_difference_type_t<D>>> offs) const &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D,
                        detail::from_end_<range_difference_type_t<D>>,
                        detail::from_end_<range_difference_type_t<D>>>)
            RANGES_CXX14_CONSTEXPR
            invoke_result_t<
                Slice,
                D,
                detail::from_end_<range_difference_type_t<D>>,
                detail::from_end_<range_difference_type_t<D>>>
            operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    detail::from_end_<range_difference_type_t<D>>> offs) &&
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{4,end}]
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<Slice, D &, range_difference_type_t<D>, end_fn>)
            RANGES_CXX14_CONSTEXPR invoke_result_t<Slice, D &, range_difference_type_t<D>, end_fn>
            operator[](detail::slice_bounds<range_difference_type_t<D>, end_fn> offs) &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<Slice, D const &, range_difference_type_t<D>, end_fn>)
            constexpr invoke_result_t<Slice, D const &, range_difference_type_t<D>, end_fn>
            operator[](detail::slice_bounds<range_difference_type_t<D>, end_fn> offs) const &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<Slice, D, range_difference_type_t<D>, end_fn>)
            RANGES_CXX14_CONSTEXPR invoke_result_t<Slice, D, range_difference_type_t<D>, end_fn>
            operator[](detail::slice_bounds<range_difference_type_t<D>, end_fn> offs) &&
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{end-4,end}]
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<Slice, D &, detail::from_end_<range_difference_type_t<D>>, end_fn>)
            RANGES_CXX14_CONSTEXPR
            invoke_result_t<Slice, D &, detail::from_end_<range_difference_type_t<D>>, end_fn>
            operator[](
                detail::slice_bounds<detail::from_end_<range_difference_type_t<D>>, end_fn> offs) &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D const &,
                        detail::from_end_<range_difference_type_t<D>>,
                        end_fn>)
            constexpr
            invoke_result_t<
                Slice,
                D const &,
                detail::from_end_<range_difference_type_t<D>>,
                end_fn>
            operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    end_fn> offs) const &
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            CONCEPT_template(typename D = Derived, typename Slice = view::slice_fn)(
                requires Same<D, Derived> &&
                    Invocable<
                        Slice,
                        D,
                        detail::from_end_<range_difference_type_t<D>>,
                        end_fn>)
            RANGES_CXX14_CONSTEXPR
            invoke_result_t<Slice, D, detail::from_end_<range_difference_type_t<D>>, end_fn>
            operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    end_fn> offs) &&
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            /// Implicit conversion to something that looks like a container.
            CONCEPT_template(typename Container, typename D = Derived)(
                requires detail::ConvertibleToContainer<D, Container>)
            RANGES_CXX14_CONSTEXPR operator Container ()
            {
                return ranges::to_<Container>(derived());
            }
            /// \overload
            CONCEPT_template(typename Container, typename D = Derived)(
                requires detail::ConvertibleToContainer<D const, Container>)
            constexpr operator Container () const
            {
                return ranges::to_<Container>(derived());
            }
            /// \brief Print a range to an ostream
        private:
            CONCEPT_template(typename Stream, typename Rng)(
                requires Same<Derived, meta::_t<std::remove_cv<Rng>>>)
            static Stream & print_(Stream &sout, Rng &rng)
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

            template<typename D = Derived>
            friend auto operator<<(std::ostream &sout, Derived const &rng) ->
                CONCEPT_broken_friend_return_type(std::ostream&)(
                    requires Same<D, Derived> && InputRange<D const>)
            {
                return view_interface::print_(sout, rng);
            }
            /// \overload
            template<typename D = Derived>
            friend auto operator<<(std::ostream &sout, Derived &rng) ->
                CONCEPT_broken_friend_return_type(std::ostream&)(
                    requires Same<D, Derived> && not Range<D const> && InputRange<D>)
            {
                return view_interface::print_(sout, rng);
            }
            /// \overload
            template<typename D = Derived>
            friend auto operator<<(std::ostream &sout, Derived &&rng) ->
                CONCEPT_broken_friend_return_type(std::ostream&)(
                    requires Same<D, Derived> && not Range<D const> && InputRange<D>)
            {
                return view_interface::print_(sout, rng);
            }
        };
        /// @}
    }
}

#endif
