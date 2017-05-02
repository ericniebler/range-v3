/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2017.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_POLY_HPP
#define RANGES_V3_UTILITY_POLY_HPP

#if defined(__cpp_noexcept_function_type) && \
    defined(__cpp_nontype_template_args) && \
    defined(__cpp_lib_invoke) && \
    defined(__cpp_if_constexpr) && \
    defined(__cpp_template_auto) && \
    defined(__cpp_inline_variables)

#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <range/v3/detail/config.hpp>
#include <meta/meta.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct poly_access
            {
                template <std::size_t N, typename This, typename...As>
                static auto call(This&& _this, As&&...args) ->
                    decltype(static_cast<This&&>(_this)
                        .template _poly_call_<N>(static_cast<As&&>(args)...))
                {
                    return static_cast<This&&>(_this)
                        .template _poly_call_<N>(static_cast<As&&>(args)...);
                }

                template <class Node>
                static typename Node::_poly_self_ self_();

                template <class Node>
                using self = decltype(poly_access::self_<Node>());
            };
        }

        template <auto...Ps>
        struct members
        {};

        template <class I>
        struct poly;

        struct bad_poly_access
          : std::exception
        {
            bad_poly_access() = default;
            char const* what() const noexcept override
            {
                return "bad_poly_access";
            }
        };

        struct bad_poly_cast
          : std::bad_cast
        {
            bad_poly_cast() = default;
            char const* what() const noexcept override
            {
                return "bad_poly_cast";
            }
        };

        template <class...I>
        struct extends : private I...
        {
            using subsumptions = meta::list<I...>;

            template <class Base>
            struct interface : Base
            {};

            template <class...>
            using members = members<>;
        };

        template <std::size_t N, typename This, typename...As>
        auto call(This&& _this, As&&...args) ->
            decltype(detail::poly_access::call<N>(
                static_cast<This&&>(_this), static_cast<As&&>(args)...))
        {
            return detail::poly_access::call<N>(
                static_cast<This&&>(_this), static_cast<As&&>(args)...);
        }

        template <class Node>
        using self = detail::poly_access::self<Node>;

        namespace detail
        {
        struct TypeInfo;

        struct poly_base
        {};

        template <class I, class = void>
        struct subsumptions_of_
        {
            using type = meta::list<I>;
        };

        template <class I>
        struct subsumptions_of_<I, std::void_t<typename I::subsumptions>>
        {
            using type =
                meta::push_front<
                    meta::join<
                        meta::transform<
                            typename I::subsumptions,
                            meta::quote_trait<subsumptions_of_>>>,
                    I>;
        };

        template <class I>
        using subsumptions_of =
            meta::reverse<
                meta::unique<
                    meta::reverse<
                        meta::push_back<meta::_t<subsumptions_of_<I>>, TypeInfo>>>>;

        struct bottom
        {
            template <class T>
            [[noreturn]] /* implicit */ operator T&&() const
            {
                std::terminate();
            }
        };

        struct archetype_node
        {
            template <class State, class I>
            using invoke = typename I::template interface<State>;
        };

        template <class I>
        struct archetype_root;

        template <class I>
        using archetype =
            meta::reverse_fold<subsumptions_of<I>, archetype_root<I>, archetype_node>;

        struct archetype_root_base : bottom
        {
            template <std::size_t, class...As>
            [[noreturn]] bottom _poly_call_(As&&...) const
            {
                std::terminate();
            }
            friend bool operator==(archetype_root_base const&, archetype_root_base const&);
            friend bool operator<(archetype_root_base const&, archetype_root_base const&);
        };

        template <class I>
        struct archetype_root : archetype_root_base
        {
            using _poly_self_ = archetype<I>;
        };

        template <class I, class T>
        using members_of = typename I::template members<T>;

        template <class I, class T>
        using interface_of = typename I::template interface<T>;

        struct data
        {
            data() = default;
            // Suppress compiler-generated copy ops to not copy anything:
            data(data const&)
            {}
            data& operator=(data const&)
            {
                return *this;
            }
            union
            {
                void* pobj_ = nullptr;
                std::aligned_storage_t<sizeof(double[2])> buff_;
            };
        };

        template <class B>
        using requires_ = meta::if_<B, int>;

        template <class T>
        using uncvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

        using _rref =
            meta::quote_trait<std::add_rvalue_reference>;
        using _lref =
            meta::quote_trait<std::add_lvalue_reference>;

        template<typename T>
        struct _xref
          : meta::id<meta::quote_trait<meta::id>>
        {};
        template<typename T>
        using xref = meta::_t<_xref<T>>;
        template<typename T>
        struct _xref<T &&>
          : meta::id<meta::compose<_rref, xref<T>>>
        {};
        template<typename T>
        struct _xref<T &>
          : meta::id<meta::compose<_lref, xref<T>>>
        {};
        template<typename T>
        struct _xref<T const>
          : meta::id<meta::quote_trait<std::add_const>>
        {};
        template<typename T>
        struct _xref<T volatile>
          : meta::id<meta::quote_trait<std::add_volatile>>
        {};
        template<typename T>
        struct _xref<T const volatile>
          : meta::id<meta::quote_trait<std::add_cv>>
        {};

        template <class A, class B>
        using same_cvref_t = meta::invoke<xref<B>, A>;

        template <class I, class XRef>
        struct _poly;

        template <class IRef>
        struct _poly_ref;

        template <class U, class I>
        using arg =
            meta::if_<std::is_same<uncvref_t<U>, archetype<I>>, _poly_ref<same_cvref_t<I, U>>, U>;

        template <class Member, class I>
        struct signature_of_;

        template <class R, class C, class...As, class I>
        struct signature_of_<R (C::*)(As...), I>
        {
            using type = R(*)(data&, arg<As, I>...);
        };

        template <class R, class C, class...As, class I>
        struct signature_of_<R (C::*)(As...) const, I>
        {
            using type = R(*)(data const&, arg<As, I>...);
        };

        template <class R, class This, class...As, class I>
        struct signature_of_<R(*)(This&, As...), I>
        {
            using type = R(*)(data&, arg<As, I>...);
        };

        template <class R, class This, class...As, class I>
        struct signature_of_<R(*)(This const&, As...), I>
        {
            using type = R(*)(data const&, arg<As, I>...);
        };

        template <auto Arch, class I = TypeInfo>
        using signature_of = meta::_t<signature_of_<decltype(Arch), I>>;

        template <auto User, class Sig = signature_of<User>>
        struct arg_types_;

        template <auto User, class Ret, class Data, class...Args>
        struct arg_types_<User, Ret(*)(Data, Args...)>
        {
            using type = meta::list<Args...>;
        };

        template <auto User>
        using arg_types = meta::_t<arg_types_<User>>;

        template <class R, class...Args>
        using fn_ptr_t = R(*)(Args...);

        struct throw_thunk
        {
            template <class R, class...Args>
            constexpr operator fn_ptr_t<R, Args...> () const noexcept
            {
                return [](Args...) -> R { throw bad_poly_access(); };
            }
        };

        inline constexpr throw_thunk const throw_ {};

        template <class T, class U = std::decay_t<T>>
        inline constexpr bool in_situ_v =
            sizeof(U) <= sizeof(data) && std::is_nothrow_move_constructible<U>::value;

        template <class T>
        T& get(data& d)
        {
            if constexpr (in_situ_v<T>)
                return *static_cast<T*>(static_cast<void*>(&d.buff_));
            else
                return *static_cast<T*>(d.pobj_);
        }

        template <class T>
        T const & get(data const& d)
        {
            if constexpr (in_situ_v<T>)
                return *static_cast<T const*>(static_cast<void const*>(&d.buff_));
            else
                return *static_cast<T const*>(d.pobj_);
        }

        enum class state : short { empty, in_situ, on_heap };

        inline void noop_destroy(data&)
        {}
        inline void noop_copy(data const&, data&)
        {}
        inline void noop_move(data&, data&)
        {}

        template <class, class U>
        U&& convert(U&& u)
        {
            return static_cast<U&&>(u);
        }

        template <class Arg, class I, class XRef>
        auto convert(_poly<I, XRef> u)
        {
            return u.template as<Arg>();
        }

        template <class Arg, class I>
        auto convert(_poly_ref<I> u)
        {
            return u.template as<Arg>();
        }

        template <class T, auto User, class = arg_types<User>>
        struct thunk_fn;

        template <class T, auto User, class...Args>
        struct thunk_fn<T, User, meta::list<Args...>>
        {
            template <class R, class D, class... As>
            constexpr operator fn_ptr_t<R, D&, As...> () const noexcept
            {
                return [](D& d, As...as) -> R
                {
                    return std::invoke(User, get<T>(d), convert<Args>(static_cast<As&&>(as))...);
                };
            }
        };

        template <class T, auto User>
        inline constexpr thunk_fn<T, User> thunk {};

        template <class T>
        struct basic_thunks
        {
            static void destroy(data& d)
            {
                if constexpr (in_situ_v<T>)
                    get<T>(d).~T();
                else
                    delete &get<T>(d);
            }
            static void copy(data const& from, data& to)
            {
                if constexpr (std::is_copy_constructible<T>::value)
                {
                    if constexpr (in_situ_v<T>)
                        ::new(static_cast<void*>(&to.buff_)) T(get<T>(from));
                    else
                        to.pobj_ = new T(get<T>(from));
                }
            }
            static void move(data& from, data& to)
            {
                if constexpr (in_situ_v<T>)
                {
                    ::new(static_cast<void*>(&to.buff_)) T(std::move(get<T>(from)));
                    get<T>(from).~T();
                }
                else
                    to.pobj_ = std::exchange(from.pobj_, nullptr);
            }
            static std::type_info const& type(T const&)
            {
                return typeid(T);
            }
            static void* cast(T& t, std::type_info const& ti)
            {
                return ti == typeid(T)
                    ? std::addressof(t)
                    : throw ::ranges::bad_poly_cast();
            }
        };

        struct TypeInfo
        {
            template <class Base>
            struct interface : Base
            {
                std::type_info const& type_info() const noexcept
                {
                    return ::ranges::call<0>(*this);
                }
                template <class T>
                T& cast()
                {
                    return *static_cast<T*>(::ranges::call<1>(*this, typeid(T)));
                }
                template <class T>
                T const& cast() const
                {
                    return const_cast<interface*>(this)->cast<T>();
                }
            };

            template <class T>
            using members = ::ranges::members<
                &basic_thunks<T>::type,
                &basic_thunks<T>::cast>;
        };

        template <class>
        struct vtable_;

        template <class I>
        using vtable = vtable_<subsumptions_of<I>>;

        template <class I, class T>
        inline constexpr vtable<I> const vtbl_of{meta::id<T>{}};

        template <class I>
        inline constexpr vtable<I> const vtbl{};

        template <class I, class = members_of<I, archetype<I>>>
        struct vtable_node;

        template <class I, auto...Arch>
        struct vtable_node<I, members<Arch...>>
          : std::tuple<signature_of<Arch, I>...>
        {
        private:
            template <class T, auto...User>
            static constexpr std::tuple<signature_of<Arch, I>...> make(members<User...>)
            {
                return {thunk<T, User>...};
            }
            template <class T>
            static vtable<I> const* as_base_ptr_fn_()
            {
                return &vtbl_of<I, T>;
            }
        public:
            constexpr vtable_node() noexcept
              : std::tuple<signature_of<Arch, I>...>{
                    static_cast<signature_of<Arch, I>>(throw_)...}
            {}
            template <class T>
            explicit constexpr vtable_node(meta::id<T>) noexcept
              : std::tuple<signature_of<Arch, I>...>(make<T>(members_of<I, T>{}))
              , as_base_ptr_(&as_base_ptr_fn_<T>)
            {}
            vtable<I> const* (*as_base_ptr_)() = nullptr;
        };

        template <class...S>
        struct vtable_<meta::list<S...>>
          : vtable_node<S>...
        {
            constexpr vtable_() noexcept
              : vtable_node<S>{}...
              , state_(state::empty)
              , destroy_(noop_destroy)
              , copy_(noop_copy)
              , move_(noop_move)
            {}
            template <class T>
            explicit constexpr vtable_(meta::id<T> t) noexcept
              : vtable_node<S>{t}...
              , state_(in_situ_v<T> ? state::in_situ : state::on_heap)
              , destroy_(basic_thunks<T>::destroy)
              , copy_(basic_thunks<T>::copy)
              , move_(basic_thunks<T>::move)
            {}
            state state_;
            void (*destroy_)(data&);
            void (*copy_)(data const &, data&);
            void (*move_)(data &, data&);
        };

        template <class I>
        decltype(auto) select(vtable_node<meta::id_t<I>> const& vtbl)
        {
            return vtbl;
        }

        template <class P, class I, class Tail>
        struct poly_base_node
          : Tail
        {
        private:
            friend struct poly<I>;
            friend poly_access;
            using _poly_self_ = ::ranges::poly<P>;

            template <std::size_t K, typename...As>
            decltype(auto) _poly_call_(As&&...as)
            {
                auto& poly = static_cast<::ranges::poly<P>&>(*this);
                return std::get<K>(select<I>(*poly.vptr_))(
                    poly, static_cast<As&&>(as)...);
            }
            template <std::size_t K, typename...As>
            decltype(auto) _poly_call_(As&&...as) const
            {
                auto& poly = static_cast<::ranges::poly<P> const&>(*this);
                return std::get<K>(select<I>(*poly.vptr_))(
                    poly, static_cast<As&&>(as)...);
            }
        };

        template <class P>
        struct poly_base_helper
        {
            template <class State, class I>
            using invoke = interface_of<I, poly_base_node<P, I, State>>;
        };

        template <class I>
        using poly_impl =
            meta::reverse_fold<subsumptions_of<I>, poly_base, poly_base_helper<I>>;

        template <class I, class XRef>
        struct _poly
        {
        private:
            friend _poly_ref<meta::invoke<XRef, I>>;
            data* d_;
            void* (*cast_)(data&, std::type_info const&);

            template <class T>
            /* implicit */ _poly(T&& _this)
              : d_(const_cast<data*>(static_cast<data const*>(
                    &static_cast<::ranges::poly<I> const &>(_this))))
              , cast_(std::get<1>(select<TypeInfo>(
                    *static_cast<::ranges::poly<I> const &>(_this).vptr_)))
            {
                static_assert(
                    std::is_convertible<T&&, meta::invoke<XRef, uncvref_t<T>>>::value,
                    "Conversion not allowed because it would lose qualifiers.");
            }
            template <class T>
            meta::invoke<XRef, uncvref_t<T>>&& as() const
            {
                using U = uncvref_t<T>;
                using V = meta::invoke<XRef, U>;
                return static_cast<V&&>(*static_cast<U*>(cast_(*d_, typeid(T))));
            }
        };

        template <class I>
        struct _poly_ref<I &&>
          : private _poly<I, _rref>, archetype<I>
        {
            template <class P, requires_<std::is_base_of<I, P>> = 0>
            /* implicit */ _poly_ref(::ranges::poly<P>&& p)
              : _poly<I, _rref>(std::move(p))
            {}
            using _poly<I, _rref>::as;
        };

        template <class I>
        struct _poly_ref<I &>
          : private _poly<I, _lref>, archetype<I>
        {
            template <class P, requires_<std::is_base_of<I, P>> = 0>
            /* implicit */ _poly_ref(::ranges::poly<P>& p)
              : _poly<I, _lref>(p)
            {}
            using _poly<I, _lref>::as;
        };

        template <class I>
        struct _poly_ref<I const &>
          : private _poly<I, xref<I const &>>, archetype<I>
        {
            template <class P, requires_<std::is_base_of<I, P>> = 0>
            /* implicit */ _poly_ref(::ranges::poly<P> const& p)
              : _poly<I, xref<I const &>>(p)
            {}
            using _poly<I, xref<I const &>>::as;
        };

        template <class Fun>
        struct sig_t
        {
            template <class T>
            constexpr Fun T::* operator()(Fun T::*t) const
            {
                return t;
            }
            constexpr Fun* operator()(Fun* t) const
            {
                return t;
            }

            template <class F, class T>
            constexpr F T::* operator()(F T::*t) const
            {
                return t;
            }
            template <class F>
            constexpr F* operator()(F* t) const
            {
                return t;
            }
        };

        template <class R, class...As>
        struct sig_t<R(As...) const>
        {
            using Fun = R(As...) const;
            template <class T>
            constexpr Fun T::* operator()(Fun T::*t) const
            {
                return t;
            }
            template <class F, class T>
            constexpr F T::* operator()(F T::*t) const
            {
                return t;
            }
        };

        template <class R>
        struct sig_t<R()>
        {
            using Fun = R();
            using CMFun = R() const;

            template <class T>
            constexpr Fun T::* operator()(Fun T::*t) const
            {
                return t;
            }
            constexpr Fun* operator()(Fun* t) const
            {
                return t;
            }
            template <class T>
            constexpr CMFun T::* operator()(CMFun T::*t) const volatile //@nolint
            {
                return t;
            }

            template <class F, class T>
            constexpr F T::* operator()(F T::*t) const
            {
                return t;
            }
        };

        template <class R, class A, class...As>
        struct sig_t<R(A&, As...)>
        {
            using Fun = R(A&, As...);
            using CMFun = R(A&, As...) const;
            using CCFun = R(A const&, As...);

            template <class T>
            constexpr Fun T::* operator()(Fun T::*t) const
            {
                return t;
            }
            constexpr Fun* operator()(Fun* t) const
            {
                return t;
            }
            template <class T>
            constexpr CMFun T::* operator()(CMFun T::*t) const volatile //@nolint
            {
                return t;
            }
            constexpr CCFun* operator()(CCFun* t) const volatile //@nolint
            {
                return t;
            }

            template <class F, class T>
            constexpr F T::* operator()(F T::*t) const
            {
                return t;
            }
            template <class F>
            constexpr F* operator()(F* t) const
            {
                return t;
            }
        };

        } // namespace detail

        template <class I>
        struct poly final
          : detail::poly_impl<I>, private detail::data
        {
        private:
            template <class> friend struct detail::_poly_ref;
            template <class, class> friend struct detail::_poly;
            template <class> friend struct poly;
            template <class, class, class> friend struct detail::poly_base_node;
            using copyable = std::is_copy_constructible<detail::poly_impl<I>>;
            template <class T>
            using not_poly = meta::not_<meta::is<T, ::ranges::poly>>;
            using poly_or_nonesuch = meta::if_<copyable, poly, struct nonesuch>;

            detail::vtable<I> const *vptr_ = &detail::vtbl<I>;
        public:
            poly() = default;
            poly(poly&& that)
            {
                that.vptr_->move_(that, *this);
                vptr_ = std::exchange(that.vptr_, &detail::vtbl<I>);
            }
            poly(poly_or_nonesuch const& that)
            {
                that.vptr_->copy_(that, *this);
                vptr_ = that.vptr_;
            }
            ~poly()
            {
                vptr_->destroy_(*this);
            }
            poly& operator=(poly that) noexcept
            {
                vptr_->destroy_(*this);
                that.vptr_->move_(that, *this);
                vptr_ = std::exchange(that.vptr_, &detail::vtbl<I>);
                return *this;
            }

            template <
                class T,
                class U = std::decay_t<T>,
                detail::requires_<not_poly<U>> = 0,
                detail::requires_<std::is_constructible<U, T>> = 0,
                class = detail::members_of<I, U>>
            /* implicit */ poly(T &&t)
            {
                static_assert(std::is_copy_constructible<U>::value || !copyable::value);
                if constexpr (detail::in_situ_v<U>)
                    ::new(static_cast<void*>(&buff_)) U(static_cast<T&&>(t));
                else
                    pobj_ = new U(static_cast<T&&>(t));
                vptr_ = &detail::vtbl_of<I, U>;
            }

            template <class I2, meta::if_<std::is_base_of<I, I2>, int> = 0>
            poly(poly<I2> that)
            {
                static_assert(!copyable::value || poly<I2>::copyable::value);
                if (that.vptr_->state_ != detail::state::empty)
                {
                    that.vptr_->move_(that, *this);
                    vptr_ = detail::select<I>(*that.vptr_).as_base_ptr_();
                    that.vptr_ = &detail::vtbl<I2>;
                }
            }

            template <
                class T,
                class U = std::decay_t<T>,
                detail::requires_<not_poly<U>> = 0,
                detail::requires_<std::is_constructible<U, T>> = 0,
                class = detail::members_of<I, U>>
            poly& operator=(T&& t)
            {
                *this = poly(static_cast<T&&>(t));
                return *this;
            }
            template <class I2, meta::if_<std::is_base_of<I, I2>, int> = 0>
            poly& operator=(poly<I2> that)
            {
                *this = poly(std::move(that));
                return *this;
            }

            bool empty() const noexcept
            {
                return vptr_->state_ == detail::state::empty;
            }

            void swap(poly& that) noexcept
            {
                switch (vptr_->state_)
                {
                case detail::state::empty:
                    *this = std::move(that);
                    break;            
                case detail::state::on_heap:
                    if (detail::state::on_heap == that.vptr_->state_)
                    {
                        std::swap(pobj_, that.pobj_);
                        std::swap(vptr_, that.vptr_);
                    }
                    else // fall through
                case detail::state::in_situ:
                    std::swap(*this, that);
                }
            }

            explicit operator bool() const noexcept
            {
                return !empty();
            }

            bool operator!() const noexcept
            {
                return empty();
            }
        };

        template <class I>
        void swap(poly<I>& left, poly<I>& right) noexcept
        {
            left.swap(right);
        }

        template <std::size_t N, class I, class P, class Tail, typename...As>
        decltype(auto) call(detail::poly_base_node<P, I, Tail>&& _this, As&&...args)
        {
            using This = detail::interface_of<I, detail::poly_base_node<P, I, Tail>>;
            return detail::poly_access::call<N>(
                static_cast<This&&>(_this), static_cast<As&&>(args)...);
        }

        template <std::size_t N, class I, class P, class Tail, typename...As>
        decltype(auto) call(detail::poly_base_node<P, I, Tail>& _this, As&&...args)
        {
            using This = detail::interface_of<I, detail::poly_base_node<P, I, Tail>>;
            return detail::poly_access::call<N>(
                static_cast<This&>(_this), static_cast<As&&>(args)...);
        }

        template <std::size_t N, class I, class P, class Tail, typename...As>
        decltype(auto) call(detail::poly_base_node<P, I, Tail> const& _this, As&&...args)
        {
            using This = detail::interface_of<I, detail::poly_base_node<P, I, Tail>>;
            return detail::poly_access::call<N>(
                static_cast<This const&>(_this), static_cast<As&&>(args)...);
        }

        template <std::size_t N, class I, class P, typename...As>
        decltype(auto) call(detail::archetype_root<P>&&, As&&...args)
        {
            static_assert(std::is_base_of<I, P>::value);
            detail::archetype<I> _this;
            return detail::poly_access::call<N>(
                std::move(_this), static_cast<As&&>(args)...);
        }

        template <std::size_t N, class I, class P, typename...As>
        decltype(auto) call(detail::archetype_root<P>&, As&&...args)
        {
            static_assert(std::is_base_of<I, P>::value);
            detail::archetype<I> _this;
            return detail::poly_access::call<N>(_this, static_cast<As&&>(args)...);
        }

        template <std::size_t N, class I, class P, typename...As>
        decltype(auto) call(detail::archetype_root<P> const&, As&&...args)
        {
            static_assert(std::is_base_of<I, P>::value);
            detail::archetype<I> const _this;
            return detail::poly_access::call<N>(_this, static_cast<As&&>(args)...);
        }

        template <class Sig>
        inline constexpr detail::sig_t<Sig> const sig = {};

        struct Noncopyable : extends<>
        {
            template <class Base>
            struct interface : Base
            {
                interface() = default;
                interface(interface const&) = delete;
                interface(interface &&) = default;
                interface& operator=(interface const&) = delete;
                interface& operator=(interface &&) = default;
            };
        };

        struct EqualityComparable
        {
            static bool implicit_bool_(bool);

            template <class T>
            static auto is_equal_(T const& _this, T const& that) ->
                decltype(implicit_bool_(_this == that))
            {
                return _this == that;
            }

            template <class Base>
            struct interface : Base
            {
                friend bool operator==(self<Base> const& _this, self<Base> const& that)
                {
                    return _this.type_info() == that.type_info()
                        ? ::ranges::call<0, EqualityComparable>(_this, that)
                        : false;
                }
                friend bool operator!=(self<Base> const& _this, self<Base> const& that)
                {
                    return !(_this == that);
                }
            };

            template <class T>
            using members = ::ranges::members<&is_equal_<T>>;
        };
    }
}

#endif
#endif
