// (C) Copyright David Abrahams 2002.
// (C) Copyright Jeremy Siek    2002.
// (C) Copyright Thomas Witt    2002.
// (C) copyright Eric Niebler   2013.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_V3_UTILITY_ITERATOR_ADAPTOR_HPP
#define RANGES_V3_UTILITY_ITERATOR_ADAPTOR_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        //
        // Default template argument handling for iterator_adaptor
        //
        namespace detail
        {
            // A metafunction which computes an iterator_adaptor's base class,
            // a specialization of iterator_facade.
            template<
                typename Derived
              , typename Base
              , typename Value
              , typename Category
              , typename Reference
              , typename Difference
              , typename Pointer
            >
            using iterator_adaptor_base = iterator_facade<
                Derived
              , conditional_with_default_t<
                    Value
                  , conditional_t<
                        std::is_same<Reference, use_default>::value
                      , iterator_value<Base>
                      , std::remove_reference<Reference>
                    >
                >
              , conditional_with_default_t<
                    Category
                  , iterator_category<Base>
                >
              , conditional_with_default_t<
                    Reference
                  , conditional_t<
                        std::is_same<Value, use_default>::value
                      , iterator_reference<Base>
                      , std::add_lvalue_reference<Value>
                    >
                >
              , conditional_with_default_t<
                    Difference
                  , iterator_difference<Base>
                >
              , Pointer
            >;
        }

        //
        // Iterator Adaptor
        //
        // The parameter ordering changed slightly with respect to former
        // versions of iterator_adaptor The idea is that when the user needs
        // to fiddle with the reference type it is highly likely that the
        // iterator category has to be adjusted as well.  Any of the
        // following four template arguments may be ommitted or explicitly
        // replaced by use_default.
        //
        //   Value - if supplied, the value_type of the resulting iterator, unless
        //      const. If const, a conforming compiler strips constness for the
        //      value_type. If not supplied, iterator_traits<Base>::value_type is used
        //
        //   Category - the traversal category of the resulting iterator. If not
        //      supplied, iterator_traversal<Base>::type is used.
        //
        //   Reference - the reference type of the resulting iterator, and in
        //      particular, the result type of operator*(). If not supplied but
        //      Value is supplied, Value& is used. Otherwise
        //      iterator_traits<Base>::reference is used.
        //
        //   Difference - the difference_type of the resulting iterator. If not
        //      supplied, iterator_traits<Base>::difference_type is used.
        //
        template<
            typename Derived
          , typename Base
          , typename Value        = use_default
          , typename Category     = use_default
          , typename Reference    = use_default
          , typename Difference   = use_default
          , typename Pointer      = use_default
        >
        struct iterator_adaptor
          : detail::iterator_adaptor_base<
                Derived, Base, Value, Category, Reference, Difference, Pointer
            >
        {
        private:
            friend struct iterator_core_access;

            Base it_;

            // Core iterator interface for iterator_facade.  This is private
            // to prevent temptation for Derived classes to use it, which
            // will often result in an error.  Derived classes should use
            // base_reference(), below, to get direct access to it_.
            typename iterator_adaptor::reference dereference() const
            {
                return *it_;
            }
            template<typename OD, typename OB, typename V, typename C, typename R, typename D, typename P>
            bool equal(iterator_adaptor<OD, OB, V, C, R, D, P> const& x) const
            {
                return it_ == x.base();
            }
            void increment()
            {
                ++it_;
            }
            void decrement()
            {
                --it_;
            }
            void advance(typename iterator_adaptor::difference_type n)
            {
                it_ += n;
            }
            template<typename OD, typename OB, typename V, typename C, typename R, typename D, typename P>
            typename iterator_adaptor::difference_type
            distance_to(iterator_adaptor<OD, OB, V, C, R, D, P> const& y) const
            {
                return y.base() - it_;
            }
        protected:
            // for convenience in derived classes
            using iterator_adaptor_ = iterator_adaptor;

            // lvalue access to the Base object for Derived
            Base& base_reference() &
            {
                return it_;
            }
            Base&& base_reference() &&
            {
                return std::move(it_);
            }
            Base const& base_reference() const &
            {
                return it_;
            }
        public:
            using base_type = Base;

            iterator_adaptor()
              : it_{}
            {}
            explicit iterator_adaptor(Base it)
              : it_(std::move(it))
            {}
            Base const& base() const
            {
                return it_;
            }
        };
    }
}

#endif
