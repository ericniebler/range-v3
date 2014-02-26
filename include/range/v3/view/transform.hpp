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

            template<bool Const>
            using reference_t =
                result_of_t<detail::add_const_if_t<invokable_t<UnaryFunction>, Const> &(
                    range_reference_t<detail::add_const_if_t<InputRange, Const>>)>;

            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<Const>
                  , typename std::remove_reference<reference_t<Const>>::type
                  , range_category_t<InputRange>
                  , reference_t<Const>
                  , range_difference_t<InputRange>
                >
            {
            private:
                friend struct transform_range_view;
                friend struct ranges::iterator_core_access;
                using base_range = detail::add_const_if_t<InputRange, Const>;
                using base_range_iterator = range_iterator_t<base_range>;
                using transform_range_view_ = detail::add_const_if_t<transform_range_view, Const>;

                transform_range_view_ *rng_;
                base_range_iterator it_;

                basic_iterator(transform_range_view_ &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
                {}
                void increment()
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->base()));
                    ++it_;
                }
                void decrement()
                {
                    RANGES_ASSERT(it_ != ranges::begin(rng_->base()));
                    --it_;
                }
                void advance(range_difference_t<base_range> n)
                {
                    it_ += n;
                }
                template<bool OtherConst>
                range_difference_t<base_range> distance_to(basic_iterator<OtherConst> const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return that.it_ - it_;
                }
                template<bool OtherConst>
                bool equal(basic_iterator<OtherConst> const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                reference_t<Const> dereference() const
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->base()));
                    return rng_->fun()(*it_);
                }
            public:
                constexpr basic_iterator()
                  : rng_{}, it_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, enable_if_t<!OtherConst> = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : rng_(that.rng_), it_(std::move(that).it_)
                {}
            };
        public:
            using iterator       = basic_iterator<false>;
            using const_iterator = basic_iterator<true>;

            transform_range_view(InputRange && rng, UnaryFunction fun)
              : rng_and_fun_{std::forward<InputRange>(rng), make_invokable(std::move(fun))}
            {}
            iterator begin()
            {
                return {*this, ranges::begin(base())};
            }
            iterator end()
            {
                return {*this, ranges::end(base())};
            }
            const_iterator begin() const
            {
                return {*this, ranges::begin(base())};
            }
            const_iterator end() const
            {
                return {*this, ranges::end(base())};
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
            invokable_t<UnaryFunction> const & fun() const
            {
                return rng_and_fun_.second();
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
