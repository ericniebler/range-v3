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
        template <auto...Ps>
        struct members;

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

        namespace detail
        {
            struct call_helper
            {
                template <std::size_t N, typename This, typename...As>
                static decltype(auto) call(This&& _this, As&&...args)
                {
                    return static_cast<This&&>(_this)
                        .template call<N>(static_cast<As&&>(args)...);
                }
            };

            struct any
            {
                template <class T>
                [[noreturn]] /* implicit */ operator T&&() const
                {
                    std::terminate();
                }
            };

            struct archetype
            {
                template <std::size_t N, class...As>
                [[noreturn]] any call(As&&...) const
                {
                    std::terminate();
                }
            };

            struct data_obj
            {
                union
                {
                    void* pobj_ = nullptr;
                    std::aligned_storage_t<sizeof(double[2])> buff_;
                };
            };

            template <class I, class T>
            using members_of = typename I::template members<T>;

            template <class I, class T = archetype>
            using interface_of = typename I::template interface<T>;

            template <class Member>
            struct signature_of_;

            template <class R, class C, class...As>
            struct signature_of_<R (C::*)(As...)>
            {
                using type = R(*)(data_obj&, As...);
            };

            template <class R, class C, class...As>
            struct signature_of_<R (C::*)(As...) const>
            {
                using type = R(*)(data_obj const&, As...);
            };

            template <class R, class This, class...As>
            struct signature_of_<R(*)(This&, As...)>
            {
                using type = R(*)(data_obj&, As...);
            };

            template <class R, class This, class...As>
            struct signature_of_<R(*)(This const&, As...)>
            {
                using type = R(*)(data_obj const&, As...);
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
                    return +[](Args...) -> R { throw bad_poly_access(); };
                }
            };
            namespace
            {
                inline constexpr throw_thunk const throw_ {};
            }

            namespace
            {
                template <class T, class U = std::decay_t<T>>
                inline constexpr bool in_situ_v =
                    sizeof(U) <= sizeof(data_obj) && std::is_nothrow_move_constructible_v<U>;
            }

            template <class T>
            T& get(data_obj& d)
            {
                if constexpr (in_situ_v<T>)
                    return *static_cast<T*>(static_cast<void*>(&d.buff_));
                else
                    return *static_cast<T*>(d.pobj_);
            }

            template <class T>
            T const & get(data_obj const& d)
            {
                if constexpr (in_situ_v<T>)
                    return *static_cast<T const*>(static_cast<void const*>(&d.buff_));
                else
                    return *static_cast<T const*>(d.pobj_);
            }

            enum class state : short { empty, in_situ, on_heap };

            inline void noop_destroy(data_obj&)
            {}
            inline void noop_copy(data_obj const&, data_obj&)
            {}
            inline void noop_move(data_obj&, data_obj&)
            {}
            inline std::type_info const& noop_type() noexcept
            {
                return typeid(void);
            }

            template <class>
            struct vtable_;

            template <auto...Arch>
            struct vtable_<members<Arch...>>
            {
                using type =
                    std::tuple<
                        state,
                        void(*)(data_obj&),                     // destroy
                        void(*)(data_obj const &, data_obj&),   // copy
                        void(*)(data_obj&, data_obj&),          // move
                        std::type_info const&(*)() noexcept,    // type
                        signature_of<Arch>...>;
            };

            template <class I>
            using vtable = meta::_t<vtable_<members_of<I, interface_of<I>>>>;

            template <class VTable, class Arch>
            struct make_noop_vtable_;

            template <class VTable, auto...Arch>
            struct make_noop_vtable_<VTable, members<Arch...>>
            {
                static constexpr VTable const value {
                    state::empty,
                    noop_destroy,
                    noop_copy,
                    noop_move,
                    noop_type,
                    static_cast<signature_of<Arch>>(throw_)...
                };
            };

            template <class VTable, auto...Arch>
            constexpr VTable const make_noop_vtable_<VTable, members<Arch...>>::value;

            template <class I>
            constexpr vtable<I> const* noop_vptr =
                &make_noop_vtable_<vtable<I>, members_of<I, interface_of<I>>>::value;

            template <class I>
            struct data_vptr
            {
                vtable<I> const* vptr_ = noop_vptr<I>;
            };

            template <class I>
            struct data
              : data_vptr<I>, data_obj
            {};

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

            namespace
            {
                template <class T, auto User>
                inline constexpr thunk_fn<T, User> thunk {};
            }

            template <class T>
            struct basic_thunks
            {
                static void destroy(data_obj& d)
                {
                    if constexpr (in_situ_v<T>)
                        get<T>(d).~T();
                    else
                        delete &get<T>(d);
                }
                static void copy(data_obj const& from, data_obj& to)
                {
                    if constexpr (std::is_copy_constructible_v<T>)
                    {
                        if constexpr (in_situ_v<T>)
                            ::new(static_cast<void*>(&to.buff_)) T(get<T>(from));
                        else
                            to.pobj_ = new T(get<T>(from));
                    }
                }
                static void move(data_obj& from, data_obj& to)
                {
                    if constexpr (in_situ_v<T>)
                    {
                        ::new(static_cast<void*>(&to.buff_)) T(std::move(get<T>(from)));
                        get<T>(from).~T();
                    }
                    else
                        to.pobj_ = std::exchange(from.pobj_, nullptr);
                }
                static std::type_info const& type() noexcept
                {
                    return typeid(T);
                }
            };

            template <class T, class VTable, class User>
            struct make_vtable_;

            template <class T, class VTable, auto...User>
            struct make_vtable_<T, VTable, members<User...>>
            {
                static constexpr VTable const value {
                    in_situ_v<T> ? state::in_situ : state::on_heap,
                    &basic_thunks<T>::destroy,
                    &basic_thunks<T>::copy,
                    &basic_thunks<T>::move,
                    &basic_thunks<T>::type,
                    thunk<T, User>...
                };
            };

            template <class T, class VTable, auto...User>
            constexpr VTable const make_vtable_<T, VTable, members<User...>>::value;

            template <class I, class T>
            constexpr vtable<I> const* vptr =
                &make_vtable_<T, vtable<I>, members_of<I, T>>::value;

            namespace
            {
                inline constexpr struct in_place_t {} in_place {};
            }

            template <class I>
            struct storage
            {
                storage() = default;
                storage(storage&& that)
                {
                    std::get<3>(*that.data_.vptr_)(that.data_, data_);
                    data_.vptr_ = std::exchange(that.data_.vptr_, noop_vptr<I>);
                }
                storage(storage const& that)
                {
                    std::get<2>(*that.data_.vptr_)(that.data_, data_);
                    data_.vptr_ = that.data_.vptr_;
                }
                ~storage()
                {
                    std::get<1>(*data_.vptr_)(data_);
                    data_.vptr_ = noop_vptr<I>;
                }
                storage& operator=(storage that) noexcept
                {
                    std::get<1>(*data_.vptr_)(data_); // destroy this
                    std::get<3>(*data_.vptr_)(that.data_, data_); // Move (nothrow)
                    data_.vptr_ = std::exchange(that.data_.vptr_, noop_vptr<I>);
                    return *this;
                }
                std::type_info const& type() const noexcept
                {
                    return std::get<4>(*data_.vptr_)();
                }
                bool empty() const noexcept
                {
                    return std::get<0>(*data_.vptr_) == state::empty;
                }
                void swap(storage<I>& that) noexcept
                {
                    switch (std::get<0>(*data_.vptr_))
                    {
                    case state::empty:
                        *this = std::move(that);
                        break;            
                    case state::on_heap:
                        if (state::on_heap == std::get<0>(*that.data_.vptr_))
                        {
                            std::swap(data_.pobj_, that.data_.pobj_);
                            std::swap(data_.vptr_, that.data_.vptr_);
                        }
                        else // fall through
                    case state::in_situ:
                        std::swap(*this, that);
                    }
                }

            private:
                friend struct poly<I>;
                friend call_helper;

                template <class T, class U = std::decay_t<T>>
                explicit storage(in_place_t, T&& t)
                {
                    if constexpr (in_situ_v<U>)
                        ::new(static_cast<void*>(&data_.buff_)) U(static_cast<T&&>(t));
                    else
                        data_.pobj_ = new U(static_cast<T&&>(t));
                    data_.vptr_ = vptr<I, U>;
                }

                template <std::size_t K, typename...As>
                decltype(auto) call(As&&...as)
                {
                    return std::get<K+5>(*data_.vptr_)(data_, static_cast<As&&>(as)...);
                }
                template <std::size_t K, typename...As>
                decltype(auto) call(As&&...as) const
                {
                    return std::get<K+5>(*data_.vptr_)(data_, static_cast<As&&>(as)...);
                }

                data<I> data_;
            };

            template <class I>
            using poly_base = interface_of<I, storage<I>>;

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

        template <std::size_t N, typename This, typename...As>
        decltype(auto) call(This&& _this, As&&...args)
        {
            return detail::call_helper::call<N>(
                static_cast<This&&>(_this), static_cast<As&&>(args)...);
        }

        /**
         Type-erasing wrapper. Use as follows to, e.g., define a std::function-like type:

         \code
            template <class Sig>
            struct Function;

            // Declare an abstract interface
            template <class R, class...As>
            struct Function<R(As...)>
            {
                // The interface is defined here, as a nested class template.
                template <class Base>
                struct interface : ranges::extends<Base>
                {
                    using ranges::extends<Base>::extends;
                    // The public members to expose go here:
                    R operator()(As...as)
                    {
                        // Use ranges::call to dispatch to the correct handler. ranges::call<N>
                        // will dispatch to the N-th element captured in the `members` alias below.
                        return static_cast<R>(
                            ranges::call<0>(*this, static_cast<As&&>(as)...));
                    }
                };
                // For a type T, capture the relevant members to which ranges::call will dispatch.
                // ranges::sig is only needed to disambiguate overloads.
                template <class T>
                using members =
                    ranges::members<ranges::sig<R(As...)>(&T::operator())>;
            };

            template <class Sig>
            using function = ranges::poly<Function<Sig>>;
         \endcode
         */

        template <class I>
        struct poly
          : detail::poly_base<I>
        {
        private:
            template <class T>
            using if_not_self = meta::if_<meta::not_<std::is_same<T, poly>>, int>;

        public:
            poly() = default;
            poly(poly&&) = default;
            poly(poly const&) = default;

            template <
                class T,
                class U = std::decay_t<T>,
                if_not_self<U> = 0,
                class = detail::members_of<I, U>>
            /* implicit */ poly(T &&t)
              : detail::poly_base<I>{detail::in_place, static_cast<T&&>(t)}
            {}

            poly& operator=(poly&& that) = default;
            poly& operator=(poly const& that) = default;

            template <
                class T,
                class U = std::decay_t<T>,
                if_not_self<U> = 0,
                class = detail::members_of<I, U>>
            poly& operator=(T&& t)
            {
                *this = poly(static_cast<T&&>(t));
                return *this;
            }

            std::type_info const& type() const noexcept
            {
                return this->poly::storage::type();
            }

            bool empty() const noexcept
            {
                return this->poly::storage::empty();
            }

            explicit operator bool() const noexcept
            {
                return !empty();
            }

            bool operator!() const noexcept
            {
                return empty();
            }

            void swap(poly& that) noexcept
            {
                this->poly::storage::swap(that);
            }
        };

        template <class I>
        void swap(poly<I>& left, poly<I>& right) noexcept
        {
            left.swap(right);
        }

        template <class Base>
        struct extends
          : private Base
        {
            extends() = default;
            using Base::Base;
            explicit extends(Base&& b)
            : Base(std::move(b))
            {}
            explicit extends(Base const & b)
            : Base(b)
            {}
        private:
            friend detail::call_helper;
            template <class> friend struct poly;
        };

        namespace
        {
            template <class Sig>
            inline constexpr detail::sig_t<Sig> const sig = {};
        }
    }
}

#endif
