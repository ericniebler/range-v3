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
#include <range/v3/range/access.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/iterator/common_iterator.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/iterator/operations.hpp>

namespace ranges
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

            constexpr explicit from_end_(Int dist)
              : dist_(dist)
            {}

            CPP_template(typename Other)(
                requires Integral<Other> && ExplicitlyConvertibleTo<Other, Int>)
            constexpr operator from_end_<Other> () const
            {
                return from_end_<Other>{static_cast<Other>(dist_)};
            }
        };

        template<typename Rng>
        using from_end_of_t = from_end_<range_difference_t<Rng>>;

        CPP_def
        (
            template(typename Rng)
            concept CanEmpty,
                requires (Rng &rng)
                (
                    ranges::empty(rng)
                )
        );

        constexpr bool has_fixed_size_(cardinality c) noexcept
        {
            return c >= 0 || c == infinite;
        }

        template<bool>
        struct dependent_
        {
            template<typename T>
            using invoke = T;
        };
    } // namespace detail
    /// \endcond

    /// \addtogroup group-core
    /// @{
    template<typename Derived, cardinality Cardinality /* = finite*/>
    struct view_interface
      : basic_view<Cardinality>
    {
    protected:
        template<bool B>
        using D = meta::invoke<detail::dependent_<B>, Derived>;

        constexpr /*c++14*/ Derived &derived() noexcept
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
                requires detail::has_fixed_size_(Cardinality))
        {
            return Cardinality == 0;
        }
        /// \overload
        template<bool True = true>
        constexpr /*c++14*/ auto empty()
            noexcept(noexcept(bool(ranges::begin(std::declval<D<True> &>()) ==
                ranges::end(std::declval<D<True> &>())))) ->
            CPP_ret(bool)(
                requires True && (!detail::has_fixed_size_(Cardinality)) &&
                    ForwardRange<D<True>>)
        {
            return bool(ranges::begin(derived()) == ranges::end(derived()));
        }
        /// \overload
        template<bool True = true>
        constexpr auto empty() const
            noexcept(noexcept(bool(ranges::begin(std::declval<D<True> const &>()) ==
                ranges::end(std::declval<D<True> const &>())))) ->
            CPP_ret(bool)(
                requires True && not detail::has_fixed_size_(Cardinality) &&
                    ForwardRange<D<True> const>)
        {
            return bool(ranges::begin(derived()) == ranges::end(derived()));
        }
        CPP_template(bool True = true)(
            requires True && detail::CanEmpty<D<True>>)
        constexpr /*c++14*/ explicit operator bool()
            noexcept(noexcept(ranges::empty(std::declval<D<True> &>())))
        {
            return !ranges::empty(derived());
        }
        /// \overload
        CPP_template(bool True = true)(
            requires True && detail::CanEmpty<D<True> const>)
        constexpr explicit operator bool() const
            noexcept(noexcept(ranges::empty(std::declval<D<True> const &>())))
        {
            return !ranges::empty(derived());
        }
        /// If the size of the range is known at compile-time and finite,
        /// return it.
        template<bool True = true>
        static constexpr auto size() noexcept ->
            CPP_ret(std::size_t)(
                requires True && Cardinality >= 0)
        {
            return static_cast<std::size_t>(Cardinality);
        }
        /// If `Sentinel<sentinel_t<Derived>, iterator_t<Derived>>` is satisfied,
        /// and if `Derived` is a `ForwardRange`, then return `end - begin` cast
        /// to an unsigned integer.
        template<bool True = true>
        constexpr /*c++14*/ auto size() ->
            CPP_ret(meta::_t<std::make_unsigned<range_difference_t<D<True>>>>)(
                requires True && Cardinality < 0 &&
                    SizedSentinel<sentinel_t<D<True>>, iterator_t<D<True>>> &&
                    ForwardRange<D<True>>)
        {
            using size_type = meta::_t<std::make_unsigned<range_difference_t<D<True>>>>;
            return static_cast<size_type>(derived().end() - derived().begin());
        }
        /// \overload
        template<bool True = true>
        constexpr auto size() const ->
            CPP_ret(meta::_t<std::make_unsigned<range_difference_t<D<True>>>>)(
                requires True && (Cardinality < 0) &&
                    SizedSentinel<sentinel_t<D<True> const>, iterator_t<D<True> const>> &&
                    ForwardRange<D<True> const>)
        {
            using size_type = meta::_t<std::make_unsigned<range_difference_t<D<True>>>>;
            return static_cast<size_type>(derived().end() - derived().begin());
        }
        /// Access the first element in a range:
        template<bool True = true>
        constexpr /*c++14*/ auto front() ->
            CPP_ret(range_reference_t<D<True>>)(
                requires True && ForwardRange<D<True>>)
        {
            return *derived().begin();
        }
        /// \overload
        template<bool True = true>
        constexpr /*c++14*/ auto front() const ->
            CPP_ret(range_reference_t<D<True> const>)(
                requires True && ForwardRange<D<True> const>)
        {
            return *derived().begin();
        }
        /// Access the last element in a range:
        template<bool True = true>
        constexpr /*c++14*/ auto back() ->
            CPP_ret(range_reference_t<D<True>>)(
                requires True &&
                    CommonRange<D<True>> &&
                    BidirectionalRange<D<True>>)
        {
            return *prev(derived().end());
        }
        /// \overload
        template<bool True = true>
        constexpr /*c++14*/ auto back() const ->
            CPP_ret(range_reference_t<D<True> const>)(
                requires True &&
                    CommonRange<D<True> const> &&
                    BidirectionalRange<D<True> const>)
        {
            return *prev(derived().end());
        }
        /// Simple indexing:
        template<bool True = true>
        constexpr /*c++14*/
        auto operator[](range_difference_t<D<True>> n) ->
            CPP_ret(range_reference_t<D<True>>)(
                requires True && RandomAccessRange<D<True>>)
        {
            return derived().begin()[n];
        }
        /// \overload
        template<bool True = true>
        constexpr /*c++14*/
        auto operator[](range_difference_t<D<True>> n) const ->
            CPP_ret(range_reference_t<D<True> const>)(
                requires True && RandomAccessRange<D<True> const>)
        {
            return derived().begin()[n];
        }
        /// Returns a reference to the element at specified location pos, with bounds checking.
        template<bool True = true>
        constexpr /*c++14*/
        auto at(range_difference_t<D<True>> n) ->
            CPP_ret(range_reference_t<D<True>>)(
                requires True && RandomAccessRange<D<True>> && SizedRange<D<True>>)
        {
            using size_type = range_size_t<Derived>;
            if (n < 0 || size_type(n) >= ranges::size(derived()))
            {
                throw std::out_of_range("view_interface::at");
            }
            return derived().begin()[n];
        }
        /// \overload
        template<bool True = true>
        constexpr /*c++14*/
        auto at(range_difference_t<D<True>> n) const ->
            CPP_ret(range_reference_t<D<True> const>)(
                requires True &&
                    RandomAccessRange<D<True> const> &&
                    SizedRange<D<True> const>)
        {
            using size_type = range_size_t<Derived const>;
            if (n < 0 || size_type(n) >= ranges::size(derived()))
            {
                throw std::out_of_range("view_interface::at");
            }
            return derived().begin()[n];
        }
        /// Python-ic slicing:
        //      rng[{4,6}]
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True && InputRange<D<True> &>)
        constexpr /*c++14*/
        auto operator[](detail::slice_bounds<range_difference_t<D<True>>> offs) &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True && InputRange<D<True> const &>)
        constexpr
        auto operator[](detail::slice_bounds<range_difference_t<D<True>>> offs) const &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True && InputRange<D<True>>)
        constexpr /*c++14*/
        auto operator[](detail::slice_bounds<range_difference_t<D<True>>> offs) &&
        {
            return Slice{}(detail::move(derived()), offs.from, offs.to);
        }
        //      rng[{4,end-2}]
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True && InputRange<D<True> &> && SizedRange<D<True> &>)
        constexpr /*c++14*/
        auto operator[](
            detail::slice_bounds<
                range_difference_t<D<True>>, detail::from_end_of_t<D<True>>> offs) &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True && InputRange<D<True> const &> && SizedRange<D<True> const &>)
        constexpr /*c++14*/
        auto operator[](
            detail::slice_bounds<
                range_difference_t<D<True>>, detail::from_end_of_t<D<True>>> offs) const &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True && InputRange<D<True>> && SizedRange<D<True>>)
        constexpr /*c++14*/
        auto operator[](
            detail::slice_bounds<
                range_difference_t<D<True>>, detail::from_end_of_t<D<True>>> offs) &&
        {
            return Slice{}(detail::move(derived()), offs.from, offs.to);
        }
        //      rng[{end-4,end-2}]
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True &&
                (ForwardRange<D<True> &> || (InputRange<D<True> &> && SizedRange<D<True> &>)))
        constexpr /*c++14*/
        auto operator[](
            detail::slice_bounds<
                detail::from_end_of_t<D<True>>, detail::from_end_of_t<D<True>>> offs) &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True &&
                (ForwardRange<D<True> const &> ||
                    (InputRange<D<True> const &> && SizedRange<D<True> const &>)))
        constexpr /*c++14*/
        auto operator[](
            detail::slice_bounds<
                detail::from_end_of_t<D<True>>, detail::from_end_of_t<D<True>>> offs) const &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True &&
                (ForwardRange<D<True>> || (InputRange<D<True>> && SizedRange<D<True>>)))
        constexpr /*c++14*/
        auto operator[](
            detail::slice_bounds<
                detail::from_end_of_t<D<True>>, detail::from_end_of_t<D<True>>> offs) &&
        {
            return Slice{}(detail::move(derived()), offs.from, offs.to);
        }
        //      rng[{4,end}]
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True && InputRange<D<True> &>)
        constexpr /*c++14*/
        auto operator[](detail::slice_bounds<range_difference_t<D<True>>, end_fn> offs) &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True && InputRange<D<True> const &>)
        constexpr /*c++14*/
        auto operator[](detail::slice_bounds<range_difference_t<D<True>>, end_fn> offs) const &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True && InputRange<D<True>>)
        constexpr /*c++14*/
        auto operator[](detail::slice_bounds<range_difference_t<D<True>>, end_fn> offs) &&
        {
            return Slice{}(detail::move(derived()), offs.from, offs.to);
        }
        //      rng[{end-4,end}]
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True &&
                (ForwardRange<D<True> &> || (InputRange<D<True> &> && SizedRange<D<True> &>)))
        constexpr /*c++14*/
        auto operator[](
            detail::slice_bounds<detail::from_end_of_t<D<True>>, end_fn> offs) &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True &&
                (ForwardRange<D<True> const &> ||
                    (InputRange<D<True> const &> && SizedRange<D<True> const &>)))
        constexpr /*c++14*/
        auto operator[](
            detail::slice_bounds<detail::from_end_of_t<D<True>>, end_fn> offs) const &
        {
            return Slice{}(derived(), offs.from, offs.to);
        }
        /// \overload
        CPP_template(bool True = true, typename Slice = view::slice_fn)(
            requires True &&
                (ForwardRange<D<True>> || (InputRange<D<True>> && SizedRange<D<True>>)))
        constexpr /*c++14*/
        auto operator[](
            detail::slice_bounds<detail::from_end_of_t<D<True>>, end_fn> offs) &&
        {
            return Slice{}(detail::move(derived()), offs.from, offs.to);
        }
        /// Implicit conversion to something that looks like a container.
        CPP_template(typename Container, bool True = true)(
            requires detail::ConvertibleToContainer<D<True>, Container>)
        constexpr /*c++14*/ operator Container ()
        {
            return ranges::to<Container>(derived());
        }
        /// \overload
        CPP_template(typename Container, bool True = true)(
            requires detail::ConvertibleToContainer<D<True> const, Container>)
        constexpr operator Container () const
        {
            return ranges::to<Container>(derived());
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

        template<bool True = true>
        friend auto operator<<(std::ostream &sout, Derived const &rng) ->
            CPP_broken_friend_ret(std::ostream &)(
                requires True && InputRange<D<True> const>)
        {
            return view_interface::print_(sout, rng);
        }
        /// \overload
        template<bool True = true>
        friend auto operator<<(std::ostream &sout, Derived &rng) ->
            CPP_broken_friend_ret(std::ostream &)(
                requires True && (!Range<D<True> const>) && InputRange<D<True>>)
        {
            return view_interface::print_(sout, rng);
        }
        /// \overload
        template<bool True = true>
        friend auto operator<<(std::ostream &sout, Derived &&rng) ->
            CPP_broken_friend_ret(std::ostream &)(
                requires True && (!Range<D<True> const>) && InputRange<D<True>>)
        {
            return view_interface::print_(sout, rng);
        }
    };
    /// @}
}

#endif
