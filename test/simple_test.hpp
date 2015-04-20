// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_SIMPLE_TEST_HPP
#define RANGES_SIMPLE_TEST_HPP

#include <cstdlib>
#include <utility>
#include <iostream>
#include <typeinfo>

namespace test_impl
{
    inline int &test_failures()
    {
        static int test_failures = 0;
        return test_failures;
    }

    template<typename T>
    struct streamable_base
    {};

    template<typename T>
    std::ostream &operator<<(std::ostream &sout, streamable_base<T> const &)
    {
        return sout << "<non-streamable type>";
    }

    template<typename T>
    struct streamable : streamable_base<T>
    {
    private:
        T const &t_;
    public:
        explicit streamable(T const &t) : t_(t) {}
        template<typename U = T>
        friend auto operator<<(std::ostream &sout, streamable const &s) ->
            decltype(sout << std::declval<U const &>())
        {
            return sout << s.t_;
        }
    };

    template<typename T>
    streamable<T> stream(T const &t)
    {
        return streamable<T>{t};
    }

    template<typename T>
    struct R
    {
    private:
        char const *filename_;
        int lineno_;
        char const *expr_;
        T t_;
        bool dismissed_ = false;

        template<typename U>
        void oops(U const &u) const
        {
            std::cerr
                << "> ERROR: CHECK failed \"" << expr_ << "\"\n"
                << "> \t" << filename_ << '(' << lineno_ << ')' << '\n';
            if(dismissed_)
                std::cerr
                    << "> \tEXPECTED: " << stream(u) << "\n> \tACTUAL: " << stream(t_) << '\n';
            ++test_failures();
        }
        void dismiss()
        {
            dismissed_ = true;
        }
        template<typename V = T>
        auto eval_(int) -> decltype(!std::declval<V>())
        {
            return !t_;
        }
        bool eval_(long)
        {
            return true;
        }
    public:
        R(char const *filename, int lineno, char const *expr, T && t)
          : filename_(filename), lineno_(lineno), expr_(expr)
          , t_(std::forward<T>(t))
        {}
        ~R()
        {
            if(!dismissed_ && eval_(42))
                this->oops(42);
        }
        template<typename U>
        void operator==(U const &u)
        {
            dismiss();
            if(!(t_ == u)) this->oops(u);
        }
        template<typename U>
        void operator!=(U const &u)
        {
            dismiss();
            if(!(t_ != u)) this->oops(u);
        }
        template<typename U>
        void operator<(U const &u)
        {
            dismiss();
            if(!(t_ < u)) this->oops(u);
        }
        template<typename U>
        void operator<=(U const &u)
        {
            dismiss();
            if(!(t_ <= u)) this->oops(u);
        }
        template<typename U>
        void operator>(U const &u)
        {
            dismiss();
            if(!(t_ > u)) this->oops(u);
        }
        template<typename U>
        void operator>=(U const &u)
        {
            dismiss();
            if(!(t_ >= u)) this->oops(u);
        }
    };

    struct S
    {
    private:
        char const *filename_;
        int lineno_;
        char const *expr_;
    public:
        S(char const *filename, int lineno, char const *expr)
          : filename_(filename), lineno_(lineno), expr_(expr)
        {}
        template<typename T>
        R<T> operator->*(T && t)
        {
            return {filename_, lineno_, expr_, std::forward<T>(t)};
        }
    };
}

inline int test_result()
{
    return ::test_impl::test_failures() ? EXIT_FAILURE : EXIT_SUCCESS;
}

#define CHECK(...)                                                                                  \
    (void)(::test_impl::S{__FILE__, __LINE__, #__VA_ARGS__} ->* __VA_ARGS__)                        \
    /**/

#endif
