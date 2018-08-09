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

#ifndef RANGES_V3_RANGE_ACCESS_HPP
#define RANGES_V3_RANGE_ACCESS_HPP

#include <cstddef>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{
        struct range_access
        {
            /// \cond
        private:
            template<typename T>
            static std::false_type single_pass_2_(long);
            template<typename T>
            static typename T::single_pass single_pass_2_(int);

            template<typename T>
            struct single_pass_
            {
                using type = decltype(range_access::single_pass_2_<T>(42));
            };

            template<typename T>
            static meta::id<basic_mixin<T>> mixin_base_2_(long);
            template<typename T>
            static meta::id<typename T::mixin> mixin_base_2_(int);

            template<typename Cur>
            struct mixin_base_
              : decltype(range_access::mixin_base_2_<Cur>(42))
            {};

        public:
            template<typename Cur>
            using single_pass_t = meta::_t<single_pass_<Cur>>;

            template<typename Cur>
            using mixin_base_t = meta::_t<mixin_base_<Cur>>;

            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_cursor(Rng &rng, long)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                rng.begin_cursor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_cursor(Rng &rng, int)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                static_cast<Rng const &>(rng).begin_cursor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_cursor(Rng &rng, long)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                rng.end_cursor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_cursor(Rng &rng, int)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                static_cast<Rng const &>(rng).end_cursor()
            )

            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_adaptor(Rng &rng, long)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                rng.begin_adaptor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto begin_adaptor(Rng &rng, int)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                static_cast<Rng const &>(rng).begin_adaptor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_adaptor(Rng &rng, long)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                rng.end_adaptor()
            )
            template<typename Rng>
            static RANGES_CXX14_CONSTEXPR auto end_adaptor(Rng &rng, int)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                static_cast<Rng const &>(rng).end_adaptor()
            )

            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto read(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.read()
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto arrow(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.arrow()
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto move(Cur const &pos)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.move()
            )
            template<typename Cur, typename T>
            static RANGES_CXX14_CONSTEXPR auto write(Cur &pos, T &&t)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                pos.write((T &&) t)
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto next(Cur & pos)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.next()
            )
            template<typename Cur, typename O>
            static RANGES_CXX14_CONSTEXPR auto equal(Cur const &pos, O const &other)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.equal(other)
            )
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR auto prev(Cur & pos)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.prev()
            )
            template<typename Cur, typename D>
            static RANGES_CXX14_CONSTEXPR auto advance(Cur & pos, D n)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.advance(n)
            )
            template<typename Cur, typename O>
            static RANGES_CXX14_CONSTEXPR auto distance_to(Cur const &pos, O const &other)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                pos.distance_to(other)
            )

        private:
            template<typename Cur>
            using sized_cursor_difference_t =
                decltype(range_access::distance_to(std::declval<Cur>(), std::declval<Cur>()));

            template<typename T>
            static std::ptrdiff_t cursor_difference_2_(detail::any);
            template<typename T>
            static sized_cursor_difference_t<T> cursor_difference_2_(long);
            template<typename T>
            static typename T::difference_type cursor_difference_2_(int);

            template<typename Cur>
            struct cursor_difference
            {
                using type = decltype(range_access::cursor_difference_2_<Cur>(42));
            };

            template<typename T>
            using cursor_reference_t = decltype(std::declval<T const &>().read());

            template<typename T>
            static meta::id<uncvref_t<cursor_reference_t<T>>> cursor_value_2_(long);
            template<typename T>
            static meta::id<typename T::value_type> cursor_value_2_(int);

            template<typename Cur>
            struct cursor_value
              : decltype(range_access::cursor_value_2_<Cur>(42))
            {};

        public:
            template<typename Cur>
            using cursor_difference_t = typename cursor_difference<Cur>::type;

            template<typename Cur>
            using cursor_value_t = typename cursor_value<Cur>::type;

            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR Cur &pos(basic_iterator<Cur> &it) noexcept
            {
                return it.pos();
            }
            template<typename Cur>
            static constexpr Cur const &pos(basic_iterator<Cur> const &it) noexcept
            {
                return it.pos();
            }
            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR Cur &&pos(basic_iterator<Cur> &&it) noexcept
            {
                return detail::move(it.pos());
            }

            template<typename Cur>
            static RANGES_CXX14_CONSTEXPR Cur cursor(basic_iterator<Cur> it)
            {
                return std::move(it.pos());
            }
            /// endcond
        };
        /// @}

        /// \cond
        namespace detail
        {
            //
            // Concepts that the range cursor must model
            //
            CONCEPT_def
            (
                template(typename T)
                concept Cursor,
                    Semiregular<T> && Semiregular<range_access::mixin_base_t<T>> &&
                    Constructible<range_access::mixin_base_t<T>, T> &&
                    Constructible<range_access::mixin_base_t<T>, T const &>
                    // Axiom: mixin_base_t<T> has a member get(), accessible to derived classes,
                    //   which perfectly-returns the contained cursor object and does not throw
                    //   exceptions.
            );
            CONCEPT_def
            (
                template(typename T)
                concept HasCursorNext,
                    requires (T &t)
                    (
                        range_access::next(t)
                    )
            );
            CONCEPT_def
            (
                template(typename S, typename C)
                concept CursorSentinel,
                    requires (S &s, C &c)
                    (
                        range_access::equal(c, s),
                        concepts::requires_<ConvertibleTo<decltype(
                            range_access::equal(c, s)), bool>>
                    ) &&
                    Semiregular<S> && Cursor<C>
            );
            CONCEPT_def
            (
                template(typename T)
                concept ReadableCursor,
                    requires (T &t)
                    (
                        range_access::read(t)
                    )
            );
            CONCEPT_def
            (
                template(typename T)
                concept HasCursorArrow,
                    requires (T const &t)
                    (
                        range_access::arrow(t)
                    )
            );
            CONCEPT_def
            (
                template(typename T, typename U)
                concept WritableCursor,
                    requires (T &t, U &&u)
                    (
                        range_access::write(t, (U &&) u)
                    )
            );
            CONCEPT_def
            (
                template(typename S, typename C)
                concept SizedCursorSentinel,
                    requires (S &s, C &c)
                    (
                        range_access::distance_to(c, s),
                        concepts::requires_<SignedIntegral<decltype(
                            range_access::distance_to(c, s))>>
                    ) &&
                    CursorSentinel<S, C>
            );
            CONCEPT_def
            (
                template(typename T, typename U)
                concept OutputCursor,
                    WritableCursor<T, U> && Cursor<T>
            );
            CONCEPT_def
            (
                template(typename T)
                concept InputCursor,
                    ReadableCursor<T> && Cursor<T> && HasCursorNext<T>
            );
            CONCEPT_def
            (
                template(typename T)
                concept ForwardCursor,
                    InputCursor<T> && CursorSentinel<T, T> &&
                    !range_access::single_pass_t<uncvref_t<T>>::value
            );
            CONCEPT_def
            (
                template(typename T)
                concept BidirectionalCursor,
                    requires (T &t)
                    (
                        range_access::prev(t)
                    ) &&
                    ForwardCursor<T>
            );
            CONCEPT_def
            (
                template(typename T)
                concept RandomAccessCursor,
                    requires (T &t)
                    (
                        range_access::advance(t, range_access::distance_to(t, t))
                    ) &&
                    BidirectionalCursor<T> && SizedCursorSentinel<T, T>
            );
            CONCEPT_def
            (
                template(typename T)
                concept InfiniteCursor,
                    T::is_infinite::value
            );

            using cursor_tag =
                concepts::tag<CursorConcept>;

            using input_cursor_tag =
                concepts::tag<InputCursorConcept, cursor_tag>;

            using forward_cursor_tag =
                concepts::tag<ForwardCursorConcept, input_cursor_tag>;

            using bidirectional_cursor_tag =
                concepts::tag<BidirectionalCursorConcept, forward_cursor_tag>;

            using random_access_cursor_tag =
                concepts::tag<RandomAccessCursorConcept, bidirectional_cursor_tag>;

            template<typename T>
            using cursor_tag_of =
                concepts::tag_of<
                    meta::list<
                        RandomAccessCursorConcept,
                        BidirectionalCursorConcept,
                        ForwardCursorConcept,
                        InputCursorConcept,
                        CursorConcept>,
                    T>;

            template<typename Cur, bool Readable = (bool) ReadableCursor<Cur>>
            struct is_writable_cursor_
              : std::true_type
            {};

            template<typename Cur>
            struct is_writable_cursor_<Cur, true>
              : meta::bool_<(bool) WritableCursor<Cur, range_access::cursor_value_t<Cur>>>
            {};

            template<typename Cur>
            struct is_writable_cursor
              : detail::is_writable_cursor_<Cur>
            {};
        }
        /// \endcond
    }
}

#endif
