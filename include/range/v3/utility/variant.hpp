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
#include <typeinfo>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/typelist.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct void_t
        {};

        namespace detail
        {
            template<typename T>
            using decay_t = typename std::decay<T>::type;

            template<typename T, typename U>
            struct common_type_or_void
            {
                using type = decltype(true? std::declval<T>() : std::declval<U>());
            };
 
            template<typename U>
            struct common_type_or_void<void, U>
            {
                using type = void;
            };
 
            template<typename T>
            struct common_type_or_void<T, void>
            {
                using type = void;
            };
 
            template<>
            struct common_type_or_void<void, void>
            {
                using type = void;
            };

            template<typename T, typename U>
            using common_type_or_void_t = typename common_type_or_void<T, U>::type;

            template<typename List>
            union variant_data
            {
                template<typename Fun>
                void apply(std::type_info const*, Fun &&) const
                {}
            };

            template<typename T, typename ...Ts>
            union variant_data<typelist<T, Ts...>>
            {
            private:
                template<typename U>
                using enable_if_ok =
                    typename std::enable_if<
                        typelist_in<U, typelist<Ts...>>::value, int>::type;
                using head_t = T;
                using tail_t = variant_data<typelist<Ts...>>;

                head_t head;
                tail_t tail;

                template<typename This, typename Fun>
                static auto
                apply_(This & this_, std::type_info const* ti, Fun && fun) ->
                    typename std::enable_if<(ranges::Callable<Fun, decltype((this_.head))>())>::type
                {
                    if(*ti == typeid(T))
                        detail::forward<Fun>(fun)(this_.head);
                    else
                        this_.tail.apply(ti, detail::forward<Fun>(fun));
                }
                template<typename This, typename Fun>
                static auto
                apply_(This & this_, std::type_info const* ti, Fun && fun) ->
                    typename std::enable_if<!(ranges::Callable<Fun, decltype((this_.head))>())>::type
                {
                    assert(*ti != typeid(T));
                    this_.tail.apply(ti, detail::forward<Fun>(fun));
                }
            public:
                variant_data()
                {}
                constexpr variant_data(T const &t)
                  : head(t)
                {}
                constexpr variant_data(T &&t)
                  : head(std::move(t))
                {}
                template<typename U, enable_if_ok<U> = 0>
                constexpr variant_data(U const &u)
                  : tail{u}
                {}
                template<typename U, enable_if_ok<U> = 0>
                constexpr variant_data(U &&u)
                  : tail{detail::forward<U>(u)}
                {}
                ~variant_data()
                {}
                template<typename Fun>
                auto apply(std::type_info const* ti, Fun && fun) ->
                    decltype(variant_data::apply_(*this, ti, std::declval<Fun>()))
                {
                    return variant_data::apply_(*this, ti, std::forward<Fun>(fun));
                }
                template<typename Fun>
                auto apply(std::type_info const* ti, Fun && fun) const ->
                    decltype(variant_data::apply_(*this, ti, std::declval<Fun>()))
                {
                    return variant_data::apply_(*this, ti, std::forward<Fun>(fun));
                }
            };

            struct deleter
            {
                template<typename T>
                void operator()(T const & t) const
                {
                    t.T::~T();
                }
            };

            template<typename T>
            struct setter
            {
                T && t_;
                setter(T && t)
                  : t_(std::forward<T>(t))
                {}
                void operator()(T & t) const
                {
                    ::new(static_cast<void*>(&t)) T(std::forward<T>(t_));
                }
                template<typename U>
                typename std::enable_if<
                    !std::is_same<decay_t<U>, decay_t<T>>::value
                >::type
                operator()(U &) const
                {}
            };

            template<typename Fun, template<typename> class Tfx = identity>
            struct make_result_t
            {
                template<typename Arg>
                using apply =
                    identity<concepts::Callable::result_t<Fun, typename Tfx<Arg>::type>>;
            };

            template<typename Fun, typename List, template<typename> class Tfx = identity>
            using result_list_t =
                typelist_transform_t<List, make_result_t<Fun, Tfx>::template apply>;

            template<typename T>
            using add_const_lvalue_reference =
                std::add_lvalue_reference<T const>;

            template<typename Fun, typename Res>
            struct apply_visitor
            {
                apply_visitor(Fun && fun, Res & res)
                  : fun_(std::forward<Fun>(fun)), res_(res)
                {}
                template<typename T>
                void operator()(T && t) const
                {
                    this->apply_(std::forward<T>(t), std::is_void<result_of_t<Fun(T &&)>>{});
                }
            private:
                template<typename T>
                void apply_(T && t, std::true_type) const
                {
                    fun_(std::forward<T>(t));
                }
                template<typename T>
                void apply_(T && t, std::false_type) const
                {
                    res_ = fun_(std::forward<T>(t));
                }
                Res & res_;
                Fun fun_;
            };

            template<typename Data, typename Fun, typename Res>
            static Res apply_visitor_(std::type_info const *type, Data && data, Fun && fun,
                Res & res)
            {
                apply_visitor<Fun, Res> vis{std::forward<Fun>(fun), res};
                std::forward<Data>(data).apply(type, vis);
                return std::move(res);
            }
            template<typename Data, typename Fun>
            static void apply_visitor_(std::type_info const *type, Data && data, Fun && fun,
                ranges::void_t &)
            {
                std::forward<Data>(data).apply(type, std::forward<Fun>(fun));
            }
        }

        template<typename ...Ts>
        struct variant
        {
        private:
            using unique_t =
                typelist_replace_t<
                    void
                  , void_t
                  , typelist_unique_t<typelist<detail::decay_t<Ts>...>>
                >;
            using head_t = typelist_front_t<unique_t>;
            using data_t = detail::variant_data<unique_t>;
            const std::type_info* type_;
            data_t data_;

            template<typename Fun, template<typename> class Tfx = detail::identity>
            using result_t =
                typelist_expand_t<
                    ranges::variant,
                    typelist_replace_t<
                        void
                      , void_t
                      , typelist_unique_t<detail::result_list_t<Fun, unique_t, Tfx>>>>;
            template<typename Fun, typename Or = void,
                template<typename> class Tfx = detail::identity>
            using result_or_t =
                detail::conditional_t<
                    std::is_same<result_t<Fun, Tfx>, variant<void_t>>::value
                  , Or
                  , result_t<Fun, Tfx>
                >;
            void clear_()
            {
                if(is_valid())
                {
                    // Delete the old value
                    this->apply(detail::deleter{});
                    // Set the type to void
                    type_ = &typeid(void);
                }
            }
        public:
            constexpr variant()
              : type_(&typeid(void))
              , data_{}
            {}
            template<typename U,
                typename = decltype(data_t{std::declval<U>()})>
            constexpr explicit variant(U && u)
              : type_(&typeid(detail::decay_t<U>))
              , data_{detail::forward<U>(u)}
            {}
            ~variant()
            {
                clear_();
            }
            template<typename U,
                typename = decltype(data_t{std::declval<U>()})>
            variant& operator=(U && u)
            {
                std::type_info const *const new_type = &typeid(detail::decay_t<U>);
                // Delete the old value
                clear_();
                // Set the new value (this can throw)
                void_t res;
                detail::apply_visitor_(new_type, data_, detail::setter<U>{std::forward<U>(u)}, res);
                // Success, set the new type
                type_ = new_type;
                return *this;
            }
            bool is_valid() const
            {
                return type() != typeid(void);
            }
            std::type_info const & type() const
            {
                return *type_;
            }
            template<typename Fun>
            result_or_t<Fun, void, std::add_lvalue_reference>
            apply(Fun && fun)
            {
                result_or_t<Fun, void_t, std::add_lvalue_reference> res;
                return detail::apply_visitor_(type_, data_, std::forward<Fun>(fun), res);
            }
            //template<typename Fun>
            //result_or_t<Fun, void, detail::add_const_lvalue_reference>
            //apply(Fun && fun) const
            //{
            //    result_or_t<Fun, void_t, detail::add_const_lvalue_reference> res;
            //    return this->apply_(data_, std::forward<Fun>(fun), res);
            //}
        };
    }
}

#endif
