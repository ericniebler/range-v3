/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_UTILITY_BASIC_ITERATOR_HPP
#define RANGES_V3_UTILITY_BASIC_ITERATOR_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_access.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/nullptr_v.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Cur>
            using cursor_reference_t =
                decltype(range_access::read(std::declval<Cur const &>()));

            // Compute the rvalue reference type of a cursor
            template<typename Cur>
            auto cursor_move(Cur const &cur, int) ->
                decltype(range_access::move(cur));
            template<typename Cur>
            auto cursor_move(Cur const &cur, long) ->
                aux::move_t<cursor_reference_t<Cur>>;

            template<typename Cur>
            using cursor_rvalue_reference_t =
                decltype(detail::cursor_move(std::declval<Cur const &>(), 42));

            // Define conversion operators from the proxy reference type
            // to the common reference types, so that basic_iterator can model Readable
            // even with getters/setters.
            template<typename Derived, typename Head>
            struct proxy_reference_conversion
            {
                operator Head() const
                    noexcept(noexcept(Head(Head(std::declval<Derived const &>().read_()))))
                {
                    return Head(static_cast<Derived const *>(this)->read_());
                }
            };

            // Collect the reference types associated with cursors
            template<typename Cur, bool Readable>
            struct cursor_traits_
            {
            private:
                struct private_ {};
            public:
                using value_t_ = private_;
                using reference_t_ = private_;
                using rvalue_reference_t_ = private_;
                using common_refs = meta::list<>;
            };

            template<typename Cur>
            struct cursor_traits_<Cur, true>
            {
                using value_t_ = range_access::cursor_value_t<Cur>;
                using reference_t_ = cursor_reference_t<Cur>;
                using rvalue_reference_t_ = cursor_rvalue_reference_t<Cur>;
            private:
                using R1 = reference_t_;
                using R2 = common_reference_t<reference_t_, value_t_ &>;
                using R3 = common_reference_t<reference_t_, rvalue_reference_t_>;
                using tmp1 = meta::list<value_t_, R1>;
                using tmp2 =
                    meta::if_<meta::in<tmp1, uncvref_t<R2>>, tmp1, meta::push_back<tmp1, R2>>;
                using tmp3 =
                    meta::if_<meta::in<tmp2, uncvref_t<R3>>, tmp2, meta::push_back<tmp2, R3>>;
            public:
                using common_refs = meta::unique<meta::pop_front<tmp3>>;
            };

            template<typename Cur>
            using cursor_traits = cursor_traits_<Cur, (bool) ReadableCursor<Cur>>;

            template<typename Cur, bool Readable>
            struct basic_proxy_reference_;
            template<typename Cur>
            using basic_proxy_reference = basic_proxy_reference_<Cur, (bool) ReadableCursor<Cur>>;

            // The One Proxy Reference type to rule them all. basic_iterator uses this
            // as the return type of operator* when the cursor type has a set() member
            // function of the correct signature (i.e., if it can accept a value_type &&).
            template<typename Cur, bool Readable /*= (bool) ReadableCursor<Cur>*/>
            struct basic_proxy_reference_
              : cursor_traits<Cur>
                // The following adds conversion operators to the common reference
                // types, so that basic_proxy_reference can model Readable
              , meta::inherit<
                    meta::transform<
                        typename cursor_traits<Cur>::common_refs,
                        meta::bind_front<
                            meta::quote<proxy_reference_conversion>,
                            basic_proxy_reference_<Cur, Readable>>>>
            {
            private:
                Cur *cur_;
                template<typename, bool>
                friend struct basic_proxy_reference_;
                template<typename, typename>
                friend struct proxy_reference_conversion;
                using typename cursor_traits<Cur>::value_t_;
                using typename cursor_traits<Cur>::reference_t_;
                using typename cursor_traits<Cur>::rvalue_reference_t_;
                static_assert((bool) CommonReference<value_t_ &, reference_t_>,
                    "Your readable and writable cursor must have a value type and a reference "
                    "type that share a common reference type. See the ranges::common_reference "
                    "type trait.");
                RANGES_CXX14_CONSTEXPR
                reference_t_ read_() const
                    noexcept(noexcept(reference_t_(range_access::read(std::declval<Cur const &>()))))
                {
                    return range_access::read(*cur_);
                }
                template<typename T>
                RANGES_CXX14_CONSTEXPR
                void write_(T &&t) const
                {
                    range_access::write(*cur_, (T &&) t);
                }
            public:
                basic_proxy_reference_() = default;
                basic_proxy_reference_(basic_proxy_reference_ const &) = default;
                CONCEPT_template(typename OtherCur)(
                    requires ConvertibleTo<OtherCur *, Cur *>)
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_(basic_proxy_reference<OtherCur> const &that) noexcept
                  : cur_(that.cur_)
                {}
                RANGES_CXX14_CONSTEXPR
                explicit basic_proxy_reference_(Cur &cur) noexcept
                  : cur_(&cur)
                {}
                CONCEPT_requires(ReadableCursor<Cur>)
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ & operator=(basic_proxy_reference_ && that)
                {
                    return *this = that;
                }
                CONCEPT_requires(ReadableCursor<Cur>)
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ & operator=(basic_proxy_reference_ const &that)
                {
                    this->write_(that.read_());
                    return *this;
                }
                CONCEPT_requires(ReadableCursor<Cur>)
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ const & operator=(basic_proxy_reference_ && that) const
                {
                    return *this = that;
                }
                CONCEPT_requires(ReadableCursor<Cur>)
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ const & operator=(basic_proxy_reference_ const &that) const
                {
                    this->write_(that.read_());
                    return *this;
                }
                CONCEPT_template(typename OtherCur)(
                    requires ReadableCursor<OtherCur> &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>())
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ & operator=(basic_proxy_reference<OtherCur> && that)
                {
                    return *this = that;
                }
                CONCEPT_template(typename OtherCur)(
                    requires ReadableCursor<OtherCur> &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>())
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ & operator=(basic_proxy_reference<OtherCur> const &that)
                {
                    this->write_(that.read_());
                    return *this;
                }
                CONCEPT_template(typename OtherCur)(
                    requires ReadableCursor<OtherCur> &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>())
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ const & operator=(basic_proxy_reference<OtherCur> && that) const
                {
                    return *this = that;
                }
                CONCEPT_template(typename OtherCur)(
                    requires ReadableCursor<OtherCur> &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>())
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ const & operator=(basic_proxy_reference<OtherCur> const &that) const
                {
                    this->write_(that.read_());
                    return *this;
                }
                CONCEPT_template(typename T)(
                    requires WritableCursor<Cur, T>)
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ & operator=(T &&t)
                {
                    this->write_((T &&) t);
                    return *this;
                }
                CONCEPT_template(typename T)(
                    requires WritableCursor<Cur, T &&>)
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference_ const & operator=(T &&t) const
                {
                    this->write_((T &&) t);
                    return *this;
                }
                CONCEPT_template(typename V = value_t_)(
                    requires ReadableCursor<Cur> && EqualityComparable<V>)
                RANGES_CXX14_CONSTEXPR
                friend bool operator==(basic_proxy_reference_ const &x, value_t_ const &y)
                {
                    return x.read_() == y;
                }
                CONCEPT_template(typename V = value_t_)(
                    requires ReadableCursor<Cur> && EqualityComparable<V>)
                RANGES_CXX14_CONSTEXPR
                friend bool operator!=(basic_proxy_reference_ const &x, value_t_ const &y)
                {
                    return !(x == y);
                }
                CONCEPT_template(typename V = value_t_)(
                    requires ReadableCursor<Cur> && EqualityComparable<V>)
                RANGES_CXX14_CONSTEXPR
                friend bool operator==(value_t_ const &x, basic_proxy_reference_ const &y)
                {
                    return x == y.read_();
                }
                CONCEPT_template(typename V = value_t_)(
                    requires ReadableCursor<Cur> && EqualityComparable<V>)
                RANGES_CXX14_CONSTEXPR
                friend bool operator!=(value_t_ const &x, basic_proxy_reference_ const &y)
                {
                    return !(x == y);
                }
                CONCEPT_template(typename V = value_t_)(
                    requires ReadableCursor<Cur> && EqualityComparable<V>)
                RANGES_CXX14_CONSTEXPR
                friend bool operator==(basic_proxy_reference_ const &x, basic_proxy_reference_ const &y)
                {
                    return x.read_() == y.read_();
                }
                CONCEPT_template(typename V = value_t_)(
                    requires ReadableCursor<Cur> && EqualityComparable<V>)
                RANGES_CXX14_CONSTEXPR
                friend bool operator!=(basic_proxy_reference_ const &x, basic_proxy_reference_ const &y)
                {
                    return !(x == y);
                }
            };

            auto iter_cat(input_cursor_tag) ->
                ranges::input_iterator_tag;
            auto iter_cat(forward_cursor_tag) ->
                ranges::forward_iterator_tag;
            auto iter_cat(bidirectional_cursor_tag) ->
                ranges::bidirectional_iterator_tag;
            auto iter_cat(random_access_cursor_tag) ->
                ranges::random_access_iterator_tag;

            template<typename Cur, bool Readable /*= (bool) ReadableCursor<Cur>*/>
            struct iterator_associated_types_base_
            {
            protected:
                using reference_t = basic_proxy_reference<Cur>;
                using const_reference_t = basic_proxy_reference<Cur const>;
                using cursor_tag_t = concepts::tag<detail::OutputCursorConcept, cursor_tag>;
            public:
                using reference = void;
                using difference_type = range_access::cursor_difference_t<Cur>;
            };

            template<typename Cur>
            using cursor_arrow_t =
                decltype(range_access::arrow(std::declval<Cur const &>()));

            template<typename Cur>
            struct iterator_associated_types_base_<Cur, true>
            {
            protected:
                using cursor_tag_t = detail::cursor_tag_of<Cur>;
                using reference_t =
                    meta::if_<
                        is_writable_cursor<Cur const>,
                        basic_proxy_reference<Cur const>,
                        meta::if_<
                            is_writable_cursor<Cur>,
                            basic_proxy_reference<Cur>,
                            cursor_reference_t<Cur>>>;
                using const_reference_t =
                    meta::if_<
                        is_writable_cursor<Cur const>,
                        basic_proxy_reference<Cur const>,
                        cursor_reference_t<Cur>>;
            public:
                using difference_type = range_access::cursor_difference_t<Cur>;
                using value_type = range_access::cursor_value_t<Cur>;
                using reference = reference_t;
                using iterator_category =
                    decltype(detail::iter_cat(cursor_tag_t()));
                using pointer = meta::_t<meta::if_c<
                    HasCursorArrow<Cur>,
                    meta::defer<cursor_arrow_t, Cur>,
                    std::add_pointer<reference>>>;
                using common_reference = common_reference_t<reference, value_type &>;
            };

            template<typename Cur>
            using iterator_associated_types_base =
                iterator_associated_types_base_<Cur, (bool) ReadableCursor<Cur>>;
        }
        /// \endcond

        /// \addtogroup group-utility Utility
        /// @{
        ///
        template<typename T>
        struct basic_mixin : private box<T>
        {
            CONCEPT_requires(DefaultConstructible<T>)
            constexpr basic_mixin()
                noexcept(std::is_nothrow_default_constructible<T>::value)
              : box<T>{}
            {}
            CONCEPT_requires(MoveConstructible<T>)
            explicit constexpr basic_mixin(T &&t)
                noexcept(std::is_nothrow_move_constructible<T>::value)
              : box<T>(detail::move(t))
            {}
            CONCEPT_requires(CopyConstructible<T>)
            explicit constexpr basic_mixin(T const &t)
                noexcept(std::is_nothrow_copy_constructible<T>::value)
              : box<T>(t)
            {}
        protected:
            using box<T>::get;
        };

#if RANGES_BROKEN_CPO_LOOKUP
        namespace _basic_iterator_ { template <typename> struct adl_hook {}; }
#endif

        template<typename Cur>
        struct basic_iterator
          : range_access::mixin_base_t<Cur>
          , detail::iterator_associated_types_base<Cur>
#if RANGES_BROKEN_CPO_LOOKUP
          , private _basic_iterator_::adl_hook<basic_iterator<Cur>>
#endif
        {
        private:
            template<typename>
            friend struct basic_iterator;
            friend range_access;
            using mixin_t = range_access::mixin_base_t<Cur>;
            static_assert((bool) detail::Cursor<Cur>, "");
            using assoc_types_ = detail::iterator_associated_types_base<Cur>;
            using typename assoc_types_::cursor_tag_t;
            using typename assoc_types_::reference_t;
            using typename assoc_types_::const_reference_t;
            RANGES_CXX14_CONSTEXPR Cur &pos() noexcept
            {
                return this->mixin_t::get();
            }
            constexpr Cur const &pos() const noexcept
            {
                return this->mixin_t::get();
            }

        public:
            using typename assoc_types_::difference_type;
            constexpr basic_iterator() = default;
            CONCEPT_template(typename OtherCur)(
                requires ConvertibleTo<OtherCur, Cur> &&
                    Constructible<mixin_t, OtherCur>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator(basic_iterator<OtherCur> that)
              : mixin_t{std::move(that.pos())}
            {}
            // Mix in any additional constructors provided by the mixin
            using mixin_t::mixin_t;

            CONCEPT_template(typename T)(
                requires not defer::Same<uncvref_t<T>, basic_iterator>() &&
                    !detail::defer::HasCursorNext<Cur> &&
                    detail::defer::WritableCursor<Cur, T>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator & operator=(T &&t)
            noexcept(noexcept(std::declval<Cur &>().write(static_cast<T &&>(t))))
            {
                pos().write(static_cast<T &&>(t));
                return *this;
            }

            CONCEPT_template(typename T)(
                requires not defer::Same<uncvref_t<T>, basic_iterator>() &&
                    !detail::defer::HasCursorNext<Cur> &&
                    detail::defer::WritableCursor<Cur const, T>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator const & operator=(T &&t) const
            noexcept(noexcept(std::declval<Cur const &>().write(static_cast<T &&>(t))))
            {
                pos().write(static_cast<T &&>(t));
                return *this;
            }

            CONCEPT_requires(detail::ReadableCursor<Cur> &&
                !detail::is_writable_cursor<Cur>::value)
            constexpr const_reference_t operator*() const
            noexcept(noexcept(range_access::read(std::declval<Cur const &>())))
            {
                return range_access::read(pos());
            }
            CONCEPT_requires(detail::HasCursorNext<Cur> &&
                detail::is_writable_cursor<Cur>::value)
            RANGES_CXX14_CONSTEXPR reference_t operator*()
            noexcept(noexcept(reference_t{std::declval<Cur &>()}))
            {
                return reference_t{pos()};
            }
            CONCEPT_requires(detail::HasCursorNext<Cur> &&
                detail::is_writable_cursor<Cur const>::value)
            constexpr const_reference_t operator*() const
            noexcept(noexcept(const_reference_t{std::declval<Cur const &>()}))
            {
                return const_reference_t{pos()};
            }
            CONCEPT_requires(not detail::HasCursorNext<Cur>)
            RANGES_CXX14_CONSTEXPR basic_iterator & operator*() noexcept
            {
                return *this;
            }

            // Use cursor's arrow() member, if any.
            CONCEPT_template(typename C = Cur)(
                requires detail::HasCursorArrow<C>)
            constexpr detail::cursor_arrow_t<C> operator->() const
            noexcept(noexcept(range_access::arrow(std::declval<C const &>())))
            {
                return range_access::arrow(pos());
            }
            // Otherwise, if reference_t is an lvalue reference to cv-qualified
            // value_type_t, return the address of **this.
            CONCEPT_template(typename C = Cur)(
                requires not detail::HasCursorArrow<Cur> &&
                    detail::ReadableCursor<Cur> &&
                    std::is_lvalue_reference<const_reference_t>::value &&
                    Same<typename detail::iterator_associated_types_base<C>::value_type,
                        uncvref_t<const_reference_t>>)
            constexpr meta::_t<std::add_pointer<const_reference_t>>
            operator->() const
            noexcept(noexcept(*std::declval<basic_iterator const &>()))
            {
                return std::addressof(**this);
            }

            CONCEPT_requires(detail::HasCursorNext<Cur>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator & operator++()
            {
                range_access::next(pos());
                return *this;
            }
            CONCEPT_requires(not detail::HasCursorNext<Cur>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator & operator++() noexcept
            {
                return *this;
            }

            CONCEPT_requires(not Same<detail::input_cursor_tag, detail::cursor_tag_of<Cur>>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator operator++(int)
            {
                basic_iterator tmp{*this};
                ++*this;
                return tmp;
            }
            CONCEPT_requires(Same<detail::input_cursor_tag, detail::cursor_tag_of<Cur>>)
            RANGES_CXX14_CONSTEXPR
            void operator++(int)
            {
                ++*this;
            }

            CONCEPT_template(class Cur2)(
                requires detail::CursorSentinel<Cur2, Cur>)
            friend constexpr bool operator==(basic_iterator const &left,
                basic_iterator<Cur2> const &right)
            {
                return range_access::equal(left.pos(), range_access::pos(right));
            }
            CONCEPT_template(class Cur2)(
                requires detail::CursorSentinel<Cur2, Cur>)
            friend constexpr bool operator!=(basic_iterator const &left,
                basic_iterator<Cur2> const &right)
            {
                return !(left == right);
            }
            CONCEPT_template(class S)(
                requires detail::CursorSentinel<S, Cur>)
            friend constexpr bool operator==(basic_iterator const &left,
                S const &right)
            {
                return range_access::equal(left.pos(), right);
            }
            CONCEPT_template(class S)(
                requires detail::CursorSentinel<S, Cur>)
            friend constexpr bool operator!=(basic_iterator const &left,
                S const &right)
            {
                return !(left == right);
            }
            CONCEPT_template(class S)(
                requires detail::CursorSentinel<S, Cur>)
            friend constexpr bool operator==(S const &left,
                basic_iterator const &right)
            {
                return right == left;
            }
            CONCEPT_template(class S)(
                requires detail::CursorSentinel<S, Cur>)
            friend constexpr bool operator!=(S const &left,
                basic_iterator const &right)
            {
                return right != left;
            }
            CONCEPT_requires(detail::BidirectionalCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator & operator--()
            {
                range_access::prev(pos());
                return *this;
            }
            CONCEPT_requires(detail::BidirectionalCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator operator--(int)
            {
                basic_iterator tmp(*this);
                --*this;
                return tmp;
            }
            CONCEPT_requires(detail::RandomAccessCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator & operator+=(difference_type n)
            {
                range_access::advance(pos(), n);
                return *this;
            }
            CONCEPT_requires(detail::RandomAccessCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            friend basic_iterator operator+(basic_iterator left, difference_type n)
            {
                left += n;
                return left;
            }
            CONCEPT_requires(detail::RandomAccessCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            friend basic_iterator operator+(difference_type n, basic_iterator right)
            {
                right += n;
                return right;
            }
            CONCEPT_requires(detail::RandomAccessCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            basic_iterator & operator-=(difference_type n)
            {
                range_access::advance(pos(), -n);
                return *this;
            }
            CONCEPT_requires(detail::RandomAccessCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            friend basic_iterator operator-(basic_iterator left, difference_type n)
            {
                left -= n;
                return left;
            }
            CONCEPT_template(typename Cur2)(
                requires detail::SizedCursorSentinel<Cur2, Cur>)
            RANGES_CXX14_CONSTEXPR
            friend difference_type operator-(basic_iterator<Cur2> const &left,
                basic_iterator const &right)
            {
                return range_access::distance_to(right.pos(), range_access::pos(left));
            }
            CONCEPT_template(typename S)(
                requires detail::SizedCursorSentinel<S, Cur>)
            RANGES_CXX14_CONSTEXPR
            friend difference_type operator-(S const &left,
                basic_iterator const &right)
            {
                return range_access::distance_to(right.pos(), left);
            }
            CONCEPT_template(typename S)(
                requires detail::SizedCursorSentinel<S, Cur>)
            RANGES_CXX14_CONSTEXPR
            friend difference_type operator-(basic_iterator const &left,
                S const &right)
            {
                return -(right - left);
            }
            // symmetric comparisons
            CONCEPT_requires(detail::SizedCursorSentinel<Cur, Cur>)
            RANGES_CXX14_CONSTEXPR
            friend bool operator<(basic_iterator const &left, basic_iterator const &right)
            {
                return 0 < (right - left);
            }
            CONCEPT_requires(detail::SizedCursorSentinel<Cur, Cur>)
            RANGES_CXX14_CONSTEXPR
            friend bool operator<=(basic_iterator const &left, basic_iterator const &right)
            {
                return 0 <= (right - left);
            }
            CONCEPT_requires(detail::SizedCursorSentinel<Cur, Cur>)
            RANGES_CXX14_CONSTEXPR
            friend bool operator>(basic_iterator const &left, basic_iterator const &right)
            {
                return (right - left) < 0;
            }
            CONCEPT_requires(detail::SizedCursorSentinel<Cur, Cur>)
            RANGES_CXX14_CONSTEXPR
            friend bool operator>=(basic_iterator const &left, basic_iterator const &right)
            {
                return (right - left) <= 0;
            }
            CONCEPT_requires(detail::RandomAccessCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            const_reference_t operator[](difference_type n) const
            {
                return *(*this + n);
            }

#if !RANGES_BROKEN_CPO_LOOKUP
            // Optionally support hooking iter_move when the cursor sports a
            // move() member function.
            CONCEPT_template(typename C = Cur)(
                requires Same<C, Cur> && detail::InputCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            friend auto iter_move(basic_iterator const &it)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                range_access::move(static_cast<basic_iterator<C> const &>(it).pos())
            )
#endif
        };

#if RANGES_BROKEN_CPO_LOOKUP
        namespace _basic_iterator_
        {
            // Optionally support hooking iter_move when the cursor sports a
            // move() member function.
            CONCEPT_template(typename Cur)(
                requires detail::InputCursor<Cur>)
            RANGES_CXX14_CONSTEXPR
            auto iter_move(basic_iterator<Cur> const &it)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                range_access::move(range_access::pos(it))
            )
        }
#endif

        /// Get a cursor from a basic_iterator
        struct get_cursor_fn
        {
            template<typename Cur>
            RANGES_CXX14_CONSTEXPR
            Cur &operator()(basic_iterator<Cur> &it) const noexcept
            {
                return range_access::pos(it);
            }
            template<typename Cur>
            RANGES_CXX14_CONSTEXPR
            Cur const &operator()(basic_iterator<Cur> const &it) const noexcept
            {
                return range_access::pos(it);
            }
            template<typename Cur>
            RANGES_CXX14_CONSTEXPR
            Cur operator()(basic_iterator<Cur> && it) const
                noexcept(std::is_nothrow_move_constructible<Cur>::value)
            {
                return range_access::pos(std::move(it));
            }
        };

        /// \sa `get_cursor_fn`
        /// \ingroup group-utility
        RANGES_INLINE_VARIABLE(get_cursor_fn, get_cursor)
        /// @}
    }
}

/// \cond
namespace concepts
{
    // common_reference specializations for basic_proxy_reference
    template<typename Cur, typename U, typename TQual, typename UQual>
    struct basic_common_reference<::ranges::detail::basic_proxy_reference_<Cur, true>, U, TQual, UQual>
      : basic_common_reference<::ranges::detail::cursor_reference_t<Cur>, U, TQual, UQual>
    {};
    template<typename T, typename Cur, typename TQual, typename UQual>
    struct basic_common_reference<T, ::ranges::detail::basic_proxy_reference_<Cur, true>, TQual, UQual>
      : basic_common_reference<T, ::ranges::detail::cursor_reference_t<Cur>, TQual, UQual>
    {};
    template<typename Cur1, typename Cur2, typename TQual, typename UQual>
    struct basic_common_reference<::ranges::detail::basic_proxy_reference_<Cur1, true>, ::ranges::detail::basic_proxy_reference_<Cur2, true>, TQual, UQual>
      : basic_common_reference<::ranges::detail::cursor_reference_t<Cur1>, ::ranges::detail::cursor_reference_t<Cur2>, TQual, UQual>
    {};

    // common_type specializations for basic_proxy_reference
    template<typename Cur, typename U>
    struct common_type<::ranges::detail::basic_proxy_reference_<Cur, true>, U>
      : common_type<::ranges::range_access::cursor_value_t<Cur>, U>
    {};
    template<typename T, typename Cur>
    struct common_type<T, ::ranges::detail::basic_proxy_reference_<Cur, true>>
      : common_type<T, ::ranges::range_access::cursor_value_t<Cur>>
    {};
    template<typename Cur1, typename Cur2>
    struct common_type<::ranges::detail::basic_proxy_reference_<Cur1, true>, ::ranges::detail::basic_proxy_reference_<Cur2, true>>
      : common_type<::ranges::range_access::cursor_value_t<Cur1>, ::ranges::range_access::cursor_value_t<Cur2>>
    {};
}

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Cur, bool IsReadable>
            struct std_iterator_traits_;
            template<typename Cur>
            using std_iterator_traits =
                std_iterator_traits_<Cur, (bool) ReadableCursor<Cur>>;

            template<typename Cur, bool IsReadable>
            struct std_iterator_traits_
            {
                using iterator_category = std::output_iterator_tag;
                using difference_type =
                    typename iterator_associated_types_base<Cur>::difference_type;
                using value_type = void;
                using reference = void;
                using pointer = void;
            };

            template<typename Cur>
            struct std_iterator_traits_<Cur, true>
              : iterator_associated_types_base<Cur>
            {
                using iterator_category =
                    ::meta::_t<
                        downgrade_iterator_category<
                            typename std_iterator_traits_::iterator_category,
                            typename std_iterator_traits_::reference>>;
            };
        }
        /// \endcond
    }
}

namespace std
{
    template<typename Cur>
    struct iterator_traits< ::ranges::basic_iterator<Cur>>
      : ::ranges::detail::std_iterator_traits<Cur>
    {};
}
/// \endcond

#endif
