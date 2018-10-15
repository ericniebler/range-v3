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

#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/view/view.hpp>

namespace ranges {
    inline namespace v3 {
        /// \cond
        namespace detail {
            template<typename Rng, typename T, typename Fun>
            using ExclusiveScanConstraint3 =
                Assignable<T &, result_of_t<Fun &(T &&, range_reference_t<Rng> &&)>>;

            template<typename Rng, typename T, typename Fun>
            using ExclusiveScanConstraint2 = meta::and_<
                Invocable<Fun &, T, range_reference_t<Rng>>,
                meta::defer<ExclusiveScanConstraint3, Rng, T, Fun>>;
        }
        /// \endcond

        template<typename Rng, typename T, typename Fun>
        using ExclusiveScanConstraint = meta::and_<
            InputRange<Rng>,
            CopyConstructible<T>,
            meta::defer<detail::ExclusiveScanConstraint2, Rng, T, Fun>>;

        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename T, typename Fun>
        struct exclusive_scan_view
          : view_adaptor<exclusive_scan_view<Rng, T, Fun>, Rng>
        {
        private:
            friend range_access;
            CONCEPT_ASSERT(ExclusiveScanConstraint<Rng, T, Fun>());

            semiregular_t<T> init_;
            semiregular_t<Fun> fun_;
            using single_pass = SinglePass<iterator_t<Rng>>;
            using use_sentinel_t = meta::or_<meta::not_<BoundedRange<Rng>>, single_pass>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                friend struct adaptor<!IsConst>;
                using exclusive_scan_view_t = meta::const_if_c<IsConst, exclusive_scan_view>;
                using CRng = meta::const_if_c<IsConst, Rng>;
                semiregular_t<T> sum_;
                exclusive_scan_view_t *rng_;

                auto move_or_copy_init(std::false_type) noexcept
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    (rng_->init_)
                )

                // If the base range is single-pass, we can move the init value.
                auto move_or_copy_init(std::true_type) noexcept
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    std::move(rng_->init_)
                )
            public:
                using single_pass = exclusive_scan_view::single_pass;
                adaptor() = default;
                adaptor(exclusive_scan_view_t &rng)
                  : rng_(&rng)
                {}
                template<bool Other,
                    CONCEPT_REQUIRES_(IsConst && !Other)>
                adaptor(adaptor<Other> that)
                  : rng_(that.rng_)
                {}
                iterator_t<CRng> begin(exclusive_scan_view_t &)
                {
                    sum_ = move_or_copy_init(single_pass{});
                    return ranges::begin(rng_->base());
                }
                T read(iterator_t<CRng> const &) const
                {
                    return sum_;
                }
                void next(iterator_t<CRng> &it)
                {
                    RANGES_EXPECT(it != ranges::end(rng_->base()));
                    sum_ = invoke(rng_->fun_, static_cast<T &&>(std::move(sum_)), *it);
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
            CONCEPT_REQUIRES(ExclusiveScanConstraint<Rng const, T, Fun const>())
            adaptor<true> begin_adaptor() const
            {
                return {*this};
            }
            CONCEPT_REQUIRES(ExclusiveScanConstraint<Rng const, T, Fun const>())
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
                                            std::move(init), protect(std::move(fun))))
                )
            public:
                template<typename Rng, typename T, typename Fun = plus,
                    CONCEPT_REQUIRES_(ExclusiveScanConstraint<Rng, T, Fun>())>
                exclusive_scan_view<all_t<Rng>, T, Fun>
                operator()(Rng &&rng, T init, Fun fun = Fun{}) const
                {
                    return {all(static_cast<Rng &&>(rng)), std::move(init), std::move(fun)};
                }

#ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename T, typename Fun = plus,
                    CONCEPT_REQUIRES_(!ExclusiveScanConstraint<Rng, T, Fun>())>
                void operator()(Rng &&, T, Fun = Fun{}) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument passed to view::exclusive_scan must be a model of the "
                        "InputRange concept.");
                    CONSEPT_ASSERT_MSG(CopyConstructible<T>(),
                        "The second argument passed to view::exclusive_scan must be a model of the "
                        "CopyConstructible concept.");
                    CONCEPT_ASSERT_MSG(Invocable<Fun &, T, range_reference_t<Rng>>(),
                        "The third argument passed to view::exclusive_scan must be invokable with "
                        "the init value passed as the first argument, and "
                        "a value from the range passed as the second argument.");
                    CONCEPT_ASSERT_MSG(
                        Assignable<T &, result_of_t<Fun &(T &&, range_reference_t<Rng> &&)>>(),
                        "The result of invoking the third argument must be assignable to the init "
                        "value.");
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
