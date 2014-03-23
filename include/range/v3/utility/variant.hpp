// Boost.Range library
//
//  Copyright Eric Niebler 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_VARIANT_HPP
#define RANGES_V3_UTILITY_VARIANT_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/typelist.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T>
            using add_ref_t =
                meta_eval<std::add_lvalue_reference>::apply<T>;

            template<typename T>
            using add_cref_t =
                meta_compose<
                    meta_eval<std::add_lvalue_reference>::apply,
                    meta_eval<std::add_const>::apply>::apply<T>;
        }

        template<std::size_t N>
        using size_t = std::integral_constant<std::size_t, N>;

        struct void_t
        {};

        template<typename...Ts>
        struct tagged_variant;

        template<std::size_t N, typename Variant>
        struct tagged_variant_element;

        template<std::size_t N, typename Variant>
        using tagged_variant_element_t = typename tagged_variant_element<N, Variant>::type;

        template<std::size_t N, typename...Ts>
        detail::add_ref_t<tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> &var);

        template<std::size_t N, typename...Ts>
        detail::add_cref_t<tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> const &var);

        template<std::size_t N, typename...Ts>
        meta_eval<std::add_rvalue_reference>::
            apply<tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> &&var);

        namespace detail
        {
            struct any
            {
                template<typename T>
                any(T &&)
                {}
            };

            template<typename T>
            struct ref
            {
            private:
                T *t_;
            public:
                ref() = default;
                ref(T &t)
                  : t_(std::addressof(t))
                {}
                T &get() const
                {
                    return *t_;
                }
            };

            template<typename T>
            T &unwrap_ref(T &t)
            {
                return t;
            }

            template<typename T>
            T &unwrap_ref(ref<T> t)
            {
                return t.get();
            }

            template<typename T>
            struct wrap_ref
            {
                using type = T;
            };

            template<typename T>
            struct wrap_ref<T &>
            {
                using type = ref<T>;
            };

            template<typename T>
            using wrap_ref_t = typename wrap_ref<T>::type;

            template<typename BinaryFunction, typename T, std::size_t N,
                CONCEPT_REQUIRES_(ranges::Callable<BinaryFunction, T, size_t<N>>())>
            void apply_if(BinaryFunction &&fun, T &t, size_t<N> u)
            {
                detail::forward<BinaryFunction>(fun)(t, u);
            }

            template<typename BinaryFunction, typename T, std::size_t N,
                CONCEPT_REQUIRES_(!ranges::Callable<BinaryFunction, T, size_t<N>>())>
            void apply_if(BinaryFunction &&, T &, size_t<N>)
            {
                RANGES_ASSERT(false);
            }

            template<typename...List>
            union variant_data;

            template<>
            union variant_data<>
            {
                template<typename Fun, std::size_t N>
                void apply(std::size_t, Fun &&, size_t<N>) const
                {
                    RANGES_ASSERT(false);
                }
            };

            template<typename T, typename ...Ts>
            union variant_data<T, Ts...>
            {
            private:
                template<typename U>
                using enable_if_ok = enable_if_t<typelist_in<U, typelist<Ts...>>::value>;
                using head_t = decay_t<wrap_ref_t<T>>;
                using tail_t = variant_data<Ts...>;

                head_t head;
                tail_t tail;

                template<typename This, typename Fun, std::size_t N>
                static void apply_(This &this_, std::size_t n, Fun &&fun, size_t<N> u)
                {
                    if(0 == n)
                        detail::apply_if(detail::forward<Fun>(fun), this_.head, u);
                    else
                        this_.tail.apply(n - 1, detail::forward<Fun>(fun), size_t<N + 1>{});
                }
            public:
                variant_data()
                {}
                template<typename U,
                    enable_if_t<std::is_constructible<head_t, U>::value> = 0>
                variant_data(size_t<0>, U &&u)
                  : head(std::forward<U>(u))
                {}
                template<std::size_t N, typename U,
                    enable_if_t<0 != N && std::is_constructible<tail_t, size_t<N - 1>, U>::value> = 0>
                variant_data(size_t<N>, U &&u)
                  : tail{size_t<N - 1>{}, std::forward<U>(u)}
                {}
                ~variant_data()
                {}
                template<typename Fun, std::size_t N>
                void apply(std::size_t n, Fun &&fun, size_t<N> u)
                {
                    this->apply_(*this, n, std::forward<Fun>(fun), u);
                }
                template<typename Fun, std::size_t N>
                void apply(std::size_t n, Fun &&fun, size_t<N> u) const
                {
                    this->apply_(*this, n, std::forward<Fun>(fun), u);
                }
            };

            struct delete_fun
            {
                template<typename T>
                void operator()(T const &t) const
                {
                    t.T::~T();
                }
            };

            // Is there a less dangerous way?
            template<typename T>
            struct set_fun
            {
            private:
                T &&t_;
            public:
                set_fun(T &&t)
                  : t_(std::forward<T>(t))
                {}
                template<typename U,
                    enable_if_t<std::is_constructible<U, T>::value> = 0>
                void operator()(U &u) const
                {
                    ::new(static_cast<void*>(std::addressof(u))) U(std::forward<T>(t_));
                }
                template<typename U,
                    enable_if_t<!std::is_constructible<U, T>::value> = 0>
                void operator()(U &u) const
                {
                    RANGES_ASSERT(false);
                }
            };

            template<typename T>
            struct get_fun
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
                void operator()(any) const
                {
                    RANGES_ASSERT(false);
                }
            };

            struct move_fun
            {
                template<typename T>
                T &&operator()(T &t) const
                {
                    return std::move(t);
                }
            };

            struct identity_fun
            {
                template<typename T>
                T &operator()(T &t) const
                {
                    return t;
                }
            };

            template<typename Fun, typename Variant>
            struct apply_visitor
            {
            private:
                Variant &var_;
                Fun fun_;
                template<typename T, std::size_t N>
                void apply_(T &&t, size_t<N>, std::true_type) const
                {
                    fun_(std::forward<T>(t));
                }
                template<typename T, std::size_t N>
                void apply_(T &&t, size_t<N>, std::false_type) const
                {
                    var_.template set<N>(fun_(std::forward<T>(t)));
                }
            public:
                apply_visitor(Fun &&fun, Variant &var)
                  : fun_(std::forward<Fun>(fun)), var_(var)
                {}
                template<typename T, std::size_t N>
                void operator()(T &&t, size_t<N> u) const
                {
                    this->apply_(std::forward<T>(t), u, std::is_void<result_of_t<Fun(T &&)>>{});
                }
            };

            template<typename Fun>
            struct apply_visitor<Fun, void_t>
            {
            private:
                Fun fun_;
            public:
                apply_visitor(Fun &&fun, void_t &)
                  : fun_(std::forward<Fun>(fun))
                {}
                template<typename T, std::size_t N>
                void operator()(T &&t, size_t<N>) const
                {
                    fun_(std::forward<T>(t));
                }
            };

            template<typename Data, typename Fun, typename Variant>
            static void apply_visitor_(std::size_t n, Data &data, Fun &&fun, Variant &var)
            {
                apply_visitor<Fun, Variant> vis{std::forward<Fun>(fun), var};
                data.apply(n, vis, size_t<0>{});
            }
            template<typename Data, typename Fun>
            static void apply_visitor_(std::size_t n, Data &data, Fun &&fun)
            {
                void_t var;
                detail::apply_visitor_(n, data, std::forward<Fun>(fun), var);
            }

            template<typename Fun, typename Types>
            using variant_result_t =
                typelist_expand_t<
                    ranges::tagged_variant,
                    typelist_replace_t<void, void_t,
                        typelist_transform_t<Types,
                            meta_bind1st<concepts::Callable::result_t, Fun>::template apply> > >;

            template<typename UnaryFunction>
            struct unwrap_fun
            {
            private:
                UnaryFunction fun_;
            public:
                unwrap_fun(UnaryFunction &&fun)
                  : fun_(std::forward<UnaryFunction>(fun))
                {}
                template<typename T>
                auto operator()(T &&t) const ->
                    decltype(fun_(detail::unwrap_ref(std::forward<T>(t))))
                {
                    return fun_(detail::unwrap_ref(std::forward<T>(t)));
                }
            };
        }

        template<typename ...Ts>
        struct tagged_variant
        {
        private:
            using types_t = typelist<Ts...>;
            using data_t = detail::variant_data<Ts...>;
            std::size_t which_;
            data_t data_;

            void clear_()
            {
                if(is_valid())
                {
                    void_t res;
                    detail::apply_visitor_(which_, data_, detail::delete_fun{}, res);
                    which_ = (std::size_t)-1;
                }
            }

            template<std::size_t N, typename...Us>
            friend detail::add_ref_t<tagged_variant_element_t<N, tagged_variant<Us...>>>
            get(tagged_variant<Us...> &var);

            template<std::size_t N, typename...Us>
            friend detail::add_cref_t<tagged_variant_element_t<N, tagged_variant<Us...>>>
            get(tagged_variant<Us...> const &var);

            template<std::size_t N, typename...Us>
            friend meta_eval<std::add_rvalue_reference>::
                apply<tagged_variant_element_t<N, tagged_variant<Us...>>>
            get(tagged_variant<Us...> &&var);
        public:
            tagged_variant()
              : which_((std::size_t)-1), data_{}
            {}
            template<std::size_t N, typename U,
                enable_if_t<std::is_constructible<data_t, size_t<N>, U>::value> = 0>
            tagged_variant(size_t<N> n, U &&u)
              : which_(N), data_{n, detail::forward<U>(u)}
            {}
            tagged_variant(tagged_variant &&that)
              : tagged_variant{}
            {
                if(that.is_valid())
                    detail::apply_visitor_(that.which_, that.data_, detail::move_fun{}, *this);
            }
            tagged_variant(tagged_variant const &that)
              : tagged_variant{}
            {
                if(that.is_valid())
                    detail::apply_visitor_(that.which_, that.data_, detail::identity_fun{}, *this);
            }
            tagged_variant &operator=(tagged_variant &&that)
            {
                clear_();
                if(that.is_valid())
                    detail::apply_visitor_(that.which_, that.data_, detail::move_fun{}, *this);
                return *this;
            }
            tagged_variant &operator=(tagged_variant const &that)
            {
                clear_();
                if(that.is_valid())
                    detail::apply_visitor_(that.which_, that.data_, detail::identity_fun{}, *this);
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
                enable_if_t<std::is_constructible<data_t, size_t<N>, U>::value> = 0>
            void set(U &&u)
            {
                clear_();
                detail::apply_visitor_(N, data_, detail::set_fun<U>{std::forward<U>(u)});
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
                detail::unwrap_fun<Fun> ufun{std::forward<Fun>(fun)};
                detail::apply_visitor_(which_, data_, ufun, res);
                return res;
            }
            template<typename Fun,
                typename Args = typelist_transform_t<types_t, detail::add_cref_t>,
                typename Result = detail::variant_result_t<Fun, Args >>
            Result apply(Fun &&fun) const
            {
                Result res;
                detail::unwrap_fun<Fun> ufun{std::forward<Fun>(fun)};
                detail::apply_visitor_(which_, data_, ufun, res);
                return res;
            }
        };

        template<std::size_t N, typename...Ts>
        struct tagged_variant_element<N, tagged_variant<Ts...>>
        {
            using type =
                detail::lazy_conditional_t<
                    std::is_reference<typelist_element_t<N, typelist<Ts...>>>::value,
                    typelist_element<N, typelist<Ts...>>,
                    std::decay<typelist_element_t<N, typelist<Ts...>>>>;
        };

        // Accessors
        template<std::size_t N, typename...Ts>
        detail::add_ref_t<tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> &var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta_eval<std::remove_reference>::apply<
                    tagged_variant_element_t<N, tagged_variant<Ts...>>>;
            elem_t *elem = nullptr;
            detail::unwrap_fun<detail::get_fun<elem_t>> get{elem};
            detail::apply_visitor_(N, var.data_, get);
            RANGES_ASSERT(elem != nullptr);
            return *elem;
        }

        template<std::size_t N, typename...Ts>
        detail::add_cref_t<tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> const &var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta_compose<
                    meta_eval<std::remove_reference>::apply,
                    meta_eval<std::add_const>::apply>::apply<
                        tagged_variant_element_t<N, tagged_variant<Ts...>>>;
            elem_t *elem = nullptr;
            detail::unwrap_fun<detail::get_fun<elem_t>> get{elem};
            detail::apply_visitor_(N, var.data_, get);
            RANGES_ASSERT(elem != nullptr);
            return *elem;
        }

        template<std::size_t N, typename...Ts>
        meta_eval<std::add_rvalue_reference>::
            apply<tagged_variant_element_t<N, tagged_variant<Ts...>>>
        get(tagged_variant<Ts...> &&var)
        {
            RANGES_ASSERT(N == var.which());
            using elem_t =
                meta_eval<std::remove_reference>::apply<
                    tagged_variant_element_t<N, tagged_variant<Ts...>>>;
            elem_t *elem = nullptr;
            detail::unwrap_fun<detail::get_fun<elem_t>> get{elem};
            detail::apply_visitor_(N, var.data_, get);
            RANGES_ASSERT(elem != nullptr);
            return std::forward<tagged_variant_element_t<N, tagged_variant<Ts...>>>(*elem);
        }
    }
}

#endif
