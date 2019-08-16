/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_INTERSPERSE_HPP
#define RANGES_V3_VIEW_INTERSPERSE_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng>
    struct intersperse_view
      : view_adaptor<intersperse_view<Rng>, Rng,
                     (range_cardinality<Rng>::value > 0)
                         ? static_cast<cardinality>(range_cardinality<Rng>::value * 2 - 1)
                         : range_cardinality<Rng>::value>
    {
        intersperse_view() = default;
        constexpr intersperse_view(Rng rng, range_value_t<Rng> val)
          : intersperse_view::view_adaptor{detail::move(rng)}
          , val_(detail::move(val))
        {}
        CPP_member
        constexpr auto CPP_fun(size)()(const requires sized_range<Rng const>)
        {
            auto const n = ranges::size(this->base());
            return n ? n * 2 - 1 : 0;
        }
        CPP_member
        constexpr auto CPP_fun(size)()(requires sized_range<Rng>)
        {
            auto const n = ranges::size(this->base());
            return n ? n * 2 - 1 : 0;
        }

    private:
        friend range_access;
        template<bool Const>
        struct cursor_adaptor : adaptor_base
        {
        private:
            friend struct cursor_adaptor<!Const>;
            using CRng = meta::const_if_c<Const, Rng>;
            bool toggle_ = false;
            range_value_t<Rng> val_;

        public:
            cursor_adaptor() = default;
            explicit constexpr cursor_adaptor(range_value_t<Rng> const & val)
              : val_{val}
            {}
            CPP_template(bool Other)( //
                requires Const && (!Other)) cursor_adaptor(cursor_adaptor<Other> that)
              : toggle_(that.toggle_)
              , val_(std::move(that.val_))
            {}
            template<typename View>
            constexpr iterator_t<CRng> begin(View & view)
            {
                auto first = ranges::begin(view.base());
                toggle_ = first != ranges::end(view.base());
                return first;
            }
            constexpr range_value_t<Rng> read(iterator_t<CRng> const & it) const
            {
                return toggle_ ? *it : val_;
            }
            CPP_member
            constexpr auto equal(iterator_t<CRng> const & it0,
                                 iterator_t<CRng> const & it1,
                                 cursor_adaptor const & other) const -> CPP_ret(bool)( //
                requires sentinel_for<iterator_t<CRng>, iterator_t<CRng>>)
            {
                return it0 == it1 && toggle_ == other.toggle_;
            }
            constexpr void next(iterator_t<CRng> & it)
            {
                if(toggle_)
                    ++it;
                toggle_ = !toggle_;
            }
            CPP_member
            constexpr auto prev(iterator_t<CRng> & it) -> CPP_ret(void)( //
                requires bidirectional_range<CRng>)
            {
                toggle_ = !toggle_;
                if(toggle_)
                    --it;
            }
            CPP_member
            constexpr auto distance_to(iterator_t<CRng> const & it,
                                       iterator_t<CRng> const & other_it,
                                       cursor_adaptor const & other) const
                -> CPP_ret(range_difference_t<Rng>)( //
                    requires sized_sentinel_for<iterator_t<CRng>, iterator_t<CRng>>)
            {
                return (other_it - it) * 2 + (other.toggle_ - toggle_);
            }
            CPP_member
            constexpr auto advance(iterator_t<CRng> & it,
                                   range_difference_t<CRng> n) -> CPP_ret(void)( //
                requires random_access_range<CRng>)
            {
                ranges::advance(it, n >= 0 ? (n + toggle_) / 2 : (n - !toggle_) / 2);
                if(n % 2 != 0)
                    toggle_ = !toggle_;
            }
        };
        template<bool Const>
        struct sentinel_adaptor : adaptor_base
        {
        private:
            using CRng = meta::const_if_c<Const, Rng>;

        public:
            sentinel_adaptor() = default;
            CPP_template(bool Other)( //
                requires Const && (!Other)) sentinel_adaptor(sentinel_adaptor<Other>)
            {}
            static constexpr bool empty(iterator_t<CRng> const & it,
                                        cursor_adaptor<Const> const &,
                                        sentinel_t<CRng> const & sent)
            {
                return it == sent;
            }
        };
        constexpr auto begin_adaptor() -> cursor_adaptor<false>
        {
            return cursor_adaptor<false>{val_};
        }
        CPP_member
        constexpr auto begin_adaptor() const -> CPP_ret(cursor_adaptor<true>)( //
            requires range<Rng const>)
        {
            return cursor_adaptor<true>{val_};
        }
        CPP_member
        constexpr auto end_adaptor() -> CPP_ret(cursor_adaptor<false>)( //
            requires common_range<Rng> && (!single_pass_iterator_<iterator_t<Rng>>))
        {
            return cursor_adaptor<false>{val_};
        }
        CPP_member
        constexpr auto end_adaptor() noexcept -> CPP_ret(sentinel_adaptor<false>)( //
            requires(!common_range<Rng>) || single_pass_iterator_<iterator_t<Rng>>)
        {
            return {};
        }
        template<bool Const = true>
        constexpr auto end_adaptor() const -> CPP_ret(cursor_adaptor<Const>)( //
            requires Const && range<meta::const_if_c<Const, Rng>> &&
                common_range<meta::const_if_c<Const, Rng>> &&
            (!single_pass_iterator_<iterator_t<meta::const_if_c<Const, Rng>>>))
        {
            return cursor_adaptor<true>{val_};
        }
        template<bool Const = true>
        constexpr auto end_adaptor() const noexcept
            -> CPP_ret(sentinel_adaptor<Const>)( //
                requires Const && range<meta::const_if_c<Const, Rng>> &&
                (!common_range<meta::const_if_c<Const, Rng>> ||
                 single_pass_iterator_<iterator_t<meta::const_if_c<Const, Rng>>>))
        {
            return {};
        }

        range_value_t<Rng> val_;
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    template<typename Rng>
    intersperse_view(Rng &&, range_value_t<Rng>)->intersperse_view<views::all_t<Rng>>;
#endif

    namespace views
    {
        struct intersperse_fn
        {
        private:
            friend view_access;
            template<typename T>
            static constexpr auto CPP_fun(bind)(intersperse_fn intersperse, T t)( //
                requires copyable<T>)
            {
                return make_pipeable(bind_back(intersperse, std::move(t)));
            }

        public:
            template<typename Rng>
            constexpr auto operator()(Rng && rng, range_value_t<Rng> val) const
                -> CPP_ret(intersperse_view<all_t<Rng>>)( //
                    requires viewable_range<Rng> && input_range<Rng> &&
                        convertible_to<range_reference_t<Rng>, range_value_t<Rng>> &&
                            semiregular<range_value_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(val)};
            }
        };

        /// \relates intersperse_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<intersperse_fn>, intersperse)
    } // namespace views
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::intersperse_view)

#endif
