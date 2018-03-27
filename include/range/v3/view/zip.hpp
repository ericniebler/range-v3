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

#ifndef RANGES_V3_VIEW_ZIP_HPP
#define RANGES_V3_VIEW_ZIP_HPP

#include <tuple>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/common_tuple.hpp>
#include <range/v3/view/zip_with.hpp>

RANGES_DISABLE_WARNINGS

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            struct indirect_zip_fn_
            {
                // tuple value
                template<typename ...Its,
                    CONCEPT_REQUIRES_(meta::and_<Readable<Its>...>() && sizeof...(Its) != 2)>
                [[noreturn]] auto operator()(copy_tag, Its...) const ->
                    std::tuple<value_type_t<Its>...>
                {
                    RANGES_EXPECT(false);
                }

                // tuple reference
                template<typename ...Its,
                    CONCEPT_REQUIRES_(meta::and_<Readable<Its>...>() && sizeof...(Its) != 2)>
                auto operator()(Its const &...its) const
                    noexcept(meta::and_c<noexcept(reference_t<Its>(*its))...>::value)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    common_tuple<reference_t<Its>...>{*its...}
                )

                // tuple rvalue reference
                template<typename ...Its,
                    CONCEPT_REQUIRES_(meta::and_<Readable<Its>...>() && sizeof...(Its) != 2)>
                auto operator()(move_tag, Its const &...its) const
                    noexcept(meta::and_c<
                        noexcept(rvalue_reference_t<Its>(iter_move(its)))...>::value)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    common_tuple<rvalue_reference_t<Its>...>{iter_move(its)...}
                )

                // pair value
                template<typename It1, typename It2,
                    CONCEPT_REQUIRES_(Readable<It1>() && Readable<It2>())>
                [[noreturn]] auto operator()(copy_tag, It1, It2) const ->
                    std::pair<value_type_t<It1>, value_type_t<It2>>
                {
                    RANGES_EXPECT(false);
                }

                // pair reference
                template<typename It1, typename It2,
                    CONCEPT_REQUIRES_(Readable<It1>() && Readable<It2>())>
                auto operator()(It1 const &it1, It2 const &it2) const
                    noexcept(noexcept(reference_t<It1>(*it1)) &&
                             noexcept(reference_t<It2>(*it2)))
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    common_pair<reference_t<It1>, reference_t<It2>>{*it1, *it2}
                )

                // pair rvalue reference
                template<typename It1, typename It2,
                    CONCEPT_REQUIRES_(Readable<It1>() && Readable<It2>())>
                auto operator()(move_tag, It1 const &it1, It2 const &it2) const
                    noexcept(noexcept(rvalue_reference_t<It1>(iter_move(it1))) &&
                             noexcept(rvalue_reference_t<It2>(iter_move(it2))))
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    common_pair<rvalue_reference_t<It1>, rvalue_reference_t<It2>>{
                        iter_move(it1), iter_move(it2)}
                )
            };
        } // namespace detail
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename...Rngs>
        struct zip_view
          : iter_zip_with_view<detail::indirect_zip_fn_, Rngs...>
        {
            zip_view() = default;
            explicit zip_view(Rngs...rngs)
              : iter_zip_with_view<detail::indirect_zip_fn_, Rngs...>{
                  detail::indirect_zip_fn_{}, std::move(rngs)...}
            {}
        };

        namespace view
        {
            struct zip_fn
            {
                template<typename ...Rngs>
                using Concept = meta::and_<InputRange<Rngs>...>;

                template<typename...Rngs,
                    CONCEPT_REQUIRES_(Concept<Rngs...>())>
                zip_view<all_t<Rngs>...> operator()(Rngs &&... rngs) const
                {
                    CONCEPT_ASSERT(meta::and_<Range<Rngs>...>());
                    return zip_view<all_t<Rngs>...>{all(static_cast<Rngs&&>(rngs))...};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename...Rngs,
                    CONCEPT_REQUIRES_(!Concept<Rngs...>())>
                void operator()(Rngs &&...) const
                {
                    CONCEPT_ASSERT_MSG(meta::and_<InputRange<Rngs>...>(),
                        "All of the objects passed to view::zip must model the InputRange "
                        "concept");
                }
            #endif
            };

            /// \relates zip_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(zip_fn, zip)
        }
        /// @}
    }
}

RANGES_RE_ENABLE_WARNINGS

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::zip_view)

#endif
