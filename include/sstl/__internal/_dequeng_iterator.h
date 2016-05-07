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
   friend TDeque;

   friend _dequeng_iterator<typename std::add_const<TDeque>::type>;
   friend _dequeng_iterator<typename std::remove_const<TDeque>::type>;

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
   _dequeng_iterator() = default;

   _dequeng_iterator(TDeque* deque, pointer pos) _sstl_noexcept_
      : _deque(deque)
      , _pos(pos)
   {}

   operator _dequeng_iterator<typename std::add_const<TDeque>::type>() const
   {
      return _dequeng_iterator<typename std::add_const<TDeque>::type>{ _deque, _pos};
   }

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
      if(_pos == _deque->_derived()._last_pointer)
         _pos = nullptr;
      else
         _pos = _deque->_inc_pointer(_pos);
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
         _pos = _deque->_derived()._last_pointer;
      else
         _pos = _deque->_dec_pointer(_pos);
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
      if(_pos == nullptr)
      {
         sstl_assert(inc <= 0);
         _pos = _deque->_derived()._last_pointer;
         ++inc;
      }
      _pos = _deque->_apply_offset_to_pointer(_pos, inc);
      if(_deque->_is_pointer_one_past_last_pointer(_pos))
         _pos = nullptr;
      return *this;
   }

   _dequeng_iterator& operator-=(difference_type dec) _sstl_noexcept_
   {
      if(_pos == nullptr)
      {
         sstl_assert(dec >= 0);
         _pos = _deque->_derived()._last_pointer;
         --dec;
      }
      _pos = _deque->_apply_offset_to_pointer(_pos, -dec);
      if(_deque->_is_pointer_one_past_last_pointer(_pos))
         _pos = nullptr;
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

   bool operator==(const _dequeng_iterator<typename std::add_const<TDeque>::type>& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _pos==rhs._pos;
   }

   bool operator!=(const _dequeng_iterator<typename std::add_const<TDeque>::type>& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return !operator==(rhs);
   }

   bool operator<(const _dequeng_iterator<typename std::add_const<TDeque>::type>& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _linearized_pos() < rhs._linearized_pos();
   }

   bool operator>(const _dequeng_iterator<typename std::add_const<TDeque>::type>& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _linearized_pos() > rhs._linearized_pos();
   }

   bool operator<=(const _dequeng_iterator<typename std::add_const<TDeque>::type>& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _linearized_pos() <= rhs._linearized_pos();
   }

   bool operator>=(const _dequeng_iterator<typename std::add_const<TDeque>::type>& rhs) const _sstl_noexcept_
   {
      sstl_assert(_deque == rhs._deque);
      return _linearized_pos() >= rhs._linearized_pos();
   }

private:
   difference_type _linearized_pos() const _sstl_noexcept_
   {
      if(_pos != nullptr)
      {
         if(_pos >= _deque->_derived()._first_pointer)
            return _pos - _deque->_derived()._first_pointer;
         else
            return (_deque->_derived()._end_storage - _deque->_derived()._first_pointer) + (_pos - _deque->_derived()._begin_storage());
      }
      else
      {
         return _deque->size();
      }
   }

private:
   TDeque* _deque{ nullptr };
   pointer _pos{ nullptr };
};

#endif
