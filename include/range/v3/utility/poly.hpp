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
            struct call_helper
            {
                template <std::size_t N, typename This, typename...As>
                static decltype(auto) call(This&& _this, As&&...args)
                {
                    return static_cast<This&&>(_this)
                        .template poly_call_<N>(static_cast<As&&>(args)...);
                }
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
        };

        template <std::size_t N, typename This, typename...As>
        decltype(auto) call(This&& _this, As&&...args)
        {
            return detail::call_helper::call<N>(
                static_cast<This&&>(_this), static_cast<As&&>(args)...);
        }

        namespace detail
        {
            struct TypeInfo;

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

            struct archetype_root
            {
                template <std::size_t, class...As>
                [[noreturn]] bottom poly_call_(As&&...) const
                {
                    std::terminate();
                }
            };

            struct archetype_helper
            {
                template <class State, class I>
                using invoke = typename I::template interface<State>;
            };

            template <class I>
            using archetype =
                meta::reverse_fold<subsumptions_of<I>, archetype_root, archetype_helper>;

            template <class I, class T>
            using members_of = typename I::template members<T>;

            template <class I, class T = archetype<I>>
            using interface_of = typename I::template interface<T>;

            struct data
            {
                data() = default;
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

            template <class Member>
            struct signature_of_;

            template <class R, class C, class...As>
            struct signature_of_<R (C::*)(As...)>
            {
                using type = R(*)(data&, As...);
            };

            template <class R, class C, class...As>
            struct signature_of_<R (C::*)(As...) const>
            {
                using type = R(*)(data const&, As...);
            };

            template <class R, class This, class...As>
            struct signature_of_<R(*)(This&, As...)>
            {
                using type = R(*)(data&, As...);
            };

            template <class R, class This, class...As>
            struct signature_of_<R(*)(This const&, As...)>
            {
                using type = R(*)(data const&, As...);
            };

            template <auto Arch>
            using signature_of = meta::_t<signature_of_<decltype(Arch)>>;

            template <class R, class...Args>
            using fn_t = R(*)(Args...);

            struct throw_thunk
            {
                template <class R, class...Args>
                constexpr operator fn_t<R, Args...> () const noexcept
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
            inline std::type_info const& noop_type() noexcept
            {
                return typeid(void);
            }

            template <class T, auto User>
            struct thunk_fn
            {
                template <class R, class D, class... As>
                constexpr operator fn_t<R, D&, As...> () const noexcept
                {
                    return [](D& d, As...as) -> R
                    {
                        return std::invoke(User, get<T>(d), static_cast<As&&>(as)...);
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
                    // BUGBUG
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
                static void const* ccast(T const& t, std::type_info const& ti)
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
                        return *static_cast<T const*>(::ranges::call<2>(*this, typeid(T)));
                    }
                };

                template <class T>
                using members = ::ranges::members<
                    &basic_thunks<T>::type,
                    &basic_thunks<T>::cast,
                    &basic_thunks<T>::ccast>;
            };

            template <class>
            struct vtable_;

            template <class I>
            using vtable = vtable_<subsumptions_of<I>>;

            template <class I, class T = void>
            inline constexpr vtable<I> const vtbl{meta::id<T>{}};

            template <class I>
            inline constexpr vtable<I> const vtbl<I, void>{};

            template <class I, class = members_of<I, archetype<I>>>
            struct vtable_node;

            template <class I, auto...Arch>
            struct vtable_node<I, members<Arch...>>
              : std::tuple<signature_of<Arch>...>
            {
            private:
                template <class T, auto...User>
                static constexpr std::tuple<signature_of<Arch>...> make(members<User...>)
                {
                    return {thunk<T, User>...};
                }
                static constexpr std::tuple<signature_of<Arch>...> noop()
                {
                    return {static_cast<signature_of<Arch>>(throw_)...};
                }
                template <class T>
                static vtable<I> const* as_base_ptr_fn_()
                {
                    return &vtbl<I, T>;
                }
            public:
                constexpr vtable_node() noexcept
                  : std::tuple<signature_of<Arch>...>(noop())
                {}
                template <class T>
                explicit constexpr vtable_node(meta::id<T> t) noexcept
                  : std::tuple<signature_of<Arch>...>(make<T>(members_of<I, T>{}))
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

            template <class Poly, class I, class Tail>
            struct poly_base_node
              : Tail
            {
            private:
                friend struct poly<I>;
                friend call_helper;

                template <std::size_t K, typename...As>
                decltype(auto) poly_call_(As&&...as)
                {
                    auto& poly = static_cast<Poly&>(*this);
                    return std::get<K>(select<I>(*poly.vptr_))(poly, static_cast<As&&>(as)...);
                }
                template <std::size_t K, typename...As>
                decltype(auto) poly_call_(As&&...as) const
                {
                    auto& poly = static_cast<Poly const&>(*this);
                    return std::get<K>(select<I>(*poly.vptr_))(poly, static_cast<As&&>(as)...);
                }
            };

            template <class Poly>
            struct poly_base_helper
            {
                template <class State, class I>
                using invoke = interface_of<I, poly_base_node<Poly, I, State>>;
            };

            template <class Poly, class I>
            using poly_base =
                meta::reverse_fold<subsumptions_of<I>, meta::nil_, poly_base_helper<Poly>>;

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
          : detail::poly_base<poly<I>, I>, private detail::data
        {
        private:
            template <class> friend struct poly;
            template <class, class, class> friend struct detail::poly_base_node;
            template <class T>
            using if_not_poly = meta::if_<meta::not_<meta::is<T, ::ranges::poly>>, int>;
            detail::vtable<I> const *vptr_ = &detail::vtbl<I>;
        public:
            poly() = default;
            poly(poly&& that)
            {
                that.vptr_->move_(that, *this);
                vptr_ = std::exchange(that.vptr_, &detail::vtbl<I>);
            }
            poly(poly const& that)
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
                if_not_poly<U> = 0,
                class = detail::members_of<I, U>>
            /* implicit */ poly(T &&t)
            {
                if constexpr (detail::in_situ_v<U>)
                    ::new(static_cast<void*>(&buff_)) U(static_cast<T&&>(t));
                else
                    pobj_ = new U(static_cast<T&&>(t));
                vptr_ = &detail::vtbl<I, U>;
            }

            template <class I2, meta::if_<std::is_base_of<I, I2>, int> = 0>
            poly(poly<I2> that)
            {
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
                if_not_poly<U> = 0,
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

        template <class Sig>
        inline constexpr detail::sig_t<Sig> const sig = {};
    }
}

#endif
#endif
