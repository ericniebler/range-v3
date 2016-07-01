/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>

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
            Rng rng_;
            Regex rex_;
            SubMatchRange subs_;
            std::regex_constants::match_flag_type flags_;
        public:
            using iterator =
                std::regex_token_iterator<range_iterator_t<Rng>>;

            tokenize_view() = default;
            tokenize_view(Rng rng, Regex && rex, SubMatchRange subs,
                std::regex_constants::match_flag_type flags)
              : rng_(std::move(rng))
              , rex_(std::forward<Regex>(rex))
              , subs_(std::move(subs))
              , flags_(flags)
            {}
            iterator begin()
            {
                return {ranges::begin(rng_), ranges::end(rng_), rex_, subs_, flags_};
            }
            CONCEPT_REQUIRES(View<Rng const>())
            iterator begin() const
            {
                return {ranges::begin(rng_), ranges::end(rng_), rex_, subs_, flags_};
            }
            iterator end() const
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
                tokenize_view<all_t<Rng>, Regex, int>
                operator()(Rng && rng, Regex && rex, int sub = 0,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) const
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    CONCEPT_ASSERT(BoundedRange<Rng>());
                    static_assert(std::is_same<range_value_t<Rng>,
                        typename std::remove_reference<Regex>::type::value_type>::value,
                        "The character range and the regex have different character types");
                    return {all(std::forward<Rng>(rng)), std::forward<Regex>(rex), sub,
                            flags};
                }

                template<typename Rng, typename Regex>
                tokenize_view<all_t<Rng>, Regex, std::vector<int>>
                operator()(Rng && rng, Regex && rex, std::vector<int> subs,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) const
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    CONCEPT_ASSERT(BoundedRange<Rng>());
                    static_assert(std::is_same<range_value_t<Rng>,
                        typename std::remove_reference<Regex>::type::value_type>::value,
                        "The character range and the regex have different character types");
                    return {all(std::forward<Rng>(rng)), std::forward<Regex>(rex),
                            std::move(subs), flags};
                }

                template<typename Rng, typename Regex>
                tokenize_view<all_t<Rng>, Regex, std::initializer_list<int>>
                operator()(Rng && rng, Regex && rex,
                    std::initializer_list<int> subs, std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) const
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    CONCEPT_ASSERT(BoundedRange<Rng>());
                    static_assert(std::is_same<range_value_t<Rng>,
                        typename std::remove_reference<Regex>::type::value_type>::value,
                        "The character range and the regex have different character types");
                    return {all(std::forward<Rng>(rng)), std::forward<Regex>(rex),
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
                    -> decltype(base()(std::forward<Args>(args)...))
                {
                    return base()(std::forward<Args>(args)...);
                }

                template<typename Arg0, typename ...Args>
                auto operator()(Arg0 && arg0, std::initializer_list<int> subs,
                    Args &&...args) const
                    -> decltype(base()(std::forward<Arg0>(arg0), std::move(subs),
                                       std::forward<Args>(args)...))
                {
                    return base()(std::forward<Arg0>(arg0), std::move(subs),
                                  std::forward<Args>(args)...);
                }

                template<typename Arg0, typename Arg1, typename ...Args>
                auto operator()(Arg0 && arg0, Arg1 && arg1, std::initializer_list<int> subs,
                    Args &&...args) const
                    -> decltype(base()(std::forward<Arg0>(arg0), std::forward<Arg1>(arg1),
                                       std::move(subs), std::forward<Args>(args)...))
                {
                    return base()(std::forward<Arg0>(arg0), std::forward<Arg1>(arg1),
                                  std::move(subs), std::forward<Args>(args)...);
                }
            };

            /// \relates tokenize_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(tokenize_fn, tokenize)
        }
        /// @}
    }
}

#endif
