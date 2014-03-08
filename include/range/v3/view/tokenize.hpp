// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_TOKENIZE_HPP
#define RANGES_V3_VIEW_TOKENIZE_HPP

#include <regex>
#include <vector>
#include <utility>
#include <type_traits>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/range_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename BidirectionalRange, typename Regex, typename SubMatchRange>
        struct tokenize_range_view : private range_base
        {
        private:
            BidirectionalRange rng_;
            Regex rex_;
            SubMatchRange subs_;
            std::regex_constants::match_flag_type flags_;
        public:
            using iterator =
                std::regex_token_iterator<range_iterator_t<BidirectionalRange>>;
            using const_iterator =
                std::regex_token_iterator<range_iterator_t<BidirectionalRange const>>;

            tokenize_range_view(BidirectionalRange &&rng, Regex && rex, SubMatchRange subs,
                std::regex_constants::match_flag_type flags)
              : rng_(std::forward<BidirectionalRange>(rng))
              , rex_(std::forward<Regex>(rex)), subs_(std::move(subs)), flags_(flags)
            {}
            iterator begin()
            {
                return {ranges::begin(rng_), ranges::end(rng_), rex_, subs_, flags_};
            }
            iterator end()
            {
                return {};
            }
            const_iterator begin() const
            {
                return {ranges::begin(rng_), ranges::end(rng_), rex_, subs_, flags_};
            }
            const_iterator end() const
            {
                return {};
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            BidirectionalRange & base()
            {
                return rng_;
            }
            BidirectionalRange const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct tokenizer_ : bindable<tokenizer_>
            {
                template<typename BidirectionalRange, typename Regex>
                static tokenize_range_view<BidirectionalRange, Regex, int>
                invoke(tokenizer_, BidirectionalRange && rng, Regex && rex, int sub = 0,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default)
                {
                    CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                    static_assert(std::is_same<range_value_t<BidirectionalRange>,
                        typename std::remove_reference<Regex>::type::value_type>::value,
                        "The character range and the regex have different character types");
                    return {std::forward<BidirectionalRange>(rng), std::forward<Regex>(rex), sub,
                            flags};
                }

                template<typename BidirectionalRange, typename Regex>
                static tokenize_range_view<BidirectionalRange, Regex, std::vector<int>>
                invoke(tokenizer_, BidirectionalRange && rng, Regex && rex, std::vector<int> subs,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default)
                {
                    CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                    static_assert(std::is_same<range_value_t<BidirectionalRange>,
                        typename std::remove_reference<Regex>::type::value_type>::value,
                        "The character range and the regex have different character types");
                    return {std::forward<BidirectionalRange>(rng), std::forward<Regex>(rex),
                            std::move(subs), flags};
                }

                template<typename BidirectionalRange, typename Regex>
                static tokenize_range_view<BidirectionalRange, Regex, std::initializer_list<int>>
                invoke(tokenizer_, BidirectionalRange && rng, Regex && rex,
                    std::initializer_list<int> subs, std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default)
                {
                    CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                    static_assert(std::is_same<range_value_t<BidirectionalRange>,
                        typename std::remove_reference<Regex>::type::value_type>::value,
                        "The character range and the regex have different character types");
                    return {std::forward<BidirectionalRange>(rng), std::forward<Regex>(rex),
                            std::move(subs), flags};
                }

                /// \overload
                template<typename Regex>
                static auto invoke(tokenizer_ tokenize, Regex && rex, int sub = 0,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) ->
                    decltype(tokenize.move_bind(std::placeholders::_1, std::forward<Regex>(rex),
                            std::move(sub), std::move(flags)))
                {
                    return tokenize.move_bind(std::placeholders::_1, std::forward<Regex>(rex),
                        std::move(sub), std::move(flags));
                }

                template<typename Regex>
                static auto invoke(tokenizer_ tokenize, Regex && rex, std::vector<int> subs,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) ->
                    decltype(tokenize.move_bind(std::placeholders::_1, std::forward<Regex>(rex),
                        std::move(subs), std::move(flags)))
                {
                    return tokenize.move_bind(std::placeholders::_1, std::forward<Regex>(rex),
                        std::move(subs), std::move(flags));
                }

                template<typename Regex>
                static auto invoke(tokenizer_ tokenize, Regex && rex,
                    std::initializer_list<int> subs, std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default) ->
                    decltype(tokenize.move_bind(std::placeholders::_1, std::forward<Regex>(rex),
                            std::move(subs), std::move(flags)))
                {
                    return tokenize.move_bind(std::placeholders::_1, std::forward<Regex>(rex),
                        std::move(subs), std::move(flags));
                }
            };

            // Damn C++ and its imperfect forwarding of initializer_list.
            struct tokenizer : tokenizer_
            {
            private:
                tokenizer_ const & base() const
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

            RANGES_CONSTEXPR tokenizer tokenize {};
        }
    }
}

#endif
