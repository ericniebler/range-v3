// (C) Copyright David Abrahams 2002.
// (C) Copyright Jeremy Siek    2002.
// (C) Copyright Thomas Witt    2002.
// (C) copyright Jeffrey Lee Hellrung, Jr. 2012.
// (C) copyright Eric Niebler   2013.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_V3_UTILITY_ITERATOR_FACADE_HPP
#define RANGES_V3_UTILITY_ITERATOR_FACADE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        // This forward declaration is required for the friend declaration
        // in iterator_core_access
        template<typename I, typename V, typename TC, typename R, typename D>
        struct iterator_facade;

        //namespace concepts
        //{
        //    struct IteratorFacade
        //    {
        //        template<typename T>
        //        auto requires(T && t -> decltype(
        //            concepts::valid_expr(
        //                concepts::is_void((t.increment(), concepts::void_)),
        //                t.dereference()
        //            ));
        //    };

        //    struct OutputIteratorFacade
        //      : refines<IteratorFacade>
        //    {};

        //    struct InputIteratorFacade
        //      : refines<IteratorFacade>
        //    {
        //        template<typename T>
        //        auto requires(T && t) -> decltype(
        //            concepts::valid_expr(
        //                concepts::convertible_to<bool>(t.equal(t))
        //            ));
        //    };

        //    struct BidirectionalIteratorFacade
        //      : refines<InputIteratorFacade>
        //    {
        //        template<typename T>
        //        auto requires(T && t) -> decltype(
        //            concepts::valid_expr(
        //                concepts::is_void((t.decrement(), concepts::void_))
        //            ));
        //    };

        //    struct RandomAccessIteratorFacade
        //      : refines<BidirectionalIteratorFacade>
        //    {
        //        template<typename T>
        //        auto requires(T && t) -> decltype(
        //            concepts::valid_expr(
        //                concepts::is_void((t.advance(42), concepts::void_)),
        //                concepts::model_of<concepts::SignedIntegral>(t.distance_to(42))
        //            ));
        //    };
        //}

        //template<typename T>
        //constexpr bool IteratorFacade()
        //{
        //    return concepts::models<concepts::IteratorFacade, T>();
        //}

        //template<typename T>
        //constexpr bool OutputIteratorFacade()
        //{
        //    return concepts::models<concepts::OutputIteratorFacade, T>();
        //}

        //template<typename T>
        //constexpr bool InputIteratorFacade()
        //{
        //    return concepts::models<concepts::InputIteratorFacade, T>();
        //}

        //template<typename T>
        //constexpr bool BidirectionalIteratorFacade()
        //{
        //    return concepts::models<concepts::BidirectionalIteratorFacade, T>();
        //}

        //template<typename T>
        //constexpr bool RandomAccessIteratorFacade()
        //{
        //    return concepts::models<concepts::RandomAccessIteratorFacade, T>();
        //}

        namespace detail
        {
            template<bool B, typename U, typename V>
            using lazy_conditional = typename std::conditional<B, U, V>::type;

            template<typename T>
            struct is_reference_to_const
              : std::false_type
            {};

            template<typename T>
            struct is_reference_to_const<T const&>
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
                using result_type = Value;
                template<typename Iterator>
                static result_type apply(Iterator const & i)
                {
                    return *i;
                }
            };

            template<typename Iterator, typename Reference>
            struct operator_brackets_const_proxy
            {
                struct result_type
                {
                private:
                    Iterator const it_;
                    explicit result_type(Iterator i)
                      : it_(std::move(i))
                    {}
                    friend struct operator_brackets_const_proxy;
                public:
                    void operator=(result_type&) = delete;
                    operator Reference() const
                    {
                        return *it_;
                    }
                };
                static result_type apply(Iterator i)
                {
                    return result_type{std::move(i)};
                }
            };

            template<typename Iterator, typename Reference>
            struct operator_brackets_proxy
            {
                struct result_type
                {
                private:
                using value_type = typename std::iterator_traits<Iterator>::value_type;
                    Iterator const it_;
                    explicit result_type(Iterator i)
                      : it_(std::move(i))
                    {}
                    friend struct operator_brackets_proxy;
                public:
                    operator Reference() const
                    {
                        return *it_;
                    }
                    operator_brackets_proxy const & operator=(result_type&) const = delete;
                    operator_brackets_proxy const & operator=(value_type const & x) const
                    {
                        *it_ = x;
                        return *this;
                    }
                    operator_brackets_proxy const & operator=(value_type && x) const
                    {
                        *it_ = std::move(x);
                        return *this;
                    }
                };
                static result_type apply(Iterator i)
                {
                    return result_type{std::move(i)};
                }
            };

            template<typename Iterator, typename ValueType, typename Reference>
            using operator_brackets_dispatch =
                typename std::conditional<
                    iterator_writability_disabled<ValueType, Reference>::value,
                    typename std::conditional<
                        std::is_pod<ValueType>::value,
                        operator_brackets_value<typename std::remove_const<ValueType>::type>,
                        operator_brackets_const_proxy<Iterator, Reference>
                    >::type,
                    operator_brackets_proxy<Iterator, Reference>
                >::type;

            //
            // enable if for use in operator implementation.
            //
            template<typename Facade1, typename Facade2, typename Return>
            using enable_if_interoperable =
                typename std::enable_if<
                    std::is_convertible<Facade1, Facade2>::value ||
                    std::is_convertible<Facade2, Facade1>::value
                  , Return
                >::type;

            //
            // Generates associated types for an iterator_facade with the
            // given parameters.
            //
            template<
                typename ValueParam
              , typename Category
              , typename Reference
              , typename Difference
            >
            struct iterator_facade_types
            {
                using iterator_category = Category;
                using value_type = typename std::remove_const<ValueParam>::type;

                // Not the real associated pointer type
                using pointer = typename detail::lazy_conditional<
                    detail::iterator_writability_disabled<ValueParam, Reference>::value
                  , std::add_pointer<const value_type>
                  , std::add_pointer<value_type>
                >::type;
            };

            // iterators whose dereference operators reference the same value
            // for all iterators into the same sequence (like many input
            // iterators) need help with their postfix ++: the referenced
            // value must be read and stored away before the increment occurs
            // so that *a++ yields the originally referenced element and not
            // the next one.
            template<typename Iterator>
            struct postfix_increment_proxy
            {
            private:
                using value_type = typename std::iterator_traits<Iterator>::value_type;
                mutable value_type value_;
            public:
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
            private:
                using value_type = typename std::iterator_traits<Iterator>::value_type;
                mutable value_type value_;
                Iterator it_;
            public:
                explicit writable_postfix_increment_proxy(Iterator x)
                  : value_(*x)
                  , it_(std::move(x))
                {}
                // Dereferencing must return a proxy so that both *r++ = o and
                // value_type(*r++) can work.  In this case, *r is the same as
                // *r++, and the conversion operatorbelow is used to ensure
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
                template<typename T>
                T const& operator=(T const& x) const
                {
                    *it_ = x;
                    return x;
                }
                // This overload just in case only non-const objects are writable
                template<typename T>
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
                typename detail::lazy_conditional<
                    // A proxy is only needed for readable iterators
                    std::is_convertible<Reference, Value const&>::value &&
                    // No forward iterator can have values that disappear
                    // before positions can be re-visited
                    !std::is_convertible<Category, std::forward_iterator_tag>::value
                  , std::conditional<
                        is_non_proxy_reference<Reference, Value>::value
                      , postfix_increment_proxy<Iterator>
                      , writable_postfix_increment_proxy<Iterator>
                    >
                  , detail::identity<Iterator>
                >::type;

            // operator->() needs special support for input iterators to strictly meet the
            // standard's requirements. If *i is not a reference type, we must still
            // produce an lvalue to which a pointer can be formed.  We do that by
            // returning a proxy object containing an instance of the reference object.
            template<typename Reference>
            struct operator_arrow_dispatch // proxy references
            {
            private:
                struct proxy
                {
                    Reference* operator->()
                    {
                        return std::addressof(m_ref);
                    }
                private:
                    friend struct operator_arrow_dispatch;
                    explicit proxy(Reference x)
                      : m_ref(std::move(x))
                    {}
                    Reference m_ref;
                };
            public:
                using result_type = proxy;
                static result_type apply(Reference x)
                {
                    return result_type{std::move(x)};
                }
            };

            template<typename T>
            struct operator_arrow_dispatch<T&> // "real" references
            {
                using result_type = T *;
                static result_type apply(T& x)
                {
                    return std::addressof(x);
                }
            };

            template<typename I1, typename I2>
            using choose_difference_type =
                typename std::conditional<
                   std::is_convertible<I2, I1>::value
                 , typename std::iterator_traits<I1>::difference_type
                 , typename std::iterator_traits<I2>::difference_type
               >::type;
        } // namespace detail

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

            template<typename I, typename V, typename TC, typename R, typename D>
            friend struct iterator_facade;

            template<typename Facade>
            static typename Facade::reference dereference(Facade const& f)
            {
                return f.dereference();
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

            //
            // Curiously Recurring Template interface.
            //
            template<typename I, typename V, typename TC, typename R, typename D>
            static I& derived(iterator_facade<I, V, TC, R, D>& facade)
            {
                return *static_cast<I*>(&facade);
            }
            template<typename I, typename V, typename TC, typename R, typename D>
            static I const& derived(iterator_facade<I, V, TC, R, D> const& facade)
            {
                return *static_cast<I const*>(&facade);
            }
        };

        //
        // iterator_facade - use as a public base typename for defining new
        // standard-conforming iterators.
        //
        template<
            typename Derived             // The derived iterator type being constructed
          , typename Value
          , typename Category
          , typename Reference   = Value&
          , typename Difference  = std::ptrdiff_t
        >
        struct iterator_facade
        {
        private:
            using operator_arrow_dispatch_ =
                detail::operator_arrow_dispatch<Reference>;
            using associated_types =
                detail::iterator_facade_types<Value, Category, Reference, Difference>;
            using operator_brackets_dispatch_ =
                detail::operator_brackets_dispatch<
                    Derived
                  , Value
                  , Reference
                >;
            //
            // Curiously Recurring Template interface.
            //
            Derived& derived()
            {
                return *static_cast<Derived*>(this);
            }
            Derived const& derived() const
            {
                return *static_cast<Derived const*>(this);
            }
        protected:
            // For use by derived classes
            using iterator_facade_ =
                iterator_facade<Derived, Value, Category, Reference, Difference>;

        public:
            using value_type = typename associated_types::value_type;
            using reference = Reference;
            using difference_type = Difference;
            using pointer = typename operator_arrow_dispatch_::result_type;
            using iterator_category = typename associated_types::iterator_category;

            reference operator*() const
            {
                return iterator_core_access::dereference(derived());
            }
            pointer operator->() const
            {
                return operator_arrow_dispatch_::apply(*derived());
            }
            typename operator_brackets_dispatch_::result_type
            operator[](difference_type n) const
            {
                return operator_brackets_dispatch_::apply(derived() + n);
            }
            Derived& operator++()
            {
                iterator_core_access::increment(derived());
                return derived();
            }
            Derived& operator--()
            {
                iterator_core_access::decrement(derived());
                return derived();
            }
            Derived operator--(int)
            {
                Derived tmp(derived());
                --*this;
                return tmp;
            }
            Derived& operator+=(difference_type n)
            {
                iterator_core_access::advance(derived(), n);
                return derived();
            }
            Derived& operator-=(difference_type n)
            {
                iterator_core_access::advance(derived(), -n);
                return derived();
            }
            Derived operator-(difference_type x) const &
            {
                Derived result(derived());
                result -= x;
                return result;
            }
            Derived operator-(difference_type x) &&
            {
                *this -= x;
                return std::move(*this);
            }
        };

        template<typename I, typename V, typename TC, typename R, typename D>
        detail::postfix_increment_result<I, V, R, TC>
        operator++(iterator_facade<I, V, TC, R, D>& i, int)
        {
            detail::postfix_increment_result<I, V, R, TC> tmp(*static_cast<I*>(&i));
            ++i;
            return tmp;
        }

        //
        // Comparison operator implementation. The library supplied operators
        // enables the user to provide fully interoperable constant/mutable
        // iterator types. I.e. the library provides all operators
        // for all mutable/constant iterator combinations.
        //
        // Note though that this kind of interoperability for constant/mutable
        // iterators is not required by the standard for container iterators.
        // All the standard asks for is a conversion mutable -> constant.
        // Most standard library implementations nowadays provide fully interoperable
        // iterator implementations, but there are still heavily used implementations
        // that do not provide them. (Actually it's even worse, they do not provide
        // them for only a few iterators.)
        //
        // ?? Maybe a BOOST_ITERATOR_NO_FULL_INTEROPERABILITY macro should
        //    enable the user to turn off mixed type operators
        //
        // The library takes care to provide only the right operator overloads.
        // I.e.
        //
        // bool operator==(Iterator,      Iterator);
        // bool operator==(ConstIterator, Iterator);
        // bool operator==(Iterator,      ConstIterator);
        // bool operator==(ConstIterator, ConstIterator);
        //
        //   ...
        //
        // In order to do so it uses c++ idioms that are not yet widely supported
        // by current compiler releases. The library is designed to degrade gracefully
        // in the face of compiler deficiencies. In general compiler
        // deficiencies result in less strict error checking and more obscure
        // error messages, functionality is not affected.
        //
        // For full operation compiler support for "Substitution Failure Is Not An Error"
        // (aka. enable_if) and is_convertible is required.
        //
        // The following problems occur if support is lacking.
        //
        // Pseudo code
        //
        // ---------------
        // AdaptorA<Iterator1> a1;
        // AdaptorA<Iterator2> a2;
        //
        // // This will result in a no such overload error in full operation
        // // If enable_if or is_convertible is not supported
        // // The instantiation will fail with an error hopefully indicating that
        // // there is no operator== for Iterator1, Iterator2
        // // The same will happen if no enable_if is used to remove
        // // false overloads from the templated conversion constructor
        // // of AdaptorA.
        //
        // a1 == a2;
        // ----------------
        //
        // AdaptorA<Iterator> a;
        // AdaptorB<Iterator> b;
        //
        // // This will result in a no such overload error in full operation
        // // If enable_if is not supported the static assert used
        // // in the operatorimplementation will fail.
        // // This will accidently work if std::is_convertible is not supported.
        //
        // a == b;
        // ----------------
        //
        template<
            typename Derived1, typename V1, typename TC1, typename R1, typename D1
          , typename Derived2, typename V2, typename TC2, typename R2, typename D2
        >
        detail::enable_if_interoperable<Derived1, Derived2, bool>
        operator==(
            iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs
          , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs)
        {
            return iterator_core_access::equal(
                *static_cast<Derived1 const*>(&lhs)
              , *static_cast<Derived2 const*>(&rhs)
              , std::is_convertible<Derived2, Derived1>()
            );
        }

        template<
            typename Derived1, typename V1, typename TC1, typename R1, typename D1
          , typename Derived2, typename V2, typename TC2, typename R2, typename D2
        >
        detail::enable_if_interoperable<Derived1, Derived2, bool>
        operator!=(
            iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs
          , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs)
        {
            return !iterator_core_access::equal(
                *static_cast<Derived1 const*>(&lhs)
              , *static_cast<Derived2 const*>(&rhs)
              , std::is_convertible<Derived2, Derived1>()
            );
        }

        template<
            typename Derived1, typename V1, typename TC1, typename R1, typename D1
          , typename Derived2, typename V2, typename TC2, typename R2, typename D2
        >
        detail::enable_if_interoperable<Derived1, Derived2, bool>
        operator<(
            iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs
          , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs)
        {
            return 0 > iterator_core_access::distance_from(
                *static_cast<Derived1 const*>(&lhs)
              , *static_cast<Derived2 const*>(&rhs)
              , std::is_convertible<Derived2, Derived1>()
            );
        }

        template<
            typename Derived1, typename V1, typename TC1, typename R1, typename D1
          , typename Derived2, typename V2, typename TC2, typename R2, typename D2
        >
        detail::enable_if_interoperable<Derived1, Derived2, bool>
        operator>(
            iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs
          , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs)
        {
            return 0 < iterator_core_access::distance_from(
                *static_cast<Derived1 const*>(&lhs)
              , *static_cast<Derived2 const*>(&rhs)
              , std::is_convertible<Derived2, Derived1>()
            );
        }

        template<
            typename Derived1, typename V1, typename TC1, typename R1, typename D1
          , typename Derived2, typename V2, typename TC2, typename R2, typename D2
        >
        detail::enable_if_interoperable<Derived1, Derived2, bool>
        operator<=(
            iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs
          , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs)
        {
            return 0 >= iterator_core_access::distance_from(
                *static_cast<Derived1 const*>(&lhs)
              , *static_cast<Derived2 const*>(&rhs)
              , std::is_convertible<Derived2, Derived1>()
            );
        }

        template<
            typename Derived1, typename V1, typename TC1, typename R1, typename D1
          , typename Derived2, typename V2, typename TC2, typename R2, typename D2
        >
        detail::enable_if_interoperable<Derived1, Derived2, bool>
        operator>=(
            iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs
          , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs)
        {
            return 0 <= iterator_core_access::distance_from(
                *static_cast<Derived1 const*>(&lhs)
              , *static_cast<Derived2 const*>(&rhs)
              , std::is_convertible<Derived2, Derived1>()
            );
        }

        // operator- requires an additional part in the static assertion
        template<
            typename Derived1, typename V1, typename TC1, typename R1, typename D1
          , typename Derived2, typename V2, typename TC2, typename R2, typename D2
        >
        detail::enable_if_interoperable<
            Derived1, Derived2
          , detail::choose_difference_type<Derived1, Derived2>
        >
        operator-(
            iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs
          , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs)
        {
            return iterator_core_access::distance_from(
                *static_cast<Derived1 const*>(&lhs)
              , *static_cast<Derived2 const*>(&rhs)
              , std::is_convertible<Derived2, Derived1>()
            );
        }

        template<typename Derived, typename V, typename TC, typename R, typename D>
        inline Derived operator+(
            iterator_facade<Derived, V, TC, R, D> const& i
          , typename Derived::difference_type n)
        {
            Derived tmp(static_cast<Derived const&>(i));
            return tmp += n;
        }

        template<typename Derived, typename V, typename TC, typename R, typename D>
        inline Derived operator+(
            iterator_facade<Derived, V, TC, R, D> && i
          , typename Derived::difference_type n)
        {
            Derived tmp(static_cast<Derived &&>(i));
            return tmp += n;
        }

        template<typename Derived, typename V, typename TC, typename R, typename D>
        inline Derived operator+(
            typename Derived::difference_type n
          , iterator_facade<Derived, V, TC, R, D> const& i)
        {
            Derived tmp(static_cast<Derived const&>(i));
            return tmp += n;
        }

        template<typename Derived, typename V, typename TC, typename R, typename D>
        inline Derived operator+(
            typename Derived::difference_type n
          , iterator_facade<Derived, V, TC, R, D> && i)
        {
            Derived tmp(static_cast<Derived &&>(i));
            return tmp += n;
        }
    }
}

#endif // RANGES_V3_UTILITY_ITERATOR_FACADE_HPP
