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

#include <cassert>
#include <iterator>

template <class It, bool Sized = false>
class sentinel;

template <class It>
class output_iterator;

template <class It>
class input_iterator;

template <class It>
class forward_iterator;

template <class It>
class bidirectional_iterator;

template <class It>
class random_access_iterator;


template <class Iter, bool Sized>
inline Iter base(sentinel<Iter, Sized> i) { return i.base(); }

template <class Iter>
inline Iter base(output_iterator<Iter> i) { return i.base(); }

template <class Iter>
inline Iter base(input_iterator<Iter> i) { return i.base(); }

template <class Iter>
inline Iter base(forward_iterator<Iter> i) { return i.base(); }

template <class Iter>
inline Iter base(bidirectional_iterator<Iter> i) { return i.base(); }

template <class Iter>
inline Iter base(random_access_iterator<Iter> i) { return i.base(); }

template <class Iter>    // everything else
inline Iter base(Iter i) { return i; }


template <class It, bool Sized>
class sentinel
{
    It it_;
public:
    sentinel() : it_() {}
    explicit sentinel(It it) : it_(it) {}
    It base() const { return it_; }
    friend bool operator==(const sentinel& x, const sentinel& y)
    {
        assert(x.it_ == y.it_);
        return true;
    }
    friend bool operator!=(const sentinel& x, const sentinel& y)
    {
        assert(x.it_ == y.it_);
        return false;
    }
    template<typename I>
    friend bool operator==(const I& x, const sentinel& y)
    {
        using ::base;
        return base(x) == y.it_;
    }
    template<typename I>
    friend bool operator!=(const I& x, const sentinel& y)
    {
        return !(x == y);
    }
    template<typename I>
    friend bool operator==(const sentinel& x, const I& y)
    {
        using ::base;
        return x.it_ == base(y);
    }
    template<typename I>
    friend bool operator!=(const sentinel& x, const I& y)
    {
        return !(x == y);
    }
};

// For making sized iterator ranges:
template<template<typename> class I, typename It>
std::ptrdiff_t operator-(sentinel<It, true> end, I<It> begin)
{
    return base(end) - base(begin);
}

template <class It>
class output_iterator
{
    It it_;

    template <class U> friend class output_iterator;
public:
    typedef          std::output_iterator_tag                  iterator_category;
    typedef void                                               value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    It base() const {return it_;}

    output_iterator () {}
    explicit output_iterator(It it) : it_(it) {}
    template <class U>
        output_iterator(const output_iterator<U>& u) :it_(u.it_) {}

    reference operator*() const {return *it_;}

    output_iterator& operator++() {++it_; return *this;}
    output_iterator operator++(int)
        {output_iterator tmp(*this); ++(*this); return tmp;}
};

template <class It>
class input_iterator
{
    It it_;

    template <class U> friend class input_iterator;
public:
    typedef          std::input_iterator_tag                   iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    It base() const {return it_;}

    input_iterator() : it_() {}
    explicit input_iterator(It it) : it_(it) {}
    template <class U>
        input_iterator(const input_iterator<U>& u) :it_(u.it_) {}

    reference operator*() const {return *it_;}
    pointer operator->() const {return it_;}

    input_iterator& operator++() {++it_; return *this;}
    input_iterator operator++(int)
        {input_iterator tmp(*this); ++(*this); return tmp;}

    friend bool operator==(const input_iterator& x, const input_iterator& y)
        {return x.it_ == y.it_;}
    friend bool operator!=(const input_iterator& x, const input_iterator& y)
        {return !(x == y);}
};

template <class T, class U>
inline
bool
operator==(const input_iterator<T>& x, const input_iterator<U>& y)
{
    return x.base() == y.base();
}

template <class T, class U>
inline
bool
operator!=(const input_iterator<T>& x, const input_iterator<U>& y)
{
    return !(x == y);
}

template <class It>
class forward_iterator
{
    It it_;

    template <class U> friend class forward_iterator;
public:
    typedef          std::forward_iterator_tag                 iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    It base() const {return it_;}

    forward_iterator() : it_() {}
    explicit forward_iterator(It it) : it_(it) {}
    template <class U>
        forward_iterator(const forward_iterator<U>& u) :it_(u.it_) {}

    reference operator*() const {return *it_;}
    pointer operator->() const {return it_;}

    forward_iterator& operator++() {++it_; return *this;}
    forward_iterator operator++(int)
        {forward_iterator tmp(*this); ++(*this); return tmp;}

    friend bool operator==(const forward_iterator& x, const forward_iterator& y)
        {return x.it_ == y.it_;}
    friend bool operator!=(const forward_iterator& x, const forward_iterator& y)
        {return !(x == y);}
};

template <class T, class U>
inline
bool
operator==(const forward_iterator<T>& x, const forward_iterator<U>& y)
{
    return x.base() == y.base();
}

template <class T, class U>
inline
bool
operator!=(const forward_iterator<T>& x, const forward_iterator<U>& y)
{
    return !(x == y);
}

template <class It>
class bidirectional_iterator
{
    It it_;

    template <class U> friend class bidirectional_iterator;
public:
    typedef          std::bidirectional_iterator_tag           iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    It base() const {return it_;}

    bidirectional_iterator() : it_() {}
    explicit bidirectional_iterator(It it) : it_(it) {}
    template <class U>
        bidirectional_iterator(const bidirectional_iterator<U>& u) :it_(u.it_) {}

    reference operator*() const {return *it_;}
    pointer operator->() const {return it_;}

    bidirectional_iterator& operator++() {++it_; return *this;}
    bidirectional_iterator operator++(int)
        {bidirectional_iterator tmp(*this); ++(*this); return tmp;}

    bidirectional_iterator& operator--() {--it_; return *this;}
    bidirectional_iterator operator--(int)
        {bidirectional_iterator tmp(*this); --(*this); return tmp;}
};

template <class T, class U>
inline
bool
operator==(const bidirectional_iterator<T>& x, const bidirectional_iterator<U>& y)
{
    return x.base() == y.base();
}

template <class T, class U>
inline
bool
operator!=(const bidirectional_iterator<T>& x, const bidirectional_iterator<U>& y)
{
    return !(x == y);
}

template <class It>
class random_access_iterator
{
    It it_;

    template <class U> friend class random_access_iterator;
public:
    typedef          std::random_access_iterator_tag           iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    It base() const {return it_;}

    random_access_iterator() : it_() {}
    explicit random_access_iterator(It it) : it_(it) {}
   template <class U>
        random_access_iterator(const random_access_iterator<U>& u) :it_(u.it_) {}

    reference operator*() const {return *it_;}
    pointer operator->() const {return it_;}

    random_access_iterator& operator++() {++it_; return *this;}
    random_access_iterator operator++(int)
        {random_access_iterator tmp(*this); ++(*this); return tmp;}

    random_access_iterator& operator--() {--it_; return *this;}
    random_access_iterator operator--(int)
        {random_access_iterator tmp(*this); --(*this); return tmp;}

    random_access_iterator& operator+=(difference_type n) {it_ += n; return *this;}
    random_access_iterator operator+(difference_type n) const
        {random_access_iterator tmp(*this); tmp += n; return tmp;}
    friend random_access_iterator operator+(difference_type n, random_access_iterator x)
        {x += n; return x;}
    random_access_iterator& operator-=(difference_type n) {return *this += -n;}
    random_access_iterator operator-(difference_type n) const
        {random_access_iterator tmp(*this); tmp -= n; return tmp;}

    reference operator[](difference_type n) const {return it_[n];}
};

template <class T, class U>
inline
bool
operator==(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return x.base() == y.base();
}

template <class T, class U>
inline
bool
operator!=(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return !(x == y);
}

template <class T, class U>
inline
bool
operator<(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return x.base() < y.base();
}

template <class T, class U>
inline
bool
operator<=(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return !(y < x);
}

template <class T, class U>
inline
bool
operator>(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return y < x;
}

template <class T, class U>
inline
bool
operator>=(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return !(x < y);
}

template <class T, class U>
inline
typename std::iterator_traits<T>::difference_type
operator-(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return x.base() - y.base();
}

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

namespace ranges
{
    template<typename I0, bool S, typename I1>
    struct common_type<sentinel<I0, S>, I1>
    {
        using type = common_iterator<I1, sentinel<I0, S>>;
    };
    template<typename I0, typename I1, bool S>
    struct common_type<I0, sentinel<I1, S>>
    {
        using type = common_iterator<I0, sentinel<I1, S>>;
    };
    template<typename I, bool B>
    struct common_type<sentinel<I, B>, sentinel<I, B>>
    {
        using type = sentinel<I>;
    };
    template<typename I0, bool S, typename I1, typename TQual, typename UQual>
    struct common_reference_base<sentinel<I0, S>, I1, TQual, UQual>
    {
        using type = common_iterator<I1, sentinel<I0, S>>;
    };
    template<typename I0, typename I1, bool S, typename TQual, typename UQual>
    struct common_reference_base<I0, sentinel<I1, S>, TQual, UQual>
    {
        using type = common_iterator<I0, sentinel<I1, S>>;
    };
    template<typename I, bool B, typename TQual, typename UQual>
    struct common_reference_base<sentinel<I, B>, sentinel<I, B>, TQual, UQual>
    {
        using type = sentinel<I>;
    };
}

#endif  // ITERATORS_H
