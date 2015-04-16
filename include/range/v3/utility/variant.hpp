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

#ifndef RANGES_V3_UTILITY_VARIANT_HPP
#define RANGES_V3_UTILITY_VARIANT_HPP

#include <new>
#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/addressof.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            using add_ref_t =
                meta::quote_trait<std::add_lvalue_reference>;

            using add_cref_t =
                meta::compose<meta::quote_trait<std::add_lvalue_reference>, meta::quote_trait<std::add_const>>;
        }
        /// \endcond

        template<typename...Ts>
        struct tagged_variant;

        template<std::size_t N, typename Var>
        struct tagged_variant_element;

        template<std::size_t N, typename Var>
        using tagged_variant_element_t = typename tagged_variant_element<N, Var>::type;

        /// \cond
        namespace detail
        {
            template<typename Fun, typename T, std::size_t N,
                typename = decltype(std::declval<Fun>()(std::declval<T &>(), meta::size_t<N>{}))>
            RANGES_RELAXED_CONSTEXPR
            void apply_if(Fun &&fun, T &t, meta::size_t<N> u)
            {
                std::forward<Fun>(fun)(t, u);
            }

            inline void apply_if(any, any, any)
            {
                RANGES_ASSERT(false);
            }

            template<typename...List>
            union variant_data_non_trivial;

            template<>
            union variant_data_non_trivial<>
            {
                void move(std::size_t, variant_data_non_trivial &&) const
                {
                    RANGES_ASSERT(false);
                }
                void copy(std::size_t, variant_data_non_trivial const &) const
                {
                    RANGES_ASSERT(false);
                }
                bool equal(std::size_t, variant_data_non_trivial const &) const
                {
                    RANGES_ASSERT(false);
                    return true;
                }
                template<typename Fun, std::size_t N = 0>
                void apply(std::size_t, Fun &&, meta::size_t<N> = meta::size_t<N>{}) const
                {
                    RANGES_ASSERT(false);
                }
                ~variant_data_non_trivial() = default;
            };

            template<typename T, typename ...Ts>
            union variant_data_non_trivial<T, Ts...>
            {
            private:
                template<typename...Us>
                friend union variant_data;
                using head_t = decay_t<meta::if_<std::is_reference<T>, ref_t<T &>, T>>;
                using tail_t = variant_data_non_trivial<Ts...>;

                head_t head;
                tail_t tail;

                template<typename This, typename Fun, std::size_t N>
                static void apply_(This &this_, std::size_t n, Fun &&fun, meta::size_t<N> u)
                {
                    if(0 == n)
                        detail::apply_if(detail::forward<Fun>(fun), this_.head, u);
                    else
                        this_.tail.apply(n - 1, detail::forward<Fun>(fun), meta::size_t<N + 1>{});
                }
            public:
                variant_data_non_trivial() {}
                // BUGBUG in-place construction?
                template<typename U,
                    meta::if_<std::is_constructible<head_t, U>, int> = 0>
                RANGES_RELAXED_CONSTEXPR
                variant_data_non_trivial(meta::size_t<0>, U &&u)
                  : head(std::forward<U>(u))
                {}
                template<std::size_t N, typename U,
                         meta::if_c<0 != N && std::is_constructible<tail_t, meta::size_t<N - 1>, U>::value, int> = 0>
                RANGES_RELAXED_CONSTEXPR variant_data_non_trivial(meta::size_t<N>, U &&u)
                  : tail{meta::size_t<N - 1>{}, std::forward<U>(u)}
                {}
                ~variant_data_non_trivial() {}
                void move(std::size_t n, variant_data_non_trivial &&that)
                {
                    if(n == 0)
                        ::new(static_cast<void *>(&head)) head_t(std::move(that).head);
                    else
                        tail.move(n - 1, std::move(that).tail);
                }
                void copy(std::size_t n, variant_data_non_trivial const &that)
                {
                    if(n == 0)
                        ::new(static_cast<void *>(&head)) head_t(that.head);
                    else
                        tail.copy(n - 1, that.tail);
                }
                template<typename U, typename...Us>
                RANGES_RELAXED_CONSTEXPR
                bool equal(std::size_t n, variant_data_non_trivial<U, Us...> const &that) const
                {
                    if(n == 0)
                        return head == that.head;
                    else
                        return tail.equal(n - 1, that.tail);
                }
                template<typename Fun, std::size_t N = 0>
                RANGES_RELAXED_CONSTEXPR
                void apply(std::size_t n, Fun &&fun, meta::size_t<N> u = meta::size_t<N>{})
                {
                    variant_data_non_trivial::apply_(*this, n, std::forward<Fun>(fun), u);
                }
                template<typename Fun, std::size_t N = 0>
                RANGES_RELAXED_CONSTEXPR
                void apply(std::size_t n, Fun &&fun, meta::size_t<N> u = meta::size_t<N>{}) const
                {
                    variant_data_non_trivial::apply_(*this, n, std::forward<Fun>(fun), u);
                }
            };

            struct uninitialized_t_ {};

            template<typename...List>
            union variant_data_trivial;

            template<>
            union variant_data_trivial<>
            {
                void move(std::size_t, variant_data_trivial &&) const
                {
                    RANGES_ASSERT(false);
                }
                void copy(std::size_t, variant_data_trivial const &) const
                {
                    RANGES_ASSERT(false);
                }
                bool equal(std::size_t, variant_data_trivial const &) const
                {
                    RANGES_ASSERT(false);
                    return true;
                }
                template<typename Fun, std::size_t N = 0>
                void apply(std::size_t, Fun &&, meta::size_t<N> = meta::size_t<N>{}) const
                {
                    RANGES_ASSERT(false);
                }
                ~variant_data_trivial() = default;
            };

            template<typename T, typename ...Ts>
            union variant_data_trivial<T, Ts...>
            {
            private:
                template<typename...Us>
                friend union variant_data_trivial;
                using head_t = decay_t<meta::if_<std::is_reference<T>, ref_t<T &>, T>>;
                using tail_t = variant_data_trivial<Ts...>;
                using trivially_copy_assignable_types = meta::all_of<meta::list<T, Ts...>, meta::quote<std::is_trivially_copy_assignable>>;

                head_t head;
                tail_t tail;
                uninitialized_t_ uninitialized;

                template<typename This, typename Fun, std::size_t N>
                RANGES_RELAXED_CONSTEXPR
                static void apply_(This &this_, std::size_t n, Fun &&fun, meta::size_t<N> u)
                {
                    if(0 == n)
                        detail::apply_if(detail::forward<Fun>(fun), this_.head, u);
                    else
                        this_.tail.apply(n - 1, detail::forward<Fun>(fun), meta::size_t<N + 1>{});
                }
            public:

                RANGES_RELAXED_CONSTEXPR variant_data_trivial() : uninitialized{}
                {}
                // BUGBUG in-place construction?
                template<typename U,
                    meta::if_<std::is_constructible<head_t, U>, int> = 0>
                RANGES_RELAXED_CONSTEXPR
                variant_data_trivial(meta::size_t<0>, U &&u)
                  : head(std::forward<U>(u))
                {}
                template<std::size_t N, typename U,
                         meta::if_c<0 != N && std::is_constructible<tail_t, meta::size_t<N - 1>, U>::value, int> = 0>
                RANGES_RELAXED_CONSTEXPR variant_data_trivial(meta::size_t<N>, U &&u)
                  : tail{meta::size_t<N - 1>{}, std::forward<U>(u)}
                {}
                ~variant_data_trivial() = default;
                CONCEPT_REQUIRES(trivially_copy_assignable_types{})
                RANGES_RELAXED_CONSTEXPR void move(std::size_t n, variant_data_trivial &&that)
                {
                    if(n == 0)
                        (*this) = variant_data_trivial(meta::size_t<0>{}, std::move(that).head);
                    else
                        tail.move(n - 1, std::move(that).tail);
                }
                CONCEPT_REQUIRES(!trivially_copy_assignable_types{})
                void move(std::size_t n, variant_data_trivial &&that)
                {
                    if(n == 0)
                        ::new(static_cast<void *>(&head)) head_t(std::move(that).head);
                    else
                        tail.move(n - 1, std::move(that).tail);
                }
                CONCEPT_REQUIRES(trivially_copy_assignable_types{})
                RANGES_RELAXED_CONSTEXPR void copy(std::size_t n, variant_data_trivial const &that)
                {
                    if(n == 0)
                        (*this) = variant_data_trivial(meta::size_t<0>{}, that.head);
                    else
                        tail.copy(n - 1, that.tail);
                }
                CONCEPT_REQUIRES(!trivially_copy_assignable_types{})
                void copy(std::size_t n, variant_data_trivial const &that)
                {
                    if(n == 0)
                        ::new(static_cast<void *>(&head)) head_t(that.head);
                    else
                        tail.copy(n - 1, that.tail);
                }
                template<typename U, typename...Us>
                RANGES_RELAXED_CONSTEXPR
                bool equal(std::size_t n, variant_data_trivial<U, Us...> const &that) const
                {
                    if(n == 0)
                        return head == that.head;
                    else
                        return tail.equal(n - 1, that.tail);
                }
                template<typename Fun, std::size_t N = 0>
                RANGES_RELAXED_CONSTEXPR
                void apply(std::size_t n, Fun &&fun, meta::size_t<N> u = meta::size_t<N>{})
                {
                    variant_data_trivial::apply_(*this, n, std::forward<Fun>(fun), u);
                }
                template<typename Fun, std::size_t N = 0>
                RANGES_RELAXED_CONSTEXPR
                void apply(std::size_t n, Fun &&fun, meta::size_t<N> u = meta::size_t<N>{}) const
                {
                    variant_data_trivial::apply_(*this, n, std::forward<Fun>(fun), u);
                }
            };

        template<typename... Ts>
        using variant_data
        = meta::if_<meta::all_of<meta::list<Ts...>, meta::quote<std::is_trivially_destructible>>,
                    variant_data_trivial<Ts...>, variant_data_non_trivial<Ts...>>;

            struct variant_core_access
            {
                template<typename...Ts>
                static RANGES_RELAXED_CONSTEXPR
                variant_data<Ts...> &data(tagged_variant<Ts...> &var)
                {
                    return var.data_;
                }

                template<typename...Ts>
                static RANGES_RELAXED_CONSTEXPR
                variant_data<Ts...> const &data(tagged_variant<Ts...> const &var)
                {
                    return var.data_;
                }

                template<typename...Ts>
                static RANGES_RELAXED_CONSTEXPR
                variant_data<Ts...> &&data(tagged_variant<Ts...> &&var)
                {
                    return std::move(var).data_;
                }
            };

            struct delete_fun
            {
                template<typename T>
                void operator()(T const &t) const
                {
                    t.~T();
                }
            };

            struct assert_otherwise
            {
                static void assert_false(any) { RANGES_ASSERT(false); }
                using fun_t = void(*)(any);
                operator fun_t() const
                {
                    return &assert_false;
                }
            };

            // Is there a less dangerous way?
            template<typename T>
            struct construct_fun : assert_otherwise
            {
            private:
                T &&t_;
            public:
                RANGES_RELAXED_CONSTEXPR construct_fun(T &&t)
                  : t_(std::forward<T>(t))
                {}
                template<typename U,
                    meta::if_<std::is_constructible<U, T>, int> = 0>
                RANGES_RELAXED_CONSTEXPR void operator()(U &u) const
                {
                    ::new((void*)addressof(u)) U(std::forward<T>(t_));
                }
            };

            template<typename T>
            struct get_fun : assert_otherwise
            {
            private:
                T *&t_;
            public:
                RANGES_RELAXED_CONSTEXPR get_fun(T *&t)
                  : t_(t)
                {}
                RANGES_RELAXED_CONSTEXPR void operator()(T &t) const
                {
                    t_ = ranges::addressof(t);
                }
            };

            template<typename Fun, typename Var = std::nullptr_t>
            struct apply_visitor
            {
            private:
                Var &var_;
                Fun fun_;
                template<typename T, std::size_t N>
                RANGES_RELAXED_CONSTEXPR
                void apply_(T &&t, meta::size_t<N> n, std::true_type) const
                {
                    fun_(std::forward<T>(t), n);
                    var_.template set<N>(nullptr);
                }
                template<typename T, std::size_t N>
                RANGES_RELAXED_CONSTEXPR
                void apply_(T &&t, meta::size_t<N> n, std::false_type) const
                {
                    var_.template set<N>(fun_(std::forward<T>(t), n));
                }
            public:
                RANGES_RELAXED_CONSTEXPR apply_visitor(Fun &&fun, Var &var)
                  : var_(var), fun_(std::forward<Fun>(fun))
                {}
                template<typename T, std::size_t N>
                RANGES_RELAXED_CONSTEXPR
                void operator()(T &&t, meta::size_t<N> u) const
                {
                    using result_t = result_of_t<Fun(T &&, meta::size_t<N>)>;
                    this->apply_(std::forward<T>(t), u, std::is_void<result_t>{});
                }
            };

            template<typename Fun>
            struct apply_visitor<Fun, std::nullptr_t>
            {
            private:
                Fun fun_;
            public:
                RANGES_RELAXED_CONSTEXPR apply_visitor(Fun &&fun)
                  : fun_(std::forward<Fun>(fun))
                {}
                template<typename T, std::size_t N>
                RANGES_RELAXED_CONSTEXPR
                void operator()(T &&t, meta::size_t<N> n) const
                {
                    fun_(std::forward<T>(t), n);
                }
            };

            template<typename Fun>
            struct ignore2nd
            {
            private:
                Fun fun_;
            public:
                RANGES_RELAXED_CONSTEXPR ignore2nd(Fun &&fun)
                  : fun_(std::forward<Fun>(fun))
                {}
                template<typename T, typename U>
                RANGES_RELAXED_CONSTEXPR auto operator()(T &&t, U &&) const ->
                    decltype(fun_(std::forward<T>(t)))
                {
                    return fun_(std::forward<T>(t));
                }
            };

            template<typename Fun>
            RANGES_RELAXED_CONSTEXPR
            apply_visitor<ignore2nd<Fun>> make_unary_visitor(Fun &&fun)
            {
                return {std::forward<Fun>(fun)};
            }
            template<typename Fun, typename Var>
            RANGES_RELAXED_CONSTEXPR
            apply_visitor<ignore2nd<Fun>, Var> make_unary_visitor(Fun &&fun, Var &var)
            {
                return{std::forward<Fun>(fun), var};
            }
            template<typename Fun>
            RANGES_RELAXED_CONSTEXPR
            apply_visitor<Fun> make_binary_visitor(Fun &&fun)
            {
                return{std::forward<Fun>(fun)};
            }
            template<typename Fun, typename Var>
            RANGES_RELAXED_CONSTEXPR
            apply_visitor<Fun, Var> make_binary_visitor(Fun &&fun, Var &var)
            {
                return{std::forward<Fun>(fun), var};
            }

            template<typename To, typename From>
            struct unique_visitor;

            template<typename ...To, typename ...From>
            struct unique_visitor<tagged_variant<To...>, tagged_variant<From...>>
            {
            private:
                tagged_variant<To...> &var_;
            public:
                RANGES_RELAXED_CONSTEXPR unique_visitor(tagged_variant<To...> &var)
                  : var_(var)
                {}
                template<typename T, std::size_t N>
                RANGES_RELAXED_CONSTEXPR
                void operator()(T &&t, meta::size_t<N>) const
                {
                    using E = meta::at_c<meta::list<From...>, N>;
                    using F = meta::find<meta::list<To...>, E>;
                    constexpr std::size_t M = sizeof...(To) - F::size();
                    var_.template set<M>(std::forward<T>(t));
                }
            };

            template<typename Fun, typename Types>
            using variant_result_t =
                meta::apply_list<
                    meta::quote<tagged_variant>,
                    meta::replace<
                        meta::transform<
                            Types,
                            meta::bind_front<meta::quote<concepts::Function::result_t>, Fun>>,
                        void,
                        std::nullptr_t>>;

            template<typename Fun, typename Types>
            using variant_result_i_t =
                meta::apply_list<
                    meta::quote<tagged_variant>,
                    meta::replace<
                        meta::transform<
                            Types,
                            meta::as_list<meta::make_index_sequence<Types::size()>>,
                            meta::bind_front<meta::quote<concepts::Function::result_t>, Fun>>,
                        void, std::nullptr_t>>;

            template<typename Fun>
            struct unwrap_ref_fun
            {
            private:
                Fun fun_;
            public:
                RANGES_RELAXED_CONSTEXPR unwrap_ref_fun(Fun &&fun)
                  : fun_(std::forward<Fun>(fun))
                {}
                template<typename T>
                RANGES_RELAXED_CONSTEXPR
                auto operator()(T &&t) const ->
                    decltype(fun_(unwrap_reference(std::forward<T>(t))))
                {
                    return fun_(unwrap_reference(std::forward<T>(t)));
                }
                template<typename T, std::size_t N>
                RANGES_RELAXED_CONSTEXPR
                auto operator()(T &&t, meta::size_t<N> n) const ->
                    decltype(fun_(unwrap_reference(std::forward<T>(t)), n))
                {
                    return fun_(unwrap_reference(std::forward<T>(t)), n);
                }
            };


            template<class T>
            struct tagged_variant_without_trivial_destructor {
                RANGES_RELAXED_CONSTEXPR tagged_variant_without_trivial_destructor() = default;
                RANGES_RELAXED_CONSTEXPR tagged_variant_without_trivial_destructor(tagged_variant_without_trivial_destructor const&) = default;
                RANGES_RELAXED_CONSTEXPR tagged_variant_without_trivial_destructor(tagged_variant_without_trivial_destructor &&) = default;
                RANGES_RELAXED_CONSTEXPR tagged_variant_without_trivial_destructor& operator=(tagged_variant_without_trivial_destructor const&) = default;
                RANGES_RELAXED_CONSTEXPR tagged_variant_without_trivial_destructor& operator=(tagged_variant_without_trivial_destructor &&) = default;

                ~tagged_variant_without_trivial_destructor() {
                    static_cast<T*>(this)->clear_();
                }
            };

            struct tagged_variant_with_trivial_destructor {
                RANGES_RELAXED_CONSTEXPR tagged_variant_with_trivial_destructor() = default;
                RANGES_RELAXED_CONSTEXPR tagged_variant_with_trivial_destructor(tagged_variant_with_trivial_destructor const&) = default;
                RANGES_RELAXED_CONSTEXPR tagged_variant_with_trivial_destructor(tagged_variant_with_trivial_destructor &&) = default;
                RANGES_RELAXED_CONSTEXPR tagged_variant_with_trivial_destructor& operator=(tagged_variant_with_trivial_destructor const&) { return *this; }
                RANGES_RELAXED_CONSTEXPR tagged_variant_with_trivial_destructor& operator=(tagged_variant_with_trivial_destructor &&) { return *this; };
                ~tagged_variant_with_trivial_destructor() = default;
            };
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{
        template<typename ...Ts>
        struct tagged_variant
        : meta::if_<meta::all_of<meta::list<Ts...>, meta::quote<std::is_trivially_destructible>>,
                    detail::tagged_variant_with_trivial_destructor,
                    detail::tagged_variant_without_trivial_destructor<tagged_variant<Ts...>>>
        {
        private:
            friend struct detail::tagged_variant_without_trivial_destructor<tagged_variant<Ts...>>;
            friend struct detail::variant_core_access;
            using types_t = meta::list<Ts...>;
            using data_t = detail::variant_data<Ts...>;
            using trivially_destructible_elements
                    = meta::all_of<types_t, meta::quote<std::is_trivially_destructible>>;
            std::size_t which_;
            data_t data_;

            void clear_()
            {
                if(is_valid())
                {
                    data_.apply(which_, detail::make_unary_visitor(detail::delete_fun{}));
                    which_ = (std::size_t)-1;
                }
            }
        public:
            RANGES_RELAXED_CONSTEXPR tagged_variant()
              : which_((std::size_t)-1), data_{}
            {}
            template<std::size_t N, typename U,
                meta::if_<std::is_constructible<data_t, meta::size_t<N>, U>, int> = 0>
            RANGES_RELAXED_CONSTEXPR tagged_variant(meta::size_t<N> n, U &&u)
              : which_(N), data_{n, detail::forward<U>(u)}
            {
                static_assert(N < sizeof...(Ts), "");
            }
            RANGES_RELAXED_CONSTEXPR tagged_variant(tagged_variant &&that)
              : tagged_variant{}
            {
                if(that.is_valid())
                {
                    data_.move(that.which_, std::move(that).data_);
                    which_ = that.which_;
                }
            }
            RANGES_RELAXED_CONSTEXPR tagged_variant(tagged_variant const &that)
              : tagged_variant{}
            {
                if(that.is_valid())
                {
                    data_.copy(that.which_, that.data_);
                    which_ = that.which_;
                }
            }
            CONCEPT_REQUIRES(!trivially_destructible_elements{})
            RANGES_RELAXED_CONSTEXPR tagged_variant &operator=(tagged_variant &&that)
            {
                clear_();
                if(that.is_valid())
                {
                    data_.move(that.which_, std::move(that).data_);
                    which_ = that.which_;
                }
                return *this;
            }
            CONCEPT_REQUIRES(trivially_destructible_elements{})
            RANGES_RELAXED_CONSTEXPR tagged_variant &operator=(tagged_variant &&that)
            {
                if(that.is_valid())
                {
                    data_.move(that.which_, std::move(that).data_);
                    which_ = that.which_;
                }
                return *this;
            }
            tagged_variant &operator=(tagged_variant const &that)
            {
                clear_();
                if(that.is_valid())
                {
                    data_.copy(that.which_, that.data_);
                    which_ = that.which_;
                }
                return *this;
            }
            ~tagged_variant() = default;
            static constexpr std::size_t size()
            {
                return sizeof...(Ts);
            }
            template<std::size_t N, typename U,
                meta::if_c<std::is_constructible<data_t, meta::size_t<N>, U>{}
            && trivially_destructible_elements{}, int> = 0>
            RANGES_RELAXED_CONSTEXPR void set(U &&u)
            {

                *this = tagged_variant(meta::size_t<N>{}, std::forward<U>(u));
                // // trivially destructible elements -> no need to clear_()
                // data_.apply(N, detail::make_unary_visitor(detail::construct_fun<U>{std::forward<U>(u)}));
                // which_ = N;
            }
            template<std::size_t N, typename U,
                     meta::if_c<std::is_constructible<data_t, meta::size_t<N>, U>{}
            && !trivially_destructible_elements{}, int> = 0>
            RANGES_RELAXED_CONSTEXPR void set(U &&u)
            {
                clear_();
                data_.apply(N, detail::make_unary_visitor(detail::construct_fun<U>{std::forward<U>(u)}));
                which_ = N;
            }
            RANGES_RELAXED_CONSTEXPR bool is_valid() const
            {
                return which() != (std::size_t)-1;
            }
            RANGES_RELAXED_CONSTEXPR explicit operator bool() const {
                return is_valid();
            }
            RANGES_RELAXED_CONSTEXPR std::size_t which() const
            {
                return which_;
            }
            template<typename Fun,
                typename Args = meta::transform<types_t, detail::add_ref_t>,
                typename Result = detail::variant_result_t<Fun, Args>>
            RANGES_RELAXED_CONSTEXPR Result apply(Fun &&fun)
            {
                Result res;
                data_.apply(which_, detail::make_unary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }
            template<typename Fun,
                typename Args = meta::transform<types_t, detail::add_cref_t>,
                typename Result = detail::variant_result_t<Fun, Args >>
            RANGES_RELAXED_CONSTEXPR Result apply(Fun &&fun) const
            {
                Result res;
                data_.apply(which_, detail::make_unary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }

            template<typename Fun,
                typename Args = meta::transform<types_t, detail::add_ref_t>,
                typename Result = detail::variant_result_i_t<Fun, Args>>
            RANGES_RELAXED_CONSTEXPR Result apply_i(Fun &&fun)
            {
                Result res;
                data_.apply(which_, detail::make_binary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }
            template<typename Fun,
                typename Args = meta::transform<types_t, detail::add_cref_t>,
                typename Result = detail::variant_result_i_t<Fun, Args >>
            RANGES_RELAXED_CONSTEXPR Result apply_i(Fun &&fun) const
            {
                Result res;
                data_.apply(which_, detail::make_binary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }
        };

        template<typename...Ts, typename...Us,
            CONCEPT_REQUIRES_(meta::and_c<(bool)EqualityComparable<Ts, Us>()...>::value)>
        RANGES_RELAXED_CONSTEXPR
        bool operator==(tagged_variant<Ts...> const &lhs, tagged_variant<Us...> const &rhs)
        {
            RANGES_ASSERT(lhs.which() < sizeof...(Ts));
            RANGES_ASSERT(rhs.which() < sizeof...(Us));
            return lhs.which() == rhs.which() &&
                detail::variant_core_access::data(lhs).equal(lhs.which(), detail::variant_core_access::data(rhs));
        }

        template<typename...Ts, typename...Us,
            CONCEPT_REQUIRES_(meta::and_c<(bool)EqualityComparable<Ts, Us>()...>::value)>
        RANGES_RELAXED_CONSTEXPR
        bool operator!=(tagged_variant<Ts...> const &lhs, tagged_variant<Us...> const &rhs)
        {
            return !(lhs == rhs);
        }

        template<std::size_t N, typename...Ts>
        struct tagged_variant_element<N, tagged_variant<Ts...>>
          : meta::if_<
                std::is_reference<meta::at_c<meta::list<Ts...>, N>>,
                meta::id<meta::at_c<meta::list<Ts...>, N>>,
                std::decay<meta::at_c<meta::list<Ts...>, N>>>
        {};

        ////////////////////////////////////////////////////////////////////////////////////////////
        // get
        template<std::size_t N, typename...Ts>
        RANGES_RELAXED_CONSTEXPR
        meta::apply<detail::add_ref_t, tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> &var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta::eval<std::remove_reference<
                    tagged_variant_element_t<N, tagged_variant<Ts...>>>>;
            using get_fun = detail::get_fun<elem_t>;
            elem_t *elem = nullptr;
            auto &data = detail::variant_core_access::data(var);
            data.apply(N, detail::make_unary_visitor(detail::unwrap_ref_fun<get_fun>{get_fun(elem)}));
            RANGES_ASSERT(elem != nullptr);
            return *elem;
        }

        template<std::size_t N, typename...Ts>
        RANGES_RELAXED_CONSTEXPR
        meta::apply<detail::add_cref_t, tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> const &var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta::apply<
                    meta::compose<
                        meta::quote_trait<std::remove_reference>,
                        meta::quote_trait<std::add_const>
                    >, tagged_variant_element_t<N, tagged_variant<Ts...>>>;
            using get_fun = detail::get_fun<elem_t>;
            elem_t *elem = nullptr;
            auto &data = detail::variant_core_access::data(var);
            data.apply(N, detail::make_unary_visitor(detail::unwrap_ref_fun<get_fun>{get_fun(elem)}));
            RANGES_ASSERT(elem != nullptr);
            return *elem;
        }

        template<std::size_t N, typename...Ts>
        RANGES_RELAXED_CONSTEXPR
        meta::eval<std::add_rvalue_reference<tagged_variant_element_t<N, tagged_variant<Ts...>>>>
        get(tagged_variant<Ts...> &&var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta::eval<std::remove_reference<
                    tagged_variant_element_t<N, tagged_variant<Ts...>>>>;
            using get_fun = detail::get_fun<elem_t>;
            elem_t *elem = nullptr;
            auto &data = detail::variant_core_access::data(var);
            data.apply(N, detail::make_unary_visitor(detail::unwrap_ref_fun<get_fun>{get_fun(elem)}));
            RANGES_ASSERT(elem != nullptr);
            return std::forward<tagged_variant_element_t<N, tagged_variant<Ts...>>>(*elem);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // set
        template<std::size_t N, typename...Ts, typename U>
        RANGES_RELAXED_CONSTEXPR
        void set(tagged_variant<Ts...> &var, U &&u)
        {
            var.template set<N>(std::forward<U>(u));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // tagged_variant_unique
        template<typename Var>
        struct tagged_variant_unique;

        template<typename ...Ts>
        struct tagged_variant_unique<tagged_variant<Ts...>>
        {
            using type =
                meta::apply_list<
                    meta::quote<tagged_variant>,
                    meta::unique<meta::list<Ts...>>>;
        };

        template<typename Var>
        using tagged_variant_unique_t = typename tagged_variant_unique<Var>::type;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // unique_variant
        template<typename...Ts>
        RANGES_RELAXED_CONSTEXPR
        tagged_variant_unique_t<tagged_variant<Ts...>>
        unique_variant(tagged_variant<Ts...> const &var)
        {
            using From = tagged_variant<Ts...>;
            using To = tagged_variant_unique_t<From>;
            auto &data = detail::variant_core_access::data(var);
            To res;
            data.apply(var.which(), detail::unique_visitor<To, From>{res});
            return res;
        }
        /// @}
    }
}

#endif
