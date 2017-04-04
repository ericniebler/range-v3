/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_ANY_HPP
#define RANGES_V3_UTILITY_ANY_HPP

#include <memory>
#include <typeinfo>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct bad_any_cast
          : std::bad_cast
        {
            virtual const char* what() const noexcept override
            {
                return "bad any_cast";
            }
        };

        struct any;

        template<typename T>
        meta::if_c<std::is_reference<T>() || Copyable<T>(), T>
        any_cast(any &);

        template<typename T>
        meta::if_c<std::is_reference<T>() || Copyable<T>(), T>
        any_cast(any const &);

        template<typename T>
        meta::if_c<std::is_reference<T>() || Copyable<T>(), T>
        any_cast(any &&);

        template<typename T>
        T * any_cast(any *) noexcept;

        template<typename T>
        T const * any_cast(any const *) noexcept;

        struct any
        {
        private:
            template<typename T>
            friend meta::if_c<std::is_reference<T>() || Copyable<T>(), T>
            any_cast(any &);

            template<typename T>
            friend meta::if_c<std::is_reference<T>() || Copyable<T>(), T>
            any_cast(any const &);

            template<typename T>
            friend meta::if_c<std::is_reference<T>() || Copyable<T>(), T>
            any_cast(any &&);

            template<typename T>
            friend T * any_cast(any *) noexcept;

            template<typename T>
            friend T const * any_cast(any const *) noexcept;

            struct interface
            {
                virtual ~interface()
                {}
                virtual interface *clone() const = 0;
                virtual std::type_info const & type() const noexcept = 0;
            };

            template<class T>
            struct impl final : interface
            {
            private:
                T obj;
            public:
                impl() = default;
                impl(T o)
                  : obj(std::move(o))
                {}
                T &get() { return obj; }
                T const &get() const { return obj; }
                impl *clone() const override
                {
                    return new impl{obj};
                }
                std::type_info const & type() const noexcept override
                {
                    return typeid(T);
                }
            };

            std::unique_ptr<interface> ptr_;
        public:
            any() noexcept = default;
            template<typename TRef, typename T = detail::decay_t<TRef>,
                CONCEPT_REQUIRES_(Copyable<T>() && !Same<T, any>())>
            any(TRef &&t)
              : ptr_(new impl<T>(static_cast<TRef&&>(t)))
            {}
            any(any &&) noexcept = default;
            any(any const &that)
              : ptr_{that.ptr_ ? that.ptr_->clone() : nullptr}
            {}
            any &operator=(any &&) noexcept = default;
            any &operator=(any const &that)
            {
                ptr_.reset(that.ptr_ ? that.ptr_->clone() : nullptr);
                return *this;
            }
            template<typename TRef, typename T = detail::decay_t<TRef>,
                CONCEPT_REQUIRES_(Copyable<T>() && !Same<T, any>())>
            any &operator=(TRef &&t)
            {
                any{static_cast<TRef&&>(t)}.swap(*this);
                return *this;
            }
            void clear() noexcept
            {
                ptr_.reset();
            }
            bool empty() const noexcept
            {
                return !ptr_;
            }
            std::type_info const & type() const noexcept
            {
                return ptr_ ? ptr_->type() : typeid(void);
            }
            void swap(any &that) noexcept
            {
                ptr_.swap(that.ptr_);
            }
            friend void swap(any &x, any &y) noexcept
            {
                x.swap(y);
            }
        };

        /// \throw bad_any_cast
        template<typename T>
        meta::if_c<std::is_reference<T>() || Copyable<T>(), T>
        any_cast(any &x)
        {
            if(x.type() != typeid(detail::decay_t<T>))
                throw bad_any_cast{};
            return static_cast<any::impl<detail::decay_t<T>>*>(x.ptr_.get())->get();
        }

        /// \overload
        template<typename T>
        meta::if_c<std::is_reference<T>() || Copyable<T>(), T>
        any_cast(any const &x)
        {
            if(x.type() != typeid(detail::decay_t<T>))
                throw bad_any_cast{};
            return static_cast<any::impl<detail::decay_t<T>> const *>(x.ptr_.get())->get();
        }

        /// \overload
        template<typename T>
        meta::if_c<std::is_reference<T>() || Copyable<T>(), T>
        any_cast(any &&x)
        {
            if(x.type() != typeid(detail::decay_t<T>))
                throw bad_any_cast{};
            return static_cast<any::impl<detail::decay_t<T>>*>(x.ptr_.get())->get();
        }

        /// \overload
        template<typename T>
        T * any_cast(any *p) noexcept
        {
            if(p && p->ptr_)
                if(any::impl<T> *q = dynamic_cast<any::impl<T>*>(p->ptr_.get()))
                    return &q->get();
            return nullptr;
        }

        /// \overload
        template<typename T>
        T const * any_cast(any const *p) noexcept
        {
            if(p && p->ptr_)
                if(any::impl<T> const *q = dynamic_cast<any::impl<T> const *>(p->ptr_.get()))
                    return &q->get();
            return nullptr;
        }
    }
}

#endif
