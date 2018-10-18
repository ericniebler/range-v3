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

#ifndef RANGES_V3_VIEW_SINGLE_HPP
#define RANGES_V3_VIEW_SINGLE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename T>
        struct single_view
          : view_interface<single_view<T>, (cardinality)1> {
        private:
            CONCEPT_ASSERT(CopyConstructible<T>());
            CONCEPT_ASSERT(Satisfies<T, std::is_object>());
            semiregular_t<T> value_;
            template<typename... Args>
            constexpr single_view(in_place_t, std::true_type, Args &&...args)
              : value_{static_cast<Args &&>(args)...}
            {}
            template<typename... Args>
            constexpr single_view(in_place_t, std::false_type, Args &&...args)
              : value_{in_place, static_cast<Args &&>(args)...}
            {}
        public:
            single_view() = default;
            constexpr explicit single_view(T const &t)
              : value_(t)
            {}
            constexpr explicit single_view(T &&t)
              : value_(std::move(t))
            {}
            template<class... Args,
                CONCEPT_REQUIRES_(Constructible<T, Args...>())>
            constexpr single_view(in_place_t, Args&&... args)
              : single_view{
                    in_place,
                    meta::bool_<(bool)SemiRegular<T>()>{},
                    static_cast<Args &&>(args)...}
            {}
            RANGES_CXX14_CONSTEXPR T *begin() noexcept
            {
                return data();
            }
            constexpr T const *begin() const noexcept
            {
                return data();
            }
            RANGES_CXX14_CONSTEXPR T *end() noexcept
            {
                return data() + 1;
            }
            constexpr T const *end() const noexcept
            {
                return data() + 1;
            }
            constexpr static std::size_t size() noexcept
            {
                return 1u;
            }
            RANGES_CXX14_CONSTEXPR T *data() noexcept
            {
                return std::addressof(static_cast<T &>(value_));
            }
            constexpr T const *data() const noexcept
            {
                return std::addressof(static_cast<T const &>(value_));
            }
        };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        template<class T>
        explicit single_view(T&&) -> single_view<detail::decay_t<T>>;
#endif

        namespace view
        {
            struct single_fn
            {
                template<typename Val, CONCEPT_REQUIRES_(CopyConstructible<Val>())>
                single_view<Val> operator()(Val value) const
                {
                    return single_view<Val>{std::move(value)};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                // For error reporting
                template<typename Arg, typename Val = detail::decay_t<Arg>,
                    CONCEPT_REQUIRES_(!(CopyConstructible<Val>() && Constructible<Val, Arg>()))>
                void operator()(Arg &&) const
                {
                    CONCEPT_ASSERT_MSG(CopyConstructible<Val>(),
                        "The object passed to view::single must be a model of the CopyConstructible "
                        "concept; that is, it needs to be copy and move constructible, and destructible.");
                    CONCEPT_ASSERT_MSG(!CopyConstructible<Val>() || Constructible<Val, Arg>(),
                        "The object type passed to view::single must be initializable from the "
                        "actual argument expression.");
                }
            #endif
            };

            /// \relates single_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(single_fn, single)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::single_view)

#endif
