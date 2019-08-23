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

#ifndef RANGES_V3_DETAIL_RANGE_ACCESS_HPP
#define RANGES_V3_DETAIL_RANGE_ACCESS_HPP

#include <cstddef>
#include <utility>

#include <meta/meta.hpp>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/iterator/concepts.hpp>

namespace ranges
{
    /// \addtogroup group-views
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
        static std::false_type contiguous_2_(long);
        template<typename T>
        static typename T::contiguous contiguous_2_(int);

        template<typename T>
        struct contiguous_
        {
            using type = decltype(range_access::contiguous_2_<T>(42));
        };

        template<typename T>
        static basic_mixin<T> mixin_base_2_(long);
        template<typename T>
        static typename T::mixin mixin_base_2_(int);

        template<typename Cur>
        struct mixin_base_
        {
            using type = decltype(range_access::mixin_base_2_<Cur>(42));
        };

    public:
        template<typename Cur>
        using single_pass_t = meta::_t<single_pass_<Cur>>;

        template<typename Cur>
        using contiguous_t = meta::_t<contiguous_<Cur>>;

        template<typename Cur>
        using mixin_base_t = meta::_t<mixin_base_<Cur>>;

        // clang-format off
        template<typename Rng>
        static constexpr auto CPP_auto_fun(begin_cursor)(Rng &rng)
        (
            return rng.begin_cursor()
        )
        template<typename Rng>
        static constexpr auto CPP_auto_fun(end_cursor)(Rng &rng)
        (
            return rng.end_cursor()
        )

        template<typename Rng>
        static constexpr auto CPP_auto_fun(begin_adaptor)(Rng &rng)
        (
            return rng.begin_adaptor()
        )
        template<typename Rng>
        static constexpr auto CPP_auto_fun(end_adaptor)(Rng &rng)
        (
            return rng.end_adaptor()
        )

        template<typename Cur>
        static constexpr auto CPP_auto_fun(read)(Cur const &pos)
        (
            return pos.read()
        )
        template<typename Cur>
        static constexpr auto CPP_auto_fun(arrow)(Cur const &pos)
        (
            return pos.arrow()
        )
        template<typename Cur>
        static constexpr auto CPP_auto_fun(move)(Cur const &pos)
        (
            return pos.move()
        )
        template<typename Cur, typename T>
        static constexpr auto CPP_auto_fun(write)(Cur &pos, T &&t)
        (
            return pos.write((T &&) t)
        )
        template<typename Cur>
        static constexpr auto CPP_auto_fun(next)(Cur & pos)
        (
            return pos.next()
        )
        template<typename Cur, typename O>
        static constexpr auto CPP_auto_fun(equal)(Cur const &pos, O const &other)
        (
            return pos.equal(other)
        )
        template<typename Cur>
        static constexpr auto CPP_auto_fun(prev)(Cur & pos)
        (
            return pos.prev()
        )
        template<typename Cur, typename D>
        static constexpr auto CPP_auto_fun(advance)(Cur & pos, D n)
        (
            return pos.advance(n)
        )
        template<typename Cur, typename O>
        static constexpr auto CPP_auto_fun(distance_to)(Cur const &pos, O const &other)
        (
            return pos.distance_to(other)
        )

    private:
        template<typename Cur>
        using sized_cursor_difference_t = decltype(
            range_access::distance_to(std::declval<Cur>(), std::declval<Cur>()));
        // clang-format on

        template<typename T>
        static std::ptrdiff_t cursor_difference_2_(detail::ignore_t);
        template<typename T>
        static sized_cursor_difference_t<T> cursor_difference_2_(long);
        template<typename T>
        static typename T::difference_type cursor_difference_2_(int);

        template<typename T>
        using cursor_reference_t = decltype(std::declval<T const &>().read());

        template<typename T>
        static meta::id<uncvref_t<cursor_reference_t<T>>> cursor_value_2_(long);
        template<typename T>
        static meta::id<typename T::value_type> cursor_value_2_(int);

#ifdef RANGES_WORKAROUND_CWG_1554
        template<typename Cur>
        struct cursor_difference
        {
            using type = decltype(range_access::cursor_difference_2_<Cur>(42));
        };

        template<typename Cur>
        struct cursor_value : decltype(range_access::cursor_value_2_<Cur>(42))
        {};
#endif // RANGES_WORKAROUND_CWG_1554
    public:
#ifdef RANGES_WORKAROUND_CWG_1554
        template<typename Cur>
        using cursor_difference_t = meta::_t<cursor_difference<Cur>>;

        template<typename Cur>
        using cursor_value_t = meta::_t<cursor_value<Cur>>;
#else  // ^^^ workaround ^^^ / vvv no workaround vvv
        template<typename Cur>
        using cursor_difference_t = decltype(range_access::cursor_difference_2_<Cur>(42));

        template<typename Cur>
        using cursor_value_t = meta::_t<decltype(range_access::cursor_value_2_<Cur>(42))>;
#endif // RANGES_WORKAROUND_CWG_1554

        template<typename Cur>
        static constexpr Cur & pos(basic_iterator<Cur> & it) noexcept
        {
            return it.pos();
        }
        template<typename Cur>
        static constexpr Cur const & pos(basic_iterator<Cur> const & it) noexcept
        {
            return it.pos();
        }
        template<typename Cur>
        static constexpr Cur && pos(basic_iterator<Cur> && it) noexcept
        {
            return detail::move(it.pos());
        }

        template<typename Cur>
        static constexpr Cur cursor(basic_iterator<Cur> it)
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
        // clang-format off
        //
        CPP_def
        (
            template(typename T)
            concept cursor,
                semiregular<T> && semiregular<range_access::mixin_base_t<T>> &&
                constructible_from<range_access::mixin_base_t<T>, T> &&
                constructible_from<range_access::mixin_base_t<T>, T const &>
                // Axiom: mixin_base_t<T> has a member get(), accessible to derived classes,
                //   which perfectly-returns the contained cursor object and does not throw
                //   exceptions.
        );
        CPP_def
        (
            template(typename T)
            concept has_cursor_next,
                requires (T &t)
                (
                    range_access::next(t)
                )
        );
        CPP_def
        (
            template(typename S, typename C)
            concept sentinel_for_cursor,
                requires (S &s, C &c)
                (
                    range_access::equal(c, s),
                    concepts::requires_<convertible_to<decltype(
                        range_access::equal(c, s)), bool>>
                ) &&
                semiregular<S> && cursor<C>
        );
        CPP_def
        (
            template(typename T)
            concept readable_cursor,
                requires (T &t)
                (
                    range_access::read(t)
                )
        );
        CPP_def
        (
            template(typename T)
            concept has_cursor_arrow,
                requires (T const &t)
                (
                    range_access::arrow(t)
                )
        );
        CPP_def
        (
            template(typename T, typename U)
            concept writable_cursor,
                requires (T &t, U &&u)
                (
                    range_access::write(t, (U &&) u)
                )
        );
        CPP_def
        (
            template(typename S, typename C)
            concept sized_sentinel_for_cursor,
                requires (S &s, C &c)
                (
                    range_access::distance_to(c, s),
                    concepts::requires_<signed_integer_like_<decltype(
                        range_access::distance_to(c, s))>>
                ) &&
                sentinel_for_cursor<S, C>
        );
        CPP_def
        (
            template(typename T, typename U)
            concept output_cursor,
                writable_cursor<T, U> && cursor<T>
        );
        CPP_def
        (
            template(typename T)
            concept input_cursor,
                readable_cursor<T> && cursor<T> && has_cursor_next<T>
        );
        CPP_def
        (
            template(typename T)
            concept forward_cursor,
                input_cursor<T> && sentinel_for_cursor<T, T> &&
                !range_access::single_pass_t<uncvref_t<T>>::value
        );
        CPP_def
        (
            template(typename T)
            concept bidirectional_cursor,
                requires (T &t)
                (
                    range_access::prev(t)
                ) &&
                forward_cursor<T>
        );
        CPP_def
        (
            template(typename T)
            concept random_access_cursor,
                requires (T &t)
                (
                    range_access::advance(t, range_access::distance_to(t, t))
                ) &&
                bidirectional_cursor<T> && sized_sentinel_for_cursor<T, T>
        );
        CPP_def
        (
            template(typename T)
            concept contiguous_cursor,
                requires (T &t)
                (
                    concepts::requires_<std::is_lvalue_reference<decltype(range_access::read(t))>::value>
                ) &&
                random_access_cursor<T> &&
                range_access::contiguous_t<uncvref_t<T>>::value
        );
        // clang-format on

        using cursor_tag = concepts::tag<cursor_concept>;

        using input_cursor_tag = concepts::tag<input_cursor_concept, cursor_tag>;

        using forward_cursor_tag =
            concepts::tag<forward_cursor_concept, input_cursor_tag>;

        using bidirectional_cursor_tag =
            concepts::tag<bidirectional_cursor_concept, forward_cursor_tag>;

        using random_access_cursor_tag =
            concepts::tag<random_access_cursor_concept, bidirectional_cursor_tag>;

        using contiguous_cursor_tag =
            concepts::tag<contiguous_cursor_concept, random_access_cursor_tag>;

        template<typename T>
        using cursor_tag_of = concepts::tag_of<meta::list<contiguous_cursor_concept,    //
                                                          random_access_cursor_concept, //
                                                          bidirectional_cursor_concept, //
                                                          forward_cursor_concept,       //
                                                          input_cursor_concept,         //
                                                          cursor_concept>,
                                               T>;

        template<typename Cur, bool IsReadable>
        struct is_writable_cursor_ : std::true_type
        {};

        template<typename Cur>
        struct is_writable_cursor_<Cur, true>
          : meta::bool_<(bool)writable_cursor<Cur, range_access::cursor_value_t<Cur>>>
        {};

        template<typename Cur>
        struct is_writable_cursor
          : detail::is_writable_cursor_<Cur, (bool)readable_cursor<Cur>>
        {};
    } // namespace detail
    /// \endcond
} // namespace ranges

#endif
