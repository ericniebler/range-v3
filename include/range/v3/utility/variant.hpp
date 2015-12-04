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

        template<std::size_t N, typename...Ts, typename...Args>
        void emplace(variant<Ts...> &var, Args &&...args);

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

        template<typename T, std::size_t Index>
        struct indexed_element
          : reference_wrapper<T>
        {
            using reference_wrapper<T>::reference_wrapper;
        };
        template<typename T, std::size_t Index>
        struct indexed_element<T &, Index>
          : reference_wrapper<T>
        {
            using reference_wrapper<T>::reference_wrapper;
        };
        template<typename T, std::size_t Index>
        struct indexed_element<T &&, Index>
          : reference_wrapper<T, true>
        {
            using reference_wrapper<T, true>::reference_wrapper;
        };
        template<std::size_t Index>
        struct indexed_element<void, Index>
        {
            void get() const
            {}
        };

        /// \cond
        namespace detail
        {
            template<typename T, typename Index>
            struct indexed_datum
            {
            private:
                T datum_;
            public:
                CONCEPT_REQUIRES(DefaultConstructible<T>())
                indexed_datum()
                  : datum_{}
                {}
                template<typename... Ts,
                    CONCEPT_REQUIRES_(Constructible<T, Ts &&...>())>
                indexed_datum(Ts &&... ts)
                  : datum_(std::forward<Ts>(ts)...)
                {}
                indexed_element<T, Index::value> ref()
                {
                    return {datum_};
                }
                indexed_element<T const, Index::value> ref() const
                {
                    return {datum_};
                }
                T &get() noexcept
                {
                    return datum_;
                }
                T const &get() const noexcept
                {
                    return datum_;
                }
            };
            template<typename T, typename Index>
            struct indexed_datum<T &, Index>
              : reference_wrapper<T>
            {
                indexed_datum() = delete;
                using reference_wrapper<T>::reference_wrapper;
                indexed_element<T &, Index::value> ref() const
                {
                    return {this->get()};
                }
            };
            template<typename T, typename Index>
            struct indexed_datum<T &&, Index>
              : reference_wrapper<T, true>
            {
                indexed_datum() = delete;
                using reference_wrapper<T, true>::reference_wrapper;
                indexed_element<T &&, Index::value> ref() const
                {
                    return {this->get()};
                }
            };
            template<typename Index>
            struct indexed_datum<void, Index>
            {
                void get() const
                {}
                indexed_element<void, Index::value> ref() const
                {
                    return {};
                }
            };

            template<typename...Ts>
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
            };

            template<typename T, typename ...Ts>
            union variant_data_<T, Ts...>
            {
                T head;
                variant_data_<Ts...> tail;

                variant_data_() {}
                template<typename ...Args,
                    CONCEPT_REQUIRES_(Constructible<T, Args...>())>
                constexpr variant_data_(meta::size_t<0>, Args && ...args)
                  : head(detail::forward<Args>(args)...)
                {}
                template<std::size_t N, typename ...Args,
                    CONCEPT_REQUIRES_(0 != N &&
                        Constructible<variant_data_<Ts...>, meta::size_t<N - 1>, Args &&...>())>
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
                        tail.move_copy_construct(n - 1, ((That &&) that).tail);
                }
                template<typename U, typename...Us>
                bool equal(std::size_t n, variant_data_<U, Us...> const &that) const
                {
                    return n == 0 ? head.get() == that.head.get() : tail.equal(n - 1, that.tail);
                }
            };

            template<typename... Ts>
            using variant_data =
                meta::apply_list<
                    meta::quote<variant_data_>,
                    meta::transform<
                        meta::list<Ts...>,
                        meta::as_list<meta::make_index_sequence<sizeof...(Ts)> >,
                        meta::quote<indexed_datum> > >;

            struct indexed_element_fn
            {
                template<typename T>
                auto operator()(T &&t) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    t.ref()
                )
            };

            template<std::size_t N = 0, typename Fun, typename Proj = indexed_element_fn>
            void variant_visit_(variant_data_<>, std::size_t, Fun, Proj = {})
            {}
            template<std::size_t N = 0, typename Data, typename Fun, typename Proj = indexed_element_fn>
            void variant_visit_(Data &d, std::size_t n, Fun fun, Proj proj = {})
            {
                0 == n ? (void) fun(proj(d.head)) :
                    detail::variant_visit_<N + 1>(d.tail, n - 1, std::move(fun), std::move(proj));
            }

            struct empty_variant_tag
            {};

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

            struct delete_fn
            {
                template<typename T>
                void operator()(T const &t) const
                {
                    t.~T();
                }
            };

            template<std::size_t N, typename... Ts>
            struct construct_fn
            {
                std::tuple<Ts...> args_;

                template<typename U, std::size_t ...Is>
                void construct_(U &u, meta::index_sequence<Is...>)
                {
                    ::new((void*)std::addressof(u)) U(std::forward<Ts>(std::get<Is>(args_))...);
                }

                construct_fn(Ts &&...ts)
                  : args_{std::forward<Ts>(ts)...}
                {}
                template<typename U, std::size_t M>
                [[noreturn]] meta::if_c<N != M> operator()(indexed_datum<U, meta::size_t<M>> &)
                {
                    RANGES_ENSURE(false);
                }
                template<typename U>
                meta::if_<std::is_object<U>>
                    operator()(indexed_datum<U, meta::size_t<N>> &u)
                {
                    this->construct_(u.get(), meta::make_index_sequence<sizeof...(Ts)>{});
                }
                template<typename U>
                meta::if_<meta::not_<std::is_object<U>>>
                    operator()(indexed_datum<U, meta::size_t<N>> &u)
                {
                    this->construct_(u, meta::make_index_sequence<sizeof...(Ts)>{});
                }
            };

            template<typename T, std::size_t N>
            struct get_fn
            {
                T **t_;

                template<typename U, std::size_t M>
                [[noreturn]] meta::if_c<M != N> operator()(indexed_element<U, M>) const
                {
                    throw bad_variant_access("bad varaiant access");
                }
                template<typename U>
                void operator()(indexed_element<U, N> t) const
                {
                    *t_ = std::addressof(t.get());
                }
                void operator()(indexed_element<void, N> t) const
                {}
            };

            template<typename Variant, std::size_t N>
            struct emplace_fn
            {
                Variant *var_;
                template<typename...Ts>
                void operator()(Ts &&...ts) const
                {
                    ranges::emplace<N>(*var_, std::forward<Ts>(ts)...);
                }
            };

            template<typename Fun, typename Variant>
            struct variant_visitor
            {
                Fun fun_;
                Variant *var_;

                template<typename U, std::size_t N>
                void operator()(indexed_element<U, N> u)
                {
                    compose(emplace_fn<Variant, N>{var_}, fun_)(u);
                }
            };

            template<typename Variant, typename Fun>
            variant_visitor<Fun, Variant> make_variant_visitor(Variant &var, Fun fun)
            {
                return {std::move(fun), &var};
            }

            template<typename To, typename From>
            struct unique_visitor;

            template<typename ...To, typename ...From>
            struct unique_visitor<variant<To...>, variant<From...>>
            {
                variant<To...> *var_;

                template<typename T, std::size_t N>
                void operator()(indexed_element<T, N> t) const
                {
                    using E = meta::at_c<meta::list<From...>, N>;
                    using F = meta::find<meta::list<To...>, E>;
                    static constexpr std::size_t M = sizeof...(To) - F::size();
                    ranges::emplace<M>(*var_, t.get());
                }
                template<std::size_t N>
                void operator()(indexed_element<void, N>) const
                {
                    using E = meta::at_c<meta::list<From...>, N>;
                    using F = meta::find<meta::list<To...>, E>;
                    static constexpr std::size_t M = sizeof...(To) - F::size();
                    ranges::emplace<M>(*var_);
                }
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

        /// \addtogroup group-utility
        /// @{
        template<typename ...Ts>
        struct variant
        {
        private:
            friend struct detail::variant_core_access;
            template<typename...Us>
            friend struct variant;
            template<std::size_t Index>
            using datum_t =
                detail::indexed_datum<meta::at_c<meta::list<Ts...>, Index>, meta::size_t<Index>>;
            template<typename T>
            using add_const_t =
                meta::if_<std::is_void<T>, void, T const>;
            template<typename Fun, typename T>
            using alt_result_t =
                concepts::Function::result_t<Fun, indexed_element<T, 0>>;

            struct unbox_fn
            {
                template<typename T>
                auto operator()(T &&t) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    t.get()
                )
            };

            std::size_t index_;
            detail::variant_data<Ts...> data_;

            void clear_()
            {
                if(valid())
                {
                    detail::variant_visit_(data_, index_, detail::delete_fn{}, ident{});
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
            constexpr variant(detail::empty_variant_tag)
              : index_((std::size_t)-1)
            {}

        public:
            CONCEPT_REQUIRES(DefaultConstructible<datum_t<0>>())
            constexpr variant()
              : variant{emplaced_index<0>}
            {}
            template<std::size_t N, typename...Args,
                CONCEPT_REQUIRES_(Constructible<datum_t<N>, Args &&...>())>
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
                CONCEPT_REQUIRES_(Constructible<datum_t<N>, Args &&...>())>
            void emplace(Args &&...args)
            {
                this->clear_();
                detail::construct_fn<N, Args&&...> fn{std::forward<Args>(args)...};
                detail::variant_visit_(data_, N, std::ref(fn), ident{});
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
            variant<alt_result_t<composed<Fun, unbox_fn>, Ts>...> visit(Fun fun)
            {
                variant<alt_result_t<composed<Fun, unbox_fn>, Ts>...> res{
                    detail::empty_variant_tag{}};
                detail::variant_visit_(data_, index_,
                    detail::make_variant_visitor(res, compose(std::move(fun), unbox_fn{})));
                return res;
            }
            template<typename Fun>
            variant<alt_result_t<composed<Fun, unbox_fn>, add_const_t<Ts>>...> visit(Fun fun) const
            {
                variant<alt_result_t<composed<Fun, unbox_fn>, add_const_t<Ts>>...> res{
                    detail::empty_variant_tag{}};
                detail::variant_visit_(data_, index_,
                    detail::make_variant_visitor(res, compose(std::move(fun), unbox_fn{})));
                return res;
            }
            template<typename Fun>
            variant<alt_result_t<Fun, Ts>...> visit_i(Fun fun)
            {
                variant<alt_result_t<Fun, Ts>...> res{detail::empty_variant_tag{}};
                detail::variant_visit_(data_, index_,
                    detail::make_variant_visitor(res, std::move(fun)));
                return res;
            }
            template<typename Fun>
            variant<alt_result_t<Fun, add_const_t<Ts>>...> visit_i(Fun fun) const
            {
                variant<alt_result_t<Fun, add_const_t<Ts>>...> res{detail::empty_variant_tag{}};
                detail::variant_visit_(data_, index_,
                    detail::make_variant_visitor(res, std::move(fun)));
                return res;
            }
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
            elem_t *elem = nullptr;
            detail::variant_visit_(detail::variant_core_access::data(var),
                var.index(), detail::get_fn<elem_t, N>{&elem});
            return detail::variant_deref_(elem);
        }

        template<std::size_t N, typename...Ts>
        meta::_t<std::add_lvalue_reference<
            meta::_t<std::tuple_element<N, variant<Ts...> const>> const>>
        get(variant<Ts...> const &var)
        {
            using elem_t = meta::_t<std::remove_reference<
                meta::_t<std::tuple_element<N, variant<Ts...> const>>>>;
            elem_t *elem = nullptr;
            detail::variant_visit_(detail::variant_core_access::data(var),
                var.index(), detail::get_fn<elem_t, N>{&elem});
            return detail::variant_deref_(elem);
        }

        template<std::size_t N, typename...Ts>
        meta::_t<std::add_rvalue_reference<meta::_t<std::tuple_element<N, variant<Ts...>>>>>
        get(variant<Ts...> &&var)
        {
            using elem_t = meta::_t<std::remove_reference<
                meta::_t<std::tuple_element<N, variant<Ts...>>>>>;
            elem_t *elem = nullptr;
            detail::variant_visit_(detail::variant_core_access::data(var),
                var.index(), detail::get_fn<elem_t, N>{&elem});
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
            auto res = detail::variant_core_access::make_empty(meta::id<To>{});
            var.visit_i(detail::unique_visitor<To, From>{&res});
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
