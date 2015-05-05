/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// The constexpr random-number generator functionality has been
// adapted from libc++: https://libcxx.llvm.org
//
#ifndef RANGES_V3_ALGORITHM_SHUFFLE_HPP
#define RANGES_V3_ALGORITHM_SHUFFLE_HPP

#include <climits>
#include <random>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-concepts
        /// @{
        namespace concepts
        {
            struct UniformRandomNumberGenerator
              : refines<Function>
            {
                template<typename Gen>
                auto requires_(Gen&&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<UnsignedIntegral>(val<Gen>()())
                    ));
            };
        }

        template<typename Gen>
        using UniformRandomNumberGenerator = concepts::models<concepts::UniformRandomNumberGenerator, Gen>;
        /// @}

        /// \addtogroup group-utility
        /// @{
        namespace random_detail
        {

            RANGES_CXX14_CONSTEXPR
            unsigned
            clz(unsigned x)
            {
                return static_cast<unsigned>(__builtin_clz(x));
            }

            RANGES_CXX14_CONSTEXPR
            unsigned long
            clz(unsigned long x)
            {
                return static_cast<unsigned long>(__builtin_clzl (x));
            }

            RANGES_CXX14_CONSTEXPR
            unsigned long long
            clz(unsigned long long x)
            {
                return static_cast<unsigned long long>(__builtin_clzll(x));
            }

            template <unsigned long long Xp, size_t Rp>
            struct log2_imp
            {
                static const size_t value = Xp & ((unsigned long long)(1) << Rp) ? Rp
                                            : log2_imp<Xp, Rp - 1>::value;
            };

            template <unsigned long long Xp>
            struct log2_imp<Xp, 0>
            {
                static const size_t value = 0;
            };

            template <size_t Rp>
            struct log2_imp<0, Rp>
            {
                static const size_t value = Rp + 1;
            };

            template <class UI, UI Xp>
            struct log2
            {
                static const size_t value = log2_imp<Xp,
                                                     sizeof(UI) * CHAR_BIT - 1>::value;
            };

            template<class Engine, class UInt>
            class independent_bits_engine
            {
            public:
                // types
                typedef UInt result_type;

            private:
                typedef typename Engine::result_type Engine_result_type;
                typedef typename std::conditional
                <
                    sizeof(Engine_result_type) <= sizeof(result_type),
                    result_type,
                    Engine_result_type
                >::type Working_result_type;

                Engine& e_;
                size_t w_;
                size_t n_;
                size_t w0_;
                size_t n0_;
                Working_result_type y0_;
                Working_result_type y1_;
                Engine_result_type mask0_;
                Engine_result_type mask1_;

                static constexpr const Working_result_type Rp = Engine::max() - Engine::min()
                                                                 + Working_result_type(1);

                static constexpr const size_t m = log2<Working_result_type, Rp>::value;
                static constexpr const size_t WDt = std::numeric_limits<Working_result_type>::digits;
                static constexpr const size_t EDt = std::numeric_limits<Engine_result_type>::digits;

            public:
                // constructors and seeding functions
                RANGES_CXX14_CONSTEXPR
                independent_bits_engine(Engine& e, size_t w);

                // generating functions
                RANGES_CXX14_CONSTEXPR
                result_type operator()() {return eval(std::integral_constant<bool, Rp != 0>());}

            private:
                RANGES_CXX14_CONSTEXPR
                result_type eval(std::false_type);
                RANGES_CXX14_CONSTEXPR
                result_type eval(std::true_type);
            };

            template<class Engine, class UInt>
            RANGES_CXX14_CONSTEXPR
            independent_bits_engine<Engine, UInt>
            ::independent_bits_engine(Engine& e, size_t w)
                : e_(e),
                  w_(w),
                  n_(w_ / m + (w_ % m != 0)),
                  w0_(w_ / n_),
                  n0_(n_ - w_ % n_),
                  y0_(0), y1_(0), mask0_(0), mask1_(0)
            {
                if (Rp == 0)
                    y0_ = Rp;
                else if (w0_ < WDt)
                    y0_ = (Rp >> w0_) << w0_;
                else
                    y0_ = 0;
                if (Rp - y0_ > y0_ / n_)
                {
                    ++n_;
                    w0_ = w_ / n_;
                    if (w0_ < WDt)
                        y0_ = (Rp >> w0_) << w0_;
                    else
                        y0_ = 0;
                }
                if (w0_ < WDt - 1)
                    y1_ = (Rp >> (w0_ + 1)) << (w0_ + 1);
                else
                    y1_ = 0;
                mask0_ = w0_ > 0 ? Engine_result_type(~0) >> (EDt - w0_) :
                                   Engine_result_type(0);
                mask1_ = w0_ < EDt - 1 ?
                         Engine_result_type(~0) >> (EDt - (w0_ + 1)) :
                         Engine_result_type(~0);
            }

            template<class Engine, class UInt>
            RANGES_CXX14_CONSTEXPR
            UInt
            independent_bits_engine<Engine, UInt>::eval(std::false_type)
            {
                return static_cast<result_type>(e_() & mask0_);
            }

            template<class Engine, class UInt>
            RANGES_CXX14_CONSTEXPR
            UInt
            independent_bits_engine<Engine, UInt>::eval(std::true_type)
            {
                result_type Sp = 0;
                for (size_t k = 0; k < n0_; ++k)
                {
                    Engine_result_type u{};
                    do
                    {
                        u = e_() - Engine::min();
                    } while (u >= y0_);
                    if (w0_ < WDt)
                        Sp <<= w0_;
                    else
                        Sp = 0;
                    Sp += u & mask0_;
                }
                for (size_t k = n0_; k < n_; ++k)
                {
                    Engine_result_type u{};
                    do
                    {
                        u = e_() - Engine::min();
                    } while (u >= y1_);
                    if (w0_ < WDt - 1)
                        Sp <<= w0_ + 1;
                    else
                        Sp = 0;
                    Sp += u & mask1_;
                }
                return Sp;
            }

            template<class Int = int>
            class uniform_int_distribution
            {
            public:
                using result_type = Int;

                class param_type
                {
                    result_type a_;
                    result_type b_;
                public:
                    using distribution_type = uniform_int_distribution;

                    RANGES_CXX14_CONSTEXPR
                    explicit param_type(result_type a = 0,
                                        result_type b = std::numeric_limits<result_type>::max())
                        : a_(a), b_(b) {}

                    RANGES_CXX14_CONSTEXPR result_type a() const {return a_;}
                    RANGES_CXX14_CONSTEXPR result_type b() const {return b_;}

                    RANGES_CXX14_CONSTEXPR
                    friend bool operator==(const param_type& x, const param_type& y)
                    {return x.a_ == y.a_ && x.b_ == y.b_;}
                    RANGES_CXX14_CONSTEXPR
                    friend bool operator!=(const param_type& x, const param_type& y)
                    {return !(x == y);}
                };

            private:
                param_type p_;

            public:
                RANGES_CXX14_CONSTEXPR
                explicit uniform_int_distribution(result_type a = 0,
                                                  result_type b = std::numeric_limits<result_type>::max())
                    : p_(param_type(a, b)) {}
                RANGES_CXX14_CONSTEXPR
                explicit uniform_int_distribution(const param_type& __p) : p_(__p) {}
                RANGES_CXX14_CONSTEXPR void reset() {}

                template<class URNG>
                RANGES_CXX14_CONSTEXPR
                result_type operator()(URNG& g)
                {return (*this)(g, p_);}

                template<class URNG>
                RANGES_CXX14_CONSTEXPR
                result_type operator()(URNG& g, const param_type& p);

                RANGES_CXX14_CONSTEXPR result_type a() const {return p_.a();}
                RANGES_CXX14_CONSTEXPR result_type b() const {return p_.b();}

                RANGES_CXX14_CONSTEXPR param_type param() const {return p_;}
                RANGES_CXX14_CONSTEXPR void param(const param_type& p) {p_ = p;}

                RANGES_CXX14_CONSTEXPR result_type min() const {return a();}
                RANGES_CXX14_CONSTEXPR result_type max() const {return b();}

                RANGES_CXX14_CONSTEXPR
                friend bool operator==(const uniform_int_distribution& x,
                                       const uniform_int_distribution& y)
                {return x.p_ == y.p_;}

                RANGES_CXX14_CONSTEXPR
                friend bool operator!=(const uniform_int_distribution& x,
                                       const uniform_int_distribution& y)
                {return !(x == y);}
            };

            template<class Int>
            template<class URNG>
            RANGES_CXX14_CONSTEXPR
            typename uniform_int_distribution<Int>::result_type
            uniform_int_distribution<Int>::operator()(URNG& g, const param_type& p)
            {
                typedef typename std::conditional<sizeof(result_type) <= sizeof(uint32_t),
                                             uint32_t, uint64_t>::type UInt;
                const UInt Rp = p.b() - p.a() + UInt(1);
                if (Rp == 1)
                    return p.a();
                const size_t Dt = std::numeric_limits<UInt>::digits;
                typedef independent_bits_engine<URNG, UInt> Eng;
                if (Rp == 0)
                    return static_cast<result_type>(Eng(g, Dt)());
                size_t w = Dt - clz(Rp) - 1;
                if ((Rp & (UInt(~0) >> (Dt - w))) != 0)
                    ++w;
                Eng e(g, w);
                UInt u = UInt{};
                do
                {
                    u = e();
                } while (u >= Rp);
                return static_cast<result_type>(u + p.a());
            }
        }
        /// @}

        /// \addtogroup group-algorithms
        /// @{
        struct shuffle_fn
        {
            template<typename I, typename S, typename Gen,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() &&
                    Permutable<I>() && UniformRandomNumberGenerator<Gen>() &&
                    Convertible<
                        concepts::UniformRandomNumberGenerator::result_t<Gen>,
                        concepts::WeaklyIncrementable::difference_t<I>>())>
            RANGES_CXX14_CONSTEXPR
            I operator()(I begin, S end_, Gen && gen) const
            {
                I end = ranges::next(begin, end_), orig = end;
                auto d = end - begin;
                if(d > 1)
                {
                    using param_t = random_detail::uniform_int_distribution<std::ptrdiff_t>::param_type;
                    random_detail::uniform_int_distribution<std::ptrdiff_t> uid;
                    for(--end, --d; begin < end; ++begin, --d)
                    {
                        auto i = uid(gen, param_t{0, d});
                        if(i != 0)
                            ranges::iter_swap(begin, begin + i);
                    }
                }
                return orig;
            }

            template<typename Rng, typename Gen,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng>() && Permutable<I>() &&
                    UniformRandomNumberGenerator<Gen>() && Convertible<
                        concepts::UniformRandomNumberGenerator::result_t<Gen>,
                        concepts::WeaklyIncrementable::difference_t<I>>())>
            RANGES_CXX14_CONSTEXPR
            range_safe_iterator_t<Rng> operator()(Rng &&rng, Gen && rand) const
            {
                return (*this)(begin(rng), end(rng), std::forward<Gen>(rand));
            }
        };

        /// \sa `shuffle_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& shuffle = static_const<with_braced_init_args<shuffle_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
