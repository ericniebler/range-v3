/// \file
// Range v3 library
//
//  Copyright Mitsutaka Takeda 2018-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGE_V3_VIEW_EXCLUSIVE_SCAN_HPP
#define RANGE_V3_VIEW_EXCLUSIVE_SCAN_HPP


#include <range/v3/utility/concepts.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/view/view.hpp>

namespace ranges {
    inline namespace v3 {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename T, typename Fun>
        struct exclusive_scan_view
          : view_adaptor<exclusive_scan_view<Rng, T, Fun>, Rng>
        {
        private:
            friend range_access;
            semiregular_t<T> init_;
            semiregular_t<Fun> fun_;
            using single_pass = SinglePass<iterator_t<Rng>>;
            using use_sentinel_t = meta::or_<meta::not_<BoundedRange<Rng>>, single_pass>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                using exclusive_scan_view_t = meta::const_if_c<IsConst, exclusive_scan_view>;
                T sum_;
                exclusive_scan_view_t *rng_;

                T move_or_copy_init(std::false_type) {
                    return rng_->init_;
                }

                // If the base range is single-pass, we can move the init value.
                T move_or_copy_init(std::true_type) {
                    return std::move(rng_->init_);
                }
            public:
                using single_pass = exclusive_scan_view::single_pass;
                adaptor() = default;
                adaptor(exclusive_scan_view_t &rng)
                  : rng_(&rng)
                {}
                iterator_t<Rng> begin(exclusive_scan_view_t &)
                {
                    sum_ = move_or_copy_init(single_pass{});
                    return ranges::begin(rng_->base());
                }
                T read(iterator_t<Rng>) const
                {
                    return sum_;
                }
                void next(iterator_t<Rng> &it)
                {
                    RANGES_EXPECT(it != ranges::end(rng_->base()));

                    sum_ = invoke(rng_->fun_, std::move(sum_), *it);
                    ++it;
                }
                void prev() = delete;
            };

            adaptor<false> begin_adaptor()
            {
                return {*this};
            }
            meta::if_<use_sentinel_t, adaptor_base, adaptor<false>> end_adaptor()
            {
                return {*this};
            }
            CONCEPT_REQUIRES(Range<Rng const>() &&
                             Invocable<Fun const&, range_common_reference_t<Rng>,
                                     range_common_reference_t<Rng>>())
            adaptor<true> begin_adaptor() const
            {
                return {*this};
            }
            CONCEPT_REQUIRES(Range<Rng const>() &&
                             Invocable<Fun const&, range_common_reference_t<Rng>,
                                     range_common_reference_t<Rng>>())
            meta::if_<use_sentinel_t, adaptor_base, adaptor<true>> end_adaptor() const
            {
                return {*this};
            }

        public:
            exclusive_scan_view() = default;
            exclusive_scan_view(Rng rng, T init, Fun fun)
              : exclusive_scan_view::view_adaptor{std::move(rng)}
              , init_(std::move(init))
              , fun_(std::move(fun))
            {}
            CONCEPT_REQUIRES(SizedRange<Rng const>())
            range_size_type_t<Rng> size() const
            {
                return ranges::size(this->base());
            }
            CONCEPT_REQUIRES(SizedRange<Rng>())
            range_size_type_t<Rng> size()
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct exclusive_scan_fn
            {
            private:
                friend view_access;
                template<typename T, typename Fun = plus>
                static auto bind(exclusive_scan_fn exclusive_scan, T init, Fun fun = {})
                RANGES_DECLTYPE_AUTO_RETURN
                (
                        make_pipeable(std::bind(exclusive_scan, std::placeholders::_1,
                                                std::move(init),
                                                protect(std::move(fun))))
                )
            public:
                template<typename Rng, typename T, typename Fun>
                using Concept = meta::and_<
                        InputRange<Rng>,
                        MoveConstructible<T>,
                        IndirectInvocable<Fun, iterator_t<Rng>, iterator_t<Rng>>,
                        IndirectInvocable<Fun, iterator_t<Rng>, T*>>;

                template<typename Rng, typename T, typename Fun,
                        CONCEPT_REQUIRES_(Concept<Rng, T, Fun>())>
                exclusive_scan_view<all_t<Rng>, T, Fun> operator()(Rng && rng, T init, Fun fun) const
                {
                    return {all(static_cast<Rng&&>(rng)), std::move(init), std::move(fun)};
                }
#ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T, typename Fun,
                        CONCEPT_REQUIRES_(!Concept<Rng, T, Fun>())>
                void operator()(Rng &&, T, Fun) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                                       "The first argument passed to view::exclusive_scan must be a model of the "
                                               "InputRange concept.");
                    CONSEPT_ASSERT_MSG(MoveConstructible<T>(),
                                       "The second argument passed to view::exclusive_scan must be a model of the"
                                               "MoveConstructible concept.");
                    CONCEPT_ASSERT_MSG(IndirectInvocable<Fun, iterator_t<Rng>,
                                               iterator_t<Rng>>(),
                                       "The third argument passed to view::exclusive_scan must be callable with "
                                               "two values from the range passed as the first argument.");
                    CONCEPT_ASSERT_MSG(ConvertibleTo<result_of_t<Fun&(T, range_common_reference_t<Rng> &&)>, T>(),
                                       "The return type of the function passed to view::exclusive_scan must be "
                                               "convertible to the type of init value.");
                }
#endif
            };

            /// \relates exclusive_scan_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<exclusive_scan_fn>, exclusive_scan)
        }
        /// @}
    }
}
#endif //RANGE_V3_VIEW_EXCLUSIVE_SCAN_HPP
