// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_DETAIL_FUNCTION_WRAPPER_HPP
#define RANGES_V3_DETAIL_FUNCTION_WRAPPER_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Ret, typename PMFN>
            struct member_function_wrapper
            {
            private:
                PMFN pmfn_;
            public:
                member_function_wrapper(PMFN pmfn)
                  : pmfn_(pmfn)
                {}
                template<typename Obj, typename ...Args2>
                Ret operator()(Obj && obj, Args2 &&... args) const
                {
                    return (detail::forward<Obj>(obj).*pmfn_)(detail::forward<Args2>(args)...);
                }
                template<typename Obj, typename ...Args2>
                Ret operator()(Obj * obj, Args2 &&... args) const
                {
                    return (obj->*pmfn_)(detail::forward<Args2>(args)...);
                }
            };

            template<typename Fun>
            struct function_wrapper
              : Fun
            {
                function_wrapper(Fun fun)
                  : Fun(detail::move(fun))
                {}
            };

            template<typename Ret, typename... Args>
            struct function_wrapper<Ret(*)(Args...)>
            {
            private:
                Ret(*pfn_)(Args...);
            public:
                function_wrapper(Ret(*pfn)(Args...))
                  : pfn_(pfn)
                {}
                template<typename ...Args2>
                Ret operator()(Args2 &&... args) const
                {
                    return (*pfn_)(detail::forward<Args>(args)...);
                }
            };

            template<typename Ret, typename... Args>
            struct function_wrapper<Ret(&)(Args...)>
            {
            private:
                Ret(*pfn_)(Args...);
            public:
                function_wrapper(Ret(&pfn)(Args...))
                  : pfn_(&pfn)
                {}
                template<typename ...Args2>
                Ret operator()(Args2 &&... args) const
                {
                    return (*pfn_)(detail::forward<Args>(args)...);
                }
            };

            template<typename Type, typename Class>
            struct function_wrapper<Type Class::*>
            {
            private:
                Type Class::*pm_;
            public:
                function_wrapper(Type Class::* pm)
                  : pm_(pm)
                {}
                template<typename Obj>
                auto operator()(Obj && obj) const
                    -> decltype((detail::forward<Obj>(obj).*pm_))
                {
                    return detail::forward<Obj>(obj).*pm_;
                }
                template<typename Obj>
                auto operator()(Obj * obj) const
                    -> decltype((obj->*pm_))
                {
                    return obj->*pm_;
                }
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...)>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...)>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...)>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...) const>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...) const>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...) const>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...) volatile>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...) volatile>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...) volatile>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...) const volatile>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...) const volatile>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...) const volatile>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...)&>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...)&>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...)&>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...) const&>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...) const&>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...) const&>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...) volatile&>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...) volatile&>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...) volatile&>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...) const volatile&>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...) const volatile&>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...) const volatile&>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...)&&>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...)&&>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...)&&>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...) const&&>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...) const&&>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...) const&&>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...) volatile&&>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...) volatile&&>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...) volatile&&>::member_function_wrapper;
            };

            template<typename Ret, typename Class, typename ...Args>
            struct function_wrapper<Ret (Class::*)(Args...) const volatile&&>
              : member_function_wrapper<Ret, Ret (Class::*)(Args...) const volatile&&>
            {
                using member_function_wrapper<Ret, Ret (Class::*)(Args...) const volatile&&>::member_function_wrapper;
            };
        }
    }
}

#endif
