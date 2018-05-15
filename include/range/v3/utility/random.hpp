/// \file
// Range v3 library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

/*
 * Random-Number Utilities (randutil)
 *     Addresses common issues with C++11 random number generation.
 *     Makes good seeding easier, and makes using RNGs easy while retaining
 *     all the power.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Melissa E. O'Neill
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RANGES_V3_UTILITY_RANDOM_HPP
#define RANGES_V3_UTILITY_RANDOM_HPP

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <array>
#include <atomic>
#include <chrono>
#include <initializer_list>
#include <mutex>
#include <new>
#include <random>
#include <typeinfo>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/generate.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

#if RANGES_CXX_THREAD >= RANGES_CXX_THREAD_11
#include <thread>
#endif

// Ugly platform-specific code for auto_seeded

// Clang/C2 bug: __has_builtin(__builtin_readcyclecounter) reports true, but
// there is no corresponding builtin in C2.
#if !(defined(__clang__) && defined(__c2__))
#ifdef __has_builtin
    #if __has_builtin(__builtin_readcyclecounter)
        #define RANGES_CPU_ENTROPY __builtin_readcyclecounter()
    #endif
#endif
#endif
#ifndef RANGES_CPU_ENTROPY
    #ifdef __i386__
        #ifdef __GNUC__
            #define RANGES_CPU_ENTROPY __builtin_ia32_rdtsc()
        #else
            #include <immintrin.h>
            #define RANGES_CPU_ENTROPY __rdtsc()
        #endif
    #else
        #define RANGES_CPU_ENTROPY 0
    #endif
#endif

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_CXX17_COMPAT

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-concepts
        /// @{
        namespace concepts
        {
            struct UniformRandomNumberGenerator
            {
                template<typename Gen>
                using result_t = invoke_result_t<Gen&>;

                template<typename Gen, typename Result = result_t<Gen>>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<UnsignedIntegral, Result>(),
                        concepts::has_type<Result>(uncvref_t<Gen>::min()),
                        concepts::has_type<Result>(uncvref_t<Gen>::max()),
                        concepts::is_true(meta::bool_<
                            (uncvref_t<Gen>::min() < uncvref_t<Gen>::max())>())
                    ));
            };
        }

        template<typename Gen>
        using UniformRandomNumberGenerator =
            concepts::models<concepts::UniformRandomNumberGenerator, Gen>;
        /// @}

        /// \cond
        namespace detail
        {
            namespace randutils
            {
                template<typename T,
                    CONCEPT_REQUIRES_(Integral<T>())>
                RANGES_CXX14_CONSTEXPR std::uint32_t crushto32(T value)
                RANGES_INTENDED_MODULAR_ARITHMETIC
                {
                    if(sizeof(T) <= 4)
                        return static_cast<std::uint32_t>(value);
                    else {
                        auto result = static_cast<std::uint64_t>(value);
                        result *= 0xbc2ad017d719504d;
                        return static_cast<std::uint32_t>(result ^ (result >> 32));
                    }
                }

                template<typename T>
                RANGES_CXX14_CONSTEXPR std::uint32_t hash(T && value)
                {
                    auto hasher = std::hash<uncvref_t<T>>{};
                    return randutils::crushto32(hasher(static_cast<T&&>(value)));
                }

                constexpr std::uint32_t fnv(std::uint32_t hash, const char* pos)
                RANGES_INTENDED_MODULAR_ARITHMETIC
                {
                    return *pos == '\0' ? hash : randutils::fnv(
                        (hash * 16777619U) ^ static_cast<unsigned char>(*pos), pos+1);
                }

                constexpr std::size_t weird_seed_sources = 11;
                constexpr std::size_t seed_count = weird_seed_sources + 8;
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 6
                inline
#else
                template<class = void>
#endif
                std::array<std::uint32_t, seed_count> local_entropy(std::uint32_t s1, std::uint32_t s2)
                {
                    CONCEPT_ASSERT(seed_count >= weird_seed_sources);
                    std::array<std::uint32_t, seed_count> seeds;
                    auto it = seeds.begin();

                    static std::atomic<std::uint32_t> counter;
                    *it++ = counter.fetch_add(std::uint32_t{0xedf19156}, std::memory_order_relaxed);

                    // The heap can vary from run to run as well.
                    void* malloc_addr = std::malloc(sizeof(int));
                    std::free(malloc_addr);
                    *it++ = randutils::hash(malloc_addr);
                    *it++ = randutils::hash(&malloc_addr);

                    // Classic seed, the time.  It ought to change, especially since
                    // this is (hopefully) nanosecond resolution time.
                    auto hitime = std::chrono::high_resolution_clock::now()
                                    .time_since_epoch().count();
                    *it++ = randutils::crushto32(hitime);

                    // Address of the thing being initialized.  That can mean that
                    // different seed sequences in different places in memory will be
                    // different.  Even for the same object, it may vary from run to
                    // run in systems with ASLR, such as OS X, but on Linux it might not
                    // unless we compile with -fPIC -pic.
                    *it++ = s1;

                    // The address of the time function.  It should hopefully be in
                    // a system library that hopefully isn't always in the same place
                    // (might not change until system is rebooted though)
                    *it++ = randutils::hash(&std::chrono::high_resolution_clock::now);

                    // The address of the exit function.  It should hopefully be in
                    // a system library that hopefully isn't always in the same place
                    // (might not change until system is rebooted though).  Hopefully
                    // it's in a different library from time_func.
                    {
                        using namespace std;
                        *it++ = randutils::hash(&_Exit);
                    }

                    // The address of a local function.  That may be in a totally
                    // different part of memory.  On OS X it'll vary from run to run thanks
                    // to ASLR, on Linux it might not unless we compile with -fPIC -pic.
                    // Need the cast because it's an overloaded function and we need to
                    // pick the right one.
                    *it++ = randutils::hash(
                        static_cast<std::uint32_t(*)(std::uint64_t)>(&randutils::crushto32));

#if RANGES_CXX_THREAD >= RANGES_CXX_THREAD_11
                    // Hash our thread id.  It seems to vary from run to run on OS X, not
                    // so much on Linux.
                    *it++ = randutils::hash(std::this_thread::get_id());
#endif
                    // Hash of the ID of a type.  May or may not vary, depending on
                    // implementation.
                    *it++ = s2;

                    // Platform-specific entropy
                    *it++ = randutils::crushto32(RANGES_CPU_ENTROPY);

                    RANGES_ASSERT(static_cast<std::size_t>(it - seeds.begin()) <= weird_seed_sources);

                    // Hopefully high-quality entropy from random_device.
                    std::random_device rd{};
                    ranges::generate(it, seeds.end(), ranges::ref(rd));

                    return seeds;
                }

                template<typename I, CONCEPT_REQUIRES_(UnsignedIntegral<I>())>
                constexpr I fast_exp(I x, I power, I result = I{1})
                {
                    return power == I{0} ? result
                        : randutils::fast_exp(x * x, power >> 1, result * (power & I{1} ? x : 1));
                }

                //////////////////////////////////////////////////////////////////////////////
                //
                // seed_seq_fe
                //
                //////////////////////////////////////////////////////////////////////////////

                /*
                * seed_seq_fe implements a fixed-entropy seed sequence; it conforms to all
                * the requirements of a Seed Sequence concept.
                *
                * seed_seq_fe<N> implements a seed sequence which seeds based on a store of
                * N * 32 bits of entropy.  Typically, it would be initialized with N or more
                * integers.
                *
                * seed_seq_fe128 and seed_seq_fe256 are provided as convenience typedefs for
                * 128- and 256-bit entropy stores respectively.  These variants outperform
                * std::seed_seq, while being better mixing the bits it is provided as entropy.
                * In almost all common use cases, they serve as better drop-in replacements
                * for seed_seq.
                *
                * Technical details
                *
                * Assuming it constructed with M seed integers as input, it exhibits the
                * following properties
                *
                * * Diffusion/Avalanche:  A single-bit change in any of the M inputs has a
                *   50% chance of flipping every bit in the bitstream produced by generate.
                *   Initializing the N-word entropy store with M words requires O(N * M)
                *   time precisely because of the avalanche requirements.  Once constructed,
                *   calls to generate are linear in the number of words generated.
                *
                * * Bias freedom/Bijection: If M == N, the state of the entropy store is a
                *   bijection from the M inputs (i.e., no states occur twice, none are
                *   omitted). If M > N the number of times each state can occur is the same
                *   (each state occurs 2**(32*(M-N)) times, where ** is the power function).
                *   If M < N, some states cannot occur (bias) but no state occurs more
                *   than once (it's impossible to avoid bias if M < N; ideally N should not
                *   be chosen so that it is more than M).
                *
                *   Likewise, the generate function has similar properties (with the entropy
                *   store as the input data).  If more outputs are requested than there is
                *   entropy, some outputs cannot occur.  For example, the Mersenne Twister
                *   will request 624 outputs, to initialize its 19937-bit state, which is
                *   much larger than a 128-bit or 256-bit entropy pool.  But in practice,
                *   limiting the Mersenne Twister to 2**128 possible initializations gives
                *   us enough initializations to give a unique initialization to trillions
                *   of computers for billions of years.  If you really have 624 words of
                *   *real* high-quality entropy you want to use, you probably don't need
                *   an entropy mixer like this class at all.  But if you *really* want to,
                *   nothing is stopping you from creating a randutils::seed_seq_fe<624>.
                *
                * * As a consequence of the above properties, if all parts of the provided
                *   seed data are kept constant except one, and the remaining part is varied
                *   through K different states, K different output sequences will be produced.
                *
                * * Also, because the amount of entropy stored is fixed, this class never
                *   performs dynamic allocation and is free of the possibility of generating
                *   an exception.
                *
                * Ideas used to implement this code include hashing, a simple PCG generator
                * based on an MCG base with an XorShift output function and permutation
                * functions on tuples.
                *
                * More detail at
                *     http://www.pcg-random.org/posts/developing-a-seed_seq-alternative.html
                */

                template<std::size_t count = 4, typename IntRep = std::uint32_t,
                    std::size_t mix_rounds = 1 + (count <= 2)>
                struct seed_seq_fe {
                public:
                    CONCEPT_ASSERT(UnsignedIntegral<IntRep>());
                    typedef IntRep result_type;

                private:
                    static constexpr std::uint32_t INIT_A = 0x43b0d7e5;
                    static constexpr std::uint32_t MULT_A = 0x931e8875;

                    static constexpr std::uint32_t INIT_B = 0x8b51f9dd;
                    static constexpr std::uint32_t MULT_B = 0x58f38ded;

                    static constexpr std::uint32_t MIX_MULT_L = 0xca01f9dd;
                    static constexpr std::uint32_t MIX_MULT_R = 0x4973f715;
                    static constexpr std::uint32_t XSHIFT = sizeof(IntRep)*8/2;

                    std::array<IntRep, count> mixer_;

                    template<typename I, typename S,
                        CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>() &&
                            ConvertibleTo<reference_t<I>, IntRep>())>
                    void mix_entropy(I begin, S end)
                    {
                        auto hash_const = INIT_A;
                        auto hash = [&](IntRep value) RANGES_INTENDED_MODULAR_ARITHMETIC
                        {
                            value ^= hash_const;
                            hash_const *= MULT_A;
                            value *= hash_const;
                            value ^= value >> XSHIFT;
                            return value;
                        };
                        auto mix = [](IntRep x, IntRep y) RANGES_INTENDED_MODULAR_ARITHMETIC
                        {
                            IntRep result = MIX_MULT_L*x - MIX_MULT_R*y;
                            result ^= result >> XSHIFT;
                            return result;
                        };

                        for(auto& elem : mixer_)
                        {
                            if(begin != end)
                                elem = hash(static_cast<IntRep>(*begin++));
                            else
                                elem = hash(IntRep{0});
                        }
                        for(auto& src : mixer_)
                            for(auto& dest : mixer_)
                                if(&src != &dest)
                                    dest = mix(dest,hash(src));
                        for(; begin != end; ++begin)
                            for(auto& dest : mixer_)
                                dest = mix(dest,hash(static_cast<IntRep>(*begin)));
                    }

                public:
                    seed_seq_fe(const seed_seq_fe&)     = delete;
                    void operator=(const seed_seq_fe&)  = delete;

                    template<typename T,
                        CONCEPT_REQUIRES_(ConvertibleTo<T const&, IntRep>())>
                    seed_seq_fe(std::initializer_list<T> init)
                    {
                        seed(init.begin(), init.end());
                    }

                    template<typename I, typename S,
                        CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>() &&
                            ConvertibleTo<reference_t<I>, IntRep>())>
                    seed_seq_fe(I begin, S end)
                    {
                        seed(begin, end);
                    }

                    // generating functions
                    template<typename I, typename S,
                        CONCEPT_REQUIRES_(RandomAccessIterator<I>() && Sentinel<S, I>())>
                    void generate(I dest_begin, S dest_end) const
                    RANGES_INTENDED_MODULAR_ARITHMETIC
                    {
                        auto src_begin = mixer_.begin();
                        auto src_end   = mixer_.end();
                        auto src       = src_begin;
                        auto hash_const = INIT_B;
                        for(auto dest = dest_begin; dest != dest_end; ++dest)
                        {
                            auto dataval = *src;
                            if(++src == src_end)
                                src = src_begin;
                            dataval ^= hash_const;
                            hash_const *= MULT_B;
                            dataval *= hash_const;
                            dataval ^= dataval >> XSHIFT;
                            *dest = dataval;
                        }
                    }

                    constexpr std::size_t size() const
                    {
                        return count;
                    }

                    template<typename O,
                        CONCEPT_REQUIRES_(WeaklyIncrementable<O>() &&
                            IndirectlyCopyable<decltype(mixer_.begin()), O>())>
                    void param(O dest) const
                    RANGES_INTENDED_MODULAR_ARITHMETIC
                    {
                        const IntRep INV_A = randutils::fast_exp(MULT_A, IntRep(-1));
                        const IntRep MIX_INV_L = randutils::fast_exp(MIX_MULT_L, IntRep(-1));

                        auto mixer_copy = mixer_;
                        for(std::size_t round = 0; round < mix_rounds; ++round)
                        {
                            // Advance to the final value.  We'll backtrack from that.
                            auto hash_const = INIT_A*randutils::fast_exp(MULT_A, IntRep(count * count));

                            for(auto src = mixer_copy.rbegin(); src != mixer_copy.rend(); ++src)
                                for(auto dest = mixer_copy.rbegin(); dest != mixer_copy.rend();
                                    ++dest)
                                    if(src != dest)
                                    {
                                        IntRep revhashed = *src;
                                        auto mult_const = hash_const;
                                        hash_const *= INV_A;
                                        revhashed ^= hash_const;
                                        revhashed *= mult_const;
                                        revhashed ^= revhashed >> XSHIFT;
                                        IntRep unmixed = *dest;
                                        unmixed ^= unmixed >> XSHIFT;
                                        unmixed += MIX_MULT_R*revhashed;
                                        unmixed *= MIX_INV_L;
                                        *dest = unmixed;
                                    }
                            for(auto i = mixer_copy.rbegin(); i != mixer_copy.rend(); ++i)
                            {
                                IntRep unhashed = *i;
                                unhashed ^= unhashed >> XSHIFT;
                                unhashed *= randutils::fast_exp(hash_const, IntRep(-1));
                                hash_const *= INV_A;
                                unhashed ^= hash_const;
                                *i = unhashed;
                            }
                        }
                        ranges::copy(mixer_copy, dest);
                    }

                    template<typename I, typename S,
                        CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>() &&
                            ConvertibleTo<reference_t<I>, IntRep>())>
                    void seed(I begin, S end)
                    {
                        mix_entropy(begin, end);
                        // For very small sizes, we do some additional mixing.  For normal
                        // sizes, this loop never performs any iterations.
                        for(std::size_t i = 1; i < mix_rounds; ++i)
                            stir();
                    }

                    seed_seq_fe& stir()
                    {
                        mix_entropy(mixer_.begin(), mixer_.end());
                        return *this;
                    }

                };

                using seed_seq_fe128 = seed_seq_fe<4, std::uint32_t>;
                using seed_seq_fe256 = seed_seq_fe<8, std::uint32_t>;

                //////////////////////////////////////////////////////////////////////////////
                //
                // auto_seeded
                //
                //////////////////////////////////////////////////////////////////////////////

                /*
                * randutils::auto_seeded
                *
                *   Extends a seed sequence class with a nondeterministic default constructor.
                *   Uses a variety of local sources of entropy to portably initialize any
                *   seed sequence to a good default state.
                *
                *   In normal use, it's accessed via one of the following type aliases, which
                *   use seed_seq_fe128 and seed_seq_fe256 above.
                *
                *       randutils::auto_seed_128
                *       randutils::auto_seed_256
                *
                *   It's discussed in detail at
                *       http://www.pcg-random.org/posts/simple-portable-cpp-seed-entropy.html
                *   and its motivation (why you can't just use std::random_device) here
                *       http://www.pcg-random.org/posts/cpps-random_device.html
                */

                template<typename SeedSeq>
                struct auto_seeded : public SeedSeq {
                    auto_seeded()
                        : auto_seeded(randutils::local_entropy(
                            randutils::hash(this), randutils::crushto32(typeid(*this).hash_code())))
                    {}
                    template<std::size_t N>
                    auto_seeded(std::array<std::uint32_t, N> const& seeds)
                        : SeedSeq(seeds.begin(), seeds.end())
                    {}
                    using SeedSeq::SeedSeq;

                    const SeedSeq& base() const
                    {
                        return *this;
                    }
                    SeedSeq& base()
                    {
                        return *this;
                    }
                };

                using auto_seed_128 = auto_seeded<seed_seq_fe128>;
                using auto_seed_256 = auto_seeded<seed_seq_fe256>;
            }

            using default_URNG =
                meta::if_c<(sizeof(void*) >= 8), std::mt19937_64, std::mt19937>;

#if !RANGES_CXX_THREAD_LOCAL
            template<typename URNG>
            class sync_URNG
              : private URNG
            {
                mutable std::mutex mtx_;
            public:
                using URNG::URNG;
                sync_URNG() = default;
                using typename URNG::result_type;
                result_type operator()()
                {
                    std::lock_guard<std::mutex> guard{mtx_};
                    return static_cast<URNG &>(*this)();
                }
                using URNG::min;
                using URNG::max;
            };
            using default_random_engine = sync_URNG<default_URNG>;
#else
            using default_random_engine = default_URNG;
#endif

            template<typename T = void>
            default_random_engine& get_random_engine()
            {
                using Seeder = meta::if_c<
                    (sizeof(default_URNG) > 16),
                    randutils::auto_seed_256,
                    randutils::auto_seed_128>;

#if RANGES_CXX_THREAD_LOCAL >= RANGES_CXX_THREAD_LOCAL_11
                static thread_local default_random_engine engine{Seeder{}.base()};

#elif RANGES_CXX_THREAD_LOCAL
                static __thread bool initialized = false;
                static __thread meta::_t<std::aligned_storage<
                    sizeof(default_random_engine),
                    alignof(default_random_engine)>> storage;

                if(!initialized)
                {
                    ::new(static_cast<void*>(&storage)) default_random_engine{Seeder{}.base()};
                    initialized = true;
                }
                auto& engine = reinterpret_cast<default_random_engine&>(storage);

#else
                static default_random_engine engine{Seeder{}.base()};
#endif // RANGES_CXX_THREAD_LOCAL

                return engine;
            }
        }
        /// \endcond
    }
}

RANGES_DIAGNOSTIC_POP

#undef RANGES_CPU_ENTROPY

#endif
