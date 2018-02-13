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
                CONCEPT_template(typename ...Its)(
                    requires meta::and_<Readable<Its>...>() && sizeof...(Its) != 2)
                (attribute([[noreturn]]) std::tuple<value_type_t<Its>...>)
                operator()(copy_tag, Its...) const
                {
                    RANGES_EXPECT(false);
                }

                // tuple reference
                CONCEPT_template(typename ...Its)(
                    requires meta::and_<Readable<Its>...>() && sizeof...(Its) != 2)
                (common_tuple<reference_t<Its>...>)
                operator()(Its const &...its) const
                    noexcept(meta::and_c<noexcept(reference_t<Its>(*its))...>::value)
                {
                    return common_tuple<reference_t<Its>...>{*its...};
                }

                // tuple rvalue reference
                CONCEPT_template(typename ...Its)(
                    requires meta::and_<Readable<Its>...>() && sizeof...(Its) != 2)
                (common_tuple<rvalue_reference_t<Its>...>)
                operator()(move_tag, Its const &...its) const
                    noexcept(meta::and_c<
                        noexcept(rvalue_reference_t<Its>(iter_move(its)))...>::value)
                {
                    return common_tuple<rvalue_reference_t<Its>...>{iter_move(its)...};
                }

                // pair value
                CONCEPT_template(typename It1, typename It2)(
                    requires Readable<It1>() && Readable<It2>())
                (attribute([[noreturn]]) std::pair<value_type_t<It1>, value_type_t<It2>>)
                operator()(copy_tag, It1, It2) const
                {
                    RANGES_EXPECT(false);
                }

                // pair reference
                CONCEPT_template(typename It1, typename It2)(
                    requires Readable<It1>() && Readable<It2>())
                (common_pair<reference_t<It1>, reference_t<It2>>)
                operator()(It1 const &it1, It2 const &it2) const
                    noexcept(noexcept(reference_t<It1>(*it1)) &&
                             noexcept(reference_t<It2>(*it2)))
                {
                    return common_pair<reference_t<It1>, reference_t<It2>>{*it1, *it2};
                }

                // pair rvalue reference
                CONCEPT_template(typename It1, typename It2)(
                    requires Readable<It1>() && Readable<It2>())
                (common_pair<rvalue_reference_t<It1>, rvalue_reference_t<It2>>)
                operator()(move_tag, It1 const &it1, It2 const &it2) const
                    noexcept(noexcept(rvalue_reference_t<It1>(iter_move(it1))) &&
                             noexcept(rvalue_reference_t<It2>(iter_move(it2))))
                {
                    return common_pair<rvalue_reference_t<It1>, rvalue_reference_t<It2>>{
                        iter_move(it1), iter_move(it2)};
                }
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
                CONCEPT_def
                (
                    template(typename ...Rngs)
                    (concept Concept)(Rngs...),
                        True<meta::strict_and<InputRange<Rngs>...>>()
                );

                CONCEPT_template(typename...Rngs)(
                    requires Concept<Rngs...>())
                (zip_view<all_t<Rngs>...>) operator()(Rngs &&... rngs) const
                {
                    CONCEPT_assert(meta::and_<Range<Rngs>...>());
                    return zip_view<all_t<Rngs>...>{all(static_cast<Rngs &&>(rngs))...};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                CONCEPT_template(typename...Rngs)(
                    requires !Concept<Rngs...>())
                (void) operator()(Rngs &&...) const
                {
                    CONCEPT_assert_msg(meta::and_<InputRange<Rngs>...>(),
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
