/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_DEQUENG__
#define _SSTL_DEQUENG__

#include <utility>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <array>

#include "sstl_assert.h"
#include "__internal/_aligned_storage.h"
#include "__internal/_iterator.h"
#include "__internal/_dequeng_iterator.h"
#include "__internal/_hacky_derived_class_access.h"
#include "__internal/_debug.h"

namespace sstl
{

template<class, size_t=static_cast<size_t>(-1)>
class dequeng;

template<class T>
class dequeng<T>
{
template<class U, size_t S>
friend class dequeng; //friend declaration required for derived class' noexcept expressions

friend class _dequeng_iterator<dequeng>;
friend class _dequeng_iterator<const dequeng>;

public:
   using value_type = T;
   using size_type = size_t;
   using reference = value_type&;
   using const_reference = const value_type&;
   using pointer = value_type*;
   using const_pointer = const value_type*;
   using iterator = _dequeng_iterator<dequeng>;
   using const_iterator = _dequeng_iterator<const dequeng>;
   using reverse_iterator = std::reverse_iterator<iterator>;
   using const_reverse_iterator = std::reverse_iterator<const_iterator>;
   using difference_type = typename iterator::difference_type;

public:
   dequeng& operator=(const dequeng& rhs)
      _sstl_noexcept_(  std::is_nothrow_copy_assignable<value_type>::value
                        && std::is_nothrow_copy_constructible<value_type>::value)
   {
      if(this == &rhs)
         return *this;

      sstl_assert(rhs.size() <= capacity());
      _range_assignment(rhs.cbegin(), rhs.cend());

      return *this;
   }

   dequeng& operator=(dequeng&& rhs)
      _sstl_noexcept(std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_move_constructible<value_type>::value)
   {
      if(this == &rhs)
         return *this;

      sstl_assert(rhs.size() <= capacity());
      auto move_assignments = std::min(size(), rhs.size());
      auto move_constructions = rhs.size() - move_assignments;
      auto destructions =  move_assignments + move_constructions < size()
                           ? size() - move_assignments - move_constructions
                           : 0;
      auto src = rhs._derived()._first_pointer;
      auto dst = _derived()._first_pointer;

      size_type i;
      #if _sstl_has_exceptions()
      try
      {
      #endif
         for(i=0; i!=move_assignments; ++i)
         {
            *dst = std::move(*src);
            src->~value_type();
            src = rhs._inc_pointer(src);
            dst = _inc_pointer(dst);
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         rhs._derived()._first_pointer = src;
         rhs._derived()._size -= i;
         throw;
      }
      #endif

      #if _sstl_has_exceptions()
      try
      {
      #endif
         for(i=0; i!=move_constructions; ++i)
         {
            new(dst) value_type(std::move(*src));
            src->~value_type();
            src = rhs._inc_pointer(src);
            dst = _inc_pointer(dst);
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         auto new_lhs_last = dst;
         new_lhs_last = _dec_pointer(new_lhs_last);
         _derived()._last_pointer = new_lhs_last;
         _derived()._size += i;

         rhs._derived()._first_pointer = src;
         rhs._derived()._size -= (move_assignments + i);

         throw;
      }
      #endif

      auto new_lhs_last = dst;
      new_lhs_last = _dec_pointer(new_lhs_last);
      _derived()._last_pointer = new_lhs_last;
      for(auto i=destructions; i!=0; --i)
      {
         dst->~value_type();
         dst = _inc_pointer(dst);
      }
      _derived()._size = rhs.size();

      auto new_rhs_last = rhs._derived()._first_pointer;
      new_rhs_last = rhs._dec_pointer(new_rhs_last);
      rhs._derived()._last_pointer = new_rhs_last;
      rhs._derived()._size = 0;

      return *this;
   }

   dequeng& operator=(std::initializer_list<value_type> ilist)
      _sstl_noexcept(noexcept(std::declval<dequeng>()._range_assignment(
         std::declval<std::initializer_list<value_type>>().begin(),
         std::declval<std::initializer_list<value_type>>().end())))
   {
      _range_assignment(ilist.begin(), ilist.end());
      return *this;
   }

   void assign(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>())
   {
      sstl_assert(count <= capacity());
      auto dst = _derived()._first_pointer;

      auto assignments = std::min(size(), count);
      for(size_type i=0; i<assignments; ++i)
      {
         *dst = value;
         dst = _inc_pointer(dst);
      }

      auto constructions = count - assignments;
      size_type constructions_done;
      #if _sstl_has_exceptions()
      try
      {
      #endif
         for(constructions_done=0; constructions_done<constructions; ++constructions_done)
         {
            new(dst) value_type(value);
            dst = _inc_pointer(dst);
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         dst = _dec_pointer(dst);
         _derived()._last_pointer = dst;
         _derived()._size += constructions_done;
         throw;
      }
      #endif

      auto new_last = dst;
      new_last = _dec_pointer(new_last);

      auto destructions = size() < count ? 0 : size() - count;
      for(size_type i=0; i<destructions; ++i)
      {
         dst->~value_type();
         dst = _inc_pointer(dst);
      }

      _derived()._last_pointer = new_last;
      _derived()._size = count;
   }

   reference at(size_type idx) _sstl_noexcept(!_sstl_has_exceptions())
   {
      #if _sstl_has_exceptions()
      if(idx >= size())
      {
         throw std::out_of_range(_sstl_debug_message("access out of range"));
      }
      #endif
      sstl_assert(idx < size());
      return *_add_offset_to_pointer(_derived()._first_pointer, idx);
   }

   const_reference at(size_type idx) const
      _sstl_noexcept(noexcept(std::declval<dequeng>().at(size_type{})))
   {
      return const_cast<dequeng&>(*this).at(idx);
   }

   reference operator[](size_type idx) _sstl_noexcept_
   {
      return *_add_offset_to_pointer(_derived()._first_pointer, idx);
   }

   const_reference operator[](size_type idx) const _sstl_noexcept_
   {
      return const_cast<dequeng&>(*this)[idx];
   }

   reference front() _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *_derived()._first_pointer;
   }

   const_reference front() const _sstl_noexcept_
   {
      sstl_assert(!empty());
      return const_cast<dequeng&>(*this).front();
   }

   reference back() _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *(_derived()._last_pointer);
   }

   const_reference back() const _sstl_noexcept_
   {
      return const_cast<dequeng&>(*this).back();
   }

   iterator begin() _sstl_noexcept_
   {
      return iterator{ this, empty() ? nullptr : _derived()._first_pointer };
   }

   const_iterator begin() const _sstl_noexcept_
   {
      return const_iterator{ this, empty() ? nullptr : _derived()._first_pointer };
   }

   const_iterator cbegin() const _sstl_noexcept_
   {
      return const_iterator{ this, empty() ? nullptr : _derived()._first_pointer };
   }

   iterator end() _sstl_noexcept_
   {
      return iterator{ this, nullptr };
   }

   const_iterator end() const _sstl_noexcept_
   {
      return const_iterator{ this, nullptr };
   }

   const_iterator cend() const _sstl_noexcept_
   {
      return const_iterator{ this, nullptr };
   }

   reverse_iterator rbegin() _sstl_noexcept_
   {
      return reverse_iterator{ end() };
   }

   const_reverse_iterator rbegin() const _sstl_noexcept_
   {
      return crbegin();
   }

   const_reverse_iterator crbegin() const _sstl_noexcept_
   {
      return const_reverse_iterator{ cend() };
   }

   reverse_iterator rend() _sstl_noexcept_
   {
      return reverse_iterator{ begin() };
   }

   const_reverse_iterator rend() const _sstl_noexcept_
   {
      return crend();
   }

   const_reverse_iterator crend() const _sstl_noexcept_
   {
      return const_reverse_iterator{ cbegin() };
   }

   bool empty() const _sstl_noexcept_
   {
      return size() == 0;
   }

   bool full() const _sstl_noexcept_
   {
      return size() == capacity();
   }

   size_type size() const _sstl_noexcept_
   {
      return _derived()._size;
   }

   size_type max_size() const _sstl_noexcept_
   {
      return capacity();
   }

   size_type capacity() const _sstl_noexcept_
   {
      return _derived()._end_storage - _derived()._begin_storage();
   }

   void clear() _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      while(_derived()._size > 0)
      {
         _derived()._last_pointer->~value_type();
         _derived()._last_pointer = _dec_pointer(_derived()._last_pointer);
         --_derived()._size;
      }
   }

   iterator insert(const_iterator pos, const value_type& value)
   {
      return _emplace_value(pos, value);
   }

   iterator insert(const_iterator pos, value_type&& value)
   {
      return _emplace_value(pos, std::move(value));
   }

   void push_front(const_reference value)
      _sstl_noexcept(noexcept(std::declval<dequeng>().emplace_front(std::declval<const_reference>())))
   {
      emplace_front(value);
   }

   void push_front(value_type&& value)
      _sstl_noexcept(noexcept(std::declval<dequeng>().emplace_front(std::declval<value_type&&>())))
   {
      emplace_front(std::move(value));
   }

   template<class... Args>
   void emplace_front(Args&&... value)
      _sstl_noexcept(std::is_nothrow_constructible<value_type, typename std::add_rvalue_reference<Args>::type...>::value)
   {
      sstl_assert(!full());
      _derived()._first_pointer = _dec_pointer(_derived()._first_pointer);
      new(_derived()._first_pointer) value_type(std::forward<Args>(value)...);
      ++_derived()._size;
   }

   void push_back(const_reference value)
      _sstl_noexcept(noexcept(std::declval<dequeng>().emplace_back(std::declval<const_reference>())))
   {
      emplace_back(value);
   }

   void push_back(value_type&& value)
      _sstl_noexcept(noexcept(std::declval<dequeng>().emplace_back(std::declval<value_type&&>())))
   {
      emplace_back(std::move(value));
   }

   template<class... Args>
   void emplace_back(Args&&... args)
      _sstl_noexcept(std::is_nothrow_constructible<value_type, typename std::add_rvalue_reference<Args>::type...>::value)
   {
      sstl_assert(!full());
      _derived()._last_pointer = _inc_pointer(_derived()._last_pointer);
      new(_derived()._last_pointer) value_type(std::forward<Args>(args)...);
      ++_derived()._size;
   }

   void pop_back() _sstl_noexcept_
   {
      sstl_assert(!empty());
      _derived()._last_pointer->~value_type();
      _derived()._last_pointer = _dec_pointer(_derived()._last_pointer);
      --_derived()._size;
   }

   void pop_front()
      _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      sstl_assert(!empty());
      _derived()._first_pointer->~value_type();
      _derived()._first_pointer = _inc_pointer(_derived()._first_pointer);
      --_derived()._size;
   }

protected:
   using _type_for_derived_class_access = dequeng<T, 11>;

protected:
   dequeng() = default;
   dequeng(const dequeng&) = default;
   dequeng(dequeng&&) {}; //MSVC (VS2013) does not allow to default move special member functions
   ~dequeng() = default;

   void _count_constructor(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      sstl_assert(count <= capacity());
      auto pos = _derived()._first_pointer;
      auto last_pos = pos+count;
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(pos != last_pos)
         {
            new(pos) value_type(value);
            ++pos;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _derived()._last_pointer = pos-1;
         _derived()._size = pos - _derived()._first_pointer;
         clear();
         throw;
      }
      #endif
      _derived()._size = count;
      _derived()._last_pointer = pos-1;
   }

   template<class TIterator, class = typename std::enable_if<_is_input_iterator<TIterator>::value>::type>
   void _range_constructor(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      auto src = range_begin;
      auto dst = _derived()._begin_storage()-1;
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != range_end)
         {
            sstl_assert(size() < capacity());
            new(++dst) value_type(*src);
            ++_derived()._size;
            ++src;
         }
         _derived()._last_pointer = dst;
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _derived()._last_pointer = dst-1;
         clear();
         throw;
      }
      #endif
   }

   void _move_constructor(dequeng&& rhs)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value)
   {
      sstl_assert(rhs.size() <= capacity());
      auto src = rhs._derived()._first_pointer;
      auto dst = _derived()._begin_storage();
      auto remaining_move_constructions = rhs.size();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(remaining_move_constructions > 0)
         {
            new(dst) value_type(std::move(*src));
            src->~value_type();
            src = rhs._inc_pointer(src);
            ++dst;
            --remaining_move_constructions;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         auto number_of_move_constructions = rhs.size() - remaining_move_constructions;
         rhs._derived()._first_pointer = std::addressof(*src);
         rhs._derived()._size -= number_of_move_constructions;
         _derived()._last_pointer = dst-1;
         _derived()._size = number_of_move_constructions;
         clear();
         throw;
      }
      #endif
      _derived()._last_pointer = dst-1;
      _derived()._size = rhs.size();
      rhs._derived()._last_pointer = rhs._derived()._first_pointer-1;
      rhs._derived()._size = 0;
   }

   template<class TIterator, class = typename std::enable_if<_is_input_iterator<TIterator>::value>::type>
   void _range_assignment(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      auto src = range_begin;
      auto dst = _derived()._first_pointer;

      size_type assignments = 0;
      while(src != range_end && assignments < size())
      {
         *dst = *src;
         dst = _inc_pointer(dst);
         ++src;
         ++assignments;
      }

      size_type new_size = assignments;

      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != range_end)
         {
            sstl_assert(new_size < capacity());
            new(dst) value_type(*src);
            dst = _inc_pointer(dst);
            ++src;
            ++new_size;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         dst = _dec_pointer(dst);
         _derived()._last_pointer = dst;
         _derived()._size = new_size;
         throw;
      }
      #endif

      auto new_last_pointer = dst;
      new_last_pointer = _dec_pointer(new_last_pointer);

      size_type destructions = new_size < size() ? size() - new_size : 0;
      while(destructions > 0)
      {
         dst->~value_type();
         dst = _inc_pointer(dst);
         --destructions;
      }

      _derived()._last_pointer = new_last_pointer;
      _derived()._size = new_size;
   }

   template<class TValue>
   iterator _emplace_value(const_iterator pos, TValue&& value)
   {
      sstl_assert(!full());
      auto distance_to_begin = std::distance(cbegin(), pos);
      auto distance_to_end = std::distance(pos, cend());
      if(distance_to_begin < distance_to_end)
      {
         if(distance_to_begin > 0)
         {
            auto nonconst_pos = iterator{ this, const_cast<pointer>(std::addressof(*pos)) };
            _shift_from_begin_to_pos_by_n_positions(nonconst_pos, 1);
            --nonconst_pos;
            *nonconst_pos = std::forward<TValue>(value);
            return nonconst_pos;
         }
         else
         {
            push_front(std::forward<TValue>(value));
            return begin();
         }
      }
      else
      {
         if(distance_to_end > 0)
         {
            auto nonconst_pos = iterator{this, const_cast<pointer>(std::addressof(*pos))};
            _shift_from_pos_to_end_by_n_positions(nonconst_pos, 1);
            *nonconst_pos = std::forward<TValue>(value);
            return nonconst_pos;
         }
         else
         {
            push_back(std::forward<TValue>(value));
            return end()-1;
         }
      }
   }

   void _shift_from_begin_to_pos_by_n_positions(iterator pos, size_type n)
      _sstl_noexcept(   std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value)
   {
      auto number_of_constructions = std::min(n, static_cast<size_type>(pos-begin()));

      auto dst_first = _subtract_offset_to_pointer(_derived()._first_pointer, number_of_constructions);
      auto dst = dst_first;
      auto src = _derived()._first_pointer;

      size_type remaining_constructions;
      #if _sstl_has_exceptions()
      try
      {
      #endif
         for(remaining_constructions=number_of_constructions; remaining_constructions>0; --remaining_constructions)
         {
            new(dst) value_type(std::move(*src));
            src = _inc_pointer(src);
            dst = _inc_pointer(dst);
         }

         auto number_of_assignments = pos - begin() - number_of_constructions;
         for(size_type i=number_of_assignments; i>0; i--)
         {
            *dst = std::move(*src);
            src = _inc_pointer(src);
            dst = _inc_pointer(dst);
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         auto destructions = number_of_constructions - remaining_constructions;
         auto ptr = dst_first;
         for(size_type i=destructions; i>0; --i)
         {
            ptr->~value_type();
            ptr = _inc_pointer(ptr);
         }
         throw;
      }
      #endif

      _derived()._first_pointer = _subtract_offset_to_pointer(_derived()._first_pointer, n);
      _derived()._size += n;
   }

   void _shift_from_pos_to_end_by_n_positions(iterator pos, size_type n)
      _sstl_noexcept(   std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value)
   {
      auto number_of_constructions = std::min(n, static_cast<size_type>(end()-pos));
      auto number_of_assignments = static_cast<size_type>(end()-pos) - number_of_constructions;

      auto dst_first = _add_offset_to_pointer(_derived()._last_pointer, number_of_constructions);
      auto dst = dst_first;
      auto src = _derived()._last_pointer;

      size_type remaining_constructions;
      #if _sstl_has_exceptions()
      try
      {
      #endif
         for(remaining_constructions=number_of_constructions; remaining_constructions>0; --remaining_constructions)
         {
            new(dst) value_type(std::move(*src));
            src = _dec_pointer(src);
            dst = _dec_pointer(dst);
         }

         for(size_type i=0; i<number_of_assignments; ++i)
         {
            *dst = std::move(*src);
            src = _dec_pointer(src);
            dst = _dec_pointer(dst);
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         auto destructions = number_of_constructions - remaining_constructions;
         auto ptr = dst_first;
         for(size_type i=0; i<destructions; ++i)
         {
            ptr->~value_type();
            ptr = _dec_pointer(ptr);
         }
         throw;
      }
      #endif

      _derived()._last_pointer = _add_offset_to_pointer(_derived()._last_pointer, n);
      _derived()._size += n;
   }

   _type_for_derived_class_access& _derived() _sstl_noexcept_;
   const _type_for_derived_class_access& _derived() const _sstl_noexcept_;

   pointer _inc_pointer(pointer ptr) const _sstl_noexcept_
   {
      ptr += 1;
      if(ptr == _derived()._end_storage)
         ptr = const_cast<pointer>(_derived()._begin_storage());
      return ptr;
   }

   const_pointer _inc_pointer(const_pointer ptr) const _sstl_noexcept_
   {
      return _inc_pointer(const_cast<pointer>(ptr));
   }

   pointer _dec_pointer(pointer ptr) const _sstl_noexcept_
   {
      ptr -= 1;
      if(ptr < _derived()._begin_storage())
         ptr = const_cast<pointer>(_derived()._end_storage) - 1;
      return ptr;
   }

   const_pointer _dec_pointer(const_pointer ptr) const _sstl_noexcept_
   {
      return _dec_pointer(const_cast<pointer>(ptr));
   }

   pointer _add_offset_to_pointer(pointer ptr, size_type offset) const _sstl_noexcept_
   {
      auto begin_storage = const_cast<pointer>(_derived()._begin_storage());
      auto end_storage = const_cast<pointer>(_derived()._end_storage);

      ptr += offset;
      if(ptr >= end_storage)
      {
         ptr = begin_storage + (ptr - end_storage);
      }
      return ptr;
   }

   pointer _subtract_offset_to_pointer(pointer ptr, size_type offset) const _sstl_noexcept_
   {
      auto begin_storage = const_cast<pointer>(_derived()._begin_storage());
      auto end_storage = const_cast<pointer>(_derived()._end_storage);

      ptr -= offset;
      if(ptr < begin_storage)
      {
         ptr = end_storage - (begin_storage - ptr);
      }
      return ptr;
   }

   pointer _apply_offset_to_pointer(pointer ptr, difference_type offset) const _sstl_noexcept_
   {
      auto first_pointer = const_cast<pointer>(_derived()._first_pointer);
      auto last_pointer = const_cast<pointer>(_derived()._last_pointer);

      if(offset > 0)
      {
         ptr = _add_offset_to_pointer(ptr, offset);
      }
      else
      {
         ptr = _subtract_offset_to_pointer(ptr, -offset);
      }

      return ptr;
   }

   const_pointer _apply_offset_to_pointer(const_pointer ptr, difference_type offset) const _sstl_noexcept_
   {
      return _apply_offset_to_pointer(const_cast<pointer>(ptr), offset);
   }

   bool _is_pointer_one_past_last_pointer(const_pointer ptr) const _sstl_noexcept_
   {
      auto one_past_last_pointer = _derived()._last_pointer;
      one_past_last_pointer = _inc_pointer(one_past_last_pointer);
      return ptr == one_past_last_pointer;
   }
};

template<class T, size_t CAPACITY>
class dequeng : public dequeng<T>
{
private:
   using _base = dequeng<T>;
   using _type_for_derived_class_access = typename _base::_type_for_derived_class_access;

   template<class, size_t>
   friend class dequeng;

   friend class _dequeng_iterator<_base>;
   friend class _dequeng_iterator<const _base>;

public:
   using value_type = typename _base::value_type;
   using size_type = typename _base::size_type;
   //using difference_type = typename _base::difference_type;
   using reference = typename _base::reference;
   using const_reference = typename _base::const_reference;
   using pointer = typename _base::pointer;
   using const_pointer = typename _base::const_pointer;
   using iterator = typename _base::iterator;
   using const_iterator = typename _base::const_iterator;
   //using reverse_iterator = typename _base::reverse_iterator;
   //using const_reverse_iterator = typename _base::const_reverse_iterator;

public:
   dequeng() _sstl_noexcept_
      : _last_pointer(_begin_storage()-1)
   {
      _assert_hacky_derived_class_access_is_valid<dequeng<value_type>, dequeng, _type_for_derived_class_access>();
   }

   explicit dequeng(size_type count, const_reference value = value_type())
      _sstl_noexcept(noexcept(std::declval<_base>()._count_constructor(std::declval<size_type>(), std::declval<value_type>())))
   {
      _assert_hacky_derived_class_access_is_valid<dequeng<value_type>, dequeng, _type_for_derived_class_access>();
      _base::_count_constructor(count, value);
   }

   template<class TIterator, class = typename std::enable_if<_is_input_iterator<TIterator>::value>::type>
   dequeng(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<TIterator>(),
                                                                        std::declval<TIterator>())))
   {
      _assert_hacky_derived_class_access_is_valid<dequeng<value_type>, dequeng, _type_for_derived_class_access>();
      _base::_range_constructor(range_begin, range_end);
   }

   //copy construction from any instance with same value type (capacity doesn't matter)
   dequeng(const _base& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<const_iterator>(),
                                                                        std::declval<const_iterator>())))
   {
      _assert_hacky_derived_class_access_is_valid<dequeng<value_type>, dequeng, _type_for_derived_class_access>();
      _base::_range_constructor(const_cast<_base&>(rhs).cbegin(), const_cast<_base&>(rhs).cend());
   }

   dequeng(const dequeng& rhs)
      _sstl_noexcept(noexcept(dequeng(std::declval<const _base&>())))
      : dequeng(static_cast<const _base&>(rhs))
   {
      _assert_hacky_derived_class_access_is_valid<dequeng<value_type>, dequeng, _type_for_derived_class_access>();
   }

   //move construction from any instance with same value type (capacity doesn't matter)
   dequeng(_base&& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._move_constructor(std::declval<_base>())))
   {
      _assert_hacky_derived_class_access_is_valid<dequeng<value_type>, dequeng, _type_for_derived_class_access>();
      _base::_move_constructor(std::move(rhs));
   }

   dequeng(dequeng&& rhs)
      _sstl_noexcept(noexcept(dequeng(std::declval<_base>())))
      : dequeng(static_cast<_base&&>(rhs))
   {
      _assert_hacky_derived_class_access_is_valid<dequeng<value_type>, dequeng, _type_for_derived_class_access>();
   }

   dequeng(std::initializer_list<value_type> init)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<std::initializer_list<value_type>>().begin(),
                                                                        std::declval<std::initializer_list<value_type>>().end())))
   {
      _assert_hacky_derived_class_access_is_valid<dequeng<value_type>, dequeng, _type_for_derived_class_access>();
      _base::_range_constructor(init.begin(), init.end());
   }

   ~dequeng()
   {
      _base::clear();
   }

   //copy assignment from any instance with same value type (capacity doesn't matter)
   dequeng& operator=(const _base& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<const _base&>())))
   {
      return reinterpret_cast<dequeng&>(_base::operator=(rhs));
   }

   dequeng& operator=(const dequeng& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<const _base&>())))
   {
      return reinterpret_cast<dequeng&>(_base::operator=(rhs));
   }

   //move assignment from any instance with same value type (capacity doesn't matter)
   dequeng& operator=(_base&& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<_base&&>())))
   {
      return reinterpret_cast<dequeng&>(_base::operator=(std::move(rhs)));
   }

   dequeng& operator=(dequeng&& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<_base&&>())))
   {
      return reinterpret_cast<dequeng&>(_base::operator=(std::move(rhs)));
   }

   dequeng& operator=(std::initializer_list<value_type> ilist)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<std::initializer_list<value_type>>())))
   {
      return reinterpret_cast<dequeng&>(_base::operator=(ilist));
   }

private:
   pointer _begin_storage() _sstl_noexcept_
   {
      auto begin_storage = reinterpret_cast<_type_for_derived_class_access&>(*this)._buffer.data();
      return static_cast<pointer>(static_cast<void*>(begin_storage));
   }

   const_pointer _begin_storage() const _sstl_noexcept_
   {
      auto begin_storage = reinterpret_cast<const _type_for_derived_class_access&>(*this)._buffer.data();
      return static_cast<const_pointer>(static_cast<const void*>(begin_storage));
   }

private:
   size_type _size{ 0 };
   pointer _first_pointer{ _begin_storage() };
   pointer _last_pointer;
   pointer _end_storage{ _begin_storage() + CAPACITY };
   std::array<typename _aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type, CAPACITY> _buffer;
};

template<class T>
inline bool operator==(const dequeng<T>& lhs, const dequeng<T>& rhs)
{
   return lhs.size() == rhs.size() && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template<class T>
typename dequeng<T>::_type_for_derived_class_access& dequeng<T>::_derived() _sstl_noexcept_
{
   return reinterpret_cast<_type_for_derived_class_access&>(*this);
}

template<class T>
const typename dequeng<T>::_type_for_derived_class_access& dequeng<T>::_derived() const _sstl_noexcept_
{
   return reinterpret_cast<const _type_for_derived_class_access&>(*this);
}

}

#endif
