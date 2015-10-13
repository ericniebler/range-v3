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
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/nullptr_v.hpp>
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
                mutable value_type value_;
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
                mutable value_type value_;
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
                    CONCEPT_REQUIRES_(Writable<I, T>())>
                RANGES_CXX14_CONSTEXPR
                void operator=(T const &x) const
                {
                    *it_ = x;
                }
                // This overload just in case only non-const objects are writable
                template<typename T,
                    CONCEPT_REQUIRES_(Writable<I, T>())>
                RANGES_CXX14_CONSTEXPR
                void operator=(T &x) const
                {
                    *it_ = x;
                }
                template<typename T,
                    CONCEPT_REQUIRES_(MoveWritable<I, T>())>
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

            template<typename Cur, typename Enable = void>
            struct mixin_base_
            {
                using type = basic_mixin<Cur>;
            };

            template<typename Cur>
            struct mixin_base_<Cur, meta::void_<typename Cur::mixin>>
            {
                using type = typename Cur::mixin;
            };

            template<typename Cur>
            using mixin_base = meta::_t<mixin_base_<Cur>>;

            auto iter_cat(range_access::WeakInputCursor*) ->
                ranges::weak_input_iterator_tag;
            auto iter_cat(range_access::InputCursor*) ->
                ranges::input_iterator_tag;
            auto iter_cat(range_access::ForwardCursor*) ->
                ranges::forward_iterator_tag;
            auto iter_cat(range_access::BidirectionalCursor*) ->
                ranges::bidirectional_iterator_tag;
            auto iter_cat(range_access::RandomAccessCursor*) ->
                ranges::random_access_iterator_tag;
        }
        /// \endcond

        /// \addtogroup group-utility Utility
        /// @{
        ///
        template<typename T>
        struct basic_mixin
        {
        private:
            T t_;
        public:
            CONCEPT_REQUIRES(DefaultConstructible<T>())
            constexpr basic_mixin()
              : t_{}
            {}
            RANGES_CXX14_CONSTEXPR
            basic_mixin(T t)
              : t_(std::move(t))
            {}
            RANGES_CXX14_CONSTEXPR
            T &get() noexcept
            {
                return t_;
            }
            /// \overload
            RANGES_CXX14_CONSTEXPR
            T const &get() const noexcept
            {
                return t_;
            }
        };

        template<typename S>
        struct basic_sentinel : detail::mixin_base<S>
        {
            // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60799
            #ifndef __GNUC__
        private:
            #endif
            friend range_access;
            template<typename Cur, typename OtherSentinel>
            friend struct basic_iterator;
            RANGES_CXX14_CONSTEXPR
            S &end() noexcept
            {
                return this->detail::mixin_base<S>::get();
            }
            RANGES_CXX14_CONSTEXPR
            S const &end() const noexcept
            {
                return this->detail::mixin_base<S>::get();
            }
        private:
            using detail::mixin_base<S>::get;
        public:
            basic_sentinel() = default;
            RANGES_CXX14_CONSTEXPR basic_sentinel(S end)
              : detail::mixin_base<S>(std::move(end))
            {}
            using detail::mixin_base<S>::mixin_base;
            constexpr bool operator==(basic_sentinel<S> const &) const
            {
                return true;
            }
            constexpr bool operator!=(basic_sentinel<S> const &) const
            {
                return false;
            }
        };

        template<typename Cur, typename S>
        struct basic_iterator
          : detail::mixin_base<Cur>
        {
        private:
            friend range_access;
            friend detail::mixin_base<Cur>;
            template<typename OtherCur, typename OtherS>
            friend struct basic_iterator;
            // TODO support output iterators
            //CONCEPT_ASSERT(detail::WeakCursor<Cur>());
            CONCEPT_ASSERT(detail::WeakInputCursor<Cur>());
            using single_pass = range_access::single_pass_t<Cur>;
            using is_weak_t =
                std::is_same<detail::cursor_concept_t<Cur>, range_access::WeakInputCursor>;
            using cursor_concept_t =
                meta::if_<
                    is_weak_t,
                    range_access::WeakInputCursor,
                    meta::if_<
                        single_pass,
                        range_access::InputCursor,
                        detail::cursor_concept_t<Cur>>>;

            using detail::mixin_base<Cur>::get;
            RANGES_CXX14_CONSTEXPR Cur &pos() noexcept
            {
                return this->detail::mixin_base<Cur>::get();
            }
            RANGES_CXX14_CONSTEXPR Cur const &pos() const noexcept
            {
                return this->detail::mixin_base<Cur>::get();
            }

            // If the cursor models ForwardCursor, then positions are equality comparable.
            // Otherwise, make all cursors are trivially equal.
            constexpr bool equal2_(basic_iterator const&, range_access::InputCursor *) const
            {
                return true;
            }
            constexpr bool equal2_(basic_iterator const &that, range_access::ForwardCursor *) const
            {
                return range_access::equal(pos(), that.pos());
            }
            // For Bounded ranges:
            constexpr bool equal_(basic_iterator const &that, std::true_type *) const
            {
                return range_access::equal(pos(), that.pos());
            }
            // For non-Bounded ranges:
            constexpr bool equal_(basic_iterator const &that, std::false_type *) const
            {
                return basic_iterator::equal2_(that, _nullptr_v<cursor_concept_t>());
            }
        public:
            using reference =
                decltype(range_access::current(std::declval<Cur const &>()));
            using value_type = range_access::cursor_value_t<Cur>;
            using iterator_category = decltype(detail::iter_cat(_nullptr_v<cursor_concept_t>()));
            using difference_type = range_access::cursor_difference_t<Cur>;
            using pointer = meta::_t<std::add_pointer<reference>>;
            using common_reference = common_reference_t<reference &&, value_type &>;
        private:
            using postfix_increment_result_t =
                detail::postfix_increment_result<
                    basic_iterator, value_type, reference, iterator_category>;
        public:
            constexpr basic_iterator() = default;
            RANGES_CXX14_CONSTEXPR basic_iterator(Cur pos)
              : detail::mixin_base<Cur>{std::move(pos)}
            {}
            template<typename OtherCur, typename OtherS,
                CONCEPT_REQUIRES_(ConvertibleTo<OtherCur, Cur>())>
            basic_iterator(basic_iterator<OtherCur, OtherS> that)
              : detail::mixin_base<Cur>{std::move(that.pos())}
            {}
            // Mix in any additional constructors defined and exported by the cursor
            using detail::mixin_base<Cur>::mixin_base;
            RANGES_CXX14_CONSTEXPR reference operator*() const
                noexcept(noexcept(range_access::current(std::declval<basic_iterator const &>().pos())))
            {
                return range_access::current(pos());
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
            CONCEPT_REQUIRES(!is_weak_t())
            friend constexpr bool operator==(basic_iterator const &left,
                basic_iterator const &right)
            {
                return left.equal_(right, _nullptr_v<std::is_same<Cur, S>>());
            }
            CONCEPT_REQUIRES(!is_weak_t())
            friend constexpr bool operator!=(basic_iterator const &left,
                basic_iterator const &right)
            {
                return !(left == right);
            }
            CONCEPT_REQUIRES(!is_weak_t())
            friend constexpr bool operator==(basic_iterator const &left,
                basic_sentinel<S> const &right)
            {
                return range_access::empty(left.pos(), right.end());
            }
            CONCEPT_REQUIRES(!is_weak_t())
            friend constexpr bool operator!=(basic_iterator const &left,
                basic_sentinel<S> const &right)
            {
                return !(left == right);
            }
            CONCEPT_REQUIRES(!is_weak_t())
            friend constexpr bool operator==(basic_sentinel<S> const & left,
                basic_iterator const &right)
            {
                return range_access::empty(right.pos(), left.end());
            }
            CONCEPT_REQUIRES(!is_weak_t())
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
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend difference_type operator-(basic_iterator const &left,
                basic_iterator const &right)
            {
                return range_access::distance_to(right.pos(), left.pos());
            }
            // symmetric comparisons
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend bool operator<(basic_iterator const &left, basic_iterator const &right)
            {
                return 0 < (right - left);
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend bool operator<=(basic_iterator const &left, basic_iterator const &right)
            {
                return 0 <= (right - left);
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend bool operator>(basic_iterator const &left, basic_iterator const &right)
            {
                return (right - left) < 0;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            friend bool operator>=(basic_iterator const &left, basic_iterator const &right)
            {
                return (right - left) <= 0;
            }
            // asymmetric comparisons
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            friend constexpr bool operator<(basic_iterator const &left,
                basic_sentinel<S> const &right)
            {
                return !range_access::empty(left.pos(), right.end());
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            friend constexpr bool operator<=(basic_iterator const &left,
                basic_sentinel<S> const &right)
            {
                return true;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            friend constexpr bool operator>(basic_iterator const &left,
                basic_sentinel<S> const &right)
            {
                return false;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            friend constexpr bool operator>=(basic_iterator const &left,
                basic_sentinel<S> const &right)
            {
                return range_access::empty(left.pos(), right.end());
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            friend constexpr bool operator<(basic_sentinel<S> const &left,
                basic_iterator const &right)
            {
                return false;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            friend constexpr bool operator<=(basic_sentinel<S> const &left,
                basic_iterator const &right)
            {
                return range_access::empty(right.pos(), left.end());
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            friend constexpr bool operator>(basic_sentinel<S> const &left,
                basic_iterator const &right)
            {
                return !range_access::empty(right.pos(), left.end());
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            friend constexpr bool operator>=(basic_sentinel<S> const &left,
                basic_iterator const &right)
            {
                return true;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cur>())
            RANGES_CXX14_CONSTEXPR
            reference operator[](difference_type n) const
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
                detail::mixin_base<Cur> &mix = it;
                return mix.get();
            }
            template<typename Cur, typename Sent>
            RANGES_CXX14_CONSTEXPR
            Cur const &operator()(basic_iterator<Cur, Sent> const &it) const noexcept
            {
                detail::mixin_base<Cur> const &mix = it;
                return mix.get();
            }
            template<typename Cur, typename Sent>
            RANGES_CXX14_CONSTEXPR
            Cur operator()(basic_iterator<Cur, Sent> &&it) const
                noexcept(std::is_nothrow_copy_constructible<Cur>::value)
            {
                detail::mixin_base<Cur> &mix = it;
                return std::move(mix.get());
            }
        };

        /// \sa `get_cursor_fn`
        /// \ingroup group-utility
        namespace
        {
            constexpr auto &&get_cursor = static_const<get_cursor_fn>::value;
        }
        /// @}

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
namespace std
{
    template<typename Cur, typename S>
    struct iterator_traits< ::ranges::basic_iterator<Cur, S>>
    {
    private:
        using iterator = ::ranges::basic_iterator<Cur, S>;
    public:
        using difference_type = typename iterator::difference_type;
        using value_type = typename iterator::value_type;
        using reference = typename iterator::reference;
        using iterator_category =
            ::meta::_t<
                ::ranges::detail::downgrade_iterator_category<
                    typename iterator::iterator_category,
                    reference>>;
        using pointer = typename iterator::pointer;
    };
}
/// \endcond

#endif
