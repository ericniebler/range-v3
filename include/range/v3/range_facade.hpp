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

namespace ranges
{
    inline namespace v3
    {
        struct public_t
        {};

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
            struct CountedCursorConcept
              : concepts::refines<InputCursorConcept>
            {
                template<typename T>
                using base_iterator_t = decltype(std::declval<T>().base());

                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        t.base(),
                        t.count(),
                        T{public_t{}, t.base(), t.count()}
                        // Doesn't work. How strange.
                        //concepts::model_of<Iterator>(t.base())
                        //concepts::model_of<Integral>(t.count())
                    ));
            };

            struct IterableFacadeConcept
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        t.get_begin(),
                        t.get_end()
                    ));
            };

            struct RangeFacadeConcept
              : concepts::refines<IterableFacadeConcept>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(t.get_begin(), t.get_end())
                    ));
            };

            template<typename Range>
            static auto get_begin(Range & rng) -> decltype(rng.get_begin())
            {
                return rng.get_begin();
            }
            template<typename Range>
            static auto get_end(Range & rng) -> decltype(rng.get_end())
            {
                return rng.get_end();
            }

            template<typename Range>
            static auto begin_adaptor(Range & rng) -> decltype(rng.get_adaptor(begin_tag{}))
            {
                return rng.get_adaptor(begin_tag{});
            }
            template<typename Range>
            static auto end_adaptor(Range & rng) -> decltype(rng.get_adaptor(end_tag{}))
            {
                return rng.get_adaptor(end_tag{});
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
            using single_pass_t = typename single_pass<Cursor>::type;

            template<typename Cursor, typename Sentinel>
            Cursor get_cursor(basic_range_iterator<Cursor, Sentinel> it)
            {
                return std::move(it.pos_);
            }

            template<typename Sentinel>
            Sentinel get_sentinel(basic_range_sentinel<Sentinel> s)
            {
                return std::move(s.end_);
            }
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
            using CountedCursor =
                concepts::models<range_core_access::CountedCursorConcept, T>;

            template<typename T>
            using cursor_concept_t =
                concepts::most_refined_t<range_core_access::RandomAccessCursorConcept, T>;

            template<typename T>
            using IterableFacade =
                concepts::models<range_core_access::IterableFacadeConcept, T>;

            template<typename T>
            using RangeFacade =
                concepts::models<range_core_access::RangeFacadeConcept, T>;

            template<typename T>
            using facade_concept_t = concepts::most_refined_t<range_core_access::RangeFacadeConcept, T>;

            static auto iter_cat(range_core_access::InputCursorConcept) ->
                std::input_iterator_tag;
            static auto iter_cat(range_core_access::ForwardCursorConcept) ->
                std::forward_iterator_tag;
            static auto iter_cat(range_core_access::BidirectionalCursorConcept) ->
                std::bidirectional_iterator_tag;
            static auto iter_cat(range_core_access::RandomAccessCursorConcept) ->
                std::random_access_iterator_tag;

            template<typename Derived>
            using facade_cursor_t =
                decltype(range_core_access::get_begin(std::declval<Derived &>()));

            template<typename Derived>
            using facade_sentinel2_t =
                decltype(range_core_access::get_end(std::declval<Derived &>()));

            template<typename Derived>
            using facade_iterator_t =
                basic_range_iterator<facade_cursor_t<Derived>, facade_sentinel2_t<Derived>>;

            template<typename Derived>
            using facade_sentinel_t =
                conditional_t<
                    (Same<facade_cursor_t<Derived>, facade_sentinel2_t<Derived>>()),
                    basic_range_iterator<facade_cursor_t<Derived>, facade_sentinel2_t<Derived>>,
                    basic_range_sentinel<facade_sentinel2_t<Derived>>>;
        }

        template<typename Sentinel>
        struct basic_range_sentinel
        {
        private:
            friend range_core_access;
            template<typename Iterator, typename OtherSentinel> friend struct basic_range_iterator;
            Sentinel end_;
        public:
            basic_range_sentinel() = default;
            basic_range_sentinel(Sentinel end)
              : end_(std::move(end))
            {}
            template<typename...Ts,
                CONCEPT_REQUIRES_(Constructible<Sentinel, public_t, Ts...>())>
            basic_range_sentinel(Ts &&... ts)
              : end_{public_t{}, std::forward<Ts>(ts)...}
            {}
            constexpr bool operator==(basic_range_sentinel<Sentinel> const &) const
            {
                return true;
            }
            constexpr bool operator!=(basic_range_sentinel<Sentinel> const &) const
            {
                return false;
            }
            template<typename S = Sentinel, CONCEPT_REQUIRES_(Same<S, Sentinel>())>
            auto count() const -> decltype(range_core_access::count(std::declval<S>()))
            {
                return range_core_access::count(end_);
            }
        };

        template<typename Cursor, typename Sentinel>
        struct basic_range_iterator
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
            Cursor pos_;

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
                return range_core_access::equal(pos_, that.pos_);
            }
            constexpr bool equal_(basic_range_iterator const &that,
                std::false_type *) const
            {
                return basic_range_iterator::equal2_(that, (cursor_concept_t *)nullptr);
            }
            constexpr bool equal_(basic_range_iterator const &that,
                std::true_type *) const
            {
                return range_core_access::equal(pos_, that.pos_);
            }
        public:
            using reference =
                decltype(range_core_access::current(std::declval<Cursor const &>()));
            using value_type = detail::uncvref_t<reference>;
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
              : pos_(std::move(pos))
            {}
            template<typename...Ts,
                CONCEPT_REQUIRES_(Constructible<Cursor, public_t, Ts...>())>
            basic_range_iterator(Ts &&... ts)
              : pos_{public_t{}, std::forward<Ts>(ts)...}
            {}
            reference operator*() const
            {
                return range_core_access::current(pos_);
            }
            pointer operator->() const
            {
                return detail::operator_arrow_dispatch<reference>::apply(**this);
            }
            basic_range_iterator& operator++()
            {
                range_core_access::next(pos_);
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
                return range_core_access::empty(left.pos_, right.end_);
            }
            friend constexpr bool operator!=(basic_range_iterator const &left,
                basic_range_sentinel<Sentinel> const &right)
            {
                return !(left == right);
            }
            friend constexpr bool operator==(basic_range_sentinel<Sentinel> const & left,
                basic_range_iterator const &right)
            {
                return range_core_access::empty(right.pos_, left.end_);
            }
            friend constexpr bool operator!=(basic_range_sentinel<Sentinel> const &left,
                basic_range_iterator const &right)
            {
                return !(left == right);
            }
            CONCEPT_REQUIRES(detail::BidirectionalCursor<Cursor>())
            basic_range_iterator& operator--()
            {
                range_core_access::prev(pos_);
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
                range_core_access::advance(pos_, n);
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
                range_core_access::advance(pos_, -n);
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
                return range_core_access::distance_to(right.pos_, pos_);
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
                return !range_core_access::empty(left.pos_, right.end_);
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
                return range_core_access::empty(left.pos_, right.end_);
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
                return range_core_access::empty(right.pos_, left.end_);
            }
            CONCEPT_REQUIRES(detail::RandomAccessCursor<Cursor>())
            friend constexpr bool operator>(basic_range_sentinel<Sentinel> const &left,
                basic_range_iterator const &right)
            {
                return !range_core_access::empty(right.pos_, left.end_);
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
            // For counted_iterator. TODO find a more general way for cursors to inject
            // behaviors into the iterator.
            template<typename C = Cursor,
                CONCEPT_REQUIRES_(Same<C, Cursor>() && detail::CountedCursor<C>())>
            auto base() const -> range_core_access::CountedCursorConcept::base_iterator_t<C>
            {
                return range_core_access::base(pos_);
            }
            CONCEPT_REQUIRES(detail::CountedCursor<Cursor>())
            auto count() const -> difference_type
            {
                return range_core_access::count(pos_);
            }
        };

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
        protected:
            using range_facade_t = range_facade;
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
            friend range_core_access;

            // Default implementations
            Derived get_begin() const
            {
                return derived();
            }
            default_sentinel get_end() const
            {
                return {};
            }
        public:
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_iterator_t<D> begin() const
            {
                return {range_core_access::get_begin(derived())};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_sentinel_t<D> end() const
            {
                return {range_core_access::get_end(derived())};
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

#endif
