//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef RANGES_TEST_ITERATORS_HPP
#define RANGES_TEST_ITERATORS_HPP

#include <iterator>

template<class It, bool Sized = false>
class sentinel;

template<class It>
class output_iterator;

template<class It, bool Sized = false>
class input_iterator;

template<class It, bool Sized = false>
class forward_iterator;

template<class It, bool Sized = false>
class bidirectional_iterator;

template<class It>
class random_access_iterator;


template<class Iter, bool Sized>
RANGES_CXX14_CONSTEXPR Iter base(sentinel<Iter, Sized> i) { return i.base(); }

template<class Iter>
RANGES_CXX14_CONSTEXPR Iter base(output_iterator<Iter> i) { return i.base(); }

template<class Iter, bool Sized>
RANGES_CXX14_CONSTEXPR Iter base(input_iterator<Iter, Sized> i) { return i.base(); }

template<class Iter, bool Sized>
RANGES_CXX14_CONSTEXPR Iter base(forward_iterator<Iter, Sized> i) { return i.base(); }

template<class Iter, bool Sized>
RANGES_CXX14_CONSTEXPR Iter base(bidirectional_iterator<Iter, Sized> i) { return i.base(); }

template<class Iter>
RANGES_CXX14_CONSTEXPR Iter base(random_access_iterator<Iter> i) { return i.base(); }

template<class Iter>    // everything else
RANGES_CXX14_CONSTEXPR Iter base(Iter i) { return i; }


template<class It, bool Sized>
class sentinel
{
    It it_;
public:
    RANGES_CXX14_CONSTEXPR sentinel() : it_() {}
    RANGES_CXX14_CONSTEXPR explicit sentinel(It it) : it_(it) {}
    RANGES_CXX14_CONSTEXPR It base() const { return it_; }
    RANGES_CXX14_CONSTEXPR friend bool operator==(const sentinel& x, const sentinel& y)
    {
        RANGES_ENSURE(x.it_ == y.it_);
        return true;
    }
    RANGES_CXX14_CONSTEXPR friend bool operator!=(const sentinel& x, const sentinel& y)
    {
        RANGES_ENSURE(x.it_ == y.it_);
        return false;
    }
    template<typename I>
    RANGES_CXX14_CONSTEXPR friend bool operator==(const I& x, const sentinel& y)
    {
        using ::base;
        return base(x) == y.it_;
    }
    template<typename I>
    RANGES_CXX14_CONSTEXPR friend bool operator!=(const I& x, const sentinel& y)
    {
        return !(x == y);
    }
    template<typename I>
    RANGES_CXX14_CONSTEXPR friend bool operator==(const sentinel& x, const I& y)
    {
        using ::base;
        return x.it_ == base(y);
    }
    template<typename I>
    RANGES_CXX14_CONSTEXPR friend bool operator!=(const sentinel& x, const I& y)
    {
        return !(x == y);
    }
};

// For making sized iterator ranges:
template<template<typename> class I, typename It>
RANGES_CXX14_CONSTEXPR
auto operator-(sentinel<It, true> end, I<It> begin)
RANGES_DECLTYPE_AUTO_RETURN
(
    base(end) - base(begin)
)
template<template<typename> class I, typename It>
RANGES_CXX14_CONSTEXPR
auto operator-(I<It> begin, sentinel<It, true> end)
RANGES_DECLTYPE_AUTO_RETURN
(
    base(begin) - base(end)
)

template<class It>
class output_iterator
{
    It it_;

    template<class U> friend class output_iterator;
public:
    typedef          std::output_iterator_tag                  iterator_category;
    typedef void                                               value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    RANGES_CXX14_CONSTEXPR It base() const {return it_;}

    RANGES_CXX14_CONSTEXPR output_iterator () {}
    RANGES_CXX14_CONSTEXPR explicit output_iterator(It it) : it_(it) {}
    template<class U, class = typename std::enable_if<std::is_convertible<U, It>{}>::type>
    RANGES_CXX14_CONSTEXPR
    output_iterator(const output_iterator<U>& u) :it_(u.it_) {}

    RANGES_CXX14_CONSTEXPR reference operator*() const {return *it_;}

    RANGES_CXX14_CONSTEXPR output_iterator& operator++() {++it_; return *this;}
    RANGES_CXX14_CONSTEXPR output_iterator operator++(int)
    {output_iterator tmp(*this); ++(*this); return tmp;}
};

template<class It, bool Sized>
class input_iterator
{
    It it_;

    template<class, bool> friend class input_iterator;
public:
    typedef          std::input_iterator_tag                   iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    RANGES_CXX14_CONSTEXPR It base() const {return it_;}

    RANGES_CXX14_CONSTEXPR input_iterator() : it_() {}
    RANGES_CXX14_CONSTEXPR explicit input_iterator(It it) : it_(it) {}
    template<class U, bool USized, class = typename std::enable_if<std::is_convertible<U, It>{}>::type>
    RANGES_CXX14_CONSTEXPR input_iterator(const input_iterator<U, USized>& u) :it_(u.it_) {}

    RANGES_CXX14_CONSTEXPR reference operator*() const {return *it_;}
    RANGES_CXX14_CONSTEXPR pointer operator->() const {return it_;}

    RANGES_CXX14_CONSTEXPR input_iterator& operator++() {++it_; return *this;}
    RANGES_CXX14_CONSTEXPR input_iterator operator++(int)
        {input_iterator tmp(*this); ++(*this); return tmp;}

    RANGES_CXX14_CONSTEXPR
    friend bool operator==(const input_iterator& x, const input_iterator& y)
        {return x.it_ == y.it_;}
    RANGES_CXX14_CONSTEXPR
    friend bool operator!=(const input_iterator& x, const input_iterator& y)
        {return !(x == y);}

    template<bool B = Sized, meta::if_c<B, int> = 42>
    RANGES_CXX14_CONSTEXPR
    friend difference_type operator-(const input_iterator& x, const input_iterator& y)
        {return x.it_ - y.it_;}
};

template<class T, bool TSized, class U, bool USized>
RANGES_CXX14_CONSTEXPR
bool
operator==(const input_iterator<T, TSized>& x, const input_iterator<U, USized>& y)
{
    return x.base() == y.base();
}

template<class T, bool TSized, class U, bool USized>
RANGES_CXX14_CONSTEXPR
bool
operator!=(const input_iterator<T, TSized>& x, const input_iterator<U, USized>& y)
{
    return !(x == y);
}

template<class It, bool Sized>
class forward_iterator
{
    It it_;

    template<class, bool> friend class forward_iterator;
public:
    typedef          std::forward_iterator_tag                 iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    RANGES_CXX14_CONSTEXPR It base() const {return it_;}

    RANGES_CXX14_CONSTEXPR forward_iterator() : it_() {}
    RANGES_CXX14_CONSTEXPR explicit forward_iterator(It it) : it_(it) {}
    template<class U, bool USized, class = typename std::enable_if<std::is_convertible<U, It>{}>::type>
    RANGES_CXX14_CONSTEXPR forward_iterator(const forward_iterator<U, USized>& u) :it_(u.it_) {}

    RANGES_CXX14_CONSTEXPR reference operator*() const {return *it_;}
    RANGES_CXX14_CONSTEXPR pointer operator->() const {return it_;}

    RANGES_CXX14_CONSTEXPR forward_iterator& operator++() {++it_; return *this;}
    RANGES_CXX14_CONSTEXPR forward_iterator operator++(int)
    {forward_iterator tmp(*this); ++(*this); return tmp;}

    RANGES_CXX14_CONSTEXPR
    friend bool operator==(const forward_iterator& x, const forward_iterator& y)
    {return x.it_ == y.it_;}
    RANGES_CXX14_CONSTEXPR
    friend bool operator!=(const forward_iterator& x, const forward_iterator& y)
    {return !(x == y);}
};

template<class T, bool TSized, class U, bool USized>
RANGES_CXX14_CONSTEXPR
bool
operator==(const forward_iterator<T, TSized>& x, const forward_iterator<U, USized>& y)
{
    return x.base() == y.base();
}

template<class T, bool TSized, class U, bool USized>
RANGES_CXX14_CONSTEXPR
bool
operator!=(const forward_iterator<T, TSized>& x, const forward_iterator<U, USized>& y)
{
    return !(x == y);
}

template<class It, bool Sized>
class bidirectional_iterator
{
    It it_;

    template<class, bool> friend class bidirectional_iterator;
public:
    typedef          std::bidirectional_iterator_tag           iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    RANGES_CXX14_CONSTEXPR It base() const {return it_;}

    RANGES_CXX14_CONSTEXPR bidirectional_iterator() : it_() {}
    RANGES_CXX14_CONSTEXPR explicit bidirectional_iterator(It it) : it_(it) {}
    template<class U, bool USized, class = typename std::enable_if<std::is_convertible<U, It>{}>::type>
    RANGES_CXX14_CONSTEXPR bidirectional_iterator(const bidirectional_iterator<U, USized>& u) :it_(u.it_) {}

    RANGES_CXX14_CONSTEXPR reference operator*() const {return *it_;}
    RANGES_CXX14_CONSTEXPR pointer operator->() const {return it_;}

    RANGES_CXX14_CONSTEXPR bidirectional_iterator& operator++() {++it_; return *this;}
    RANGES_CXX14_CONSTEXPR bidirectional_iterator operator++(int)
    {bidirectional_iterator tmp(*this); ++(*this); return tmp;}

    RANGES_CXX14_CONSTEXPR bidirectional_iterator& operator--() {--it_; return *this;}
    RANGES_CXX14_CONSTEXPR bidirectional_iterator operator--(int)
    {bidirectional_iterator tmp(*this); --(*this); return tmp;}
};

template<class T, bool TSized, class U, bool USized>
RANGES_CXX14_CONSTEXPR
bool
operator==(const bidirectional_iterator<T, TSized>& x, const bidirectional_iterator<U, USized>& y)
{
    return x.base() == y.base();
}

template<class T, bool TSized, class U, bool USized>
RANGES_CXX14_CONSTEXPR
bool
operator!=(const bidirectional_iterator<T, TSized>& x, const bidirectional_iterator<U, USized>& y)
{
    return !(x == y);
}

template<class It>
class random_access_iterator
{
    It it_;

    template<class U> friend class random_access_iterator;
public:
    typedef          std::random_access_iterator_tag           iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    RANGES_CXX14_CONSTEXPR It base() const {return it_;}

    RANGES_CXX14_CONSTEXPR random_access_iterator() : it_() {}
    RANGES_CXX14_CONSTEXPR explicit random_access_iterator(It it) : it_(it) {}
    template<class U, class = typename std::enable_if<std::is_convertible<U, It>{}>::type>
    RANGES_CXX14_CONSTEXPR random_access_iterator(const random_access_iterator<U>& u) :it_(u.it_) {}

    RANGES_CXX14_CONSTEXPR reference operator*() const {return *it_;}
    RANGES_CXX14_CONSTEXPR pointer operator->() const {return it_;}

    RANGES_CXX14_CONSTEXPR random_access_iterator& operator++() {++it_; return *this;}
    RANGES_CXX14_CONSTEXPR random_access_iterator operator++(int)
    {random_access_iterator tmp(*this); ++(*this); return tmp;}

    RANGES_CXX14_CONSTEXPR random_access_iterator& operator--() {--it_; return *this;}
    RANGES_CXX14_CONSTEXPR random_access_iterator operator--(int)
    {random_access_iterator tmp(*this); --(*this); return tmp;}

    RANGES_CXX14_CONSTEXPR
    random_access_iterator& operator+=(difference_type n) {it_ += n; return *this;}
    RANGES_CXX14_CONSTEXPR
    random_access_iterator operator+(difference_type n) const
    {random_access_iterator tmp(*this); tmp += n; return tmp;}
    RANGES_CXX14_CONSTEXPR
    friend random_access_iterator operator+(difference_type n, random_access_iterator x)
    {x += n; return x;}
    RANGES_CXX14_CONSTEXPR
    random_access_iterator& operator-=(difference_type n) {return *this += -n;}
    RANGES_CXX14_CONSTEXPR
    random_access_iterator operator-(difference_type n) const
    {random_access_iterator tmp(*this); tmp -= n; return tmp;}

    RANGES_CXX14_CONSTEXPR
    reference operator[](difference_type n) const {return it_[n];}
};

template<class T, class U>
RANGES_CXX14_CONSTEXPR
bool
operator==(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return x.base() == y.base();
}

template<class T, class U>
RANGES_CXX14_CONSTEXPR
bool
operator!=(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return !(x == y);
}

template<class T, class U>
RANGES_CXX14_CONSTEXPR
bool
operator<(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return x.base() < y.base();
}

template<class T, class U>
RANGES_CXX14_CONSTEXPR
bool
operator<=(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return !(y < x);
}

template<class T, class U>
RANGES_CXX14_CONSTEXPR
bool
operator>(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return y < x;
}

template<class T, class U>
RANGES_CXX14_CONSTEXPR
bool
operator>=(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return !(x < y);
}

template<class T, class U>
RANGES_CXX14_CONSTEXPR
auto
operator-(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
RANGES_DECLTYPE_AUTO_RETURN
(
    x.base() - y.base()
)

template<typename It, bool Sized = false>
struct sentinel_type
{
    using type = It;
};

template<typename T, bool Sized>
struct sentinel_type<T*, Sized>
{
    using type = sentinel<T*, Sized>;
};

template<template<typename> class I, typename It, bool Sized>
struct sentinel_type<I<It>, Sized>
{
    using type = sentinel<It, Sized>;
};

#endif  // RANGES_TEST_ITERATORS_HPP
