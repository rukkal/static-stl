/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_ITERATOR__
#define _SSTL_ITERATOR__

#include <type_traits>
#include <iterator>

namespace sstl
{

template <class T>
struct _has_iterator_category
{
private:
    struct _two {char _lx; char _lxx;};
    template <class U> static _two _test(...);
    template <class U> static char _test(typename U::iterator_category* = 0);
public:
    static const bool value = sizeof(_test<T>(0)) == 1;
};

template <class T, class U, bool = _has_iterator_category<std::iterator_traits<T>>::value>
struct _has_iterator_category_convertible_to
    : public std::integral_constant<bool, std::is_convertible<typename std::iterator_traits<T>::iterator_category, U>::value>
{};

template <class T, class U>
struct _has_iterator_category_convertible_to<T, U, false> : public std::false_type {};

template <class T>
struct _is_input_iterator : public _has_iterator_category_convertible_to<T, std::input_iterator_tag> {};

template <class T>
struct _is_forward_iterator : public _has_iterator_category_convertible_to<T, std::forward_iterator_tag> {};

template <class T>
struct _is_bidirectional_iterator : public _has_iterator_category_convertible_to<T, std::bidirectional_iterator_tag> {};

template <class T>
struct _is_random_access_iterator : public _has_iterator_category_convertible_to<T, std::random_access_iterator_tag> {};

}

#endif
