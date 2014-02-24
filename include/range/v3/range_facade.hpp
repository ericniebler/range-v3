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

#define REQUIRES_(CAT) typename Impl = impl_t, CONCEPT_REQUIRES(detail::CAT##Impl<Impl>())
#define REQUIRES(CAT)  template<REQUIRES_(CAT)>

namespace ranges
{
    inline namespace v3
    {
        struct range_core_access
        {
            range_core_access() = delete;

            //
            // Concepts that the range impl must model
            //
            struct InputImplConcept
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        //t.done(),
                        t.current(),
                        (t.next(), concepts::void_)
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
                        (t.prev(), concepts::void_)
                    ));
            };
            struct RandomAccessImplConcept
              : concepts::refines<BidirectionalImplConcept>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<concepts::SignedIntegral>(t.distance_to(t)),
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

            struct RangeFacadeConcept
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(t.begin_impl(), t.end_impl())
                    ));
            };

            template<typename Range>
            static auto begin_impl(Range & rng) -> decltype(rng.begin_impl())
            {
                return rng.begin_impl();
            }
            template<typename Range>
            static auto end_impl(Range & rng) -> decltype(rng.end_impl())
            {
                return rng.end_impl();
            }

            template<typename Impl>
            static auto current(Impl const &impl) -> decltype(impl.current())
            {
                return impl.current();
            }
            template<typename Impl>
            static auto next(Impl & impl) -> decltype(impl.next())
            {
                impl.next();
            }
            template<typename Impl>
            static constexpr auto done(Impl const & impl) -> decltype(impl.done())
            {
                return impl.done();
            }
            template<typename Impl0, typename Impl1>
            static constexpr auto done(Impl0 const & impl0, Impl1 const &impl1) ->
                decltype(impl0.done(impl1))
            {
                return impl0.done(impl1);
            }
            template<typename Impl0, typename Impl1>
            static auto equal(Impl0 const &impl0, Impl1 const &impl1) ->
                decltype(impl0.equal(impl1))
            {
                return impl0.equal(impl1);
            }
            template<typename Impl>
            static auto prev(Impl & impl) -> decltype(impl.prev())
            {
                impl.prev();
            }
            template<typename Impl, typename Difference>
            static auto advance(Impl & impl, Difference n) ->
                decltype(impl.advance(n))
            {
                impl.advance(n);
            }
            template<typename Impl0, typename Impl1>
            static auto distance_to(Impl0 const &impl0, Impl1 const &impl1) ->
                decltype(impl0.distance_to(impl1))
            {
                return impl0.distance_to(impl1);
            }

            template<typename Range, bool Const>
            struct basic_impl_detail_t
            {
                typedef typename Range::template basic_impl<Const> type;
            };

            template<typename Range, bool Const>
            using basic_impl_t = typename basic_impl_detail_t<Range, Const>::type;

            template<typename Derived>
            using impl_t = basic_impl_t<Derived, false>;

            template<typename Derived>
            using const_impl_t = basic_impl_t<Derived, true>;
        };

        namespace detail
        {
            template<typename T>
            using InputImpl =
                concepts::models<range_core_access::InputImplConcept, T>;

            template<typename T>
            using ForwardImpl =
                concepts::models<range_core_access::ForwardImplConcept, T>;

            template<typename T>
            using BidirectionalImpl =
                concepts::models<range_core_access::BidirectionalImplConcept, T>;

            template<typename T>
            using RandomAccessImpl =
                concepts::models<range_core_access::RandomAccessImplConcept, T>;

            template<typename T>
            using InfiniteImpl =
                concepts::models<range_core_access::InfiniteImplConcept, T>;

            template<typename T>
            using impl_concept_t =
                concepts::most_refined_t<range_core_access::RandomAccessImplConcept, T>;

            template<typename T>
            using RangeFacade =
                concepts::models<range_core_access::RangeFacadeConcept, T>;

            static auto iter_cat(range_core_access::InputImplConcept) ->
                std::input_iterator_tag;
            static auto iter_cat(range_core_access::ForwardImplConcept) ->
                std::forward_iterator_tag;
            static auto iter_cat(range_core_access::BidirectionalImplConcept) ->
                std::bidirectional_iterator_tag;
            static auto iter_cat(range_core_access::RandomAccessImplConcept) ->
                std::random_access_iterator_tag;
        }

        template<typename Derived>
        struct range_facade
        {
        protected:
            using range_facade_ = range_facade;
            Derived & derived()
            {
                return static_cast<Derived &>(*this);
            }
            Derived const & derived() const
            {
                return static_cast<Derived const &>(*this);
            }
        private:
            friend Derived;
            friend struct range_core_access;

            // Default implementations
            struct default_sentinel_impl
            {
                template<typename Impl>
                static constexpr bool done(Impl const &impl)
                {
                    return range_core_access::done(impl);
                }
            };

            Derived begin_impl() const
            {
                return derived();
            }
            default_sentinel_impl end_impl() const
            {
                return {};
            }

            template<bool Const>
            struct basic_iterator;

            template<bool Const>
            struct basic_sentinel
            {
            private:
                template<bool OtherConst>
                friend struct basic_iterator;
                friend struct range_facade;
                using derived_t = detail::add_const_if_t<Derived, Const>;
                using impl_t = decltype(range_core_access::end_impl(std::declval<derived_t &>()));
                impl_t impl_;
                basic_sentinel(impl_t impl)
                  : impl_(std::move(impl))
                {}
            public:
                basic_sentinel() = default;
                // For sentinel -> const_sentinel conversion
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
                basic_sentinel(basic_sentinel<OtherConst> that)
                  : impl_(std::move(that.impl_))
                {}
            };

            template<bool Const>
            struct basic_iterator
            {
            private:
                using derived_t = detail::add_const_if_t<Derived, Const>;
                using impl_t = decltype(range_core_access::begin_impl(std::declval<derived_t &>()));
                CONCEPT_ASSERT(detail::InputImpl<impl_t>());
                using impl_concept_t = detail::impl_concept_t<impl_t>;
                impl_t impl_;
                static auto iter_diff(range_core_access::InputImplConcept) ->
                    std::ptrdiff_t;
                template<typename Impl = impl_t>
                static auto iter_diff(range_core_access::RandomAccessImplConcept) ->
                    decltype(std::declval<Impl const&>().distance_to(
                        std::declval<Impl const&>()));
                template<bool OtherConst>
                constexpr bool equal_(basic_iterator<OtherConst> const&, 
                    range_core_access::InputImplConcept *) const
                {
                    return true;
                }
                template<bool OtherConst>
                constexpr bool equal_(basic_iterator<OtherConst> const& that,
                    range_core_access::ForwardImplConcept *) const
                {
                    return range_core_access::equal(impl_, that.impl_);
                }
            public:
                using reference =
                    decltype(range_core_access::current(std::declval<impl_t const&>()));
                using value_type = detail::uncvref_t<reference>;
                using iterator_category = decltype(detail::iter_cat(impl_concept_t{}));
                using difference_type = decltype(basic_iterator::iter_diff(impl_concept_t{}));
                using pointer = typename detail::operator_arrow_dispatch<reference>::type;
            private:
                using postfix_increment_result_t =
                    detail::postfix_increment_result<
                        basic_iterator, value_type, reference, iterator_category>;
                using operator_brackets_dispatch_t =
                    detail::operator_brackets_dispatch<basic_iterator, value_type, reference>;
                friend struct range_facade;
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
                    return range_core_access::current(impl_);
                }
                pointer operator->() const
                {
                    return detail::operator_arrow_dispatch<reference>::apply(**this);
                }
                basic_iterator& operator++()
                {
                    range_core_access::next(impl_);
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
                    return equal_(that, static_cast<impl_concept_t *>(nullptr));
                }
                template<bool OtherConst>
                constexpr bool operator!=(basic_iterator<OtherConst> const &that) const
                {
                    return !(*this == that);
                }
                template<bool OtherConst>
                friend constexpr bool operator==(basic_iterator const &left,
                    basic_sentinel<OtherConst> const &right)
                {
                    return range_core_access::done(right.impl_, left.impl_);
                }
                template<bool OtherConst>
                friend constexpr bool operator!=(basic_iterator const &left,
                    basic_sentinel<OtherConst> const &right)
                {
                    return !(left == right);
                }
                template<bool OtherConst>
                friend constexpr bool operator==(basic_sentinel<OtherConst> const & left,
                    basic_iterator const &right)
                {
                    return range_core_access::done(left.impl_, right.impl_);
                }
                template<bool OtherConst>
                friend constexpr bool operator!=(basic_sentinel<OtherConst> const &left,
                    basic_iterator const &right)
                {
                    return !(left == right);
                }
                REQUIRES(Bidirectional) basic_iterator& operator--()
                {
                    range_core_access::prev(impl_);
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
                    range_core_access::advance(impl_, n);
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
                    range_core_access::advance(impl_, -n);
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
                    return range_core_access::distance_to(right.impl_, impl_);
                }
                template<bool OtherConst, REQUIRES_(Infinite)>
                friend constexpr infinity operator-(basic_sentinel<OtherConst> const &,
                    basic_iterator const &)
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
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                friend constexpr bool operator<(basic_iterator const &left,
                    basic_sentinel<OtherConst> const &right)
                {
                    return !range_core_access::done(right.impl_, left.impl_);
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                friend constexpr bool operator<=(basic_iterator const &left,
                    basic_sentinel<OtherConst> const &right)
                {
                    return true;
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                friend constexpr bool operator>(basic_iterator const &left,
                    basic_sentinel<OtherConst> const &right)
                {
                    return false;
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                friend constexpr bool operator>=(basic_iterator const &left,
                    basic_sentinel<OtherConst> const &right)
                {
                    return range_core_access::done(right.impl_, left.impl_);
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                friend constexpr bool operator<(basic_sentinel<OtherConst> const &left,
                    basic_iterator const &right)
                {
                    return false;
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                friend constexpr bool operator<=(basic_sentinel<OtherConst> const &left,
                    basic_iterator const &right)
                {
                    return range_core_access::done(left.impl_, right.impl_);
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                friend constexpr bool operator>(basic_sentinel<OtherConst> const &left,
                    basic_iterator const &right)
                {
                    return !range_core_access::done(left.impl_, right.impl_);
                }
                template<bool OtherConst, REQUIRES_(RandomAccess)>
                friend constexpr bool operator>=(basic_sentinel<OtherConst> const &left,
                    basic_iterator const &right)
                {
                    return true;
                }
                REQUIRES(RandomAccess)
                typename operator_brackets_dispatch_t::type
                operator[](difference_type n) const
                {
                    return operator_brackets_dispatch_t::apply(*this + n);
                }
            };
        public:
            using iterator = basic_iterator<false>;
            using const_iterator = basic_iterator<true>;
            using sentinel = basic_sentinel<false>;
            using const_sentinel = basic_sentinel<true>;

            iterator begin()
            {
                return {range_core_access::begin_impl(derived())};
            };
            const_iterator begin() const
            {
                return {range_core_access::begin_impl(derived())};
            };
            template<typename D = Derived, CONCEPT_REQUIRES(SameType<D, Derived>())>
            detail::conditional_t<(detail::RangeFacade<D>()), iterator, sentinel>
            end()
            {
                return {range_core_access::end_impl(derived())};
            }
            template<typename D = Derived, CONCEPT_REQUIRES(SameType<D, Derived>())>
            detail::conditional_t<(detail::RangeFacade<D>()), const_iterator, const_sentinel>
            end() const
            {
                return {range_core_access::end_impl(derived())};
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
    }
}
#undef REQUIRES
#undef REQUIRES_

#endif
