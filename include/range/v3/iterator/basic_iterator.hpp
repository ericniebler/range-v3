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
#ifndef RANGES_V3_ITERATOR_BASIC_ITERATOR_HPP
#define RANGES_V3_ITERATOR_BASIC_ITERATOR_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/detail/range_access.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-utility Utility
    /// @{
    ///
    template<typename T>
    struct basic_mixin : private box<T>
    {
        CPP_member
        constexpr CPP_ctor(basic_mixin)()(
            noexcept(std::is_nothrow_default_constructible<T>::value)
            requires DefaultConstructible<T>)
          : box<T>{}
        {}
        CPP_member
        explicit constexpr CPP_ctor(basic_mixin)(T &&t)(
            noexcept(std::is_nothrow_move_constructible<T>::value)
            requires MoveConstructible<T>)
          : box<T>(detail::move(t))
        {}
        CPP_member
        explicit constexpr CPP_ctor(basic_mixin)(T const &t)(
            noexcept(std::is_nothrow_copy_constructible<T>::value)
            requires CopyConstructible<T>)
          : box<T>(t)
        {}
    protected:
        using box<T>::get;
    };

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

        template<typename Cur>
        using cursor_value_t = typename cursor_traits<Cur>::value_t_;

        template<typename Cur, bool Readable>
        struct basic_proxy_reference_;
        template<typename Cur>
        using basic_proxy_reference = basic_proxy_reference_<Cur, (bool) ReadableCursor<Cur>>;

        // The One Proxy Reference type to rule them all. basic_iterator uses this
        // as the return type of operator* when the cursor type has a set() member
        // function of the correct signature (i.e., if it can accept a value_type &&).
        template<typename Cur, bool Readable /*= (bool) ReadableCursor<Cur>*/>
        struct RANGES_EMPTY_BASES basic_proxy_reference_
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
        // BUGBUG make these private:
        public:
            constexpr /*c++14*/
            reference_t_ read_() const
                noexcept(noexcept(reference_t_(range_access::read(std::declval<Cur const &>()))))
            {
                return range_access::read(*cur_);
            }
            template<typename T>
            constexpr /*c++14*/
            void write_(T &&t) const
            {
                range_access::write(*cur_, (T &&) t);
            }
        // public:
            basic_proxy_reference_() = default;
            basic_proxy_reference_(basic_proxy_reference_ const &) = default;
            template<typename OtherCur>
            constexpr /*c++14*/
            CPP_ctor(basic_proxy_reference_)(basic_proxy_reference<OtherCur> const &that)(
                noexcept(true)
                requires ConvertibleTo<OtherCur *, Cur *>)
              : cur_(that.cur_)
            {}
            constexpr /*c++14*/
            explicit basic_proxy_reference_(Cur &cur) noexcept
              : cur_(&cur)
            {}
            CPP_member
            constexpr /*c++14*/
            auto operator=(basic_proxy_reference_ && that) ->
                CPP_ret(basic_proxy_reference_ &)(
                    requires ReadableCursor<Cur>)
            {
                return *this = that;
            }
            CPP_member
            constexpr /*c++14*/
            auto operator=(basic_proxy_reference_ const &that) ->
                CPP_ret(basic_proxy_reference_ &)(
                    requires ReadableCursor<Cur>)
            {
                this->write_(that.read_());
                return *this;
            }
            CPP_member
            constexpr /*c++14*/
            auto operator=(basic_proxy_reference_ && that) const ->
                CPP_ret(basic_proxy_reference_ const &)(
                    requires ReadableCursor<Cur>)
            {
                return *this = that;
            }
            CPP_member
            constexpr /*c++14*/
            auto operator=(basic_proxy_reference_ const &that) const ->
                CPP_ret(basic_proxy_reference_ const &)(
                    requires ReadableCursor<Cur>)
            {
                this->write_(that.read_());
                return *this;
            }
            template<typename OtherCur>
            constexpr /*c++14*/
            auto operator=(basic_proxy_reference<OtherCur> && that) ->
                CPP_ret(basic_proxy_reference_ &)(
                    requires ReadableCursor<OtherCur> &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>)
            {
                return *this = that;
            }
            template<typename OtherCur>
            constexpr /*c++14*/
            auto operator=(basic_proxy_reference<OtherCur> const &that) ->
                CPP_ret(basic_proxy_reference_ &)(
                    requires ReadableCursor<OtherCur> &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>)
            {
                this->write_(that.read_());
                return *this;
            }
            template<typename OtherCur>
            constexpr /*c++14*/
            auto operator=(basic_proxy_reference<OtherCur> && that) const ->
                CPP_ret(basic_proxy_reference_ const &)(
                    requires ReadableCursor<OtherCur> &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>)
            {
                return *this = that;
            }
            template<typename OtherCur>
            constexpr /*c++14*/
            auto operator=(basic_proxy_reference<OtherCur> const &that) const ->
                CPP_ret(basic_proxy_reference_ const &)(
                    requires ReadableCursor<OtherCur> &&
                        WritableCursor<Cur, cursor_reference_t<OtherCur>>)
            {
                this->write_(that.read_());
                return *this;
            }
            template<typename T>
            constexpr /*c++14*/
            auto operator=(T &&t) ->
                CPP_ret(basic_proxy_reference_ &)(
                    requires WritableCursor<Cur, T>)
            {
                this->write_((T &&) t);
                return *this;
            }
            template<typename T>
            constexpr /*c++14*/
            auto operator=(T &&t) const ->
                CPP_ret(basic_proxy_reference_ const &)(
                    requires WritableCursor<Cur, T>)
            {
                this->write_((T &&) t);
                return *this;
            }
        };

        template<typename Cur, bool Readable>
        constexpr /*c++14*/
        auto operator==(basic_proxy_reference_<Cur, Readable> const &x,
            cursor_value_t<Cur> const &y) -> CPP_ret(bool)(
                requires ReadableCursor<Cur> &&
                    EqualityComparable<cursor_value_t<Cur>>)
        {
            return x.read_() == y;
        }
        template<typename Cur, bool Readable>
        constexpr /*c++14*/
        auto operator!=(basic_proxy_reference_<Cur, Readable> const &x,
            cursor_value_t<Cur> const &y) -> CPP_ret(bool)(
                requires ReadableCursor<Cur> &&
                    EqualityComparable<cursor_value_t<Cur>>)
        {
            return !(x == y);
        }
        template<typename Cur, bool Readable>
        constexpr /*c++14*/
        auto operator==(cursor_value_t<Cur> const &x,
            basic_proxy_reference_<Cur, Readable> const &y) ->
            CPP_ret(bool)(
                requires ReadableCursor<Cur> &&
                    EqualityComparable<cursor_value_t<Cur>>)
        {
            return x == y.read_();
        }
        template<typename Cur, bool Readable>
        constexpr /*c++14*/
        auto operator!=(cursor_value_t<Cur> const &x,
            basic_proxy_reference_<Cur, Readable> const &y) ->
            CPP_ret(bool)(
                requires ReadableCursor<Cur> &&
                    EqualityComparable<cursor_value_t<Cur>>)
        {
            return !(x == y);
        }
        template<typename Cur, bool Readable>
        constexpr /*c++14*/
        auto operator==(basic_proxy_reference_<Cur, Readable> const &x,
            basic_proxy_reference_<Cur, Readable> const &y) ->
            CPP_ret(bool)(
                requires ReadableCursor<Cur> &&
                    EqualityComparable<cursor_value_t<Cur>>)
        {
            return x.read_() == y.read_();
        }
        template<typename Cur, bool Readable>
        constexpr /*c++14*/
        auto operator!=(basic_proxy_reference_<Cur, Readable> const &x,
            basic_proxy_reference_<Cur, Readable> const &y) ->
            CPP_ret(bool)(
                requires ReadableCursor<Cur> &&
                    EqualityComparable<cursor_value_t<Cur>>)
        {
            return !(x == y);
        }

        auto iter_cat(input_cursor_tag) ->
            ranges::input_iterator_tag;
        auto iter_cat(forward_cursor_tag) ->
            ranges::forward_iterator_tag;
        auto iter_cat(bidirectional_cursor_tag) ->
            ranges::bidirectional_iterator_tag;
        auto iter_cat(random_access_cursor_tag) ->
            ranges::random_access_iterator_tag;

        template<typename Cur>
        using cpp20_iter_cat_of_t = decltype(detail::iter_cat(cursor_tag_of<Cur>{}));

        CPP_def
        (
            template(typename C)
            concept Cpp17InputCursor,
                InputCursor<C> &&
                CursorSentinel<C, C> &&
                // Either it is not single-pass, or else we can create a
                // proxy for postfix increment.
                (
                    !range_access::single_pass_t<uncvref_t<C>>::value ||
                    (
                        Constructible<
                            range_access::cursor_value_t<C>,
                            cursor_reference_t<C>> &&
                        MoveConstructible<range_access::cursor_value_t<C>>
                    )
                )
        );

        CPP_def
        (
            template(typename C)
            concept Cpp17ForwardCursor,
                ForwardCursor<C> &&
                std::is_reference<cursor_reference_t<C>>::value
        );

        using cpp17_input_cursor_tag =
            concepts::tag<Cpp17InputCursorConcept, cursor_tag>;
        using cpp17_forward_cursor_tag =
            concepts::tag<Cpp17ForwardCursorConcept, cpp17_input_cursor_tag>;
        using cpp17_bidirectional_cursor_tag =
            concepts::tag<BidirectionalCursorConcept, cpp17_forward_cursor_tag>;
        using cpp17_random_access_cursor_tag =
            concepts::tag<RandomAccessCursorConcept, cpp17_bidirectional_cursor_tag>;

        template<typename Cur>
        using cpp17_cursor_tag_of =
            concepts::tag_of<
                meta::list<
                    RandomAccessCursorConcept,
                    BidirectionalCursorConcept,
                    Cpp17ForwardCursorConcept,
                    Cpp17InputCursorConcept,
                    CursorConcept>,
                Cur>;

        template<typename Category, typename Base = void>
        struct with_iterator_category
          : Base
        {
            using iterator_category = Category;
        };

        template<typename Category>
        struct with_iterator_category<Category>
        {
            using iterator_category = Category;
        };

        auto cpp17_iter_cat(cpp17_input_cursor_tag) ->
            ranges::input_iterator_tag;
        auto cpp17_iter_cat(cpp17_forward_cursor_tag) ->
            ranges::forward_iterator_tag;
        auto cpp17_iter_cat(cpp17_bidirectional_cursor_tag) ->
            ranges::bidirectional_iterator_tag;
        auto cpp17_iter_cat(cpp17_random_access_cursor_tag) ->
            ranges::random_access_iterator_tag;

        template<typename Cur>
        using cpp17_iter_cat_of_t =
            decltype(detail::cpp17_iter_cat(cpp17_cursor_tag_of<Cur>{}));

        template<typename Cur, typename = void>
        struct readable_iterator_associated_types_base
          : range_access::mixin_base_t<Cur>
        {
            using range_access::mixin_base_t<Cur>::mixin_base_t;
        };

        template<typename Cur>
        struct readable_iterator_associated_types_base<
            Cur,
            always_<void, cpp17_iter_cat_of_t<Cur>>>
          : range_access::mixin_base_t<Cur>
        {
            using range_access::mixin_base_t<Cur>::mixin_base_t;
            using iterator_category = cpp17_iter_cat_of_t<Cur>;
        };

        template<typename Cur, bool Readable /*= (bool) ReadableCursor<Cur>*/>
        struct iterator_associated_types_base_
          : range_access::mixin_base_t<Cur>
        {
        // BUGBUG
        // protected:
            using iter_reference_t = basic_proxy_reference<Cur>;
            using const_reference_t = basic_proxy_reference<Cur const>;
            using cursor_tag_t = concepts::tag<detail::OutputCursorConcept, cursor_tag>;
        public:
            using reference = void;
            using difference_type = range_access::cursor_difference_t<Cur>;

            using range_access::mixin_base_t<Cur>::mixin_base_t;
        };

        template<typename Cur>
        using cursor_arrow_t =
            decltype(range_access::arrow(std::declval<Cur const &>()));

        template<typename Cur>
        struct iterator_associated_types_base_<Cur, true>
          : readable_iterator_associated_types_base<Cur>
        {
        // BUGBUG
        // protected:
            using cursor_tag_t = cursor_tag_of<Cur>;
            using iter_reference_t =
                if_then_t<
                    is_writable_cursor<Cur const>::value,
                    basic_proxy_reference<Cur const>,
                    if_then_t<
                        is_writable_cursor<Cur>::value,
                        basic_proxy_reference<Cur>,
                        cursor_reference_t<Cur>>>;
            using const_reference_t =
                if_then_t<
                    is_writable_cursor<Cur const>::value,
                    basic_proxy_reference<Cur const>,
                    cursor_reference_t<Cur>>;
        public:
            using difference_type = range_access::cursor_difference_t<Cur>;
            using value_type = range_access::cursor_value_t<Cur>;
            using reference = iter_reference_t;
            using iterator_concept = cpp20_iter_cat_of_t<Cur>;
            using pointer = meta::_t<if_then_t<
                (bool) HasCursorArrow<Cur>,
                meta::defer<cursor_arrow_t, Cur>,
                std::add_pointer<reference>>>;
            using common_reference = common_reference_t<reference, value_type &>;

            using readable_iterator_associated_types_base<Cur>::
                readable_iterator_associated_types_base;
        };

        template<typename Cur>
        using iterator_associated_types_base =
            iterator_associated_types_base_<Cur, (bool) ReadableCursor<Cur>>;

        template<typename Value>
        struct postfix_increment_proxy
        {
        private:
            Value cache_;
        public:
            template<typename T>
            constexpr postfix_increment_proxy(T &&t)
              : cache_(static_cast<T &&>(t))
            {}
            constexpr Value const &operator*() const noexcept
            {
                return cache_;
            }
        };
    }
    /// \endcond

#if RANGES_BROKEN_CPO_LOOKUP
    namespace _basic_iterator_ { template<typename> struct adl_hook {}; }
#endif

    template<typename Cur>
    struct RANGES_EMPTY_BASES basic_iterator
      : detail::iterator_associated_types_base<Cur>
#if RANGES_BROKEN_CPO_LOOKUP
      , private _basic_iterator_::adl_hook<basic_iterator<Cur>>
#endif
    {
    private:
        template<typename>
        friend struct basic_iterator;
        friend range_access;
        using base_t = detail::iterator_associated_types_base<Cur>;
        using mixin_t = range_access::mixin_base_t<Cur>;
        static_assert((bool) detail::Cursor<Cur>, "");
        using assoc_types_ = detail::iterator_associated_types_base<Cur>;
        using typename assoc_types_::iter_reference_t;
        using typename assoc_types_::const_reference_t;
        constexpr /*c++14*/ Cur &pos() noexcept
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
        template<typename OtherCur>
        constexpr /*c++14*/
        CPP_ctor(basic_iterator)(basic_iterator<OtherCur> that)(
            requires (!Same<OtherCur, Cur>) && ConvertibleTo<OtherCur, Cur> &&
                Constructible<mixin_t, OtherCur>)
          : base_t{std::move(that.pos())}
        {}
        // Mix in any additional constructors provided by the mixin
        using base_t::base_t;

        template<typename OtherCur>
        constexpr /*c++14*/
        auto operator=(basic_iterator<OtherCur> that) ->
            CPP_ret(basic_iterator &)(
                requires (!Same<OtherCur, Cur>) && ConvertibleTo<OtherCur, Cur>)
        {
            pos() = std::move(that.pos());
            return *this;
        }

        CPP_member
        constexpr auto operator*() const
        noexcept(noexcept(range_access::read(std::declval<Cur const &>()))) ->
            CPP_ret(const_reference_t)(
                requires detail::ReadableCursor<Cur> &&
                    !detail::is_writable_cursor<Cur>::value)
        {
            return range_access::read(pos());
        }
        CPP_member
        constexpr /*c++14*/ auto operator*()
        noexcept(noexcept(iter_reference_t{std::declval<Cur &>()})) ->
            CPP_ret(iter_reference_t)(
                requires detail::HasCursorNext<Cur> &&
                    detail::is_writable_cursor<Cur>::value)
        {
            return iter_reference_t{pos()};
        }
        CPP_member
        constexpr auto operator*() const
        noexcept(noexcept(const_reference_t{std::declval<Cur const &>()})) ->
            CPP_ret(const_reference_t)(
                requires detail::HasCursorNext<Cur> &&
                    detail::is_writable_cursor<Cur const>::value)
        {
            return const_reference_t{pos()};
        }
        CPP_member
        constexpr /*c++14*/ auto operator*() noexcept ->
            CPP_ret(basic_iterator &)(
                requires not detail::HasCursorNext<Cur>)
        {
            return *this;
        }

        // Use cursor's arrow() member, if any.
        template<typename C = Cur>
        constexpr auto operator->() const
        noexcept(noexcept(range_access::arrow(std::declval<C const &>()))) ->
            CPP_ret(detail::cursor_arrow_t<C>)(
                requires detail::HasCursorArrow<C>)
        {
            return range_access::arrow(pos());
        }
        // Otherwise, if iter_reference_t is an lvalue reference to cv-qualified
        // iter_value_t, return the address of **this.
        template<typename C = Cur>
        constexpr auto operator->() const
        noexcept(noexcept(*std::declval<basic_iterator const &>())) ->
            CPP_ret(meta::_t<std::add_pointer<const_reference_t>>)(
                requires not detail::HasCursorArrow<Cur> &&
                    detail::ReadableCursor<Cur> &&
                    std::is_lvalue_reference<const_reference_t>::value &&
                    Same<typename detail::iterator_associated_types_base<C>::value_type,
                        uncvref_t<const_reference_t>>)
        {
            return std::addressof(**this);
        }

        CPP_member
        constexpr /*c++14*/ auto operator++() ->
            CPP_ret(basic_iterator &)(
                requires detail::HasCursorNext<Cur>)
        {
            range_access::next(pos());
            return *this;
        }
        CPP_member
        constexpr /*c++14*/ auto operator++() noexcept ->
            CPP_ret(basic_iterator &)(
                requires not detail::HasCursorNext<Cur>)
        {
            return *this;
        }

    private:
        constexpr /*c++14*/ basic_iterator post_increment_(std::false_type, int)
        {
            basic_iterator tmp{*this};
            ++*this;
            return tmp;
        }
        // Attempt to satisfy the C++17 iterator requirements by returning a
        // proxy from postfix increment:
        template<typename A = assoc_types_, typename V = typename A::value_type>
        constexpr /*c++14*/
        auto post_increment_(std::true_type, int) ->
            CPP_ret(detail::postfix_increment_proxy<V>)(
                requires Constructible<V, typename A::reference> &&
                    MoveConstructible<V>)
        {
            detail::postfix_increment_proxy<V> p{**this};
            ++*this;
            return p;
        }
        constexpr /*c++14*/ void post_increment_(std::true_type, long)
        {
            ++*this;
        }

    public:
        CPP_member
        constexpr /*c++14*/ auto operator++(int)
        {
            return this->post_increment_(
                std::is_same<
                    detail::input_cursor_tag,
                    detail::cursor_tag_of<Cur>>{},
                0);
        }

        CPP_member
        constexpr /*c++14*/ auto operator--() ->
            CPP_ret(basic_iterator &)(
                requires detail::BidirectionalCursor<Cur>)
        {
            range_access::prev(pos());
            return *this;
        }
        CPP_member
        constexpr /*c++14*/ auto operator--(int) ->
            CPP_ret(basic_iterator)(
                requires detail::BidirectionalCursor<Cur>)
        {
            basic_iterator tmp(*this);
            --*this;
            return tmp;
        }
        CPP_member
        constexpr /*c++14*/ auto operator+=(difference_type n) ->
            CPP_ret(basic_iterator &)(
                requires detail::RandomAccessCursor<Cur>)
        {
            range_access::advance(pos(), n);
            return *this;
        }
        CPP_member
        constexpr /*c++14*/ auto operator-=(difference_type n) ->
            CPP_ret(basic_iterator &)(
                requires detail::RandomAccessCursor<Cur>)
        {
            range_access::advance(pos(), -n);
            return *this;
        }
        CPP_member
        constexpr /*c++14*/ auto operator[](difference_type n) const ->
            CPP_ret(const_reference_t)(
                requires detail::RandomAccessCursor<Cur>)
        {
            return *(*this + n);
        }

#if !RANGES_BROKEN_CPO_LOOKUP
        // Optionally support hooking iter_move when the cursor sports a
        // move() member function.
        template<typename C = Cur>
        constexpr /*c++14*/
        friend auto iter_move(basic_iterator const &it)
        noexcept(noexcept(range_access::move(std::declval<C const &>()))) ->
            CPP_broken_friend_ret(decltype(range_access::move(std::declval<C const &>())))(
                requires Same<C, Cur> && detail::InputCursor<Cur>)
        {
            return range_access::move(it.pos());
        }
#endif
    };

    template<typename Cur, typename Cur2>
    constexpr auto operator==(basic_iterator<Cur> const &left,
        basic_iterator<Cur2> const &right) ->
            CPP_ret(bool)(
                requires detail::CursorSentinel<Cur2, Cur>)
    {
        return range_access::equal(range_access::pos(left), range_access::pos(right));
    }
    template<typename Cur, typename Cur2>
    constexpr auto operator!=(basic_iterator<Cur> const &left,
        basic_iterator<Cur2> const &right) ->
            CPP_ret(bool)(
                requires detail::CursorSentinel<Cur2, Cur>)
    {
        return !(left == right);
    }
    template<typename Cur, typename S>
    constexpr auto operator==(basic_iterator<Cur> const &left,
        S const &right) ->
            CPP_ret(bool)(
                requires detail::CursorSentinel<S, Cur>)
    {
        return range_access::equal(range_access::pos(left), right);
    }
    template<typename Cur, typename S>
    constexpr auto operator!=(basic_iterator<Cur> const &left,
        S const &right) ->
            CPP_ret(bool)(
                requires detail::CursorSentinel<S, Cur>)
    {
        return !(left == right);
    }
    template<typename S, typename Cur>
    constexpr auto operator==(S const &left,
        basic_iterator<Cur> const &right) ->
            CPP_ret(bool)(
                requires detail::CursorSentinel<S, Cur>)
    {
        return right == left;
    }
    template<typename S, typename Cur>
    constexpr auto operator!=(S const &left,
        basic_iterator<Cur> const &right) ->
            CPP_ret(bool)(
                requires detail::CursorSentinel<S, Cur>)
    {
        return right != left;
    }

    template<typename Cur>
    constexpr /*c++14*/
    auto operator+(basic_iterator<Cur> left,
        typename basic_iterator<Cur>::difference_type n) ->
        CPP_ret(basic_iterator<Cur>)(
            requires detail::RandomAccessCursor<Cur>)
    {
        left += n;
        return left;
    }
    template<typename Cur>
    constexpr /*c++14*/
    auto operator+(
        typename basic_iterator<Cur>::difference_type n,
        basic_iterator<Cur> right) ->
        CPP_ret(basic_iterator<Cur>)(
            requires detail::RandomAccessCursor<Cur>)
    {
        right += n;
        return right;
    }
    template<typename Cur>
    constexpr /*c++14*/
    auto operator-(basic_iterator<Cur> left,
        typename basic_iterator<Cur>::difference_type n) ->
        CPP_ret(basic_iterator<Cur>)(
            requires detail::RandomAccessCursor<Cur>)
    {
        left -= n;
        return left;
    }
    template<typename Cur2, typename Cur>
    constexpr /*c++14*/
    auto operator-(basic_iterator<Cur2> const &left,
        basic_iterator<Cur> const &right) ->
        CPP_ret(typename basic_iterator<Cur>::difference_type)(
            requires detail::SizedCursorSentinel<Cur2, Cur>)
    {
        return range_access::distance_to(range_access::pos(right), range_access::pos(left));
    }
    template<typename S, typename Cur>
    constexpr /*c++14*/
    auto operator-(S const &left, basic_iterator<Cur> const &right) ->
        CPP_ret(typename basic_iterator<Cur>::difference_type)(
            requires detail::SizedCursorSentinel<S, Cur>)
    {
        return range_access::distance_to(range_access::pos(right), left);
    }
    template<typename Cur, typename S>
    constexpr /*c++14*/
    auto operator-(basic_iterator<Cur> const &left, S const &right) ->
        CPP_ret(typename basic_iterator<Cur>::difference_type)(
            requires detail::SizedCursorSentinel<S, Cur>)
    {
        return -(right - left);
    }
    // Asymmetric comparisons
    template<typename Left, typename Right>
    constexpr /*c++14*/
    auto operator<(basic_iterator<Left> const &left,
        basic_iterator<Right> const &right) ->
        CPP_ret(bool)(
            requires detail::SizedCursorSentinel<Right, Left>)
    {
        return 0 < (right - left);
    }
    template<typename Left, typename Right>
    constexpr /*c++14*/
    auto operator<=(basic_iterator<Left> const &left,
        basic_iterator<Right> const &right) ->
        CPP_ret(bool)(
            requires detail::SizedCursorSentinel<Right, Left>)
    {
        return 0 <= (right - left);
    }
    template<typename Left, typename Right>
    constexpr /*c++14*/
    auto operator>(basic_iterator<Left> const &left,
        basic_iterator<Right> const &right) ->
        CPP_ret(bool)(
            requires detail::SizedCursorSentinel<Right, Left>)
    {
        return (right - left) < 0;
    }
    template<typename Left, typename Right>
    constexpr /*c++14*/
    auto operator>=(basic_iterator<Left> const &left,
        basic_iterator<Right> const &right) ->
        CPP_ret(bool)(
            requires detail::SizedCursorSentinel<Right, Left>)
    {
        return (right - left) <= 0;
    }

#if RANGES_BROKEN_CPO_LOOKUP
    namespace _basic_iterator_
    {
        // Optionally support hooking iter_move when the cursor sports a
        // move() member function.
        template<typename Cur>
        constexpr /*c++14*/
        auto iter_move(basic_iterator<Cur> const &it)
        noexcept(noexcept(range_access::move(std::declval<Cur const &>()))) ->
            CPP_broken_friend_ret(decltype(range_access::move(std::declval<Cur const &>())))(
                requires detail::InputCursor<Cur>)
        {
            return range_access::move(range_access::pos(it));
        }
    }
#endif

    /// Get a cursor from a basic_iterator
    struct get_cursor_fn
    {
        template<typename Cur>
        constexpr /*c++14*/
        Cur &operator()(basic_iterator<Cur> &it) const noexcept
        {
            return range_access::pos(it);
        }
        template<typename Cur>
        constexpr /*c++14*/
        Cur const &operator()(basic_iterator<Cur> const &it) const noexcept
        {
            return range_access::pos(it);
        }
        template<typename Cur>
        constexpr /*c++14*/
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

/// \cond
namespace concepts
{
    // common_reference specializations for basic_proxy_reference
    template<typename Cur, typename U, template<typename> class TQual, template<typename> class UQual>
    struct basic_common_reference<::ranges::detail::basic_proxy_reference_<Cur, true>, U, TQual, UQual>
      : basic_common_reference<::ranges::detail::cursor_reference_t<Cur>, U, TQual, UQual>
    {};
    template<typename T, typename Cur, template<typename> class TQual, template<typename> class UQual>
    struct basic_common_reference<T, ::ranges::detail::basic_proxy_reference_<Cur, true>, TQual, UQual>
      : basic_common_reference<T, ::ranges::detail::cursor_reference_t<Cur>, TQual, UQual>
    {};
    template<typename Cur1, typename Cur2, template<typename> class TQual, template<typename> class UQual>
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
    /// \cond
    namespace detail
    {
        template<typename Cur, bool IsReadable>
        struct std_iterator_traits_
        {
            using difference_type =
                typename iterator_associated_types_base<Cur>::difference_type;
            using value_type = void;
            using reference = void;
            using pointer = void;
            using iterator_category = std::output_iterator_tag;
            using iterator_concept = std::output_iterator_tag;
        };

        template<typename Cur>
        struct std_iterator_traits_<Cur, true>
          : iterator_associated_types_base<Cur>
        {};

        template<typename Cur>
        using std_iterator_traits =
            std_iterator_traits_<Cur, (bool) ReadableCursor<Cur>>;
    }
    /// \endcond
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
