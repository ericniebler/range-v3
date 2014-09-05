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
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/optional.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T, bool IsSemiRegular = SemiRegular<T>()>
            struct value_wrapper
            {
            private:
                optional<T> t_;
            public:
                value_wrapper() = default;
                value_wrapper(T f)
                  : t_(std::move(f))
                {}
                T & get()
                {
                    RANGES_ASSERT(!!t_);
                    return *t_;
                }
                T const & get() const
                {
                    RANGES_ASSERT(!!t_);
                    return *t_;
                }
                operator T &()
                {
                    return get();
                }
                operator T const &() const
                {
                    return get();
                }
                template<typename...Args>
                auto operator()(Args &&...args) ->
                    decltype(std::declval<T &>()(std::forward<Args>(args)...))
                {
                    return get()(std::forward<Args>(args)...);
                }
                template<typename...Args>
                auto operator()(Args &&...args) const ->
                    decltype(std::declval<T const &>()(std::forward<Args>(args)...))
                {
                    return get()(std::forward<Args>(args)...);
                }
            };

            template<typename T>
            struct reference_wrapper
            {
            private:
                T *t_;
            public:
                reference_wrapper() = default;
                reference_wrapper(T &t)
                  : t_(std::addressof(t))
                {}
                T & get() const
                {
                    RANGES_ASSERT(nullptr != t_);
                    return *t_;
                }
                operator T &() const
                {
                    return get();
                }
                template<typename ...Args>
                auto operator()(Args &&...args) const ->
                    decltype((*t_)(std::forward<Args>(args)...))
                {
                    return (*t_)(std::forward<Args>(args)...);
                }
            };

            template<typename Fun, typename SinglePass>
            struct transform_adaptor : default_adaptor
            {
            private:
                Fun fun_;
            public:
                using single_pass = SinglePass;
                transform_adaptor() = default;
                transform_adaptor(Fun fun)
                  : fun_(std::move(fun))
                {}
                template<typename Cur>
                auto current(Cur const &pos) const ->
                    decltype(fun_(pos.current()))
                {
                    return fun_(pos.current());
                }
            };
        }

        template<typename Rng, typename Fun>
        struct transformed_view
          : range_adaptor<transformed_view<Rng, Fun>, Rng>
        {
        private:
            using reference_t = result_of_t<invokable_t<Fun> const(range_value_t<Rng>)>;
            friend range_core_access;
            using view_fun_t = detail::conditional_t<(bool) SemiRegular<invokable_t<Fun>>(),
                invokable_t<Fun>, detail::value_wrapper<invokable_t<Fun>>>;
            using adaptor_fun_t = detail::conditional_t<(bool) SemiRegular<invokable_t<Fun>>(),
                view_fun_t, detail::reference_wrapper<view_fun_t const>>;
            view_fun_t fun_;
            // Forward ranges must always return references. If the result of calling the function
            // is not a reference, this range is input-only.
            using single_pass = detail::or_t<
                std::integral_constant<bool, SinglePass<range_iterator_t<Rng>>()>,
                detail::not_t<std::is_reference<reference_t>>>;
            using adaptor_t = detail::transform_adaptor<adaptor_fun_t, single_pass>;
            using use_sentinel_t = detail::or_t<detail::not_t<BoundedIterable<Rng>>, single_pass>;
            adaptor_t begin_adaptor() const
            {
                return adaptor_t{fun_};
            }
            CONCEPT_REQUIRES(use_sentinel_t())
            default_adaptor end_adaptor() const
            {
                return {};
            }
            CONCEPT_REQUIRES(!use_sentinel_t())
            adaptor_t end_adaptor() const
            {
                return adaptor_t{fun_};
            }
        public:
            transformed_view() = default;
            transformed_view(Rng && rng, Fun fun)
              : range_adaptor_t<transformed_view>(std::forward<Rng>(rng))
              , fun_(invokable(std::move(fun)))
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return this->base_size();
            }
        };

        namespace view
        {
            struct transform_fn : bindable<transform_fn>
            {
            private:
                template<typename Fun>
                struct transformer1 : pipeable<transformer1<Fun>>
                {
                private:
                    Fun fun_;
                public:
                    transformer1(Fun fun)
                      : fun_(std::move(fun))
                    {}
                    template<typename Rng, typename This>
                    static transformed_view<Rng, Fun>
                    pipe(Rng && rng, This && this_)
                    {
                        return {std::forward<Rng>(rng), std::forward<This>(this_).fun_};
                    }
                };
            public:
                ///
                template<typename Rng, typename Fun>
                static transformed_view<Rng, Fun>
                invoke(transform_fn, Rng && rng, Fun fun)
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    CONCEPT_ASSERT(Invokable<Fun, range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), std::move(fun)};
                }

                /// \overload
                template<typename Fun>
                static transformer1<Fun> invoke(transform_fn, Fun fun)
                {
                    return {std::move(fun)};
                }
            };

            RANGES_CONSTEXPR transform_fn transform {};
        }
    }
}

#endif
