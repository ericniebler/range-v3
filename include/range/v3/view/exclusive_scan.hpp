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
            CPP_def
            (
                template(typename Rng, typename T, typename Fun)
                concept ExclusiveScanConstraint3,
                    Assignable<T &, invoke_result_t<Fun &, T, range_reference_t<Rng>>>
            );

            CPP_def
            (
                template(typename Rng, typename T, typename Fun)
                concept ExclusiveScanConstraint2,
                    Invocable<Fun &, T, range_reference_t<Rng>> &&
                    ExclusiveScanConstraint3<Rng, T, Fun>
            );
        }
        /// \endcond

        CPP_def
        (
            template(typename Rng, typename T, typename Fun)
            concept ExclusiveScanConstraint,
                InputRange<Rng> &&
                CopyConstructible<T> &&
                detail::ExclusiveScanConstraint2<Rng, T, Fun>
        );

        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename T, typename Fun>
        struct exclusive_scan_view
          : view_adaptor<exclusive_scan_view<Rng, T, Fun>, Rng>
        {
        private:
            friend range_access;
            CPP_assert(ExclusiveScanConstraint<Rng, T, Fun>);

            semiregular_t<T> init_;
            semiregular_t<Fun> fun_;
            using single_pass = meta::bool_<SinglePass<iterator_t<Rng>>>;
            using use_sentinel_t = meta::bool_<!BoundedRange<Rng> || single_pass{}>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                using exclusive_scan_view_t = meta::const_if_c<IsConst, exclusive_scan_view>;
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
                iterator_t<Rng> begin(exclusive_scan_view_t &)
                {
                    sum_ = move_or_copy_init(single_pass{});
                    return ranges::begin(rng_->base());
                }
                T read(iterator_t<Rng> const &) const
                {
                    return sum_;
                }
                void next(iterator_t<Rng> &it)
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
            CPP_member
            auto begin_adaptor() const -> CPP_ret(adaptor<true>)(
                requires ExclusiveScanConstraint<Rng const, T, Fun const>)
            {
                return {*this};
            }
            CPP_member
            auto end_adaptor() const ->
                CPP_ret(meta::if_<use_sentinel_t, adaptor_base, adaptor<true>>)(
                    requires ExclusiveScanConstraint<Rng const, T, Fun const>)
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
            CPP_member
            auto size() const -> CPP_ret(range_size_type_t<Rng>)(
                requires SizedRange<Rng const>)
            {
                return ranges::size(this->base());
            }
            CPP_member
            auto size() -> CPP_ret(range_size_type_t<Rng>)(
                requires SizedRange<Rng>)
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
                {
                    return make_pipeable(std::bind(exclusive_scan, std::placeholders::_1,
                        std::move(init), protect(std::move(fun))));
                }
            public:
                CPP_template(typename Rng, typename T, typename Fun = plus)(
                    requires ExclusiveScanConstraint<Rng, T, Fun>)
                exclusive_scan_view<all_t<Rng>, T, Fun>
                operator()(Rng &&rng, T init, Fun fun = Fun{}) const
                {
                    return {all(static_cast<Rng &&>(rng)), std::move(init), std::move(fun)};
                }

#ifndef RANGES_DOXYGEN_INVOKED
                CPP_template(typename Rng, typename T, typename Fun = plus)(
                    requires not ExclusiveScanConstraint<Rng, T, Fun>)
                void operator()(Rng &&, T, Fun = Fun{}) const
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The first argument passed to view::exclusive_scan must be a model of the "
                        "InputRange concept.");
                    CPP_assert_msg(CopyConstructible<T>,
                        "The second argument passed to view::exclusive_scan must be a model of the "
                        "CopyConstructible concept.");
                    CPP_assert_msg(Invocable<Fun &, T, range_reference_t<Rng>>,
                        "The third argument passed to view::exclusive_scan must be invokable with "
                        "the init value passed as the first argument, and "
                        "a value from the range passed as the second argument.");
                    CPP_assert_msg(
                        Assignable<T &, invoke_result_t<Fun &, T, range_reference_t<Rng>>>,
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
