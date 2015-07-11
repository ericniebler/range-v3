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
        using tagged_variant_element_t =
            meta::_t<tagged_variant_element<N, Var>>;

        /// \cond
        namespace detail
        {
            template<typename Fun, typename T, std::size_t N,
                typename = decltype(std::declval<Fun>()(std::declval<T &>(), meta::size_t<N>{}))>
            void apply_if(Fun &&fun, T &t, meta::size_t<N> u)
            {
                std::forward<Fun>(fun)(t, u);
            }

            [[noreturn]] inline void apply_if(any, any, any)
            {
                RANGES_ENSURE(false);
            }

            template<typename...List>
            union variant_data;

            template<>
            union variant_data<>
            {
                template <typename That,
                    CONCEPT_REQUIRES_(Same<variant_data,uncvref_t<That>>())>
                [[noreturn]] void move_copy_construct(std::size_t, That &&) const
                {
                    RANGES_ENSURE(false);
                }
                [[noreturn]] bool equal(std::size_t, variant_data const &) const
                {
                    RANGES_ENSURE(false);
                }
                template<typename Fun, std::size_t N = 0>
                [[noreturn]] void apply(std::size_t, Fun &&, meta::size_t<N> = meta::size_t<N>{}) const
                {
                    RANGES_ENSURE(false);
                }
            };

            template<typename T, typename ...Ts>
            union variant_data<T, Ts...>
            {
            private:
                template<typename...Us>
                friend union variant_data;
                using head_t = decay_t<meta::if_<std::is_reference<T>, ref_t<T &>, T>>;
                using tail_t = variant_data<Ts...>;

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
                variant_data()
                {}
                template<typename ...Args,
                    CONCEPT_REQUIRES_(Constructible<head_t, Args...>())>
                variant_data(meta::size_t<0>, Args && ...args)
                  : head(std::forward<Args>(args)...)
                {}
                template<std::size_t N, typename ...Args,
                    CONCEPT_REQUIRES_(0 != N && Constructible<tail_t, meta::size_t<N - 1>, Args...>())>
                variant_data(meta::size_t<N>, Args && ...args)
                  : tail{meta::size_t<N - 1>{}, std::forward<Args>(args)...}
                {}
                ~variant_data()
                {}
                template <typename That,
                    CONCEPT_REQUIRES_(Same<variant_data, decay_t<That>>())>
                void move_copy_construct(std::size_t n, That &&that)
                {
                    if(n == 0)
                        ::new(static_cast<void *>(&head)) head_t(std::forward<That>(that).head);
                    else
                        tail.move_copy_construct(n - 1, std::forward<That>(that).tail);
                }
                template<typename U, typename...Us>
                bool equal(std::size_t n, variant_data<U, Us...> const &that) const
                {
                    if(n == 0)
                        return head == that.head;
                    else
                        return tail.equal(n - 1, that.tail);
                }
                template<typename Fun, std::size_t N = 0>
                void apply(std::size_t n, Fun &&fun, meta::size_t<N> u = {})
                {
                    variant_data::apply_(*this, n, std::forward<Fun>(fun), u);
                }
                template<typename Fun, std::size_t N = 0>
                void apply(std::size_t n, Fun &&fun, meta::size_t<N> u = {}) const
                {
                    variant_data::apply_(*this, n, std::forward<Fun>(fun), u);
                }
            };

            struct variant_core_access
            {
                template<typename...Ts>
                static variant_data<Ts...> &data(tagged_variant<Ts...> &var)
                {
                    return var.data_;
                }

                template<typename...Ts>
                static variant_data<Ts...> const &data(tagged_variant<Ts...> const &var)
                {
                    return var.data_;
                }

                template<typename...Ts>
                static variant_data<Ts...> &&data(tagged_variant<Ts...> &&var)
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
                [[noreturn]] static void assert_false(any) { RANGES_ENSURE(false); }
                using fun_t = void(*)(any);
                operator fun_t() const
                {
                    return &assert_false;
                }
            };

            // Is there a less dangerous way?
            template<typename...Ts>
            struct construct_fun : assert_otherwise
            {
            private:
                std::tuple<Ts...> t_;

                template<typename U, std::size_t ...Is,
                    CONCEPT_REQUIRES_(Constructible<U, Ts...>())>
                void construct(U &u, meta::index_sequence<Is...>)
                {
                    ::new((void*)std::addressof(u)) U(std::get<Is>(std::move(t_))...);
                }
            public:
                construct_fun(Ts ...ts)
                  : t_{std::forward<Ts>(ts)...}
                {}
                template<typename U,
                    CONCEPT_REQUIRES_(Constructible<U, Ts...>())>
                void operator()(U &u) const
                {
                    // HACKHACKHACK: "workaround" the fact that the visitation
                    // design does not allow for mutable visitors.
                    auto& hack = const_cast<construct_fun&>(*this);
                    hack.construct(u, meta::make_index_sequence<sizeof...(Ts)>{});
                }
            };

            template<typename T>
            struct get_fun : assert_otherwise
            {
            private:
                T *&t_;
            public:
                get_fun(T *&t)
                  : t_(t)
                {}
                void operator()(T &t) const
                {
                    t_ = std::addressof(t);
                }
            };

            template<typename Fun, typename Var = std::nullptr_t>
            struct apply_visitor
            {
            private:
                Var &var_;
                Fun fun_;
                template<typename T, std::size_t N>
                void apply_(T &&t, meta::size_t<N> n, std::true_type) const
                {
                    fun_(std::forward<T>(t), n);
                    var_.template set<N>(nullptr);
                }
                template<typename T, std::size_t N>
                void apply_(T &&t, meta::size_t<N> n, std::false_type) const
                {
                    var_.template set<N>(fun_(std::forward<T>(t), n));
                }
            public:
                apply_visitor(Fun &&fun, Var &var)
                  : var_(var), fun_(std::forward<Fun>(fun))
                {}
                template<typename T, std::size_t N,
                         typename result_t = result_of_t<Fun(T &&, meta::size_t<N>)>>
                auto operator()(T &&t, meta::size_t<N> u) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    this->apply_(std::forward<T>(t), u, std::is_void<result_t>{})
                )
            };

            template<typename Fun>
            struct apply_visitor<Fun, std::nullptr_t>
            {
            private:
                Fun fun_;
            public:
                apply_visitor(Fun &&fun)
                  : fun_(std::forward<Fun>(fun))
                {}
                template<typename T, std::size_t N>
                auto operator()(T &&t, meta::size_t<N> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    fun_(std::forward<T>(t), n), void()
                )
            };

            template<typename Fun>
            struct ignore2nd
            {
            private:
                Fun fun_;
            public:
                ignore2nd(Fun &&fun)
                  : fun_(std::forward<Fun>(fun))
                {}
                template<typename T, typename U>
                auto operator()(T &&t, U &&) const ->
                    decltype(fun_(std::forward<T>(t)))
                {
                    return fun_(std::forward<T>(t));
                }
            };

            template<typename Fun>
            apply_visitor<ignore2nd<Fun>> make_unary_visitor(Fun &&fun)
            {
                return {std::forward<Fun>(fun)};
            }
            template<typename Fun, typename Var>
            apply_visitor<ignore2nd<Fun>, Var> make_unary_visitor(Fun &&fun, Var &var)
            {
                return{std::forward<Fun>(fun), var};
            }
            template<typename Fun>
            apply_visitor<Fun> make_binary_visitor(Fun &&fun)
            {
                return{std::forward<Fun>(fun)};
            }
            template<typename Fun, typename Var>
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
                unique_visitor(tagged_variant<To...> &var)
                  : var_(var)
                {}
                template<typename T, std::size_t N>
                void operator()(T &&t, meta::size_t<N>) const
                {
                    using E = meta::at_c<meta::list<From...>, N>;
                    using F = meta::find<meta::list<To...>, E>;
                    static constexpr std::size_t M = sizeof...(To) - F::size();
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
                unwrap_ref_fun(Fun &&fun)
                  : fun_(std::forward<Fun>(fun))
                {}
                template<typename T>
                auto operator()(T &&t) const ->
                    decltype(fun_(unwrap_reference(std::forward<T>(t))))
                {
                    return fun_(unwrap_reference(std::forward<T>(t)));
                }
                template<typename T, std::size_t N>
                auto operator()(T &&t, meta::size_t<N> n) const ->
                    decltype(fun_(unwrap_reference(std::forward<T>(t)), n))
                {
                    return fun_(unwrap_reference(std::forward<T>(t)), n);
                }
            };
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{
        template<typename ...Ts>
        struct tagged_variant
        {
        private:
            friend struct detail::variant_core_access;
            using types_t = meta::list<Ts...>;
            using data_t = detail::variant_data<Ts...>;
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

            template <typename That,
                CONCEPT_REQUIRES_(Same<tagged_variant, detail::decay_t<That>>())>
            void assign_(That &&that)
            {
                if(that.is_valid())
                {
                    data_.move_copy_construct(that.which_, std::forward<That>(that).data_);
                    which_ = that.which_;
                }
            }

            struct empty_tag { };
            tagged_variant(empty_tag)
              : which_((std::size_t)-1)
            {}

        public:
            CONCEPT_REQUIRES(!Constructible<data_t, meta::size_t<0>>())
            tagged_variant()
              : tagged_variant{empty_tag{}}
            {}
            CONCEPT_REQUIRES(Constructible<data_t, meta::size_t<0>>())
            tagged_variant()
              : tagged_variant{meta::size_t<0>{}}
            {}
            template<std::size_t N, typename...Args,
                CONCEPT_REQUIRES_(Constructible<data_t, meta::size_t<N>, Args...>())>
            tagged_variant(meta::size_t<N> n, Args &&...args)
              : which_(N), data_{n, detail::forward<Args>(args)...}
            {
                static_assert(N < sizeof...(Ts), "");
            }
            tagged_variant(tagged_variant &&that)
              : tagged_variant{empty_tag{}}
            {
                assign_(std::move(that));
            }
            tagged_variant(tagged_variant const &that)
              : tagged_variant{empty_tag{}}
            {
                assign_(that);
            }
            tagged_variant &operator=(tagged_variant &&that)
            {
                clear_();
                assign_(std::move(that));
                return *this;
            }
            tagged_variant &operator=(tagged_variant const &that)
            {
                clear_();
                assign_(that);
                return *this;
            }
            ~tagged_variant()
            {
                clear_();
            }
            static constexpr std::size_t size()
            {
                return sizeof...(Ts);
            }
            template<std::size_t N, typename ...Args,
                CONCEPT_REQUIRES_(Constructible<data_t, meta::size_t<N>, Args...>())>
            void set(Args &&...args)
            {
                clear_();
                data_.apply(N, detail::make_unary_visitor(detail::construct_fun<Args&&...>{std::forward<Args>(args)...}));
                which_ = N;
            }
            bool is_valid() const
            {
                return which() != (std::size_t)-1;
            }
            std::size_t which() const
            {
                return which_;
            }
            template<typename Fun,
                typename Args = meta::transform<types_t, detail::add_ref_t>,
                typename Result = detail::variant_result_t<Fun, Args>>
            Result apply(Fun &&fun)
            {
                Result res;
                data_.apply(which_, detail::make_unary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }
            template<typename Fun,
                typename Args = meta::transform<types_t, detail::add_cref_t>,
                typename Result = detail::variant_result_t<Fun, Args >>
            Result apply(Fun &&fun) const
            {
                Result res;
                data_.apply(which_, detail::make_unary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }

            template<typename Fun,
                typename Args = meta::transform<types_t, detail::add_ref_t>,
                typename Result = detail::variant_result_i_t<Fun, Args>>
            Result apply_i(Fun &&fun)
            {
                Result res;
                data_.apply(which_, detail::make_binary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }
            template<typename Fun,
                typename Args = meta::transform<types_t, detail::add_cref_t>,
                typename Result = detail::variant_result_i_t<Fun, Args >>
            Result apply_i(Fun &&fun) const
            {
                Result res;
                data_.apply(which_, detail::make_binary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }
        };

        template<typename...Ts, typename...Us,
            CONCEPT_REQUIRES_(meta::and_c<(bool)EqualityComparable<Ts, Us>()...>::value)>
        bool operator==(tagged_variant<Ts...> const &lhs, tagged_variant<Us...> const &rhs)
        {
            RANGES_ASSERT(lhs.is_valid());
            RANGES_ASSERT(rhs.is_valid());
            return lhs.which() == rhs.which() &&
                detail::variant_core_access::data(lhs).equal(lhs.which(), detail::variant_core_access::data(rhs));
        }

        template<typename...Ts, typename...Us,
            CONCEPT_REQUIRES_(meta::and_c<(bool)EqualityComparable<Ts, Us>()...>::value)>
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
        meta::apply<detail::add_ref_t, tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> &var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta::_t<std::remove_reference<
                    tagged_variant_element_t<N, tagged_variant<Ts...>>>>;
            using get_fun = detail::get_fun<elem_t>;
            elem_t *elem = nullptr;
            auto &data = detail::variant_core_access::data(var);
            data.apply(N, detail::make_unary_visitor(detail::unwrap_ref_fun<get_fun>{get_fun(elem)}));
            RANGES_ASSERT(elem != nullptr);
            return *elem;
        }

        template<std::size_t N, typename...Ts>
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
        meta::_t<std::add_rvalue_reference<tagged_variant_element_t<N, tagged_variant<Ts...>>>>
        get(tagged_variant<Ts...> &&var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta::_t<std::remove_reference<
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
        template<std::size_t N, typename...Ts, typename...Args>
        void set(tagged_variant<Ts...> &var, Args &&...args)
        {
            var.template set<N>(std::forward<Args>(args)...);
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
