/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_DETAIL_VARIANT_HPP
#define RANGES_V3_DETAIL_VARIANT_HPP

#include <new>
#include <tuple>
#include <memory>
#include <utility>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
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
    #if !RANGES_CXX_VARIABLE_TEMPLATES
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

    #if RANGES_CXX_INLINE_VARIABLES < RANGES_CXX_INLINE_VARIABLES_17
        inline namespace
        {
            template<std::size_t I>
            constexpr auto& emplaced_index = static_const<emplaced_index_t<I>>::value;
        }
    #else // RANGES_CXX_INLINE_VARIABLES >= RANGES_CXX_INLINE_VARIABLES_17
        template<std::size_t I>
        inline constexpr emplaced_index_t<I> emplaced_index{};
    #endif  // RANGES_CXX_INLINE_VARIABLES

        /// \cond
    #define RANGES_EMPLACED_INDEX_T(I) emplaced_index_t<I>
    #endif
        /// \endcond

        struct bad_variant_access
          : std::logic_error
        {
            explicit bad_variant_access(std::string const &what_arg)
              : std::logic_error(what_arg)
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
        template<std::size_t Index>
        struct indexed_element<void, Index>
        {
            void get() const noexcept
            {}
        };

        /// \cond
        namespace detail
        {
            struct indexed_element_fn;

            template<typename I, typename S, typename O,
                CONCEPT_REQUIRES_(!SizedSentinel<S, I>())>
            O uninitialized_copy(I first, S last, O out)
            {
                for(; first != last; ++first, ++out)
                    ::new((void *) std::addressof(*out)) value_type_t<O>(*first);
                return out;
            }

            template<typename I, typename S, typename O,
                CONCEPT_REQUIRES_(SizedSentinel<S, I>())>
            O uninitialized_copy(I first, S last, O out)
            {
                return std::uninitialized_copy_n(first, (last - first), out);
            }

            template<typename I, typename O>
            O uninitialized_copy(I first, I last, O out)
            {
                return std::uninitialized_copy(first, last, out);
            }

            template<typename T, typename Index>
            struct indexed_datum
            {
            private:
                T datum_;
            public:
                CONCEPT_REQUIRES(DefaultConstructible<T>())
                constexpr indexed_datum()
                    noexcept(std::is_nothrow_default_constructible<T>::value)
                  : datum_{}
                {}
                template<typename... Ts,
                    CONCEPT_REQUIRES_(Constructible<T, Ts...>())>
                constexpr indexed_datum(Ts &&... ts)
                    noexcept(std::is_nothrow_constructible<T, Ts...>::value)
                  : datum_(static_cast<Ts&&>(ts)...)
                {}
                RANGES_CXX14_CONSTEXPR indexed_element<T, Index::value> ref() noexcept
                {
                    return {datum_};
                }
                constexpr indexed_element<T const, Index::value> ref() const noexcept
                {
                    return {datum_};
                }
                RANGES_CXX14_CONSTEXPR T &get() noexcept
                {
                    return datum_;
                }
                constexpr T const &get() const noexcept
                {
                    return datum_;
                }
            };

            template<typename T, std::size_t N, typename Index>
            struct indexed_datum<T[N], Index>;

            template<typename T, typename Index>
            struct indexed_datum<T &, Index>
              : reference_wrapper<T &>
            {
                indexed_datum() = delete;
                using reference_wrapper<T &>::reference_wrapper;
                constexpr indexed_element<T &, Index::value> ref() const noexcept
                {
                    return {this->get()};
                }
            };
            template<typename T, typename Index>
            struct indexed_datum<T &&, Index>
              : reference_wrapper<T &&>
            {
                indexed_datum() = delete;
                using reference_wrapper<T &&>::reference_wrapper;
                constexpr indexed_element<T &&, Index::value> ref() const noexcept
                {
                    return {this->get()};
                }
            };
            template<typename Index>
            struct indexed_datum<void, Index>
            {
                void get() const noexcept
                {}
                constexpr indexed_element<void, Index::value> ref() const noexcept
                {
                    return {};
                }
            };

            template<std::size_t Index, typename... Ts>
            using variant_datum_t =
                detail::indexed_datum<meta::at_c<meta::list<Ts...>, Index>, meta::size_t<Index>>;
        } // namespace detail

        template<std::size_t N, typename... Ts, typename... Args,
            meta::if_c<Constructible<detail::variant_datum_t<N, Ts...>, Args...>::value, int> = 42>
        void emplace(variant<Ts...>&, Args &&...);

        namespace detail
        {
            using variant_nil = indexed_datum<void, meta::npos>;

            template<typename Ts, bool Trivial =
                meta::apply<
                    meta::quote<meta::and_>,
                    meta::transform<Ts, meta::quote<std::is_trivially_destructible>>>::type::value>
            struct variant_data_
            {
                using type = indexed_datum<void, meta::npos>;
            };

            template<typename T, typename... Ts>
            struct variant_data_<meta::list<T, Ts...>, true>
            {
                struct type
                {
                    using head_t = T;
                    using tail_t = meta::_t<variant_data_<meta::list<Ts...>>>;
                    union
                    {
                        head_t head;
                        tail_t tail;
                    };

                    type() noexcept
                    {}
                    template<typename... Args>
                    constexpr type(meta::size_t<0>, Args &&... args)
                        noexcept(std::is_nothrow_constructible<head_t, Args...>::value)
                      : head{((Args &&) args)...}
                    {}
                    template<std::size_t N, typename... Args>
                    constexpr type(meta::size_t<N>, Args &&... args)
                        noexcept(std::is_nothrow_constructible<tail_t, meta::size_t<N - 1>, Args...>::value)
                      : tail{meta::size_t<N - 1>{}, ((Args &&) args)...}
                    {}
                };
            };

            template<typename T, typename... Ts>
            struct variant_data_<meta::list<T, Ts...>, false>
            {
                struct type
                {
                    using head_t = T;
                    using tail_t = meta::_t<variant_data_<meta::list<Ts...>>>;
                    union
                    {
                        head_t head;
                        tail_t tail;
                    };

                    type() noexcept
                    {}
                    ~type()
                    {}
                    template<typename... Args>
                    constexpr type(meta::size_t<0>, Args &&... args)
                        noexcept(std::is_nothrow_constructible<head_t, Args...>::value)
                      : head{((Args &&) args)...}
                    {}
                    template<std::size_t N, typename... Args>
                    constexpr type(meta::size_t<N>, Args &&... args)
                        noexcept(std::is_nothrow_constructible<tail_t, meta::size_t<N - 1>, Args...>::value)
                      : tail{meta::size_t<N - 1>{}, ((Args &&) args)...}
                    {}
                };
            };

            template<typename... Ts>
            using variant_data =
                meta::_t<variant_data_<
                    meta::transform<
                        meta::list<Ts...>,
                        meta::as_list<meta::make_index_sequence<sizeof...(Ts)>>,
                        meta::quote<indexed_datum>>>>;

            inline std::size_t variant_move_copy_(std::size_t, variant_nil, variant_nil)
            {
                return 0;
            }
            template<typename Data0, typename Data1>
            std::size_t variant_move_copy_(std::size_t n, Data0 &self, Data1 &&that)
            {
                using Head = typename Data0::head_t;
                return 0 == n ? ((void)::new((void*)&self.head) Head(((Data1 &&) that).head), 0) :
                    variant_move_copy_(n - 1, self.tail, ((Data1 &&) that).tail) + 1;
            }
            constexpr bool variant_equal_(std::size_t, variant_nil, variant_nil)
            {
                return true;
            }
            template<typename Data0, typename Data1>
            constexpr bool variant_equal_(std::size_t n, Data0 const &self, Data1 const &that)
            {
                return n == 0 ? self.head.get() == that.head.get() :
                    variant_equal_(n - 1, self.tail, that.tail);
            }
            template<typename Fun, typename Proj = indexed_element_fn>
            constexpr int variant_visit_(std::size_t, variant_nil, Fun, Proj = {})
            {
                return (RANGES_EXPECT(false), 0);
            }
            template<typename Data, typename Fun, typename Proj = indexed_element_fn>
            constexpr int variant_visit_(std::size_t n, Data &self, Fun fun, Proj proj = {})
            {
                return 0 == n ? ((void) invoke(fun, invoke(proj, self.head)), 0) :
                    detail::variant_visit_(n - 1, self.tail, detail::move(fun), detail::move(proj));
            }

            struct get_datum_fn
            {
                template<typename T>
                auto operator()(T &&t) const noexcept
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    t.get()
                )
            };

            struct indexed_element_fn
            {
                template<typename T>
                auto operator()(T &&t) const noexcept
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    t.ref()
                )
            };

            struct empty_variant_tag
            {};

            struct variant_core_access
            {
                template<typename...Ts>
                static constexpr variant_data<Ts...> &data(variant<Ts...> &var) noexcept
                {
                    return var.data_();
                }
                template<typename...Ts>
                static constexpr variant_data<Ts...> const &data(variant<Ts...> const &var) noexcept
                {
                    return var.data_();
                }
                template<typename...Ts>
                static constexpr variant_data<Ts...> &&data(variant<Ts...> &&var) noexcept
                {
                    return detail::move(var.data_());
                }
                template<typename...Ts>
                static variant<Ts...> make_empty(meta::id<variant<Ts...>> = {}) noexcept
                {
                    return variant<Ts...>{empty_variant_tag{}};
                }
            };

            struct delete_fn
            {
                template<typename T>
                void operator()(T const &t) const noexcept
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
                    noexcept(std::is_nothrow_constructible<U, Ts...>::value)
                {
                    ::new((void*)std::addressof(u)) U(static_cast<Ts&&>(std::get<Is>(args_))...);
                }

                construct_fn(Ts &&...ts)
                    noexcept(std::is_nothrow_constructible<std::tuple<Ts...>, Ts...>::value)
                  : args_{static_cast<Ts&&>(ts)...}
                {}
                template<typename U, std::size_t M>
                [[noreturn]] meta::if_c<N != M> operator()(indexed_datum<U, meta::size_t<M>> &) noexcept
                {
                    RANGES_EXPECT(false);
                }
                template<typename U>
                meta::if_<std::is_object<U>>
                    operator()(indexed_datum<U, meta::size_t<N>> &u)
                    noexcept(std::is_nothrow_constructible<U, Ts...>::value)
                {
                    this->construct_(u.get(), meta::make_index_sequence<sizeof...(Ts)>{});
                }
                template<typename U>
                meta::if_<meta::not_<std::is_object<U>>>
                    operator()(indexed_datum<U, meta::size_t<N>> &u)
                    noexcept(std::is_nothrow_constructible<detail::decay_t<U>, Ts...>::value)
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
                    throw bad_variant_access("bad variant access");
                }
                template<typename U>
                void operator()(indexed_element<U, N> t) const noexcept
                {
                    *t_ = std::addressof(t.get());
                }
                void operator()(indexed_element<void, N>) const noexcept
                {}
            };

            template<typename Variant, std::size_t N>
            struct emplace_fn
            {
                Variant *var_;
                template<typename...Ts>
                auto operator()(Ts &&...ts) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::emplace<N>(*var_, static_cast<Ts&&>(ts)...)
                )
            };

            template<typename Fun, typename Variant>
            struct variant_visitor
            {
                Fun fun_;
                Variant *var_;

                template<typename U, std::size_t N>
                auto operator()(indexed_element<U, N> u)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    compose(emplace_fn<Variant, N>{var_}, fun_)(u)
                )
            };

            template<typename Variant, typename Fun>
            variant_visitor<Fun, Variant> make_variant_visitor(Variant &var, Fun fun)
                noexcept(std::is_nothrow_move_constructible<Fun>::value)
            {
                return {detail::move(fun), &var};
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
                    static_assert(std::is_same<T const, E const>::value,
                        "Is indexed_element broken?");
                    using F = meta::find<meta::list<To...>, E>;
                    static constexpr std::size_t M = sizeof...(To) - F::size();
                    compose(emplace_fn<variant<To...>, M>{var_}, get_datum_fn{})(t);
                }
            };

            template<typename T>
            constexpr T &variant_deref_(T *t) noexcept
            {
                return *t;
            }
            inline void variant_deref_(void const volatile *) noexcept
            {}

            template<typename Variant, bool Trivial = std::is_trivially_destructible<
                meta::apply<meta::quote<variant_data>, meta::as_list<Variant>>>::value>
            struct variant_base
            {
                ~variant_base()
                {
                    static_cast<Variant*>(this)->clear_();
                }
            };
            template<typename ...Ts>
            struct variant_base<variant<Ts...>, true>
            {};

            template<typename Fun, typename Types, typename Indices, typename = void>
            struct variant_visit_results {};
            template<typename Fun, typename... Ts, std::size_t... Is>
            struct variant_visit_results<
                Fun, meta::list<Ts...>, meta::index_sequence<Is...>,
                meta::void_<invoke_result_t<Fun&, indexed_element<Ts, Is>>...>>
            {
                using type = variant<invoke_result_t<Fun&, indexed_element<Ts, Is>>...>;
            };
            template<typename Fun, typename... Ts>
            using variant_visit_results_t = meta::_t<
                variant_visit_results<
                    Fun, meta::list<Ts...>, meta::make_index_sequence<sizeof...(Ts)>>>;
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{
        template<typename ...Ts>
        struct variant
          : private detail::variant_data<Ts...>
          , private detail::variant_base<variant<Ts...>>
        {
        private:
            friend detail::variant_core_access;
            template<typename...>
            friend struct variant;
            friend detail::variant_base<variant, false>;
            template<std::size_t Index>
            using datum_t =
                detail::variant_datum_t<Index, Ts...>;
            template<typename T>
            using add_const_t =
                meta::if_<std::is_void<T>, void, T const>;
            using unbox_fn = detail::get_datum_fn;

            detail::variant_data<Ts...> &data_() & noexcept
            {
                return *this;
            }
            detail::variant_data<Ts...> const &data_() const & noexcept
            {
                return *this;
            }
            detail::variant_data<Ts...> &&data_() && noexcept
            {
                return static_cast<detail::variant_data<Ts...> &&>(*this);
            }

            std::size_t index_;

            void clear_() noexcept
            {
                if(valid())
                {
                    detail::variant_visit_(index_, data_(), detail::delete_fn{}, ident{});
                    index_ = (std::size_t)-1;
                }
            }
            template<typename That>
            void assign_(That &&that)
            {
                if(that.valid())
                    index_ = detail::variant_move_copy_(that.index_, data_(), ((That &&) that).data_());
            }
            constexpr variant(detail::empty_variant_tag) noexcept
              : detail::variant_data<Ts...>{}, index_((std::size_t)-1)
            {}

        public:
            CONCEPT_REQUIRES(DefaultConstructible<datum_t<0>>())
            constexpr variant()
                noexcept(std::is_nothrow_default_constructible<datum_t<0>>::value)
              : variant{emplaced_index<0>}
            {}
            template<std::size_t N, typename...Args,
                CONCEPT_REQUIRES_(Constructible<datum_t<N>, Args...>())>
            constexpr variant(RANGES_EMPLACED_INDEX_T(N), Args &&...args)
                noexcept(std::is_nothrow_constructible<datum_t<N>, Args...>::value)
              : detail::variant_data<Ts...>{meta::size_t<N>{}, static_cast<Args&&>(args)...}
              , index_(N)
            {}
            template<std::size_t N, typename T, typename...Args,
                CONCEPT_REQUIRES_(Constructible<datum_t<N>, std::initializer_list<T> &, Args...>())>
            constexpr variant(RANGES_EMPLACED_INDEX_T(N), std::initializer_list<T> il, Args &&...args)
                noexcept(std::is_nothrow_constructible<datum_t<N>, std::initializer_list<T> &, Args...>::value)
              : detail::variant_data<Ts...>{meta::size_t<N>{}, il, static_cast<Args &&>(args)...}
              , index_(N)
            {}
            template<std::size_t N,
                CONCEPT_REQUIRES_(Constructible<datum_t<N>, meta::nil_>())>
            constexpr variant(RANGES_EMPLACED_INDEX_T(N), meta::nil_)
                noexcept(std::is_nothrow_constructible<datum_t<N>, meta::nil_>::value)
              : detail::variant_data<Ts...>{meta::size_t<N>{}, meta::nil_{}}, index_(N)
            {}
            variant(variant &&that)
              : detail::variant_data<Ts...>{}
              , index_(detail::variant_move_copy_(that.index(), data_(), detail::move(that.data_())))
            {}
            variant(variant const &that)
              : detail::variant_data<Ts...>{}
              , index_(detail::variant_move_copy_(that.index(), data_(), that.data_()))
            {}
            variant &operator=(variant &&that)
            {
                // TODO do a simple move assign when index()==that.index()
                this->clear_();
                this->assign_(detail::move(that));
                return *this;
            }
            variant &operator=(variant const &that)
            {
                // TODO do a simple copy assign when index()==that.index()
                this->clear_();
                this->assign_(that);
                return *this;
            }
            static constexpr std::size_t size() noexcept
            {
                return sizeof...(Ts);
            }
            template<std::size_t N, typename ...Args,
                CONCEPT_REQUIRES_(Constructible<datum_t<N>, Args...>())>
            void emplace(Args &&...args)
            {
                this->clear_();
                detail::construct_fn<N, Args&&...> fn{static_cast<Args&&>(args)...};
                detail::variant_visit_(N, data_(), std::ref(fn), ident{});
                index_ = N;
            }
            constexpr bool valid() const noexcept
            {
                return index() != (std::size_t)-1;
            }
            constexpr std::size_t index() const noexcept
            {
                return index_;
            }
            template<typename Fun>
            detail::variant_visit_results_t<composed<Fun, unbox_fn>, Ts...> visit(Fun fun)
            {
                detail::variant_visit_results_t<composed<Fun, unbox_fn>, Ts...> res{
                    detail::empty_variant_tag{}};
                detail::variant_visit_(index_, data_(),
                    detail::make_variant_visitor(res, compose(detail::move(fun), unbox_fn{})));
                return res;
            }
            template<typename Fun>
            detail::variant_visit_results_t<composed<Fun, unbox_fn>, add_const_t<Ts>...>
            visit(Fun fun) const
            {
                detail::variant_visit_results_t<composed<Fun, unbox_fn>, add_const_t<Ts>...> res{
                    detail::empty_variant_tag{}};
                detail::variant_visit_(index_, data_(),
                    detail::make_variant_visitor(res, compose(detail::move(fun), unbox_fn{})));
                return res;
            }
            template<typename Fun>
            detail::variant_visit_results_t<Fun, Ts...> visit_i(Fun fun)
            {
                detail::variant_visit_results_t<Fun, Ts...> res{detail::empty_variant_tag{}};
                detail::variant_visit_(index_, data_(),
                    detail::make_variant_visitor(res, detail::move(fun)));
                return res;
            }
            template<typename Fun>
            detail::variant_visit_results_t<Fun, add_const_t<Ts>...> visit_i(Fun fun) const
            {
                detail::variant_visit_results_t<Fun, add_const_t<Ts>...> res{detail::empty_variant_tag{}};
                detail::variant_visit_(index_, data_(),
                    detail::make_variant_visitor(res, detail::move(fun)));
                return res;
            }
        };

        template<typename...Ts, typename...Us,
            CONCEPT_REQUIRES_(meta::and_c<(bool)EqualityComparable<Ts, Us>()...>::value)>
        bool operator==(variant<Ts...> const &lhs, variant<Us...> const &rhs)
        {
            return (!lhs.valid() && !rhs.valid()) ||
                (lhs.index() == rhs.index() &&
                    detail::variant_equal_(
                        lhs.index(),
                        detail::variant_core_access::data(lhs),
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
        meta::_t<std::add_lvalue_reference<meta::at_c<meta::list<Ts...>, N>>>
        get(variant<Ts...> &var)
        {
            using elem_t = meta::_t<std::remove_reference<meta::at_c<meta::list<Ts...>, N>>>;
            elem_t *elem = nullptr;
            auto &data = detail::variant_core_access::data(var);
            detail::variant_visit_(var.index(), data, detail::get_fn<elem_t, N>{&elem});
            return detail::variant_deref_(elem);
        }

        template<std::size_t N, typename...Ts>
        meta::_t<std::add_lvalue_reference<meta::at_c<meta::list<Ts...>, N> const>>
        get(variant<Ts...> const &var)
        {
            using elem_t = meta::_t<std::remove_reference<meta::at_c<meta::list<Ts...>, N> const>>;
            elem_t *elem = nullptr;
            auto &data = detail::variant_core_access::data(var);
            detail::variant_visit_(var.index(), data, detail::get_fn<elem_t, N>{&elem});
            return detail::variant_deref_(elem);
        }

        template<std::size_t N, typename...Ts>
        meta::_t<std::add_rvalue_reference<meta::at_c<meta::list<Ts...>, N>>>
        get(variant<Ts...> &&var)
        {
            using elem_t = meta::_t<std::remove_reference<meta::at_c<meta::list<Ts...>, N>>>;
            elem_t *elem = nullptr;
            auto &data = detail::variant_core_access::data(var);
            detail::variant_visit_(var.index(), data, detail::get_fn<elem_t, N>{&elem});
            using res_t = meta::_t<std::add_rvalue_reference<meta::at_c<meta::list<Ts...>, N>>>;
            return static_cast<res_t>(detail::variant_deref_(elem));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // emplace
        template<std::size_t N, typename... Ts, typename... Args,
            meta::if_c<Constructible<detail::variant_datum_t<N, Ts...>, Args...>::value, int>>
        void emplace(variant<Ts...> &var, Args &&...args)
        {
            var.template emplace<N>(static_cast<Args&&>(args)...);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // variant_unique
        template<typename Var>
        struct variant_unique
        {};

        template<typename ...Ts>
        struct variant_unique<variant<Ts...>>
        {
            using type = meta::apply<meta::quote<variant>, meta::unique<meta::list<Ts...>>>;
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
            RANGES_EXPECT(res.valid());
            return res;
        }
        /// @}
    }
}

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

namespace std
{
    template<typename...Ts>
    struct tuple_size<::ranges::variant<Ts...>>
      : tuple_size<tuple<Ts...>>
    {};

    template<size_t I, typename...Ts>
    struct tuple_element<I, ::ranges::variant<Ts...>>
      : tuple_element<I, tuple<Ts...>>
    {};
}

RANGES_DIAGNOSTIC_POP

#endif
