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
                CPP_template(typename F, typename T)(
                    requires ConvertibleTo<F, From> && ConvertibleTo<T, To>)
                constexpr slice_bounds(F from, T to)
                  : from(from), to(to)
                {}
            };

            template<typename Int>
            struct from_end_
            {
                Int dist_;

                CPP_template(typename Other)(
                    requires Integral<Other> && ExplicitlyConvertibleTo<Other, Int>)
                constexpr operator from_end_<Other> () const
                {
                    return {static_cast<Other>(dist_)};
                }
            };

            CPP_def
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
                CPP_assert(DerivedFrom<Derived, view_interface>);
                return static_cast<Derived &>(*this);
            }
            /// \overload
            constexpr Derived const &derived() const noexcept
            {
                CPP_assert(DerivedFrom<Derived, view_interface>);
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
            CPP_member
            constexpr auto empty() const noexcept ->
                CPP_ret(bool)(
                    requires Cardinality >= 0 || Cardinality == infinite)
            {
                return Cardinality == 0;
            }
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR auto empty()
                noexcept(noexcept(bool(ranges::begin(std::declval<D &>()) ==
                    ranges::end(std::declval<D &>())))) ->
                CPP_ret(bool)(
                    requires Same<D, Derived> &&
                        Cardinality < 0 && Cardinality != infinite &&
                        Range<D> && ForwardIterator<iterator_t<D>>)
            {
                return bool(ranges::begin(derived()) == ranges::end(derived()));
            }
            template<typename D = Derived>
            constexpr auto empty() const
                noexcept(noexcept(bool(ranges::begin(std::declval<D const &>()) ==
                    ranges::end(std::declval<D const &>())))) ->
                CPP_ret(bool)(
                    requires Same<D, Derived> &&
                        Cardinality < 0 && Cardinality != infinite &&
                        Range<D const> && ForwardIterator<iterator_t<D const>>)
            {
                return bool(ranges::begin(derived()) == ranges::end(derived()));
            }
            CPP_template(typename D = Derived)(
                requires Same<D, Derived> && detail::CanEmpty<D &>)
            RANGES_CXX14_CONSTEXPR explicit operator bool()
                noexcept(noexcept(ranges::empty(std::declval<D &>())))
            {
                return !ranges::empty(derived());
            }
            CPP_template(typename D = Derived)(
                requires Same<D, Derived> && detail::CanEmpty<D const &>)
            constexpr explicit operator bool() const
                noexcept(noexcept(ranges::empty(std::declval<D const &>())))
            {
                return !ranges::empty(derived());
            }
            /// Access the size of the range, if it can be determined:
            template<typename D = Derived>
            static constexpr auto size() noexcept ->
                CPP_ret(range_size_type_t<D>)(
                    requires Same<D, Derived> && Cardinality >= 0)
            {
                return static_cast<range_size_type_t<D>>(Cardinality);
            }
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR auto size() ->
                CPP_ret(range_size_type_t<D>)(
                    requires Same<D, Derived> && Cardinality < 0 &&
                        SizedSentinel<sentinel_t<D>, iterator_t<D>> &&
                        ForwardRange<D>)
            {
                return iter_size(derived().begin(), derived().end());
            }
            template<typename D = Derived>
            constexpr auto size() const ->
                CPP_ret(range_size_type_t<D>)(
                    requires Same<D, Derived> && Cardinality < 0 &&
                        SizedSentinel<sentinel_t<D const>, iterator_t<D const>> &&
                        ForwardRange<D const>)
            {
                return iter_size(derived().begin(), derived().end());
            }
            /// Access the first element in a range:
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR auto front() ->
                CPP_ret(range_reference_t<D>)(
                    requires Same<D, Derived> && ForwardRange<D>)
            {
                return *derived().begin();
            }
            /// \overload
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR auto front() const ->
                CPP_ret(range_reference_t<D const>)(
                    requires Same<D, Derived> && ForwardRange<D const>)
            {
                return *derived().begin();
            }
            /// Access the last element in a range:
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR auto back() ->
                CPP_ret(range_reference_t<D>)(
                    requires Same<D, Derived> &&
                        BoundedRange<D> &&
                        BidirectionalRange<D>)
            {
                return *prev(derived().end());
            }
            /// \overload
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR auto back() const ->
                CPP_ret(range_reference_t<D const>)(
                    requires Same<D, Derived> &&
                        BoundedRange<D const> &&
                        BidirectionalRange<D const>)
            {
                return *prev(derived().end());
            }
            /// Simple indexing:
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR
            auto operator[](range_difference_type_t<D> n) ->
                CPP_ret(range_reference_t<D>)(
                    requires Same<D, Derived> && RandomAccessRange<D>)
            {
                return derived().begin()[n];
            }
            /// \overload
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR
            auto operator[](range_difference_type_t<D> n) const ->
                CPP_ret(range_reference_t<D const>)(
                    requires Same<D, Derived> && RandomAccessRange<D const>)
            {
                return derived().begin()[n];
            }
            /// Returns a reference to the element at specified location pos, with bounds checking.
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR
            auto at(range_difference_type_t<D> n) ->
                CPP_ret(range_reference_t<D>)(
                    requires Same<D, Derived> && RandomAccessRange<D> && SizedRange<D>)
            {
                using size_type = range_size_type_t<Derived>;
                if (n < 0 || size_type(n) >= ranges::size(derived()))
                {
                    throw std::out_of_range("view_interface::at");
                }
                return derived().begin()[n];
            }
            /// \overload
            template<typename D = Derived>
            RANGES_CXX14_CONSTEXPR
            auto at(range_difference_type_t<D> n) const ->
                CPP_ret(range_reference_t<D const>)(
                    requires Same<D, Derived> &&
                        RandomAccessRange<D const> &&
                        SizedRange<D const>)
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
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](detail::slice_bounds<range_difference_type_t<D>> offs) & ->
                CPP_ret(
                    invoke_result_t<
                        Slice,
                        D &,
                        range_difference_type_t<D>,
                        range_difference_type_t<D>>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            constexpr
            auto operator[](detail::slice_bounds<range_difference_type_t<D>> offs) const & ->
                CPP_ret(
                    invoke_result_t<
                        Slice,
                        D const &,
                        range_difference_type_t<D>,
                        range_difference_type_t<D>>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](detail::slice_bounds<range_difference_type_t<D>> offs) && ->
                CPP_ret(
                    invoke_result_t<
                        Slice,
                        D,
                        range_difference_type_t<D>,
                        range_difference_type_t<D>>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{4,end-2}]
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](
                detail::slice_bounds<
                    range_difference_type_t<D>,
                    detail::from_end_<range_difference_type_t<D>>> offs) & ->
                CPP_ret(
                    invoke_result_t<
                        Slice,
                        D &,
                        range_difference_type_t<D>,
                        detail::from_end_<range_difference_type_t<D>>>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](
                detail::slice_bounds<
                    range_difference_type_t<D>,
                    detail::from_end_<range_difference_type_t<D>>> offs) const & ->
                CPP_ret(
                    invoke_result_t<
                        Slice,
                        D const &,
                        range_difference_type_t<D>,
                        detail::from_end_<range_difference_type_t<D>>>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](
                detail::slice_bounds<
                    range_difference_type_t<D>,
                    detail::from_end_<range_difference_type_t<D>>> offs) && ->
                CPP_ret(
                    invoke_result_t<
                        Slice,
                        D,
                        range_difference_type_t<D>,
                        detail::from_end_<range_difference_type_t<D>>>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{end-4,end-2}]
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    detail::from_end_<range_difference_type_t<D>>> offs) & ->
                CPP_ret(
                    invoke_result_t<
                        Slice,
                        D &,
                        detail::from_end_<range_difference_type_t<D>>,
                        detail::from_end_<range_difference_type_t<D>>>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    detail::from_end_<range_difference_type_t<D>>> offs) const & ->
                CPP_ret(
                    invoke_result_t<
                        Slice,
                        D const &,
                        detail::from_end_<range_difference_type_t<D>>,
                        detail::from_end_<range_difference_type_t<D>>>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    detail::from_end_<range_difference_type_t<D>>> offs) && ->
                CPP_ret(
                    invoke_result_t<
                        Slice,
                        D,
                        detail::from_end_<range_difference_type_t<D>>,
                        detail::from_end_<range_difference_type_t<D>>>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{4,end}]
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](detail::slice_bounds<range_difference_type_t<D>, end_fn> offs) & ->
                CPP_ret(
                    invoke_result_t<Slice, D &, range_difference_type_t<D>, end_fn>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](detail::slice_bounds<range_difference_type_t<D>, end_fn> offs) const & ->
                CPP_ret(
                    invoke_result_t<Slice, D const &, range_difference_type_t<D>, end_fn>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](detail::slice_bounds<range_difference_type_t<D>, end_fn> offs) && ->
                CPP_ret(
                    invoke_result_t<Slice, D, range_difference_type_t<D>, end_fn>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            //      rng[{end-4,end}]
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](
                detail::slice_bounds<detail::from_end_<range_difference_type_t<D>>, end_fn> offs) & ->
                CPP_ret(
                    invoke_result_t<Slice, D &, detail::from_end_<range_difference_type_t<D>>, end_fn>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    end_fn> offs) const & ->
                CPP_ret(
                    invoke_result_t<Slice, D const &, detail::from_end_<range_difference_type_t<D>>, end_fn>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(derived(), offs.from, offs.to);
            }
            /// \overload
            template<typename D = Derived, typename Slice = view::slice_fn>
            RANGES_CXX14_CONSTEXPR
            auto operator[](
                detail::slice_bounds<
                    detail::from_end_<range_difference_type_t<D>>,
                    end_fn> offs) && ->
                CPP_ret(
                    invoke_result_t<Slice, D, detail::from_end_<range_difference_type_t<D>>, end_fn>)(
                    requires Same<D, Derived>)
            {
                return Slice{}(detail::move(derived()), offs.from, offs.to);
            }
            /// Implicit conversion to something that looks like a container.
            CPP_template(typename Container, typename D = Derived)(
                requires detail::ConvertibleToContainer<D, Container>)
            RANGES_CXX14_CONSTEXPR operator Container ()
            {
                return ranges::to_<Container>(derived());
            }
            /// \overload
            CPP_template(typename Container, typename D = Derived)(
                requires detail::ConvertibleToContainer<D const, Container>)
            constexpr operator Container () const
            {
                return ranges::to_<Container>(derived());
            }
            /// \brief Print a range to an ostream
        private:
            template<typename Stream, typename Rng>
            static auto print_(Stream &sout, Rng &rng) ->
                CPP_ret(Stream &)(
                    requires Same<Derived, meta::_t<std::remove_cv<Rng>>>)
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
                CPP_broken_friend_ret(std::ostream &)(
                    requires Same<D, Derived> && InputRange<D const>)
            {
                return view_interface::print_(sout, rng);
            }
            /// \overload
            template<typename D = Derived>
            friend auto operator<<(std::ostream &sout, Derived &rng) ->
                CPP_broken_friend_ret(std::ostream &)(
                    requires Same<D, Derived> && not Range<D const> && InputRange<D>)
            {
                return view_interface::print_(sout, rng);
            }
            /// \overload
            template<typename D = Derived>
            friend auto operator<<(std::ostream &sout, Derived &&rng) ->
                CPP_broken_friend_ret(std::ostream &)(
                    requires Same<D, Derived> && not Range<D const> && InputRange<D>)
            {
                return view_interface::print_(sout, rng);
            }
        };
        /// @}
    }
}

#endif
