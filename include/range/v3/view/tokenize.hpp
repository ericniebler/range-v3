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

#ifndef RANGES_V3_VIEW_TOKENIZE_HPP
#define RANGES_V3_VIEW_TOKENIZE_HPP

#include <initializer_list>
#include <regex>
#include <type_traits>
#include <utility>
#include <vector>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/bind_back.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/interface.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Regex, typename SubMatchRange>
    struct tokenize_view
      : view_interface<tokenize_view<Rng, Regex, SubMatchRange>,
                       is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
    {
    private:
        CPP_assert(bidirectional_range<Rng> && view_<Rng> && common_range<Rng>);
        CPP_assert(semiregular<Regex>);
        CPP_assert(semiregular<SubMatchRange>);

        Rng rng_;
        Regex rex_;
        SubMatchRange subs_;
        std::regex_constants::match_flag_type flags_;
        template<bool Const>
        using iterator_t =
            std::regex_token_iterator<iterator_t<meta::const_if_c<Const, Rng>>>;

    public:
        tokenize_view() = default;
        tokenize_view(Rng rng, Regex rex, SubMatchRange subs,
                      std::regex_constants::match_flag_type flags)
          : rng_(std::move(rng))
          , rex_(std::move(rex))
          , subs_(std::move(subs))
          , flags_(flags)
        {}
        iterator_t<simple_view<Rng>()> begin()
        {
            meta::const_if_c<simple_view<Rng>(), Rng> & rng = rng_;
            return {ranges::begin(rng), ranges::end(rng), rex_, subs_, flags_};
        }
        template<bool Const = true>
        auto begin() const -> CPP_ret(iterator_t<Const>)( //
            requires range<Rng const>)
        {
            return {ranges::begin(rng_), ranges::end(rng_), rex_, subs_, flags_};
        }
        iterator_t<simple_view<Rng>()> end()
        {
            return {};
        }
        template<bool Const = true>
        auto end() const -> CPP_ret(iterator_t<Const>)( //
            requires range<Rng const>)
        {
            return {};
        }
        Rng base() const
        {
            return rng_;
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng, typename Regex, typename SubMatchRange)(
        requires copy_constructible<Regex> && copy_constructible<SubMatchRange>)
        tokenize_view(Rng &&, Regex, SubMatchRange)
            ->tokenize_view<views::all_t<Rng>, Regex, SubMatchRange>;
#endif

    namespace views
    {
        struct tokenizer_impl_fn
        {
            template<typename Rng, typename Regex>
            tokenize_view<all_t<Rng>, detail::decay_t<Regex>, int> operator()(
                Rng && rng, Regex && rex, int sub = 0,
                std::regex_constants::match_flag_type flags =
                    std::regex_constants::match_default) const
            {
                CPP_assert(bidirectional_range<Rng>);
                CPP_assert(common_range<Rng>);
                static_assert(
                    RANGES_IS_SAME(range_value_t<Rng>,
                                   typename detail::decay_t<Regex>::value_type),
                    "The character range and the regex have different character types");
                return {all(static_cast<Rng &&>(rng)),
                        static_cast<Regex &&>(rex),
                        sub,
                        flags};
            }

            template<typename Rng, typename Regex>
            tokenize_view<all_t<Rng>, detail::decay_t<Regex>, std::vector<int>>
            operator()(Rng && rng, Regex && rex, std::vector<int> subs,
                       std::regex_constants::match_flag_type flags =
                           std::regex_constants::match_default) const
            {
                CPP_assert(bidirectional_range<Rng>);
                CPP_assert(common_range<Rng>);
                static_assert(
                    RANGES_IS_SAME(range_value_t<Rng>,
                                   typename detail::decay_t<Regex>::value_type),
                    "The character range and the regex have different character types");
                return {all(static_cast<Rng &&>(rng)),
                        static_cast<Regex &&>(rex),
                        std::move(subs),
                        flags};
            }

            template<typename Rng, typename Regex>
            tokenize_view<all_t<Rng>, detail::decay_t<Regex>, std::initializer_list<int>>
            operator()(Rng && rng, Regex && rex, std::initializer_list<int> subs,
                       std::regex_constants::match_flag_type flags =
                           std::regex_constants::match_default) const
            {
                CPP_assert(bidirectional_range<Rng>);
                CPP_assert(common_range<Rng>);
                static_assert(
                    RANGES_IS_SAME(range_value_t<Rng>,
                                   typename detail::decay_t<Regex>::value_type),
                    "The character range and the regex have different character types");
                return {all(static_cast<Rng &&>(rng)),
                        static_cast<Regex &&>(rex),
                        std::move(subs),
                        flags};
            }

            template<typename Regex>
            auto operator()(Regex && rex, int sub = 0,
                            std::regex_constants::match_flag_type flags =
                                std::regex_constants::match_default) const
            {
                return make_pipeable(
                    bind_back(*this, static_cast<Regex &&>(rex), sub, flags));
            }

            template<typename Regex>
            auto operator()(Regex && rex, std::vector<int> subs,
                            std::regex_constants::match_flag_type flags =
                                std::regex_constants::match_default) const
            {
                return make_pipeable(
                    bind_back(*this, static_cast<Regex &&>(rex), std::move(subs), flags));
            }

            template<typename Regex>
            auto operator()(Regex && rex, std::initializer_list<int> subs,
                            std::regex_constants::match_flag_type flags =
                                std::regex_constants::match_default) const
            {
                return make_pipeable(
                    bind_back(*this, static_cast<Regex &&>(rex), subs, flags));
            }
        };

        // Damn C++ and its imperfect forwarding of initializer_list.
        struct tokenize_fn : tokenizer_impl_fn
        {
        private:
            tokenizer_impl_fn const & base() const
            {
                return *this;
            }

        public:
            template<typename... Args>
            auto operator()(Args &&... args) const
                -> decltype(base()(static_cast<Args &&>(args)...))
            {
                return base()(static_cast<Args &&>(args)...);
            }

            template<typename Arg0, typename... Args>
            auto operator()(Arg0 && arg0, std::initializer_list<int> subs,
                            Args &&... args) const
                -> decltype(base()(static_cast<Arg0 &&>(arg0), std::move(subs),
                                   static_cast<Args &&>(args)...))
            {
                return base()(static_cast<Arg0 &&>(arg0),
                              std::move(subs),
                              static_cast<Args &&>(args)...);
            }

            template<typename Arg0, typename Arg1, typename... Args>
            auto operator()(Arg0 && arg0, Arg1 && arg1, std::initializer_list<int> subs,
                            Args &&... args) const
                -> decltype(base()(static_cast<Arg0 &&>(arg0), static_cast<Arg1 &&>(arg1),
                                   std::move(subs), static_cast<Args &&>(args)...))
            {
                return base()(static_cast<Arg0 &&>(arg0),
                              static_cast<Arg1 &&>(arg1),
                              std::move(subs),
                              static_cast<Args &&>(args)...);
            }
        };

        /// \relates tokenize_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(tokenize_fn, tokenize)
    } // namespace views
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::tokenize_view)

#endif
