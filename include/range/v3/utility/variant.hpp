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
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/typelist.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/integer_sequence.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            using add_ref_t =
                meta_quote<std::add_lvalue_reference>;

            using add_cref_t =
                meta_compose<meta_quote<std::add_lvalue_reference>, meta_quote<std::add_const>>;
        }

        template<typename...Ts>
        struct tagged_variant;

        template<std::size_t N, typename Var>
        struct tagged_variant_element;

        template<std::size_t N, typename Var>
        using tagged_variant_element_t = typename tagged_variant_element<N, Var>::type;

        namespace detail
        {
            template<typename Fun, typename T, std::size_t N,
                typename = decltype(std::declval<Fun>()(std::declval<T &>(), index_t<N>{}))>
            void apply_if(Fun &&fun, T &t, index_t<N> u)
            {
                std::forward<Fun>(fun)(t, u);
            }

            inline void apply_if(any, any, any)
            {
                RANGES_ASSERT(false);
            }

            template<typename...List>
            union variant_data;

            template<>
            union variant_data<>
            {
                void move(std::size_t n, variant_data &&) const
                {
                    RANGES_ASSERT(false);
                }
                void copy(std::size_t n, variant_data const &) const
                {
                    RANGES_ASSERT(false);
                }
                bool equal(std::size_t n, variant_data const &) const
                {
                    RANGES_ASSERT(false);
                    return true;
                }
                template<typename Fun, std::size_t N = 0>
                void apply(std::size_t, Fun &&, index_t<N> = index_t<N>{}) const
                {
                    RANGES_ASSERT(false);
                }
            };

            template<typename T, typename ...Ts>
            union variant_data<T, Ts...>
            {
            private:
                template<typename...Us>
                friend union variant_data;
                template<typename U>
                using enable_if_ok = enable_if_t<typelist_in<U, typelist<Ts...>>::value>;
                using head_t = decay_t<conditional_t<std::is_reference<T>::value, ref_t<T &>, T>>;
                using tail_t = variant_data<Ts...>;

                head_t head;
                tail_t tail;

                template<typename This, typename Fun, std::size_t N>
                static void apply_(This &this_, std::size_t n, Fun &&fun, index_t<N> u)
                {
                    if(0 == n)
                        detail::apply_if(detail::forward<Fun>(fun), this_.head, u);
                    else
                        this_.tail.apply(n - 1, detail::forward<Fun>(fun), index_t<N + 1>{});
                }
            public:
                variant_data()
                {}
                // BUGBUG in-place construction?
                template<typename U,
                    enable_if_t<std::is_constructible<head_t, U>::value> = 0>
                variant_data(index_t<0>, U &&u)
                  : head(std::forward<U>(u))
                {}
                template<std::size_t N, typename U,
                    enable_if_t<0 != N && std::is_constructible<tail_t, index_t<N - 1>, U>::value> = 0>
                variant_data(index_t<N>, U &&u)
                  : tail{index_t<N - 1>{}, std::forward<U>(u)}
                {}
                ~variant_data()
                {}
                void move(std::size_t n, variant_data &&that)
                {
                    if(n == 0)
                        ::new(static_cast<void *>(&head)) head_t(std::move(that).head);
                    else
                        tail.move(n - 1, std::move(that).tail);
                }
                void copy(std::size_t n, variant_data const &that)
                {
                    if(n == 0)
                        ::new(static_cast<void *>(&head)) head_t(that.head);
                    else
                        tail.copy(n - 1, that.tail);
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
                void apply(std::size_t n, Fun &&fun, index_t<N> u = index_t<N>{})
                {
                    variant_data::apply_(*this, n, std::forward<Fun>(fun), u);
                }
                template<typename Fun, std::size_t N = 0>
                void apply(std::size_t n, Fun &&fun, index_t<N> u = index_t<N>{}) const
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
                construct_fun(T &&t)
                  : t_(std::forward<T>(t))
                {}
                template<typename U,
                    enable_if_t<std::is_constructible<U, T>::value> = 0>
                void operator()(U &u) const
                {
                    ::new((void*)std::addressof(u)) U(std::forward<T>(t_));
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
                void apply_(T &&t, index_t<N> n, std::true_type) const
                {
                    fun_(std::forward<T>(t), n);
                    var_.template set<N>(nullptr);
                }
                template<typename T, std::size_t N>
                void apply_(T &&t, index_t<N> n, std::false_type) const
                {
                    var_.template set<N>(fun_(std::forward<T>(t), n));
                }
            public:
                apply_visitor(Fun &&fun, Var &var)
                  : var_(var), fun_(std::forward<Fun>(fun))
                {}
                template<typename T, std::size_t N>
                void operator()(T &&t, index_t<N> u) const
                {
                    using result_t = result_of_t<Fun(T &&, index_t<N>)>;
                    this->apply_(std::forward<T>(t), u, std::is_void<result_t>{});
                }
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
                void operator()(T &&t, index_t<N> n) const
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
                void operator()(T &&t, index_t<N>) const
                {
                    using E = typelist_element_t<N, typelist<From...>>;
                    using F = typelist_find_t<typelist<To...>, E>;
                    static constexpr std::size_t M = sizeof...(To) - F::size();
                    var_.template set<M>(std::forward<T>(t));
                }
            };

            template<typename Fun, typename Types>
            using variant_result_t =
                typelist_expand_t<
                    typelist_replace_t<
                        typelist_transform_t<
                            Types,
                            meta_bind_front<meta_quote<concepts::Function::result_t>, Fun>>,
                        void,
                        std::nullptr_t>,
                    tagged_variant>;

            template<typename Fun, typename Types>
            using variant_result_i_t =
                typelist_expand_t<
                    typelist_replace_t<
                        typelist_transform2_t<
                            Types,
                            as_typelist_t<make_index_sequence_t<Types::size()>>,
                            meta_bind_front<meta_quote<concepts::Function::result_t>, Fun>>,
                        void, std::nullptr_t>,
                    tagged_variant>;

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
                auto operator()(T &&t, index_t<N> n) const ->
                    decltype(fun_(unwrap_reference(std::forward<T>(t)), n))
                {
                    return fun_(unwrap_reference(std::forward<T>(t)), n);
                }
            };
        }

        template<typename ...Ts>
        struct tagged_variant
        {
        private:
            friend struct detail::variant_core_access;
            using types_t = typelist<Ts...>;
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
        public:
            tagged_variant()
              : which_((std::size_t)-1), data_{}
            {}
            template<std::size_t N, typename U,
                enable_if_t<std::is_constructible<data_t, index_t<N>, U>::value> = 0>
            tagged_variant(index_t<N> n, U &&u)
              : which_(N), data_{n, detail::forward<U>(u)}
            {
                static_assert(N < sizeof...(Ts), "");
            }
            tagged_variant(tagged_variant &&that)
              : tagged_variant{}
            {
                if(that.is_valid())
                {
                    data_.move(that.which_, std::move(that).data_);
                    which_ = that.which_;
                }
            }
            tagged_variant(tagged_variant const &that)
              : tagged_variant{}
            {
                if(that.is_valid())
                {
                    data_.copy(that.which_, that.data_);
                    which_ = that.which_;
                }
            }
            tagged_variant &operator=(tagged_variant &&that)
            {
                clear_();
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
            ~tagged_variant()
            {
                clear_();
            }
            static constexpr std::size_t size()
            {
                return sizeof...(Ts);
            }
            template<std::size_t N, typename U,
                enable_if_t<std::is_constructible<data_t, index_t<N>, U>::value> = 0>
            void set(U &&u)
            {
                clear_();
                data_.apply(N, detail::make_unary_visitor(detail::construct_fun<U>{std::forward<U>(u)}));
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
                typename Args = typelist_transform_t<types_t, detail::add_ref_t>,
                typename Result = detail::variant_result_t<Fun, Args>>
            Result apply(Fun &&fun)
            {
                Result res;
                data_.apply(which_, detail::make_unary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }
            template<typename Fun,
                typename Args = typelist_transform_t<types_t, detail::add_cref_t>,
                typename Result = detail::variant_result_t<Fun, Args >>
            Result apply(Fun &&fun) const
            {
                Result res;
                data_.apply(which_, detail::make_unary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }

            template<typename Fun,
                typename Args = typelist_transform_t<types_t, detail::add_ref_t>,
                typename Result = detail::variant_result_i_t<Fun, Args>>
            Result apply_i(Fun &&fun)
            {
                Result res;
                data_.apply(which_, detail::make_binary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }
            template<typename Fun,
                typename Args = typelist_transform_t<types_t, detail::add_cref_t>,
                typename Result = detail::variant_result_i_t<Fun, Args >>
            Result apply_i(Fun &&fun) const
            {
                Result res;
                data_.apply(which_, detail::make_binary_visitor(detail::unwrap_ref_fun<Fun>{std::forward<Fun>(fun)}, res));
                return res;
            }
        };

        template<typename...Ts, typename...Us,
            CONCEPT_REQUIRES_(logical_and_c<(bool)EqualityComparable<Ts, Us>()...>::value)>
        bool operator==(tagged_variant<Ts...> const &lhs, tagged_variant<Us...> const &rhs)
        {
            RANGES_ASSERT(lhs.which() < sizeof...(Ts));
            RANGES_ASSERT(rhs.which() < sizeof...(Us));
            return lhs.which() == rhs.which() &&
                detail::variant_core_access::data(lhs).equal(lhs.which(), detail::variant_core_access::data(rhs));
        }

        template<typename...Ts, typename...Us,
            CONCEPT_REQUIRES_(logical_and_c<(bool)EqualityComparable<Ts, Us>()...>::value)>
        bool operator!=(tagged_variant<Ts...> const &lhs, tagged_variant<Us...> const &rhs)
        {
            return !(lhs == rhs);
        }

        template<std::size_t N, typename...Ts>
        struct tagged_variant_element<N, tagged_variant<Ts...>>
        {
            using type =
                detail::lazy_conditional_t<
                    std::is_reference<typelist_element_t<N, typelist<Ts...>>>::value,
                    typelist_element<N, typelist<Ts...>>,
                    std::decay<typelist_element_t<N, typelist<Ts...>>>>;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        // get
        template<std::size_t N, typename...Ts>
        meta_apply<detail::add_ref_t, tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> &var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta_quote_apply<std::remove_reference,
                    tagged_variant_element_t<N, tagged_variant<Ts...>>>;
            using get_fun = detail::get_fun<elem_t>;
            elem_t *elem = nullptr;
            auto &data = detail::variant_core_access::data(var);
            data.apply(N, detail::make_unary_visitor(detail::unwrap_ref_fun<get_fun>{get_fun(elem)}));
            RANGES_ASSERT(elem != nullptr);
            return *elem;
        }

        template<std::size_t N, typename...Ts>
        meta_apply<detail::add_cref_t, tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> const &var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta_apply<
                    meta_compose<
                        meta_quote<std::remove_reference>,
                        meta_quote<std::add_const>
                    >, tagged_variant_element_t<N, tagged_variant<Ts...>>>;
            using get_fun = detail::get_fun<elem_t>;
            elem_t *elem = nullptr;
            auto &data = detail::variant_core_access::data(var);
            data.apply(N, detail::make_unary_visitor(detail::unwrap_ref_fun<get_fun>{get_fun(elem)}));
            RANGES_ASSERT(elem != nullptr);
            return *elem;
        }

        template<std::size_t N, typename...Ts>
        meta_quote_apply<std::add_rvalue_reference, tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> &&var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta_quote_apply<std::remove_reference,
                    tagged_variant_element_t<N, tagged_variant<Ts...>>>;
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
                typelist_expand_t<typelist_unique_t<typelist<Ts...>>, tagged_variant>;
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
    }
}

#endif
