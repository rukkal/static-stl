/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_DEQUENG_ITERATOR__
#define _SSTL_DEQUENG_ITERATOR__

#include <cstddef>
#include <type_traits>
#include <iterator>
#include "_except.h"

template<class TDeque>
class _dequeng_iterator
{
private:
   static const bool is_const = std::is_const<TDeque>::value;

public:
   using iterator_category = std::random_access_iterator_tag;
   using value_type = typename TDeque::value_type;
   using difference_type = ptrdiff_t;
   using pointer = typename std::conditional<
      is_const,
      typename TDeque::const_pointer,
      typename TDeque::pointer>::type;
   using reference = typename std::conditional<
      is_const,
      typename TDeque::const_reference,
      typename TDeque::reference>::type;

public:
   _dequeng_iterator(TDeque* deque, pointer pos) _sstl_noexcept_
      : _deque(deque)
      , _pos(pos)
   {}

   reference operator*() _sstl_noexcept_
   {
      return *_pos;
   }

   pointer operator->() _sstl_noexcept_
   {
      return _pos;
   }

   _dequeng_iterator& operator++() _sstl_noexcept_
   {
      _deque->_increment_pointer(_pos);
      return *this;
   }

   _dequeng_iterator operator++(int) _sstl_noexcept_
   {
      _dequeng_iterator temp(*this);
      _deque->_increment_pointer(_pos);
      return temp;
   }

   _dequeng_iterator& operator--() _sstl_noexcept_
   {
      _deque->_decrement_pointer(_pos);
      return *this;
   }

   _dequeng_iterator operator--(int) _sstl_noexcept_
   {
      _dequeng_iterator temp(*this);
      _deque->_decrement_pointer(_pos);
      return temp;
   }

   _dequeng_iterator& operator+=(difference_type inc) _sstl_noexcept_
   {
      _deque->_offset_pointer(_pos, inc);
      return *this;
   }

   _dequeng_iterator& operator-=(difference_type dec) _sstl_noexcept_
   {
      _deque->_offset_pointer(_pos, -dec);
      return *this;
   }

   bool operator==(const _dequeng_iterator& rhs) const _sstl_noexcept_
   {
      return _pos==rhs._pos;
   }

   bool operator!=(const _dequeng_iterator& rhs) const _sstl_noexcept_
   {
      return !operator==(rhs);
   }

   friend _dequeng_iterator operator+(const _dequeng_iterator& lhs, difference_type rhs)
   {
      auto tmp = lhs;
      tmp += rhs;
      return tmp;
   }

   friend _dequeng_iterator operator+(difference_type lhs, const _dequeng_iterator& rhs)
   {
      return rhs+lhs;
   }

private:
   TDeque* _deque;
   pointer _pos;
};


#endif
