//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_TRANSFORM_HPP
#define RANGES_V3_ALGORITHM_TRANSFORM_HPP

#include <utility>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/detail/function_wrapper.hpp>
#include <range/v3/detail/compressed_pair.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename InputIterator1,
                     typename InputIterator2,
                     typename OutputIterator,
                     typename BinaryFunction>
            OutputIterator
            transform_impl(InputIterator1 first1,
                           InputIterator1 last1,
                           InputIterator2 first2,
                           InputIterator2 last2,
                           OutputIterator out,
                           BinaryFunction fun)
            {
                for (; first1 != last1; ++first1, ++first2)
                {
                    RANGE_ASSERT(first2 != last2);
                    *out = fun(*first1, *first2);
                    ++out;
                }
                return out;
            }
        }

        template<typename Rng, typename Fun>
        struct transform_range
        {
        private:
            detail::compressed_pair<Rng, detail::function_wrapper<Fun>> rng_and_fun_;

            template<typename TfxRng>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<TfxRng>
                  , typename std::remove_reference<
                        decltype(std::declval<TfxRng &>().rng_and_fun_.second()(
                            *ranges::begin(std::declval<TfxRng &>().rng_and_fun_.first())))
                    >::type
                  , range_category_t<Rng>
                  , decltype(std::declval<TfxRng &>().rng_and_fun_.second()(
                        *ranges::begin(std::declval<TfxRng &>().rng_and_fun_.first())))
                  , range_difference_t<Rng>
                >
            {
            private:
                friend struct transform_range;
                friend struct ranges::iterator_core_access;
                using base_range_iterator =
                    decltype(ranges::begin(std::declval<TfxRng &>().rng_and_fun_.first()));

                TfxRng *rng_;
                base_range_iterator it_;

                basic_iterator(TfxRng &rng, base_range_iterator it)
                  : rng_(&rng), it_(detail::move(it))
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
                  : rng_(that.rng_), it_(detail::move(that).it_)
                {}
            };

        public:
            using iterator       = basic_iterator<transform_range>;
            using const_iterator = basic_iterator<transform_range const>;

            transform_range(Rng && rng, Fun fun)
              : rng_and_fun_{detail::forward<Rng>(rng), detail::move(fun)}
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
            Rng & base()
            {
                return rng_and_fun_.first();
            }
            Rng const & base() const
            {
                return rng_and_fun_.first();
            }
        };

        struct transformer : bindable<transformer>
        {
        private:
            template<typename Fun>
            struct transformer1 : pipeable<transformer1<Fun>>
            {
            private:
                Fun fun_;
            public:
                transformer1(Fun fun)
                  : fun_(detail::move(fun))
                {}
                template<typename Rng, typename This>
                static transform_range<Rng, Fun> pipe(Rng && rng, This && this_)
                {
                    return {detail::forward<Rng>(rng), detail::forward<This>(this_).fun_};
                }
            };
        public:
            /// \brief template function transform
            ///
            /// range-based version of the transform std algorithm
            ///
            /// \pre InputRange1 is a model of the InputRange concept
            /// \pre InputRange2 is a model of the InputRange concept
            /// \pre OutputIterator is a model of the OutputIterator concept
            /// \pre UnaryOperation is a model of the UnaryFunction concept
            /// \pre BinaryOperation is a model of the BinaryFunction concept
            template<typename InputRange1,
                     typename OutputIterator,
                     typename UnaryOperation,
                     CONCEPT_REQUIRES(ranges::InputRange<InputRange1>()),
                     typename Ref1 = range_reference_t<InputRange1>,
                     CONCEPT_REQUIRES(ranges::Callable<UnaryOperation, Ref1>()),
                     typename Ref2 = result_of_t<UnaryOperation(Ref1)>,
                     CONCEPT_REQUIRES(ranges::OutputIterator<OutputIterator, Ref2>())
            >
            static OutputIterator
            invoke(transformer,
                   InputRange1 && rng,
                   OutputIterator out,
                   UnaryOperation fun)
            {
                return std::transform(ranges::begin(rng), ranges::end(rng),
                                      detail::move(out), detail::move(fun));
            }

            /// \overload
            template<typename InputRange1,
                     typename InputRange2,
                     typename OutputIterator,
                     typename BinaryOperation,
                     CONCEPT_REQUIRES(ranges::InputRange<InputRange1>()),
                     CONCEPT_REQUIRES(ranges::InputRange<InputRange2>()),
                     typename Ref1 = range_reference_t<InputRange1>,
                     typename Ref2 = range_reference_t<InputRange2>,
                     CONCEPT_REQUIRES(ranges::Callable<BinaryOperation, Ref1, Ref2>()),
                     typename Value3 = result_of_t<BinaryOperation(Ref1, Ref2)>,
                     CONCEPT_REQUIRES(ranges::OutputIterator<OutputIterator, Value3>())>
            static OutputIterator
            invoke(transformer,
                   InputRange1 && rng1,
                   InputRange2 && rng2,
                   OutputIterator out,
                   BinaryOperation fun)
            {
                return detail::transform_impl(
                    ranges::begin(rng1), ranges::end(rng1),
                    ranges::begin(rng2), ranges::end(rng2),
                    detail::move(out), detail::move(fun));
            }

            /// \overload
            template<typename InputRange1,
                     typename UnaryOperation,
                     CONCEPT_REQUIRES(ranges::InputRange<InputRange1>()),
                     typename Ref1 = range_reference_t<InputRange1>,
                     CONCEPT_REQUIRES(ranges::Callable<UnaryOperation, Ref1>())>
            static transform_range<InputRange1, UnaryOperation>
            invoke(transformer, InputRange1 && rng, UnaryOperation fun)
            {
                return {detail::forward<InputRange1>(rng), detail::move(fun)};
            }

            /// \overload
            template<typename UnaryOperation>
            static transformer1<UnaryOperation> invoke(transformer, UnaryOperation fun)
            {
                return {detail::move(fun)};
            }
        };

        constexpr transformer transform {};
    } // inline namespace v3

} // namespace ranges

#endif // include guard
