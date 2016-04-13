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
            rhs._increment_pointer(src);
            _increment_pointer(dst);
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
            rhs._increment_pointer(src);
            _increment_pointer(dst);
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         auto new_lhs_last = dst;
         _decrement_pointer(new_lhs_last);
         _derived()._last_pointer = new_lhs_last;
         _derived()._size += i;

         rhs._derived()._first_pointer = src;
         rhs._derived()._size -= (move_assignments + i);

         throw;
      }
      #endif

      auto new_lhs_last = dst;
      _decrement_pointer(new_lhs_last);
      _derived()._last_pointer = new_lhs_last;
      for(auto i=destructions; i!=0; --i)
      {
         dst->~value_type();
         _increment_pointer(dst);
      }
      _derived()._size = rhs.size();

      auto new_rhs_last = rhs._derived()._first_pointer;
      rhs._decrement_pointer(new_rhs_last);
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
         _increment_pointer(dst);
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
            _increment_pointer(dst);
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _decrement_pointer(dst);
         _derived()._last_pointer = dst;
         _derived()._size += constructions_done;
         throw;
      }
      #endif

      auto new_last = dst;
      _decrement_pointer(new_last);

      auto destructions = size() < count ? 0 : size() - count;
      for(size_type i=0; i<destructions; ++i)
      {
         dst->~value_type();
         _increment_pointer(dst);
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
      return begin()[idx];
   }

   const_reference at(size_type idx) const
      _sstl_noexcept(noexcept(std::declval<dequeng>().at(size_type{})))
   {
      return const_cast<dequeng&>(*this).at(idx);
   }

   reference operator[](size_type idx) _sstl_noexcept_
   {
      return begin()[idx];
   }

   const_reference operator[](size_type idx) const _sstl_noexcept_
   {
      return cbegin()[idx];
   }

   const_reference front() const _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *cbegin();
   }

   reference front() _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *begin();
   }

   const_reference back() const _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *(cend()-1);
   }

   reference back() _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *(end()-1);
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
      return _derived()._size == 0;
   }

   size_type size() const _sstl_noexcept_
   {
      return _derived()._size;
   }

   size_type capacity() const _sstl_noexcept_
   {
      return _derived()._end_storage - _derived()._begin_storage();
   }

   void clear() _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      //FIXME
      auto crt = _derived()._first_pointer;
      while(crt <= _derived()._last_pointer)
      {
         crt->~value_type();
         ++crt;
      }
      _derived()._first_pointer = crt;
      _derived()._size = 0;
   }

   void push_back(const_reference value)
      _sstl_noexcept(std::is_nothrow_constructible<value_type>::value)
   {
      sstl_assert(size() < capacity());
      _increment_pointer(_derived()._last_pointer);
      new(_derived()._last_pointer) value_type(value);
      ++_derived()._size;
   }

   void pop_front()
      _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      sstl_assert(!empty());
      _derived()._first_pointer->~value_type();
      _increment_pointer(_derived()._first_pointer);
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
      auto pos = _derived()._first_pointer-1;
      #if _sstl_has_exceptions()
      try
      {
      #endif
         for(size_type i=0; i<count; ++i)
         {
            new(++pos) value_type(value);
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _derived()._last_pointer = pos-1;
         clear();
         throw;
      }
      #endif
      _derived()._size = count;
      _derived()._last_pointer = pos;
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
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
      auto src = rhs.begin();
      auto dst = _derived()._begin_storage();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != rhs.end())
         {
            new(dst) value_type(std::move(*src));
            src->~value_type();
            ++src; ++dst;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         auto number_of_move_constructions = dst - _derived()._begin_storage();
         rhs._derived()._first_pointer = std::addressof(*src);
         rhs._derived()._size -= number_of_move_constructions;
         _derived()._last_pointer = dst-1;
         clear();
         throw;
      }
      #endif
      _derived()._last_pointer = dst-1;
      _derived()._size = rhs.size();
      rhs._derived()._last_pointer = rhs._derived()._first_pointer-1;
      rhs._derived()._size = 0;
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
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
         _increment_pointer(dst);
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
            _increment_pointer(dst);
            ++src;
            ++new_size;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _decrement_pointer(dst);
         _derived()._last_pointer = dst;
         _derived()._size = new_size;
         throw;
      }
      #endif

      auto new_last_pointer = dst;
      _decrement_pointer(new_last_pointer);

      size_type destructions = new_size < size() ? size() - new_size : 0;
      while(destructions > 0)
      {
         dst->~value_type();
         _increment_pointer(dst);
         --destructions;
      }

      _derived()._last_pointer = new_last_pointer;
      _derived()._size = new_size;
   }

   _type_for_derived_class_access& _derived() _sstl_noexcept_;
   const _type_for_derived_class_access& _derived() const _sstl_noexcept_;

   void _increment_pointer(pointer& ptr) _sstl_noexcept_
   {
      auto new_ptr = ptr + 1;
      if(new_ptr == _derived()._end_storage)
         new_ptr = _derived()._begin_storage();
      ptr = new_ptr;
   }

   void _increment_pointer(const_pointer& ptr) const _sstl_noexcept_
   {
      const_cast<dequeng&>(*this)._increment_pointer(const_cast<pointer&>(ptr));
   }

   void _increment_pointer(pointer& ptr) const _sstl_noexcept_
   {
      const_cast<dequeng&>(*this)._increment_pointer(ptr);
   }

   void _decrement_pointer(pointer& ptr) _sstl_noexcept_
   {
      ptr -= 1;
      if(ptr < _derived()._begin_storage())
         ptr = _derived()._end_storage - 1;
   }

   void _decrement_pointer(const_pointer& ptr) const _sstl_noexcept_
   {
      const_cast<dequeng&>(*this)._decrement_pointer(const_cast<pointer&>(ptr));
   }

   void _decrement_pointer(pointer& ptr) const _sstl_noexcept_
   {
      const_cast<dequeng&>(*this)._decrement_pointer(ptr);
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

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
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

   void assign(size_type count, const_reference value)
      _sstl_noexcept(noexcept(std::declval<_base>().assign(std::declval<size_type>(), std::declval<const_reference>())))
   {
      _base::assign(count, value);
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
