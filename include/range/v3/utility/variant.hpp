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
#include <stdexcept>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<std::size_t I>
        struct emplaced_index_t;

        template<std::size_t I>
        struct emplaced_index_t
          : meta::size_t<I>
        {};

        /// \cond
    #if RANGES_CXX_NO_VARIABLE_TEMPLATES
        template<std::size_t I>
        inline emplaced_index_t<I> emplaced_index()
        {
            return {};
        }
        template<std::size_t I>
        using _emplaced_index_t_ = emplaced_index_t<I>(&)();
    #define RANGES_EMPLACED_INDEX_T(I) _emplaced_index_t_<I>
    #else
        /// \endcond
        namespace
        {
            template<std::size_t I>
            constexpr auto &emplaced_index = static_const<emplaced_index_t<I>>::value;
        }
        /// \cond
    #define RANGES_EMPLACED_INDEX_T(I) emplaced_index_t<I>
    #endif
        /// \endcond

        struct bad_variant_access
          : std::logic_error
        {
            explicit bad_variant_access(std::string const &what_arg)
              : std::logic_error(std::move(what_arg))
            {}
            explicit bad_variant_access(char const *what_arg)
              : std::logic_error(what_arg)
            {}
        };

        /// \cond
        namespace detail
        {
            template<typename...List>
            union variant_data_;

            template<>
            union variant_data_<>
            {
                template<typename That,
                    CONCEPT_REQUIRES_(Same<variant_data_, uncvref_t<That>>())>
                [[noreturn]] void move_copy_construct(std::size_t, That &&) const
                {
                    RANGES_ENSURE(false);
                }
                [[noreturn]] bool equal(std::size_t, variant_data_ const &) const
                {
                    RANGES_ENSURE(false);
                }
                template<typename Fun, std::size_t N = 0>
                [[noreturn]] void apply(std::size_t, Fun &&, meta::size_t<N> = {}) const
                {
                    RANGES_ENSURE(false);
                }
            };

            template<typename T, typename RValue>
            struct variant_reference_wrapper_
              : reference_wrapper<T, RValue::value>
            {
                variant_reference_wrapper_() = delete;
                using reference_wrapper<T, RValue::value>::reference_wrapper;
            };

            template<typename T>
            struct is_variant_reference_wrapper_
              : std::false_type
            {};

            template<typename T, typename RValue>
            struct is_variant_reference_wrapper_<variant_reference_wrapper_<T, RValue>>
              : std::true_type
            {};

            struct variant_void
            {};

            template<typename T>
            using variant_datum_t =
                meta::if_<
                    std::is_void<T>,
                    variant_void,
                    meta::if_<
                        std::is_reference<T>,
                        variant_reference_wrapper_<
                            meta::_t<std::remove_reference<T>>,
                            std::is_rvalue_reference<T>>,
                        decay_t<T>>>;

            template<typename T, typename ...Ts>
            union variant_data_<T, Ts...>
            {
            private:
                template<typename...Us>
                friend union variant_data_;

                T head;
                variant_data_<Ts...> tail;

            public:
                variant_data_() {}
                template<typename ...Args,
                    CONCEPT_REQUIRES_(Constructible<T, Args...>())>
                constexpr variant_data_(meta::size_t<0>, Args && ...args)
                  : head(detail::forward<Args>(args)...)
                {}
                template<std::size_t N, typename ...Args,
                    CONCEPT_REQUIRES_(0 != N &&
                        Constructible<variant_data_<Ts...>, meta::size_t<N - 1>, Args...>())>
                constexpr variant_data_(meta::size_t<N>, Args && ...args)
                  : tail{meta::size_t<N - 1>{}, detail::forward<Args>(args)...}
                {}
                ~variant_data_()
                {}
                template<typename That,
                    CONCEPT_REQUIRES_(Same<variant_data_, decay_t<That>>())>
                void move_copy_construct(std::size_t n, That &&that)
                {
                    n == 0 ? (void)::new((void*)&head) T(((That &&) that).head) :
                        tail.move_copy_construct(n - 1, ((That &&)that).tail);
                }
                template<typename U, typename...Us>
                bool equal(std::size_t n, variant_data_<U, Us...> const &that) const
                {
                    return n == 0 ? head == that.head : tail.equal(n - 1, that.tail);
                }
                template<typename Fun, std::size_t N = 0>
                void apply(std::size_t n, Fun fun, meta::size_t<N> u = {})
                {
                    0 == n ? fun(head, u) :
                        tail.apply(n - 1, detail::move(fun), meta::size_t<N + 1>{});
                }
                template<typename Fun, std::size_t N = 0>
                void apply(std::size_t n, Fun fun, meta::size_t<N> u = {}) const
                {
                    0 == n ? fun(head, u) :
                        tail.apply(n - 1, detail::move(fun), meta::size_t<N + 1>{});
                }
            };

            struct empty_variant_tag
            {};

            template<typename... Ts>
            using variant_data = variant_data_<variant_datum_t<Ts>...>;

            struct variant_core_access
            {
                template<typename...Ts>
                static variant_data<Ts...> &data(variant<Ts...> &var)
                {
                    return var.data_;
                }

                template<typename...Ts>
                static variant_data<Ts...> const &data(variant<Ts...> const &var)
                {
                    return var.data_;
                }

                template<typename...Ts>
                static variant_data<Ts...> &&data(variant<Ts...> &&var)
                {
                    return std::move(var).data_;
                }

                template<typename...Ts>
                static variant<Ts...> make_empty(meta::id<variant<Ts...>>)
                {
                    return variant<Ts...>{empty_variant_tag{}};
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

            template<std::size_t N, typename... Ts>
            struct construct_fun
            {
            private:
                std::tuple<Ts...> t_;

                template<typename U, std::size_t ...Is>
                void construct(U &u, meta::index_sequence<Is...>)
                {
                    ::new((void*)std::addressof(u)) U(std::forward<Ts>(std::get<Is>(t_))...);
                }
            public:
                construct_fun(Ts &&...ts)
                  : t_{std::forward<Ts>(ts)...}
                {}
                template<typename U, std::size_t M>
                [[noreturn]] meta::if_c<N != M> operator()(U &, meta::size_t<M>)
                {
                    RANGES_ENSURE(false);
                }
                template<typename U>
                void operator()(U &u, meta::size_t<N>)
                {
                    static_assert((bool) Constructible<U, Ts &&...>(), "Shouldn't happen!");
                    this->construct(u, meta::make_index_sequence<sizeof...(Ts)>{});
                }
            };

            template<typename T, std::size_t N>
            struct get_fun
            {
            private:
                T **t_;
            public:
                get_fun(T *&t)
                  : t_(&t)
                {}
                template<typename U, std::size_t M>
                [[noreturn]] meta::if_c<M != N> operator()(U &, meta::size_t<M>) const
                {
                    throw bad_variant_access("bad varaiant access");
                }
                void operator()(T &t, meta::size_t<N>) const
                {
                    *t_ = std::addressof(t);
                }
            };

            template<std::size_t N>
            struct get_fun<void, N>
            {
                get_fun(void const volatile *)
                {}
                template<typename U, std::size_t M>
                [[noreturn]] meta::if_c<M != N> operator()(U &, meta::size_t<M>) const
                {
                    throw bad_variant_access("bad varaiant access");
                }
                void operator()(variant_void &, meta::size_t<N>) const
                {}
            };

            template<typename Fun, typename Var = void>
            struct apply_visitor
              : private function_type<Fun>
            {
            private:
                using BaseFun = function_type<Fun>;
                Var *var_;

                BaseFun &fn() { return *this; }
                BaseFun const &fn() const { return *this; }

                template<typename T, std::size_t N>
                void apply_(T &&t, meta::size_t<N> n, std::true_type) const
                {
                    fn()(std::forward<T>(t), n);
                    var_->template emplace<N>();
                }
                template<typename T, std::size_t N>
                void apply_(T &&t, meta::size_t<N> n, std::false_type) const
                {
                    var_->template emplace<N>(fn()(std::forward<T>(t), n));
                }
            public:
                apply_visitor(Fun fun, Var &var)
                  : BaseFun(as_function(detail::move(fun)))
                  , var_(std::addressof(var))
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
            struct apply_visitor<Fun, void>
              : private function_type<Fun>
            {
            private:
                using BaseFun = function_type<Fun>;

                BaseFun &fn() { return *this; }
                BaseFun const &fn() const { return *this; }
            public:
                apply_visitor(Fun fun)
                  : BaseFun(as_function(detail::move(fun)))
                {}
                template<typename T, std::size_t N>
                auto operator()(T &&t, meta::size_t<N> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    this->fn()(std::forward<T>(t), n), void()
                )
            };

            template<typename Fun>
            struct ignore2nd
              : private function_type<Fun>
            {
            private:
                using BaseFun = function_type<Fun>;

                BaseFun &fn() { return *this; }
                BaseFun const &fn() const { return *this; }
            public:
                ignore2nd(Fun &&fun)
                  : BaseFun(as_function(detail::move(fun)))
                {}
                template<typename T, typename U>
                auto operator()(T &&t, U &&) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    this->fn()(std::forward<T>(t))
                )
            };

            template<typename Fun>
            apply_visitor<ignore2nd<Fun>> make_unary_visitor(Fun fun)
            {
                return {detail::move(fun)};
            }
            template<typename Fun, typename Var>
            apply_visitor<ignore2nd<Fun>, Var> make_unary_visitor(Fun fun, Var &var)
            {
                return {detail::move(fun), var};
            }
            template<typename Fun>
            apply_visitor<Fun> make_binary_visitor(Fun fun)
            {
                return {detail::move(fun)};
            }
            template<typename Fun, typename Var>
            apply_visitor<Fun, Var> make_binary_visitor(Fun fun, Var &var)
            {
                return {detail::move(fun), var};
            }

            template<typename To, typename From>
            struct unique_visitor;

            template<typename ...To, typename ...From>
            struct unique_visitor<variant<To...>, variant<From...>>
            {
            private:
                variant<To...> *var_;
            public:
                unique_visitor(variant<To...> &var)
                  : var_(&var)
                {}
                template<typename T, std::size_t N>
                void operator()(T &&t, meta::size_t<N>) const
                {
                    using E = meta::at_c<meta::list<From...>, N>;
                    using F = meta::find<meta::list<To...>, E>;
                    static constexpr std::size_t M = sizeof...(To) - F::size();
                    var_->template emplace<M>(std::forward<T>(t));
                }
            };

            template<typename Fun>
            struct unwrap_ref_fun
              : private function_type<Fun>
            {
            private:
                using BaseFun = function_type<Fun>;

                BaseFun &fn() { return *this; }
                BaseFun const &fn() const { return *this; }
            public:
                unwrap_ref_fun(Fun fun)
                  : BaseFun(as_function(detail::move(fun)))
                {}
                template<typename T, typename RValue>
                auto operator()(variant_reference_wrapper_<T, RValue> t) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    this->fn()(t.get())
                )
                template<typename T, typename RValue, std::size_t N>
                auto operator()(variant_reference_wrapper_<T, RValue> t, meta::size_t<N> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    this->fn()(t.get(), n)
                )
                template<typename T,
                    CONCEPT_REQUIRES_(!is_variant_reference_wrapper_<decay_t<T>>())>
                auto operator()(T &&t) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    this->fn()(std::forward<T>(t))
                )
                template<typename T, std::size_t N,
                    CONCEPT_REQUIRES_(!is_variant_reference_wrapper_<decay_t<T>>())>
                auto operator()(T &&t, meta::size_t<N> n) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    this->fn()(std::forward<T>(t), n)
                )
            };

            template<typename T>
            T &variant_deref_(T *t)
            {
                return *t;
            }

            inline void variant_deref_(void const volatile *)
            {}
        }
        /// \endcond

        template<std::size_t N, typename...Ts>
        meta::_t<std::add_lvalue_reference<meta::_t<std::tuple_element<N, variant<Ts...>>>>>
        get(variant<Ts...> &var);

        template<std::size_t N, typename...Ts>
        meta::_t<std::add_lvalue_reference<
            meta::_t<std::tuple_element<N, variant<Ts...> const>> const>>
        get(variant<Ts...> const &var);

        template<std::size_t N, typename...Ts>
        meta::_t<std::add_rvalue_reference<meta::_t<std::tuple_element<N, variant<Ts...>>>>>
        get(variant<Ts...> &&var);

        /// \addtogroup group-utility
        /// @{
        template<typename ...Ts>
        struct variant
        {
        private:
            friend struct detail::variant_core_access;
            template<typename...Us>
            friend struct variant;
            using indices_t = meta::make_index_sequence<sizeof...(Ts)>;
            std::size_t index_;
            detail::variant_data<Ts...> data_;

            void clear_()
            {
                if(valid())
                {
                    data_.apply(index_, detail::make_unary_visitor(detail::delete_fun{}));
                    index_ = (std::size_t)-1;
                }
            }
            template<typename That,
                CONCEPT_REQUIRES_(Same<variant, detail::decay_t<That>>())>
            void assign_(That &&that)
            {
                if(that.valid())
                {
                    data_.move_copy_construct(that.index_, std::forward<That>(that).data_);
                    index_ = that.index_;
                }
            }
            template<typename This, typename Fun, std::size_t... Is>
            static auto apply_(This &this_, Fun fun, meta::index_sequence<Is...>) ->
                variant<decltype(fun(ranges::get<Is>(this_)))...>
            {
                variant<decltype(fun(ranges::get<Is>(this_)))...> res{detail::empty_variant_tag{}};
                this_.data_.apply(
                    this_.index_,
                    detail::make_unary_visitor(
                        detail::unwrap_ref_fun<Fun>{detail::move(fun)},
                        res));
                RANGES_ASSERT(res.valid());
                return res;
            }
            template<typename This, typename Fun, std::size_t... Is>
            static auto apply_i_(This &this_, Fun fun, meta::index_sequence<Is...>) ->
                variant<decltype(fun(ranges::get<Is>(this_), meta::size_t<Is>{}))...>
            {
                variant<decltype(fun(ranges::get<Is>(this_), meta::size_t<Is>{}))...> res{
                    detail::empty_variant_tag{}};
                this_.data_.apply(
                    this_.index_,
                    detail::make_binary_visitor(
                        detail::unwrap_ref_fun<Fun>{detail::move(fun)},
                        res));
                RANGES_ASSERT(res.valid());
                return res;
            }
            constexpr variant(detail::empty_variant_tag)
              : index_((std::size_t)-1)
            {}

        public:
            template<
                typename ElementT = detail::variant_datum_t<meta::_t<std::tuple_element<0, variant>>>,
                CONCEPT_REQUIRES_(DefaultConstructible<ElementT>())>
            constexpr variant()
              : variant{emplaced_index<0>}
            {}
            template<std::size_t N, typename...Args,
                // Handle reference types and void here:
                typename ElementT = detail::variant_datum_t<meta::_t<std::tuple_element<N, variant>>>,
                CONCEPT_REQUIRES_(Constructible<ElementT, Args &&...>())>
            constexpr variant(RANGES_EMPLACED_INDEX_T(N), Args &&...args)
              : index_(N), data_{meta::size_t<N>{}, detail::forward<Args>(args)...}
            {
                static_assert(N < sizeof...(Ts), "");
            }
            variant(variant &&that)
              : variant{detail::empty_variant_tag{}}
            {
                this->assign_(std::move(that));
            }
            variant(variant const &that)
              : variant{detail::empty_variant_tag{}}
            {
                this->assign_(that);
            }
            variant &operator=(variant &&that)
            {
                this->clear_();
                this->assign_(std::move(that));
                return *this;
            }
            variant &operator=(variant const &that)
            {
                this->clear_();
                this->assign_(that);
                return *this;
            }
            ~variant()
            {
                this->clear_();
            }
            static constexpr std::size_t size()
            {
                return sizeof...(Ts);
            }
            template<std::size_t N, typename ...Args,
                // Handle reference types and void here:
                typename ElementT = detail::variant_datum_t<meta::_t<std::tuple_element<N, variant>>>,
                CONCEPT_REQUIRES_(Constructible<ElementT, Args &&...>())>
            void emplace(Args &&...args)
            {
                this->clear_();
                detail::construct_fun<N, Args&&...> fn{std::forward<Args>(args)...};
                data_.apply(N, detail::make_binary_visitor(std::ref(fn)));
                index_ = N;
            }
            bool valid() const
            {
                return index() != (std::size_t)-1;
            }
            std::size_t index() const
            {
                return index_;
            }
            template<typename Fun>
            auto apply(Fun fun)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                variant::apply_(*this, std::move(fun), indices_t{})
            )
            template<typename Fun>
            auto apply(Fun fun) const
            RANGES_DECLTYPE_AUTO_RETURN
            (
                variant::apply_(*this, std::move(fun), indices_t{})
            )
            template<typename Fun>
            auto apply_i(Fun fun)
            RANGES_DECLTYPE_AUTO_RETURN
            (
                variant::apply_i_(*this, std::move(fun), indices_t{})
            )
            template<typename Fun>
            auto apply_i(Fun fun) const
            RANGES_DECLTYPE_AUTO_RETURN
            (
                variant::apply_i_(*this, std::move(fun), indices_t{})
            )
        };

        template<typename...Ts, typename...Us,
            CONCEPT_REQUIRES_(meta::and_c<(bool)EqualityComparable<Ts, Us>()...>::value)>
        bool operator==(variant<Ts...> const &lhs, variant<Us...> const &rhs)
        {
            return (!lhs.valid() && !rhs.valid()) ||
                (lhs.index() == rhs.index() &&
                    detail::variant_core_access::data(lhs).equal(
                        lhs.index(),
                        detail::variant_core_access::data(rhs)));
        }

        template<typename...Ts, typename...Us,
            CONCEPT_REQUIRES_(meta::and_c<(bool)EqualityComparable<Ts, Us>()...>::value)>
        bool operator!=(variant<Ts...> const &lhs, variant<Us...> const &rhs)
        {
            return !(lhs == rhs);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // get
        template<std::size_t N, typename...Ts>
        meta::_t<std::add_lvalue_reference<meta::_t<std::tuple_element<N, variant<Ts...>>>>>
        get(variant<Ts...> &var)
        {
            using elem_t = meta::_t<std::remove_reference<
                meta::_t<std::tuple_element<N, variant<Ts...>>>>>;
            using get_fun = detail::get_fun<elem_t, N>;
            elem_t *elem = nullptr;
            auto fun = detail::make_binary_visitor(detail::unwrap_ref_fun<get_fun>{get_fun(elem)});
            detail::variant_core_access::data(var).apply(var.index(), fun);
            return detail::variant_deref_(elem);
        }

        template<std::size_t N, typename...Ts>
        meta::_t<std::add_lvalue_reference<
            meta::_t<std::tuple_element<N, variant<Ts...> const>> const>>
        get(variant<Ts...> const &var)
        {
            using elem_t = meta::_t<std::remove_reference<
                meta::_t<std::tuple_element<N, variant<Ts...> const>>>>;
            using get_fun = detail::get_fun<elem_t, N>;
            elem_t *elem = nullptr;
            auto fun = detail::make_binary_visitor(detail::unwrap_ref_fun<get_fun>{get_fun(elem)});
            detail::variant_core_access::data(var).apply(var.index(), fun);
            return detail::variant_deref_(elem);
        }

        template<std::size_t N, typename...Ts>
        meta::_t<std::add_rvalue_reference<meta::_t<std::tuple_element<N, variant<Ts...>>>>>
        get(variant<Ts...> &&var)
        {
            using elem_t = meta::_t<std::remove_reference<
                meta::_t<std::tuple_element<N, variant<Ts...>>>>>;
            using get_fun = detail::get_fun<elem_t, N>;
            elem_t *elem = nullptr;
            auto fun = detail::make_binary_visitor(detail::unwrap_ref_fun<get_fun>{get_fun(elem)});
            detail::variant_core_access::data(var).apply(var.index(), fun);
            using res_t = meta::_t<std::add_rvalue_reference<
                meta::_t<std::tuple_element<N, variant<Ts...>>>>>;
            return static_cast<res_t>(detail::variant_deref_(elem));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // emplace
        template<std::size_t N, typename...Ts, typename...Args>
        void emplace(variant<Ts...> &var, Args &&...args)
        {
            var.template emplace<N>(std::forward<Args>(args)...);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // variant_unique
        template<typename Var>
        struct variant_unique
        {};

        template<typename ...Ts>
        struct variant_unique<variant<Ts...>>
        {
            using type =
                meta::apply_list<
                    meta::quote<variant>,
                    meta::unique<meta::list<Ts...>>>;
        };

        template<typename Var>
        using variant_unique_t = meta::_t<variant_unique<Var>>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // unique_variant
        template<typename...Ts>
        variant_unique_t<variant<Ts...>>
        unique_variant(variant<Ts...> const &var)
        {
            using From = variant<Ts...>;
            using To = variant_unique_t<From>;
            auto &data = detail::variant_core_access::data(var);
            auto res = detail::variant_core_access::make_empty(meta::id<To>{});
            data.apply(var.index(), detail::unique_visitor<To, From>{res});
            RANGES_ASSERT(res.valid());
            return res;
        }
        /// @}
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wmismatched-tags"

namespace std
{
    template<typename...Ts>
    struct tuple_size<ranges::variant<Ts...>>
      : tuple_size<tuple<Ts...>>
    {};

    template<size_t I, typename...Ts>
    struct tuple_element<I, ranges::variant<Ts...>>
      : tuple_element<I, tuple<Ts...>>
    {};
}

#pragma GCC diagnostic pop

#endif
