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

#include <regex>
#include <vector>
#include <utility>
#include <type_traits>
#include <initializer_list>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Regex, typename SubMatchRange>
        struct tokenize_view
          : view_interface<
                tokenize_view<Rng, Regex, SubMatchRange>,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
        {
        private:
            CONCEPT_ASSERT(BidirectionalView<Rng>() && BoundedRange<Rng>());
            CONCEPT_ASSERT(SemiRegular<Regex>());
            CONCEPT_ASSERT(SemiRegular<SubMatchRange>());

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
                meta::const_if_c<simple_view<Rng>(), Rng> &rng = rng_;
                return {ranges::begin(rng), ranges::end(rng), rex_, subs_, flags_};
            }
            template<bool Const = true,
                CONCEPT_REQUIRES_(Range<Rng const>())>
            iterator_t<Const> begin() const
            {
                return {ranges::begin(rng_), ranges::end(rng_), rex_, subs_, flags_};
            }
            iterator_t<simple_view<Rng>()> end()
            {
                return {};
            }
            template<bool Const = true,
                CONCEPT_REQUIRES_(Range<Rng const>())>
            iterator_t<Const> end() const
            {
                return {};
            }
            Rng & base()
            {
                return rng_;
            }
            Rng const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct tokenizer_impl_fn
            {
                template<typename Rng, typename Regex>
                tokenize_view<all_t<Rng>, detail::decay_t<Regex>, int>
                operator()(Rng && rng, Regex && rex, int sub = 0,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) const
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    CONCEPT_ASSERT(BoundedRange<Rng>());
                    static_assert(std::is_same<range_value_type_t<Rng>,
                        typename detail::decay_t<Regex>::value_type>::value,
                        "The character range and the regex have different character types");
                    return {all(static_cast<Rng&&>(rng)), static_cast<Regex&&>(rex), sub,
                            flags};
                }

                template<typename Rng, typename Regex>
                tokenize_view<all_t<Rng>, detail::decay_t<Regex>, std::vector<int>>
                operator()(Rng && rng, Regex && rex, std::vector<int> subs,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) const
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    CONCEPT_ASSERT(BoundedRange<Rng>());
                    static_assert(std::is_same<range_value_type_t<Rng>,
                        typename detail::decay_t<Regex>::value_type>::value,
                        "The character range and the regex have different character types");
                    return {all(static_cast<Rng&&>(rng)), static_cast<Regex&&>(rex),
                            std::move(subs), flags};
                }

                template<typename Rng, typename Regex>
                tokenize_view<all_t<Rng>, detail::decay_t<Regex>, std::initializer_list<int>>
                operator()(Rng && rng, Regex && rex,
                    std::initializer_list<int> subs, std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) const
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    CONCEPT_ASSERT(BoundedRange<Rng>());
                    static_assert(std::is_same<range_value_type_t<Rng>,
                        typename detail::decay_t<Regex>::value_type>::value,
                        "The character range and the regex have different character types");
                    return {all(static_cast<Rng&&>(rng)), static_cast<Regex&&>(rex),
                            std::move(subs), flags};
                }

                template<typename Regex>
                auto operator()(Regex && rex, int sub = 0,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, bind_forward<Regex>(rex),
                        std::move(sub), std::move(flags))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, bind_forward<Regex>(rex),
                        std::move(sub), std::move(flags)));
                }

                template<typename Regex>
                auto operator()(Regex && rex, std::vector<int> subs,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, bind_forward<Regex>(rex),
                        std::move(subs), std::move(flags))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, bind_forward<Regex>(rex),
                        std::move(subs), std::move(flags)));
                }

                template<typename Regex>
                auto operator()(Regex && rex,
                    std::initializer_list<int> subs, std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, bind_forward<Regex>(rex),
                        std::move(subs), std::move(flags))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, bind_forward<Regex>(rex),
                        std::move(subs), std::move(flags)));
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
                template<typename ...Args>
                auto operator()(Args &&...args) const
                    -> decltype(base()(static_cast<Args&&>(args)...))
                {
                    return base()(static_cast<Args&&>(args)...);
                }

                template<typename Arg0, typename ...Args>
                auto operator()(Arg0 && arg0, std::initializer_list<int> subs,
                    Args &&...args) const
                    -> decltype(base()(static_cast<Arg0&&>(arg0), std::move(subs),
                                       static_cast<Args&&>(args)...))
                {
                    return base()(static_cast<Arg0&&>(arg0), std::move(subs),
                                  static_cast<Args&&>(args)...);
                }

                template<typename Arg0, typename Arg1, typename ...Args>
                auto operator()(Arg0 && arg0, Arg1 && arg1, std::initializer_list<int> subs,
                    Args &&...args) const
                    -> decltype(base()(static_cast<Arg0&&>(arg0), static_cast<Arg1&&>(arg1),
                                       std::move(subs), static_cast<Args&&>(args)...))
                {
                    return base()(static_cast<Arg0&&>(arg0), static_cast<Arg1&&>(arg1),
                                  std::move(subs), static_cast<Args&&>(args)...);
                }
            };

            /// \relates tokenize_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(tokenize_fn, tokenize)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::tokenize_view)

#endif
