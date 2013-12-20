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

#ifndef RANGES_V3_VIEW_TRANSFORM_HPP
#define RANGES_V3_VIEW_TRANSFORM_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputRange, typename UnaryFunction>
        struct transform_range_view
        {
        private:
            compressed_pair<InputRange, invokable_t<UnaryFunction>> rng_and_fun_;

            template<typename TfxRng>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<TfxRng>
                  , typename std::remove_reference<
                        decltype(std::declval<TfxRng &>().rng_and_fun_.second()(
                            *ranges::begin(std::declval<TfxRng &>().rng_and_fun_.first())))
                    >::type
                  , range_category_t<InputRange>
                  , decltype(std::declval<TfxRng &>().rng_and_fun_.second()(
                        *ranges::begin(std::declval<TfxRng &>().rng_and_fun_.first())))
                  , range_difference_t<InputRange>
                >
            {
            private:
                friend struct transform_range_view;
                friend struct ranges::iterator_core_access;
                using base_range_iterator =
                    decltype(ranges::begin(std::declval<TfxRng &>().rng_and_fun_.first()));

                TfxRng *rng_;
                base_range_iterator it_;

                basic_iterator(TfxRng &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
                {}
                void increment()
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_and_fun_.first()));
                    ++it_;
                }
                void decrement()
                {
                    RANGES_ASSERT(it_ != ranges::begin(rng_->rng_and_fun_.first()));
                    --it_;
                }
                void advance(typename basic_iterator::difference_type n)
                {
                    it_ += n;
                }
                typename basic_iterator::difference_type distance_to(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return that.it_ - it_;
                }
                bool equal(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                auto dereference() const -> decltype(rng_->rng_and_fun_.second()(*it_))
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_and_fun_.first()));
                    return rng_->rng_and_fun_.second()(*it_);
                }
            public:
                basic_iterator()
                  : rng_{}, it_{}
                {}
                // For iterator -> const_iterator conversion
                template<typename OtherTfxRng,
                         typename = typename std::enable_if<
                                        !std::is_const<OtherTfxRng>::value>::type>
                basic_iterator(basic_iterator<OtherTfxRng> that)
                  : rng_(that.rng_), it_(std::move(that).it_)
                {}
            };

        public:
            using iterator       = basic_iterator<transform_range_view>;
            using const_iterator = basic_iterator<transform_range_view const>;

            transform_range_view(InputRange && rng, UnaryFunction fun)
              : rng_and_fun_{std::forward<InputRange>(rng), make_invokable(std::move(fun))}
            {}
            iterator begin()
            {
                return {*this, ranges::begin(rng_and_fun_.first())};
            }
            iterator end()
            {
                return {*this, ranges::end(rng_and_fun_.first())};
            }
            const_iterator begin() const
            {
                return {*this, ranges::begin(rng_and_fun_.first())};
            }
            const_iterator end() const
            {
                return {*this, ranges::end(rng_and_fun_.first())};
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            InputRange & base()
            {
                return rng_and_fun_.first();
            }
            InputRange const & base() const
            {
                return rng_and_fun_.first();
            }
        };

        namespace view
        {
            struct transformer : bindable<transformer>
            {
            private:
                template<typename UnaryFunction>
                struct transformer1 : pipeable<transformer1<UnaryFunction>>
                {
                private:
                    UnaryFunction fun_;
                public:
                    transformer1(UnaryFunction fun)
                      : fun_(std::move(fun))
                    {}
                    template<typename InputRange, typename This>
                    static transform_range_view<InputRange, UnaryFunction>
                    pipe(InputRange && rng, This && this_)
                    {
                        return {std::forward<InputRange>(rng), std::forward<This>(this_).fun_};
                    }
                };
            public:
                ///
                template<typename InputRange1, typename UnaryFunction>
                static transform_range_view<InputRange1, UnaryFunction>
                invoke(transformer, InputRange1 && rng, UnaryFunction fun)
                {
                    CONCEPT_ASSERT(ranges::InputRange<InputRange1>());
                    CONCEPT_ASSERT(ranges::Callable<invokable_t<UnaryFunction>,
                                                    range_reference_t<InputRange1>>());
                    return {std::forward<InputRange1>(rng), std::move(fun)};
                }

                /// \overload
                template<typename UnaryFunction>
                static transformer1<UnaryFunction> invoke(transformer, UnaryFunction fun)
                {
                    return {std::move(fun)};
                }
            };

            RANGES_CONSTEXPR transformer transform {};
        }
    }
}

#endif
