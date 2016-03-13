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
      if(_pos == _deque->_last_pointer())
         _pos = nullptr;
      else
         _deque->_increment_pointer(_pos);
      return *this;
   }

   _dequeng_iterator operator++(int) _sstl_noexcept_
   {
      _dequeng_iterator temp(*this);
      ++(*this);
      return temp;
   }

   _dequeng_iterator& operator--() _sstl_noexcept_
   {
      if(_pos == nullptr)
         _pos = _deque->_last_pointer();
      else
         _deque->_decrement_pointer(_pos);
      return *this;
   }

   _dequeng_iterator operator--(int) _sstl_noexcept_
   {
      _dequeng_iterator temp(*this);
      --(*this);
      return temp;
   }

   _dequeng_iterator& operator+=(difference_type inc) _sstl_noexcept_
   {
      _apply_offset_to_pos(inc);
      return *this;
   }

   _dequeng_iterator& operator-=(difference_type dec) _sstl_noexcept_
   {
      _apply_offset_to_pos(-dec);
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

   friend _dequeng_iterator operator+(const _dequeng_iterator& lhs, difference_type rhs) _sstl_noexcept_
   {
      auto tmp = lhs;
      tmp += rhs;
      return tmp;
   }

   friend _dequeng_iterator operator+(difference_type lhs, const _dequeng_iterator& rhs) _sstl_noexcept_
   {
      return rhs+lhs;
   }

private:
   void _apply_offset_to_pos(difference_type offset) _sstl_noexcept_
   {
      sstl_assert(offset >= 0 || -offset <= _deque->capacity());
      sstl_assert(offset < 0 || offset <= _deque->capacity());

      if(_pos == nullptr)
      {
         sstl_assert(offset <= 0);
         _pos = _deque->_last_pointer();
         ++offset;
      }

      _pos += offset;
      if(_pos >= _deque->_end_storage())
      {
         _pos = _deque->_begin_storage() + (_pos - _deque->_end_storage());
      }
      else if(_pos < _deque->_begin_storage())
      {
         _pos = _deque->_end_storage() - (_deque->_begin_storage() - _pos);
      }

      if(_is_pos_one_past_last_pointer())
         _pos = nullptr;
   }

   bool _is_pos_one_past_last_pointer() const _sstl_noexcept_
   {
      auto one_past_last_pointer = _deque->_last_pointer();
      _deque->_increment_pointer(one_past_last_pointer);
      return _pos == one_past_last_pointer;
   }

private:
   TDeque* _deque;
   pointer _pos;
};


#endif
