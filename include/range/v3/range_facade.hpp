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
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/utility/infinity.hpp>

#define REQUIRES_(CAT) typename Impl = impl_t, CONCEPT_REQUIRES(CAT##Impl<Impl>())
#define REQUIRES(CAT)  template<REQUIRES_(CAT)>

namespace ranges
{
    inline namespace v3
    {
        template<typename Derived>
        struct iterable_facade
        {
            struct sentinel
            {};
        protected:
            using iterable_facade_ = iterable_facade;
        private:
            friend Derived;
            Derived & derived()
            {
                return static_cast<Derived &>(*this);
            }
            Derived const & derived() const
            {
                return static_cast<Derived const &>(*this);
            }
            //
            // Concepts that the range impl must model
            //
            struct InputImplConcept
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        t.dereference(),
                        (t.increment(), concepts::void_),
                        t.done()
                    ));
            };
            struct ForwardImplConcept
              : concepts::refines<InputImplConcept>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t.equal(t))
                    ));
            };
            struct BidirectionalImplConcept
              : concepts::refines<ForwardImplConcept>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        (t.decrement(), concepts::void_)
                    ));
            };
            struct RandomAccessImplConcept
              : concepts::refines<BidirectionalImplConcept>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        (t.advance(t.distance_to(t)), concepts::void_)
                    ));
            };
            struct InfiniteImplConcept
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(typename T::is_infinite{})
                    ));
            };

            template<typename T>
            using InputImpl = concepts::models<InputImplConcept, T>;

            template<typename T>
            using ForwardImpl = concepts::models<ForwardImplConcept, T>;

            template<typename T>
            using BidirectionalImpl = concepts::models<BidirectionalImplConcept, T>;

            template<typename T>
            using RandomAccessImpl = concepts::models<RandomAccessImplConcept, T>;

            template<typename T>
            using InfiniteImpl = concepts::models<InfiniteImplConcept, T>;

            template<typename T>
            using impl_concept_t = concepts::most_refined_t<RandomAccessImplConcept, T>;

            static auto iter_cat(InputImplConcept) -> std::input_iterator_tag;
            static auto iter_cat(ForwardImplConcept) -> std::forward_iterator_tag;
            static auto iter_cat(BidirectionalImplConcept) -> std::bidirectional_iterator_tag;
            static auto iter_cat(RandomAccessImplConcept) -> std::random_access_iterator_tag;

            // Default implementations
            Derived get_impl() const
            {
                return derived();
            }

            template<bool Const>
            struct basic_iterator
            {
            private:
                using derived_t = detail::add_const_if_t<Derived, Const>;
                using impl_t = decltype(std::declval<derived_t &>().get_impl());
                CONCEPT_ASSERT(InputImpl<impl_t>());
                using impl_concept_t = iterable_facade::impl_concept_t<impl_t>;
                impl_t impl_;
                static auto iter_diff(InputImplConcept) -> std::ptrdiff_t;
                template<typename Impl = impl_t>
                static auto iter_diff(RandomAccessImplConcept) ->
                    decltype(std::declval<Impl const&>().distance_to(
                        std::declval<Impl const&>()));
                constexpr bool equal_(basic_iterator const& that, InputImplConcept) const
                {
                    return true;
                }
                constexpr bool equal_(basic_iterator const& that, ForwardImplConcept) const
                {
                    return that.impl_.equal(impl_);
                }
            public:
                using reference = decltype(std::declval<impl_t const&>().dereference());
                using value_type = detail::uncvref_t<reference>;
                using iterator_category = decltype(iterable_facade::iter_cat(impl_concept_t{}));
                using difference_type = decltype(basic_iterator::iter_diff(impl_concept_t{}));
                using pointer = typename detail::operator_arrow_dispatch<reference>::type;
            private:
                using postfix_increment_result_t =
                    detail::postfix_increment_result<
                        basic_iterator, value_type, reference, iterator_category>;
                using operator_brackets_dispatch_t =
                    detail::operator_brackets_dispatch<basic_iterator, value_type, reference>;
                friend struct iterable_facade;
                basic_iterator(impl_t data)
                  : impl_(std::move(data))
                {}
            public:
                constexpr basic_iterator() = default;
                // For iterator -> const_iterator conversion
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : impl_(std::move(that.impl_))
                {}
                reference operator*() const
                {
                    return impl_.dereference();
                }
                pointer operator->() const
                {
                    return detail::operator_arrow_dispatch<reference>::apply(**this);
                }
                basic_iterator& operator++()
                {
                    impl_.increment();
                    return *this;
                }
                postfix_increment_result_t operator++(int)
                {
                    postfix_increment_result_t tmp{*this};
                    ++*this;
                    return tmp;
                }
                template<bool OtherConst>
                constexpr bool operator==(basic_iterator<OtherConst> const &that) const
                {
                    return equal_(that, impl_concept_t{});
                }
                template<bool OtherConst>
                constexpr bool operator!=(basic_iterator<OtherConst> const &that) const
                {
                    return !(*this == that);
                }
                friend constexpr bool operator==(basic_iterator const &left, sentinel const &)
                {
                    return left.impl_.done();
                }
                friend constexpr bool operator!=(basic_iterator const &left, sentinel const &right)
                {
                    return !(left == right);
                }
                friend constexpr bool operator==(sentinel const &, basic_iterator const &right)
                {
                    return right.impl_.done();
                }
                friend constexpr bool operator!=(sentinel const &left, basic_iterator const &right)
                {
                    return !(left == right);
                }
                REQUIRES(Bidirectional) basic_iterator& operator--()
                {
                    impl_.decrement();
                    return *this;
                }
                REQUIRES(Bidirectional) basic_iterator operator--(int)
                {
                    auto tmp{*this};
                    --*this;
                    return tmp;
                }
                REQUIRES(RandomAccess) basic_iterator& operator+=(difference_type n)
                {
                    impl_.advance(n);
                    return *this;
                }
                REQUIRES(RandomAccess)
                friend basic_iterator operator+(basic_iterator left, difference_type n)
                {
                    left += n;
                    return left;
                }
                REQUIRES(RandomAccess)
                friend basic_iterator operator+(difference_type n, basic_iterator right)
                {
                    right += n;
                    return right;
                }
                REQUIRES(RandomAccess) basic_iterator& operator-=(difference_type n)
                {
                    impl_.advance(-n);
                    return *this;
                }
                REQUIRES(RandomAccess)
                friend basic_iterator operator-(basic_iterator left, difference_type n)
                {
                    left -= n;
                    return left;
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                difference_type operator-(basic_iterator<OtherConst> const &right) const
                {
                    return right.impl_.distance_to(impl_);
                }
                REQUIRES(Infinite)
                friend constexpr infinity operator-(sentinel const &, basic_iterator const &)
                {
                    return {};
                }
                // symmetric comparisons
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                bool operator<(basic_iterator<OtherConst> const &that) const
                {
                    return 0 < (that - *this);
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                bool operator<=(basic_iterator<OtherConst> const &that) const
                {
                    return 0 <= (that - *this);
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                bool operator>(basic_iterator<OtherConst> const &that) const
                {
                    return (that - *this) < 0;
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                bool operator>=(basic_iterator<OtherConst> const &that) const
                {
                    return (that - *this) <= 0;
                }
                // asymmetric comparisons
                REQUIRES(RandomAccess)
                friend constexpr bool operator<(basic_iterator const &that, sentinel const &)
                {
                    return !that.done();
                }
                REQUIRES(RandomAccess)
                friend constexpr bool operator<=(basic_iterator const &that, sentinel const &)
                {
                    return true;
                }
                REQUIRES(RandomAccess)
                friend constexpr bool operator>(basic_iterator const &that, sentinel const &)
                {
                    return false;
                }
                REQUIRES(RandomAccess)
                friend constexpr bool operator>=(basic_iterator const &that, sentinel const &)
                {
                    return that.done();
                }
                REQUIRES(RandomAccess)
                friend constexpr bool operator<(sentinel const &, basic_iterator const &that)
                {
                    return false;
                }
                REQUIRES(RandomAccess)
                friend constexpr bool operator<=(sentinel const &, basic_iterator const &that)
                {
                    return that.done();
                }
                REQUIRES(RandomAccess)
                friend constexpr bool operator>(sentinel const &, basic_iterator const &that)
                {
                    return !that.done();
                }
                REQUIRES(RandomAccess)
                friend constexpr bool operator>=(sentinel const &, basic_iterator const &that)
                {
                    return true;
                }
                REQUIRES(RandomAccess)
                typename operator_brackets_dispatch_t::result_type
                operator[](difference_type n) const
                {
                    return operator_brackets_dispatch_t::apply(*this + n);
                }
            };
        public:
            using iterator = basic_iterator<false>;
            using const_iterator = basic_iterator<true>;

            iterator begin()
            {
                return {derived().get_impl()};
            };
            const_iterator begin() const
            {
                return {derived().get_impl()};
            };
            sentinel end() const
            {
                return {};
            }
            constexpr explicit operator bool() const
            {
                return begin() != end();
            }
        };
    }
}
#undef REQUIRES
#undef REQUIRES_

#endif
