/// \file
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
            // iterators whose dereference operators reference the same value
            // for all iterators into the same sequence (like many input
            // iterators) need help with their postfix ++: the referenced
            // value must be read and stored away before the increment occurs
            // so that *a++ yields the originally referenced element and not
            // the next one.
            template<typename I>
            struct postfix_increment_proxy
            {
                using value_type = iterator_value_t<I>;
            private:
                mutable semiregular_t<value_type> value_;
            public:
                postfix_increment_proxy() = default;
                RANGES_CXX14_CONSTEXPR
                explicit postfix_increment_proxy(I const& x)
                  : value_(*x)
                {}
                // Returning a mutable reference allows nonsense like
                // (*r++).mutate(), but it imposes fewer assumptions about the
                // behavior of the value_type.  In particular, recall that
                // (*r).mutate() is legal if operator* returns by value.
                RANGES_CXX14_CONSTEXPR value_type& operator*() const
                {
                    return value_;
                }
            };

            //
            // In general, we can't determine that such an iterator isn't
            // writable -- we also need to store a copy of the old iterator so
            // that it can be written into.
            template<typename I>
            struct writable_postfix_increment_proxy
            {
                using value_type = iterator_value_t<I>;
            private:
                mutable semiregular_t<value_type> value_;
                I it_;
            public:
                writable_postfix_increment_proxy() = default;
                RANGES_CXX14_CONSTEXPR
                explicit writable_postfix_increment_proxy(I x)
                  : value_(*x)
                  , it_(std::move(x))
                {}
                // Dereferencing must return a proxy so that both *r++ = o and
                // value_type(*r++) can work.  In this case, *r is the same as
                // *r++, and the conversion operator below is used to ensure
                // readability.
                RANGES_CXX14_CONSTEXPR
                writable_postfix_increment_proxy const & operator*() const
                {
                    return *this;
                }
                // So that iter_move(r++) moves the cached value out
                RANGES_CXX14_CONSTEXPR
                friend value_type && indirect_move(writable_postfix_increment_proxy const &ref)
                {
                    return std::move(ref.value_);
                }
                // Provides readability of *r++
                RANGES_CXX14_CONSTEXPR
                operator value_type &() const
                {
                    return value_;
                }
                // Provides writability of *r++
                template<typename T,
                    CONCEPT_REQUIRES_(Writable<I, T const &>())>
                RANGES_CXX14_CONSTEXPR
                void operator=(T const &x) const
                {
                    *it_ = x;
                }
                // This overload just in case only non-const objects are writable
                template<typename T,
                    CONCEPT_REQUIRES_(Writable<I, T &>())>
                RANGES_CXX14_CONSTEXPR
                void operator=(T &x) const
                {
                    *it_ = x;
                }
                template<typename T,
                    CONCEPT_REQUIRES_(Writable<I, aux::move_t<T> &&>())>
                RANGES_CXX14_CONSTEXPR
                void operator=(T &&x) const
                {
                    *it_ = std::move(x);
                }
                // Provides X(r++)
                RANGES_CXX14_CONSTEXPR
                operator I const &() const
                {
                    return it_;
                }
            };

            template<typename Ref, typename Val>
            using is_non_proxy_reference =
                std::is_convertible<
                    meta::_t<std::remove_reference<Ref>> const volatile *,
                    Val const volatile *>;

            // A metafunction to choose the result type of postfix ++
            //
            // Because the C++98 input iterator requirements say that *r++ has
            // type T (value_type), implementations of some standard
            // algorithms like lexicographical_compare may use constructions
            // like:
            //
            //          *r++ < *s++
            //
            // If *r++ returns a proxy (as required if r is writable but not
            // multipass), this sort of expression will fail unless the proxy
            // supports the operator<.  Since there are any number of such
            // operations, we're not going to try to support them.  Therefore,
            // even if r++ returns a proxy, *r++ will only return a proxy if
            // *r also returns a proxy.
            template<typename I, typename Val, typename Ref, typename Cat>
            using postfix_increment_result =
                meta::if_<
                    DerivedFrom<Cat, ranges::forward_iterator_tag>,
                    I,
                    meta::if_<
                        is_non_proxy_reference<Ref, Val>,
                        postfix_increment_proxy<I>,
                        writable_postfix_increment_proxy<I>>>;

            template<typename Cur>
            using cursor_reference_t =
                decltype(range_access::get(std::declval<Cur const &>()));

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
                    noexcept(noexcept(Head(Head(std::declval<Derived const &>().get_()))))
                {
                    return Head(static_cast<Derived const *>(this)->get_());
                }
            };

            // Collect the reference types associated with cursors
            template<typename Cur, bool Readable = (bool) ReadableCursor<Cur>()>
            struct cursor_traits
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
            struct cursor_traits<Cur, true>
            {
                using value_t_ = range_access::cursor_value_t<Cur>;
                using reference_t_ = cursor_reference_t<Cur>;
                using rvalue_reference_t_ = cursor_rvalue_reference_t<Cur>;
            private:
                using R1 = reference_t_;
                using R2 = common_reference_t<reference_t_ &&, value_t_ &>;
                using R3 = common_reference_t<reference_t_ &&, rvalue_reference_t_ &&>;
                using tmp1 = meta::list<value_t_, R1>;
                using tmp2 =
                    meta::if_<meta::in<tmp1, uncvref_t<R2>>, tmp1, meta::push_back<tmp1, R2>>;
                using tmp3 =
                    meta::if_<meta::in<tmp2, uncvref_t<R3>>, tmp2, meta::push_back<tmp2, R3>>;
            public:
                using common_refs = meta::unique<meta::pop_front<tmp3>>;
            };

            // The One Proxy Reference type to rule them all. basic_iterator uses this
            // as the return type of operator* when the cursor type has a set() member
            // function of the correct signature (i.e., if it can accept a value_type&&).
            template<typename Cur, bool Readable = (bool) ReadableCursor<Cur>()>
            struct basic_proxy_reference
              : cursor_traits<Cur>
                // The following adds conversion operators to the common reference
                // types, so that basic_proxy_reference can model Readable
              , meta::inherit<
                    meta::transform<
                        typename cursor_traits<Cur>::common_refs,
                        meta::bind_front<
                            meta::quote<proxy_reference_conversion>,
                            basic_proxy_reference<Cur>>>>
            {
            private:
                Cur *cur_;
                template<typename OtherCur, bool OtherReadable>
                friend struct basic_proxy_reference;
                template<typename, typename>
                friend struct proxy_reference_conversion;
                using typename cursor_traits<Cur>::value_t_;
                using typename cursor_traits<Cur>::reference_t_;
                using typename cursor_traits<Cur>::rvalue_reference_t_;
                CONCEPT_ASSERT_MSG(CommonReference<value_t_ &, reference_t_ &&>(),
                    "Your readable and writable cursor must have a value type and a reference "
                    "type that share a common reference type. See the ranges::common_reference "
                    "type trait.");
                RANGES_CXX14_CONSTEXPR
                reference_t_ get_() const
                    noexcept(noexcept(reference_t_(range_access::get(std::declval<Cur const &>()))))
                {
                    return range_access::get(*cur_);
                }
                template<typename T>
                RANGES_CXX14_CONSTEXPR
                void set_(T &&t)
                {
                    range_access::set(*cur_, (T &&) t);
                }
            public:
                basic_proxy_reference() = default;
                basic_proxy_reference(basic_proxy_reference const &) = default;
                template<typename OtherCur,
                    CONCEPT_REQUIRES_(ConvertibleTo<OtherCur*, Cur*>())>
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference(basic_proxy_reference<OtherCur> const &that) noexcept
                  : cur_(that.cur_)
                {}
                RANGES_CXX14_CONSTEXPR
                explicit basic_proxy_reference(Cur &cur) noexcept
                  : cur_(&cur)
                {}
                CONCEPT_REQUIRES(ReadableCursor<Cur>())
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference &operator=(basic_proxy_reference &&that)
                {
                    return *this = that;
                }
                CONCEPT_REQUIRES(ReadableCursor<Cur>())
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference &operator=(basic_proxy_reference const &that)
                {
                    this->set_(that.get_());
                    return *this;
                }
                template<typename OtherCur,
                    CONCEPT_REQUIRES_(ReadableCursor<OtherCur>() &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>())>
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference &operator=(basic_proxy_reference<OtherCur> &&that)
                {
                    return *this = that;
                }
                template<typename OtherCur,
                    CONCEPT_REQUIRES_(ReadableCursor<OtherCur>() &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>())>
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference &operator=(basic_proxy_reference<OtherCur> const &that)
                {
                    this->set_(that.get_());
                    return *this;
                }
                template<typename T,
                    CONCEPT_REQUIRES_(WritableCursor<Cur, T &&>())>
                RANGES_CXX14_CONSTEXPR
                basic_proxy_reference &operator=(T &&t)
                {
                    this->set_((T &&) t);
                    return *this;
                }
                template<typename V = value_t_,
                    CONCEPT_REQUIRES_(ReadableCursor<Cur>() && EqualityComparable<V>())>
                RANGES_CXX14_CONSTEXPR
                friend bool operator==(basic_proxy_reference const &x, value_t_ const &y)
                {
                    return x.get_() == y;
                }
                template<typename V = value_t_,
                    CONCEPT_REQUIRES_(ReadableCursor<Cur>() && EqualityComparable<V>())>
                RANGES_CXX14_CONSTEXPR
                friend bool operator!=(basic_proxy_reference const &x, value_t_ const &y)
                {
                    return !(x == y);
                }
                template<typename V = value_t_,
                    CONCEPT_REQUIRES_(ReadableCursor<Cur>() && EqualityComparable<V>())>
                RANGES_CXX14_CONSTEXPR
                friend bool operator==(value_t_ const &x, basic_proxy_reference const &y)
                {
                    return x == y.get_();
                }
                template<typename V = value_t_,
                    CONCEPT_REQUIRES_(ReadableCursor<Cur>() && EqualityComparable<V>())>
                RANGES_CXX14_CONSTEXPR
                friend bool operator!=(value_t_ const &x, basic_proxy_reference const &y)
                {
                    return !(x == y);
                }
                template<typename V = value_t_,
                    CONCEPT_REQUIRES_(ReadableCursor<Cur>() && EqualityComparable<V>())>
                RANGES_CXX14_CONSTEXPR
                friend bool operator==(basic_proxy_reference const &x, basic_proxy_reference const &y)
                {
                    return x.get_() == y.get_();
                }
                template<typename V = value_t_,
                    CONCEPT_REQUIRES_(ReadableCursor<Cur>() && EqualityComparable<V>())>
                RANGES_CXX14_CONSTEXPR
                friend bool operator!=(basic_proxy_reference const &x, basic_proxy_reference const &y)
                {
                    return !(x == y);
                }
            };

            auto iter_cat(range_access::InputCursor*) ->
                ranges::input_iterator_tag;
            auto iter_cat(range_access::ForwardCursor*) ->
                ranges::forward_iterator_tag;
            auto iter_cat(range_access::BidirectionalCursor*) ->
                ranges::bidirectional_iterator_tag;
            auto iter_cat(range_access::RandomAccessCursor*) ->
                ranges::random_access_iterator_tag;

            template<typename Cur, typename S, bool Readable = (bool) ReadableCursor<Cur>()>
            struct iterator_associated_types_base
            {
            private:
                friend basic_iterator<Cur, S>;
                using postfix_increment_result_t = basic_iterator<Cur, S>;
                using reference_t = basic_proxy_reference<Cur>;
                using const_reference_t = basic_proxy_reference<Cur const>;
                using cursor_concept_t = range_access::OutputCursor;
            public:
                using reference = void;
                using difference_type = range_access::cursor_difference_t<Cur>;
            };

            template<typename Cur, typename S>
            struct iterator_associated_types_base<Cur, S, true>
              : iterator_associated_types_base<Cur, S, false>
            {
            private:
                friend basic_iterator<Cur, S>;
                using cursor_concept_t = detail::cursor_concept_t<Cur>;
                using reference_t =
                    meta::if_<
                        is_writable_cursor<Cur const>,
                        basic_proxy_reference<Cur const>,
                        meta::if_<
                            is_writable_cursor<Cur>,
                            basic_proxy_reference<Cur>,
                            cursor_reference_t<Cur>>>;
                using const_reference_t = reference_t;
            public:
                using value_type = range_access::cursor_value_t<Cur>;
                using reference = reference_t;
                using iterator_category =
                    decltype(detail::iter_cat(_nullptr_v<cursor_concept_t>()));
                using pointer = meta::_t<std::add_pointer<reference>>;
                using common_reference = common_reference_t<reference &&, value_type &>;
            private:
                using postfix_increment_result_t =
                    postfix_increment_result<
                        basic_iterator<Cur, S>, value_type, reference, iterator_category>;
            };
        }
        /// \endcond

        /// \addtogroup group-utility Utility
        /// @{
        ///
        template<typename T>
        struct basic_mixin : private box<T>
        {
            CONCEPT_REQUIRES(DefaultConstructible<T>())
            constexpr basic_mixin()
              : box<T>{}
            {}
            CONCEPT_REQUIRES(MoveConstructible<T>())
            constexpr basic_mixin(T &&t)
              : box<T>(detail::move(t))
            {}
            CONCEPT_REQUIRES(CopyConstructible<T>())
            constexpr basic_mixin(T const &t)
              : box<T>(t)
            {}
        protected:
            using box<T>::get;
        };

        template<typename S>
        struct basic_sentinel : range_access::mixin_base_t<S>
        {
        private:
            friend range_access;
            RANGES_CXX14_CONSTEXPR S &end() noexcept
            {
                return this->range_access::mixin_base_t<S>::get();
            }
            constexpr S const &end() const noexcept
            {
                return this->range_access::mixin_base_t<S>::get();
            }
        public:
            basic_sentinel() = default;
            RANGES_CXX14_CONSTEXPR basic_sentinel(S end)
              : range_access::mixin_base_t<S>(std::move(end))
            {}
            using range_access::mixin_base_t<S>::mixin_base_t;
        };

        struct default_end_cursor
        {
            template<typename Cur>
            static constexpr bool equal(Cur const &pos)
            {
                return range_access::done(pos);
            }
            template<typename Cur>
            static constexpr auto distance_from(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                range_access::distance_remaining(pos)
            )
        };

        template<typename Cur, typename S>
        struct basic_iterator
          : range_access::mixin_base_t<Cur>
          , detail::iterator_associated_types_base<Cur, S>
        {
        private:
            friend range_access;
            CONCEPT_ASSERT(detail::Cursor<Cur>());
            using assoc_types_ = detail::iterator_associated_types_base<Cur, S>;
            using typename assoc_types_::postfix_increment_result_t;
            using typename assoc_types_::cursor_concept_t;
            using typename assoc_types_::reference_t;
            using typename assoc_types_::const_reference_t;
            RANGES_CXX14_CONSTEXPR Cur &pos() noexcept
            {
                return this->range_access::mixin_base_t<Cur>::get();
            }
            constexpr Cur const &pos() const noexcept
            {
                return this->range_access::mixin_base_t<Cur>::get();
            }

        public:
            using typename assoc_types_::difference_type;
            constexpr basic_iterator() = default;
            RANGES_CXX14_CONSTEXPR basic_iterator(Cur pos)
              : range_access::mixin_base_t<Cur>{std::move(pos)}
            {}
            template<typename OtherCur, typename OtherS,
                CONCEPT_REQUIRES_(ConvertibleTo<OtherCur, Cur>() &&
                    Constructible<range_access::mixin_base_t<Cur>, OtherCur &&>())>
            RANGES_CXX14_CONSTEXPR
            basic_iterator(basic_iterator<OtherCur, OtherS> that)
              : range_access::mixin_base_t<Cur>{range_access::pos(std::move(that))}
            {}
            // Mix in any additional constructors defined and exported by the cursor
            using range_access::mixin_base_t<Cur>::mixin_base_t;

        private:
            RANGES_CXX14_CONSTEXPR
            reference_t dereference_(std::true_type) noexcept
            {
                return reference_t{pos()};
            }
            RANGES_CXX14_CONSTEXPR
            const_reference_t dereference_(std::true_type) const noexcept
            {
                return const_reference_t{pos()};
            }
            RANGES_CXX14_CONSTEXPR
            const_reference_t dereference_(std::false_type) const
                noexcept(noexcept(range_access::get(std::declval<Cur const &>())))
            {
                return range_access::get(pos());
            }
        public:
            RANGES_CXX14_CONSTEXPR
            reference_t operator*()
                noexcept(noexcept(std::declval<basic_iterator &>().
                    dereference_(detail::is_writable_cursor<Cur>{})))
            {
                return this->dereference_(detail::is_writable_cursor<Cur>{});
            }
            RANGES_CXX14_CONSTEXPR
            const_reference_t operator*() const
                noexcept(noexcept(std::declval<basic_iterator const &>().
                    dereference_(detail::is_writable_cursor<Cur>{})))
            {
                return this->dereference_(detail::is_writable_cursor<Cur>{});
            }
            RANGES_CXX14_CONSTEXPR
            basic_iterator& operator++()
            {
                range_access::next(pos());
                return *this;
            }
            RANGES_CXX14_CONSTEXPR postfix_increment_result_t operator++(int)
            {
                postfix_increment_result_t tmp(*this);
                ++*this;
                return tmp;
            }
            CONCEPT_REQUIRES(detail::HasEqualCursor<Cur>())
            friend constexpr bool operator==(basic_iterator const &left,
                basic_iterator const &right)
            {
                return range_access::equal(left.pos(), right.pos());
            }
            CONCEPT_REQUIRES(detail::HasEqualCursor<Cur>())
            friend constexpr bool operator!=(basic_iterator const &left,
                basic_iterator const &right)
            {
                return !(left == right);
            }
            friend constexpr bool operator==(basic_iterator const &left,
                basic_sentinel<S> const &right)
            {
                return range_access::empty(left.pos(), range_access::end(right));
            }
            friend constexpr bool operator!=(basic_iterator const &left,
                basic_sentinel<S> const &right)
            {
                return !(left == right);
            }
            friend constexpr bool operator==(basic_sentinel<S> const & left,
                basic_iterator const &right)
            {
                return range_access::empty(right.pos(), range_access::end(left));
            }
            friend constexpr bool operator!=(basic_sentinel<S> const &left,
                basic_iterator const &right)
            {
                return !(left == right);
            }
            CONCEPT_REQUIRES(detail::BidirectionalCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            basic_iterator& operator--()
            {
                range_access::prev(pos());
                return *this;
            }
            CONCEPT_REQUIRES(detail::BidirectionalCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            basic_iterator operator--(int)
            {
                basic_iterator tmp(*this);
                --*this;
                return tmp;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            basic_iterator& operator+=(difference_type n)
            {
                range_access::advance(pos(), n);
                return *this;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend basic_iterator operator+(basic_iterator left, difference_type n)
            {
                left += n;
                return left;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend basic_iterator operator+(difference_type n, basic_iterator right)
            {
                right += n;
                return right;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            basic_iterator& operator-=(difference_type n)
            {
                range_access::advance(pos(), -n);
                return *this;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend basic_iterator operator-(basic_iterator left, difference_type n)
            {
                left -= n;
                return left;
            }
            CONCEPT_REQUIRES(detail::SizedCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend difference_type operator-(basic_iterator const &left,
                basic_iterator const &right)
            {
                return range_access::distance_to(right.pos(), left.pos());
            }
            CONCEPT_REQUIRES(detail::SizedCursorRange<Cur, S>())
            RANGES_CXX14_CONSTEXPR
            friend difference_type operator-(basic_sentinel<S> const &left,
                basic_iterator const& right)
            {
                return range_access::distance_to(right.pos(), range_access::end(left));
            }
            CONCEPT_REQUIRES(detail::SizedCursorRange<Cur, S>())
            RANGES_CXX14_CONSTEXPR
            friend difference_type operator-(basic_iterator const &left,
                basic_sentinel<S> const& right)
            {
                return -range_access::distance_to(left.pos(), range_access::end(right));
            }
            // symmetric comparisons
            CONCEPT_REQUIRES(detail::SizedCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend bool operator<(basic_iterator const &left, basic_iterator const &right)
            {
                return 0 < (right - left);
            }
            CONCEPT_REQUIRES(detail::SizedCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend bool operator<=(basic_iterator const &left, basic_iterator const &right)
            {
                return 0 <= (right - left);
            }
            CONCEPT_REQUIRES(detail::SizedCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend bool operator>(basic_iterator const &left, basic_iterator const &right)
            {
                return (right - left) < 0;
            }
            CONCEPT_REQUIRES(detail::SizedCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend bool operator>=(basic_iterator const &left, basic_iterator const &right)
            {
                return (right - left) <= 0;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            const_reference_t operator[](difference_type n) const
            {
                return *(*this + n);
            }
        };

        /// Get a cursor from a basic_iterator
        struct get_cursor_fn
        {
            template<typename Cur, typename Sent>
            RANGES_CXX14_CONSTEXPR
            Cur &operator()(basic_iterator<Cur, Sent> &it) const noexcept
            {
                return range_access::pos(it);
            }
            template<typename Cur, typename Sent>
            RANGES_CXX14_CONSTEXPR
            Cur const &operator()(basic_iterator<Cur, Sent> const &it) const noexcept
            {
                return range_access::pos(it);
            }
            template<typename Cur, typename Sent>
            RANGES_CXX14_CONSTEXPR
            Cur operator()(basic_iterator<Cur, Sent> &&it) const
                noexcept(std::is_nothrow_move_constructible<Cur>::value)
            {
                return range_access::pos(std::move(it));
            }
        };

        /// \sa `get_cursor_fn`
        /// \ingroup group-utility
        RANGES_INLINE_VARIABLE(get_cursor_fn, get_cursor)
        /// @}

        /// \cond
        namespace detail
        {
            // Optionally support hooking iter_move when the cursor sports a
            // move() member function.
            template<typename C, typename S,
                CONCEPT_REQUIRES_(InputCursor<C>())>
            RANGES_CXX14_CONSTEXPR
            auto indirect_move(basic_iterator<C, S> const &it)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                range_access::move(get_cursor(it))
            )
        }
        /// \endcond

        /// \cond
        // This is so that writable postfix proxy objects satisfy Readability
        template<typename T, typename I, typename Qual1, typename Qual2>
        struct basic_common_reference<T, detail::writable_postfix_increment_proxy<I>, Qual1, Qual2>
          : basic_common_reference<T, iterator_value_t<I>, Qual1, meta::quote_trait<std::add_lvalue_reference>>
        {};

        template<typename I, typename T, typename Qual1, typename Qual2>
        struct basic_common_reference<detail::writable_postfix_increment_proxy<I>, T, Qual1, Qual2>
          : basic_common_reference<iterator_value_t<I>, T, meta::quote_trait<std::add_lvalue_reference>, Qual2>
        {};
        /// \endcond
    }
}

/// \cond
namespace ranges
{
    inline namespace v3
    {
        // common_reference specializations for basic_proxy_reference
        template<typename Cur, typename U, typename TQual, typename UQual>
        struct basic_common_reference<detail::basic_proxy_reference<Cur, true>, U, TQual, UQual>
          : basic_common_reference<detail::cursor_reference_t<Cur>, U, TQual, UQual>
        {};
        template<typename T, typename Cur, typename TQual, typename UQual>
        struct basic_common_reference<T, detail::basic_proxy_reference<Cur, true>, TQual, UQual>
          : basic_common_reference<T, detail::cursor_reference_t<Cur>, TQual, UQual>
        {};
        template<typename Cur1, typename Cur2, typename TQual, typename UQual>
        struct basic_common_reference<detail::basic_proxy_reference<Cur1, true>, detail::basic_proxy_reference<Cur2, true>, TQual, UQual>
          : basic_common_reference<detail::cursor_reference_t<Cur1>, detail::cursor_reference_t<Cur2>, TQual, UQual>
        {};

        // common_type specializations for basic_proxy_reference
        template<typename Cur, typename U>
        struct common_type<detail::basic_proxy_reference<Cur, true>, U>
          : common_type<range_access::cursor_value_t<Cur>, U>
        {};
        template<typename T, typename Cur>
        struct common_type<T, detail::basic_proxy_reference<Cur, true>>
          : common_type<T, range_access::cursor_value_t<Cur>>
        {};
        template<typename Cur1, typename Cur2>
        struct common_type<detail::basic_proxy_reference<Cur1, true>, detail::basic_proxy_reference<Cur2, true>>
          : common_type<range_access::cursor_value_t<Cur1>, range_access::cursor_value_t<Cur2>>
        {};
    }
}

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Cur, typename S, bool IsReadable = (bool) ReadableCursor<Cur>()>
            struct std_iterator_traits
              : iterator_associated_types_base<Cur, S>
            {
                using iterator_category = std::output_iterator_tag;
                using value_type = void;
                using reference = void;
                using pointer = void;
            };

            template<typename Cur, typename S>
            struct std_iterator_traits<Cur, S, true>
              : iterator_associated_types_base<Cur, S>
            {
                using iterator_category =
                    ::meta::_t<
                        downgrade_iterator_category<
                            typename std_iterator_traits::iterator_category,
                            typename std_iterator_traits::reference>>;
            };
        }
    }
}

namespace std
{
    template<typename Cur, typename S>
    struct iterator_traits< ::ranges::basic_iterator<Cur, S>>
      : ::ranges::detail::std_iterator_traits<Cur, S>
    {};
}
/// \endcond

#endif
