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

   reference operator*() const _sstl_noexcept_
   {
      return *_pos;
   }

   pointer operator->() const _sstl_noexcept_
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

   _dequeng_iterator operator-(difference_type rhs) const _sstl_noexcept_
   {
      auto tmp = *this;
      tmp -= rhs;
      return tmp;
   }

   difference_type operator-(_dequeng_iterator rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _linearized_pos() - rhs._linearized_pos();
   }

   reference operator[](difference_type offset) const _sstl_noexcept_
   {
      return *(*this + offset);
   }

   bool operator==(const _dequeng_iterator& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _pos==rhs._pos;
   }

   bool operator!=(const _dequeng_iterator& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return !operator==(rhs);
   }

   bool operator<(const _dequeng_iterator& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _linearized_pos() < rhs._linearized_pos();
   }

   bool operator>(const _dequeng_iterator& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _linearized_pos() > rhs._linearized_pos();
   }

   bool operator<=(const _dequeng_iterator& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _linearized_pos() <= rhs._linearized_pos();
   }

   bool operator>=(const _dequeng_iterator& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _linearized_pos() >= rhs._linearized_pos();
   }

private:
   void _apply_offset_to_pos(difference_type offset) _sstl_noexcept_
   {
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
         sstl_assert(_pos <= _deque->_last_pointer()+1);
      }
      else if(_pos < _deque->_begin_storage())
      {
         _pos = _deque->_end_storage() - (_deque->_begin_storage() - _pos);
         sstl_assert(_pos >= _deque->_first_pointer());
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

   difference_type _linearized_pos() const _sstl_noexcept_
   {
      if(_pos != nullptr)
      {
         if(_pos >= _deque->_first_pointer())
            return _deque->_first_pointer() - _pos;
         else
            return (_deque->_end_storage() - _deque->_first_pointer()) + (_pos - _deque->_begin_storage());
      }
      else
      {
         return _deque->size();
      }
   }

private:
   TDeque* _deque;
   pointer _pos;
};

#endif
