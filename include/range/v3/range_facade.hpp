//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_RANGE_FACADE_HPP
#define RANGES_V3_RANGE_FACADE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T>
            struct is_reference_to_const
              : std::false_type
            {};

            template<typename T>
            struct is_reference_to_const<T const&>
              : std::true_type
            {};

            template<typename T>
            struct is_reference_to_const<T const&&>
              : std::true_type
            {};

            //
            // True iff the user has explicitly disabled writability of this
            // iterator.  Pass the iterator_facade's Value parameter and its
            // nested ::reference type.
            //
            template<typename ValueParam, typename Reference>
            using iterator_writability_disabled =
                std::integral_constant<bool,
                    std::is_const<Reference>::value ||
                    is_reference_to_const<Reference>::value ||
                    std::is_const<ValueParam>::value
                >;

            // operator[] must return a proxy in case iterator destruction invalidates
            // referents.
            // To see why, consider the following implementation of operator[]:
            //   reference operator[](difference_type n) const
            //   { return *(*this + n); }
            // The problem here is that operator[] would return a reference created from
            // a temporary iterator.
            template<typename Value>
            struct operator_brackets_value
            {
                using type = Value;
                template<typename Iterator>
                static type apply(Iterator const & i)
                {
                    return *i;
                }
            };

            template<typename Iterator, typename Reference>
            struct operator_brackets_const_proxy
            {
                using type = struct proxy
                {
                private:
                    Iterator const it_;
                    explicit proxy(Iterator i)
                      : it_(std::move(i))
                    {}
                    friend struct operator_brackets_const_proxy;
                public:
                    proxy const & operator=(proxy &) const = delete;
                    operator Reference() const
                    {
                        return *it_;
                    }
                    operator iterator_value_t<Iterator>() const volatile
                    {
                        return *it_;
                    }
                };
                static type apply(Iterator i)
                {
                    return type{std::move(i)};
                }
            };

            template<typename Iterator, typename Reference>
            struct operator_brackets_proxy
            {
                using type = struct proxy
                {
                private:
                    using value_type = iterator_value_t<Iterator>;
                    Iterator const it_;
                    explicit proxy(Iterator i)
                      : it_(std::move(i))
                    {}
                    friend struct operator_brackets_proxy;
                public:
                    operator Reference() const
                    {
                        return *it_;
                    }
                    operator value_type() const volatile
                    {
                        return *it_;
                    }
                    proxy const & operator=(proxy&) const = delete;
                    proxy const & operator=(value_type const & x) const
                    {
                        *it_ = x;
                        return *this;
                    }
                    proxy const & operator=(value_type && x) const
                    {
                        *it_ = std::move(x);
                        return *this;
                    }
                };
                static type apply(Iterator i)
                {
                    return type{std::move(i)};
                }
            };

            template<typename Iterator, typename ValueType, typename Reference>
            using operator_brackets_dispatch =
                detail::conditional_t<
                    iterator_writability_disabled<ValueType, Reference>::value,
                    detail::conditional_t<
                        std::is_pod<ValueType>::value,
                        operator_brackets_value<typename std::remove_const<ValueType>::type>,
                        operator_brackets_const_proxy<Iterator, Reference>
                    >,
                    operator_brackets_proxy<Iterator, Reference>
                >;

            // iterators whose dereference operators reference the same value
            // for all iterators into the same sequence (like many input
            // iterators) need help with their postfix ++: the referenced
            // value must be read and stored away before the increment occurs
            // so that *a++ yields the originally referenced element and not
            // the next one.
            template<typename Iterator>
            struct postfix_increment_proxy
            {
                using value_type = iterator_value_t<Iterator>;
            private:
                mutable value_type value_;
            public:
                postfix_increment_proxy() = default;
                explicit postfix_increment_proxy(Iterator const& x)
                  : value_(*x)
                {}
                // Returning a mutable reference allows nonsense like
                // (*r++).mutate(), but it imposes fewer assumptions about the
                // behavior of the value_type.  In particular, recall that
                // (*r).mutate() is legal if operator* returns by value.
                value_type& operator*() const
                {
                    return value_;
                }
            };

            //
            // In general, we can't determine that such an iterator isn't
            // writable -- we also need to store a copy of the old iterator so
            // that it can be written into.
            template<typename Iterator>
            struct writable_postfix_increment_proxy
            {
                using value_type = iterator_value_t<Iterator>;
            private:
                mutable value_type value_;
                Iterator it_;
            public:
                writable_postfix_increment_proxy() = default;
                explicit writable_postfix_increment_proxy(Iterator x)
                  : value_(*x)
                  , it_(std::move(x))
                {}
                // Dereferencing must return a proxy so that both *r++ = o and
                // value_type(*r++) can work.  In this case, *r is the same as
                // *r++, and the conversion operator below is used to ensure
                // readability.
                writable_postfix_increment_proxy const& operator*() const
                {
                    return *this;
                }
                // Provides readability of *r++
                operator value_type&() const
                {
                    return value_;
                }
                // Provides writability of *r++
                template<typename T, enable_if_t<!std::is_same<T, writable_postfix_increment_proxy>::value> = 0>
                T const& operator=(T const& x) const
                {
                    *it_ = x;
                    return x;
                }
                // This overload just in case only non-const objects are writable
                template<typename T, enable_if_t<!std::is_same<T, writable_postfix_increment_proxy>::value> = 0>
                T& operator=(T& x) const
                {
                    *it_ = x;
                    return x;
                }
                // Provides X(r++)
                operator Iterator const&() const
                {
                    return it_;
                }
            };

            template<typename Reference, typename Value>
            using is_non_proxy_reference =
                std::is_convertible<
                    typename std::remove_reference<Reference>::type const volatile*
                  , Value const volatile*
                >;

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
            template<typename Iterator, typename Value, typename Reference, typename Category>
            using postfix_increment_result =
                detail::lazy_conditional_t<
                    // A proxy is only needed for readable iterators
                    std::is_convertible<Reference, Value const&>::value &&
                    // No forward iterator can have values that disappear
                    // before positions can be re-visited
                    (bool) ranges::Derived<ranges::input_iterator_tag, Category>()
                  , std::conditional<
                        is_non_proxy_reference<Reference, Value>::value
                      , postfix_increment_proxy<Iterator>
                      , writable_postfix_increment_proxy<Iterator>
                    >
                  , detail::identity<Iterator>
                >;

            // operator->() needs special support for input iterators to strictly meet the
            // standard's requirements. If *i is not an lvalue reference type, we must still
            // produce an lvalue to which a pointer can be formed.  We do that by
            // returning a proxy object containing an instance of the reference object.
            template<typename Reference, bool B = std::is_reference<Reference>::value>
            struct operator_arrow_dispatch // proxy references
            {
            private:
                struct proxy
                {
                private:
                    Reference ref_;
                public:
                    explicit proxy(Reference x)
                      : ref_(std::move(x))
                    {}
                    Reference* operator->()
                    {
                        return std::addressof(ref_);
                    }
                };
            public:
                using type = proxy;
                static type apply(Reference x)
                {
                    return type{std::move(x)};
                }
            };

            // NOTE: Below, Reference could be an rvalue reference or an lvalue reference.
            template<typename Reference>
            struct operator_arrow_dispatch<Reference, true>
            {
                using type = typename std::remove_reference<Reference>::type *;
                static type apply(Reference x)
                {
                    return std::addressof(x);
                }
            };

            template<typename BaseIterable>
            struct base_iterable_holder
            {
            private:
                BaseIterable rng_;
            public:
                base_iterable_holder() = default;
                base_iterable_holder(BaseIterable &&rng)
                  : rng_(std::forward<BaseIterable>(rng))
                {}
                BaseIterable &get()
                {
                    return rng_;
                }
                BaseIterable const &get() const
                {
                    return rng_;
                }
            };

            template<typename BaseIterable>
            struct base_iterable_holder<BaseIterable &>
            {
            private:
                BaseIterable *rng_;
            public:
                base_iterable_holder() = default;
                base_iterable_holder(BaseIterable &rng)
                  : rng_(&rng)
                {}
                BaseIterable &get() const
                {
                    return *rng_;
                }
            };
        }

        struct range_core_access
        {
            //
            // Concepts that the range cursor must model
            //
            struct InputCursorConcept
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        //t.done(),
                        t.current(),
                        (t.next(), concepts::void_)
                    ));
            };
            struct ForwardCursorConcept
              : concepts::refines<InputCursorConcept>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t.equal(t))
                    ));
            };
            struct BidirectionalCursorConcept
              : concepts::refines<ForwardCursorConcept>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        (t.prev(), concepts::void_)
                    ));
            };
            struct RandomAccessCursorConcept
              : concepts::refines<BidirectionalCursorConcept>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SignedIntegral>(t.distance_to(t)),
                        (t.advance(t.distance_to(t)), concepts::void_)
                    ));
            };
            struct InfiniteCursorConcept
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(typename T::is_infinite{})
                    ));
            };

            struct IterableFacadeConcept
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        t.begin_cursor(),
                        t.end_cursor()
                    ));
            };

            struct RangeFacadeConcept
              : concepts::refines<IterableFacadeConcept>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(t.begin_cursor(), t.end_cursor())
                    ));
            };

            template<typename Range>
            static auto begin_cursor(Range & rng) -> decltype(rng.begin_cursor())
            {
                return rng.begin_cursor();
            }
            template<typename Range>
            static auto end_cursor(Range & rng) -> decltype(rng.end_cursor())
            {
                return rng.end_cursor();
            }

            template<typename Range>
            static auto begin_adaptor(Range & rng) -> decltype(rng.begin_adaptor())
            {
                return rng.begin_adaptor();
            }
            template<typename Range>
            static auto end_adaptor(Range & rng) -> decltype(rng.end_adaptor())
            {
                return rng.end_adaptor();
            }

            template<typename Cursor>
            static auto current(Cursor const &pos) -> decltype(pos.current())
            {
                return pos.current();
            }
            template<typename Cursor>
            static auto next(Cursor & pos) -> decltype(pos.next())
            {
                pos.next();
            }
            template<typename Cursor>
            static constexpr auto done(Cursor const & pos) -> decltype(pos.done())
            {
                return pos.done();
            }
            template<typename Cursor>
            static auto equal(Cursor const &pos0, Cursor const &pos1) ->
                decltype(pos0.equal(pos1))
            {
                return pos0.equal(pos1);
            }
            template<typename Cursor, typename Sentinel>
            static constexpr auto empty(Cursor const &pos, Sentinel const &end) ->
                decltype(end.equal(pos))
            {
                return end.equal(pos);
            }
            template<typename Cursor>
            static auto prev(Cursor & pos) -> decltype(pos.prev())
            {
                pos.prev();
            }
            template<typename Cursor, typename Difference>
            static auto advance(Cursor & pos, Difference n) ->
                decltype(pos.advance(n))
            {
                pos.advance(n);
            }
            template<typename Cursor>
            static auto distance_to(Cursor const &pos0, Cursor const &pos1) ->
                decltype(pos0.distance_to(pos1))
            {
                return pos0.distance_to(pos1);
            }
            template<typename Cursor>
            static auto base(Cursor const &pos) ->
                decltype(pos.base())
            {
                return pos.base();
            }
            template<typename Cursor>
            static auto count(Cursor const &pos) ->
                decltype(pos.count())
            {
                return pos.count();
            }

        private:
            template<typename Cursor>
            using random_access_cursor_difference_t =
                decltype(range_core_access::distance_to(std::declval<Cursor>(), std::declval<Cursor>()));

            template<typename Cursor, typename Enable = void>
            struct cursor_difference2
            {
                using type = std::ptrdiff_t;
            };

            template<typename Cursor>
            struct cursor_difference2<Cursor, detail::always_t<void, random_access_cursor_difference_t<Cursor>>>
            {
                using type = random_access_cursor_difference_t<Cursor>;
            };

            template<typename Cursor, typename Enable = void>
            struct cursor_difference
              : cursor_difference2<Cursor>
            {};

            template<typename Cursor>
            struct cursor_difference<Cursor, detail::always_t<void, typename Cursor::difference_type>>
            {
                using type = typename Cursor::difference_type;
            };

            template<typename Cursor, typename Enable = void>
            struct cursor_value
            {
                using type = detail::uncvref_t<decltype(std::declval<Cursor const &>().current())>;
            };

            template<typename Cursor>
            struct cursor_value<Cursor, detail::always_t<void, typename Cursor::value_type>>
            {
                using type = typename Cursor::value_type;
            };

            template<typename T, typename Enable = void>
            struct single_pass
            {
                using type = std::false_type;
            };

            template<typename T>
            struct single_pass<T, detail::always_t<void, typename T::single_pass>>
            {
                using type = typename T::single_pass;
            };
        public:
            template<typename Cursor>
            using cursor_difference_t = typename cursor_difference<Cursor>::type;

            template<typename Cursor>
            using cursor_value_t = typename cursor_value<Cursor>::type;

            template<typename Cursor>
            using single_pass_t = typename single_pass<Cursor>::type;

            template<typename Cursor, typename Sentinel>
            static Cursor cursor(basic_range_iterator<Cursor, Sentinel> it)
            {
                return std::move(it.pos());
            }
            template<typename Sentinel>
            static Sentinel sentinel(basic_range_sentinel<Sentinel> s)
            {
                return std::move(s.end());
            }

            template<typename RangeAdaptor>
            struct base_iterable
            {
                using type = typename RangeAdaptor::base_iterable_t;
            };
            template<typename RangeFacade>
            struct range_facade
            {
                using type = typename RangeFacade::range_facade_t;
            };
            template<typename RangeAdaptor>
            struct range_adaptor
            {
                using type = typename RangeAdaptor::range_adaptor_t;
            };
        };

        namespace detail
        {
            template<typename T>
            using InputCursor =
                concepts::models<range_core_access::InputCursorConcept, T>;

            template<typename T>
            using ForwardCursor =
                concepts::models<range_core_access::ForwardCursorConcept, T>;

            template<typename T>
            using BidirectionalCursor =
                concepts::models<range_core_access::BidirectionalCursorConcept, T>;

            template<typename T>
            using RandomAccessCursor =
                concepts::models<range_core_access::RandomAccessCursorConcept, T>;

            template<typename T>
            using InfiniteCursor =
                concepts::models<range_core_access::InfiniteCursorConcept, T>;

            template<typename T>
            using cursor_concept_t =
                concepts::most_refined_t<
                    typelist<
                        range_core_access::RandomAccessCursorConcept,
                        range_core_access::BidirectionalCursorConcept,
                        range_core_access::ForwardCursorConcept,
                        range_core_access::InputCursorConcept>, T>;

            template<typename T>
            using IterableFacade =
                concepts::models<range_core_access::IterableFacadeConcept, T>;

            template<typename T>
            using RangeFacade =
                concepts::models<range_core_access::RangeFacadeConcept, T>;

            template<typename T>
            using facade_concept_t =
                concepts::most_refined_t<
                    typelist<
                        range_core_access::RangeFacadeConcept,
                        range_core_access::IterableFacadeConcept>, T>;

            static auto iter_cat(range_core_access::InputCursorConcept) ->
                ranges::input_iterator_tag;
            static auto iter_cat(range_core_access::ForwardCursorConcept) ->
                ranges::forward_iterator_tag;
            static auto iter_cat(range_core_access::BidirectionalCursorConcept) ->
                ranges::bidirectional_iterator_tag;
            static auto iter_cat(range_core_access::RandomAccessCursorConcept) ->
                ranges::random_access_iterator_tag;

            template<typename Derived>
            using facade_cursor_t =
                decltype(range_core_access::begin_cursor(std::declval<Derived &>()));

            template<typename Derived>
            using facade_sentinel2_t =
                decltype(range_core_access::end_cursor(std::declval<Derived &>()));

            template<typename Derived>
            using facade_iterator_t =
                basic_range_iterator<facade_cursor_t<Derived>, facade_sentinel2_t<Derived>>;

            template<typename Derived>
            using facade_sentinel_t =
                conditional_t<
                    (Same<facade_cursor_t<Derived>, facade_sentinel2_t<Derived>>()),
                    basic_range_iterator<facade_cursor_t<Derived>, facade_sentinel2_t<Derived>>,
                    basic_range_sentinel<facade_sentinel2_t<Derived>>>;

            template<typename T>
            struct basic_mixin
            {
            private:
                T t_;
            public:
                constexpr basic_mixin() = default;
                basic_mixin(T t)
                  : t_(std::move(t))
                {}
                T &get()
                {
                    return t_;
                }
                T const &get() const
                {
                    return t_;
                }
            };

            template<typename Cursor, typename Enable = void>
            struct has_mixin
              : std::false_type
            {};

            template<typename Cursor>
            struct has_mixin<Cursor, always_t<void, typename Cursor::mixin>>
              : std::true_type
            {};

            template<typename Cursor>
            struct get_mixin
            {
                using type = typename Cursor::mixin;
            };

            template<typename Cursor>
            using mixin_base =
                lazy_conditional_t<
                    has_mixin<Cursor>::value, get_mixin<Cursor>, identity<basic_mixin<Cursor>>>;
        }

        template<typename Cursor, typename Sentinel>
        struct basic_range_iterator;

        template<typename Sentinel>
        struct basic_range_sentinel : detail::mixin_base<Sentinel>
        {
            // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60799
            #ifndef __GNUC__
        private:
            #endif
            friend range_core_access;
            template<typename Cursor, typename OtherSentinel>
            friend struct basic_range_iterator;
            using detail::mixin_base<Sentinel>::get;
            Sentinel &end()
            {
                return this->detail::mixin_base<Sentinel>::get();
            }
            Sentinel const &end() const
            {
                return this->detail::mixin_base<Sentinel>::get();
            }
        public:
            basic_range_sentinel() = default;
            basic_range_sentinel(Sentinel end)
              : detail::mixin_base<Sentinel>(std::move(end))
            {}
            using detail::mixin_base<Sentinel>::mixin_base;
            constexpr bool operator==(basic_range_sentinel<Sentinel> const &) const
            {
                return true;
            }
            constexpr bool operator!=(basic_range_sentinel<Sentinel> const &) const
            {
                return false;
            }
        };

        template<typename Cursor, typename Sentinel>
        struct basic_range_iterator
          : detail::mixin_base<Cursor>
        {
        private:
            friend range_core_access;
            CONCEPT_ASSERT(detail::InputCursor<Cursor>());
            using single_pass = range_core_access::single_pass_t<Cursor>;
            using cursor_concept_t =
                detail::conditional_t<
                    single_pass::value,
                    range_core_access::InputCursorConcept,
                    detail::cursor_concept_t<Cursor>>;

            using detail::mixin_base<Cursor>::get;
            Cursor &pos()
            {
                return this->detail::mixin_base<Cursor>::get();
            }
            Cursor const &pos() const
            {
                return this->detail::mixin_base<Cursor>::get();
            }

            // If Iterable models RangeFacade or if the cursor models
            // ForwardCursor, then positions must be equality comparable.
            // Otherwise, it's an InputCursor in an IterableFacade, so
            // all cursors are trivially equal.
            constexpr bool equal2_(basic_range_iterator const&,
                range_core_access::InputCursorConcept *) const
            {
                return true;
            }
            constexpr bool equal2_(basic_range_iterator const &that,
                range_core_access::ForwardCursorConcept *) const
            {
                return range_core_access::equal(pos(), that.pos());
            }
            constexpr bool equal_(basic_range_iterator const &that,
                std::false_type *) const
            {
                return basic_range_iterator::equal2_(that, (cursor_concept_t *)nullptr);
            }
            constexpr bool equal_(basic_range_iterator const &that,
                std::true_type *) const
            {
                return range_core_access::equal(pos(), that.pos());
            }
        public:
            using reference =
                decltype(range_core_access::current(std::declval<Cursor const &>()));
            using value_type = range_core_access::cursor_value_t<Cursor>;
            using iterator_category = decltype(detail::iter_cat(cursor_concept_t{}));
            using difference_type = range_core_access::cursor_difference_t<Cursor>;
            using pointer = typename detail::operator_arrow_dispatch<reference>::type;
        private:
            using postfix_increment_result_t =
                detail::postfix_increment_result<
                    basic_range_iterator, value_type, reference, iterator_category>;
            using operator_brackets_dispatch_t =
                detail::operator_brackets_dispatch<basic_range_iterator, value_type, reference>;
        public:
            constexpr basic_range_iterator() = default;
            basic_range_iterator(Cursor pos)
              : detail::mixin_base<Cursor>{std::move(pos)}
            {}
            // Mix in any additional constructors defined and exported by the cursor
            using detail::mixin_base<Cursor>::mixin_base;
            reference operator*() const
            {
                return range_core_access::current(pos());
            }
            pointer operator->() const
            {
                return detail::operator_arrow_dispatch<reference>::apply(**this);
            }
            basic_range_iterator& operator++()
            {
                range_core_access::next(pos());
                return *this;
            }
            postfix_increment_result_t operator++(int)
            {
                postfix_increment_result_t tmp{*this};
                ++*this;
                return tmp;
            }
            constexpr bool operator==(basic_range_iterator const &that) const
            {
                return basic_range_iterator::equal_(that, (std::is_same<Cursor, Sentinel> *)nullptr);
            }
            constexpr bool operator!=(basic_range_iterator const &that) const
            {
                return !(*this == that);
            }
            friend constexpr bool operator==(basic_range_iterator const &left,
                basic_range_sentinel<Sentinel> const &right)
            {
                return range_core_access::empty(left.pos(), right.end());
            }
            friend constexpr bool operator!=(basic_range_iterator const &left,
                basic_range_sentinel<Sentinel> const &right)
            {
                return !(left == right);
            }
            friend constexpr bool operator==(basic_range_sentinel<Sentinel> const & left,
                basic_range_iterator const &right)
            {
                return range_core_access::empty(right.pos(), left.end());
            }
            friend constexpr bool operator!=(basic_range_sentinel<Sentinel> const &left,
                basic_range_iterator const &right)
            {
                return !(left == right);
            }
            CONCEPT_REQUIRES(detail::BidirectionalCursor<Cursor>())
            basic_range_iterator& operator--()
            {
                range_core_access::prev(pos());
                return *this;
            }
            CONCEPT_REQUIRES(detail::BidirectionalCursor<Cursor>())
            basic_range_iterator operator--(int)
            {
                auto tmp{*this};
                --*this;
                return tmp;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            basic_range_iterator& operator+=(difference_type n)
            {
                range_core_access::advance(pos(), n);
                return *this;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend basic_range_iterator operator+(basic_range_iterator left, difference_type n)
            {
                left += n;
                return left;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend basic_range_iterator operator+(difference_type n, basic_range_iterator right)
            {
                right += n;
                return right;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            basic_range_iterator& operator-=(difference_type n)
            {
                range_core_access::advance(pos(), -n);
                return *this;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend basic_range_iterator operator-(basic_range_iterator left, difference_type n)
            {
                left -= n;
                return left;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            difference_type operator-(basic_range_iterator const &right) const
            {
                return range_core_access::distance_to(right.pos(), pos());
            }
            // symmetric comparisons
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            bool operator<(basic_range_iterator const &that) const
            {
                return 0 < (that - *this);
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            bool operator<=(basic_range_iterator const &that) const
            {
                return 0 <= (that - *this);
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            bool operator>(basic_range_iterator const &that) const
            {
                return (that - *this) < 0;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            bool operator>=(basic_range_iterator const &that) const
            {
                return (that - *this) <= 0;
            }
            // asymmetric comparisons
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend constexpr bool operator<(basic_range_iterator const &left,
                basic_range_sentinel<Sentinel> const &right)
            {
                return !range_core_access::empty(left.pos(), right.end());
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend constexpr bool operator<=(basic_range_iterator const &left,
                basic_range_sentinel<Sentinel> const &right)
            {
                return true;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend constexpr bool operator>(basic_range_iterator const &left,
                basic_range_sentinel<Sentinel> const &right)
            {
                return false;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend constexpr bool operator>=(basic_range_iterator const &left,
                basic_range_sentinel<Sentinel> const &right)
            {
                return range_core_access::empty(left.pos(), right.end());
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend constexpr bool operator<(basic_range_sentinel<Sentinel> const &left,
                basic_range_iterator const &right)
            {
                return false;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend constexpr bool operator<=(basic_range_sentinel<Sentinel> const &left,
                basic_range_iterator const &right)
            {
                return range_core_access::empty(right.pos(), left.end());
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend constexpr bool operator>(basic_range_sentinel<Sentinel> const &left,
                basic_range_iterator const &right)
            {
                return !range_core_access::empty(right.pos(), left.end());
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend constexpr bool operator>=(basic_range_sentinel<Sentinel> const &left,
                basic_range_iterator const &right)
            {
                return true;
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            typename operator_brackets_dispatch_t::type
            operator[](difference_type n) const
            {
                return operator_brackets_dispatch_t::apply(*this + n);
            }
        };

        template<typename Cursor>
        std::pair<basic_range_iterator<Cursor>, basic_range_iterator<Cursor>>
        as_iterator_pair(Cursor begin, Cursor end)
        {
            return {{std::move(begin)}, {std::move(end)}};
        }

        template<typename Cursor, typename Sentinel>
        std::pair<basic_range_iterator<Cursor, Sentinel>, basic_range_sentinel<Sentinel>>
        as_iterator_pair(Cursor begin, Sentinel end)
        {
            return {{std::move(begin)}, {std::move(end)}};
        }

        struct default_sentinel
        {
            template<typename Cursor>
            static constexpr bool equal(Cursor const &pos)
            {
                return range_core_access::done(pos);
            }
        };

        template<typename Derived, bool Infinite>
        struct range_facade
          : private detail::is_infinite<Infinite>
          , private range_base
        {
        private:
            friend Derived;
            friend range_core_access;
            using range_facade_t = range_facade;
        protected:
            Derived & derived()
            {
                return static_cast<Derived &>(*this);
            }
            Derived const & derived() const
            {
                return static_cast<Derived const &>(*this);
            }
            // Default implementations
            Derived begin_cursor() const
            {
                return derived();
            }
            default_sentinel end_cursor() const
            {
                return {};
            }
        public:
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_iterator_t<D> begin()
            {
                return {range_core_access::begin_cursor(derived())};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_sentinel_t<D> end()
            {
                return {range_core_access::end_cursor(derived())};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_iterator_t<D const> begin() const
            {
                return {range_core_access::begin_cursor(derived())};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_sentinel_t<D const> end() const
            {
                return {range_core_access::end_cursor(derived())};
            }
            constexpr bool operator!() const
            {
                return begin() == end();
            }
            constexpr explicit operator bool() const
            {
                return !!*this;
            }
        };

        template<typename RangeFacade>
        using range_facade_t = meta_apply<range_core_access::range_facade, RangeFacade>;
    }
}

namespace std
{
    template<typename Cursor, typename Sentinel>
    struct iterator_traits< ::ranges::basic_range_iterator<Cursor, Sentinel>>
    {
    private:
        using iterator = ::ranges::basic_range_iterator<Cursor, Sentinel>;
    public:
        using difference_type = typename iterator::difference_type;
        using value_type = typename iterator::value_type;
        using iterator_category = typename ::ranges::detail::as_std_iterator_category<
                                      typename iterator::iterator_category
                                  >::type;
        using reference = typename iterator::reference;
        using pointer = typename iterator::pointer;
    };
}

#endif
