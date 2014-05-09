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
        template<typename Rng, typename Regex, typename SubMatchRange>
        struct tokenized_view : private range_base
        {
        private:
            detail::base_iterable_holder<Rng> rng_;
            Regex rex_;
            SubMatchRange subs_;
            std::regex_constants::match_flag_type flags_;
        public:
            using iterator =
                std::regex_token_iterator<range_iterator_t<Rng>>;

            tokenized_view() = default;
            tokenized_view(Rng &&rng, Regex && rex, SubMatchRange subs,
                std::regex_constants::match_flag_type flags)
              : rng_(std::forward<Rng>(rng))
              , rex_(std::forward<Regex>(rex)), subs_(std::move(subs)), flags_(flags)
            {}
            iterator begin()
            {
                return {begin(rng_.get()), end(rng_.get()), rex_, subs_, flags_};
            }
            CONCEPT_REQUIRES(Iterable<Rng const>())
            iterator begin() const
            {
                return {begin(rng_.get()), end(rng_.get()), rex_, subs_, flags_};
            }
            iterator end() const
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
            Rng & base()
            {
                return rng_.get();
            }
            Rng const & base() const
            {
                return rng_.get();
            }
        };

        namespace view
        {
            struct tokenizer_ : bindable<tokenizer_>
            {
                template<typename Rng, typename Regex>
                static tokenized_view<Rng, Regex, int>
                invoke(tokenizer_, Rng && rng, Regex && rex, int sub = 0,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default)
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    static_assert(std::is_same<range_value_t<Rng>,
                        typename std::remove_reference<Regex>::type::value_type>::value,
                        "The character range and the regex have different character types");
                    return {std::forward<Rng>(rng), std::forward<Regex>(rex), sub,
                            flags};
                }

                template<typename Rng, typename Regex>
                static tokenized_view<Rng, Regex, std::vector<int>>
                invoke(tokenizer_, Rng && rng, Regex && rex, std::vector<int> subs,
                    std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default)
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    static_assert(std::is_same<range_value_t<Rng>,
                        typename std::remove_reference<Regex>::type::value_type>::value,
                        "The character range and the regex have different character types");
                    return {std::forward<Rng>(rng), std::forward<Regex>(rex),
                            std::move(subs), flags};
                }

                template<typename Rng, typename Regex>
                static tokenized_view<Rng, Regex, std::initializer_list<int>>
                invoke(tokenizer_, Rng && rng, Regex && rex,
                    std::initializer_list<int> subs, std::regex_constants::match_flag_type flags =
                        std::regex_constants::match_default)
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    static_assert(std::is_same<range_value_t<Rng>,
                        typename std::remove_reference<Regex>::type::value_type>::value,
                        "The character range and the regex have different character types");
                    return {std::forward<Rng>(rng), std::forward<Regex>(rex),
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
            struct tokenize_fn : tokenizer_
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

            RANGES_CONSTEXPR tokenize_fn tokenize {};
        }
    }
}

#endif
