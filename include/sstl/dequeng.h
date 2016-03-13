/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_DEQUENG__
#define _SSTL_DEQUENG__

#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <array>

#include "sstl_assert.h"
#include "__internal/_aligned_storage.h"
#include "__internal/_iterator.h"
#include "__internal/_dequeng_iterator.h"

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
   //using difference_type = ptrdiff_t;
   using reference = value_type&;
   using const_reference = const value_type&;
   using pointer = value_type*;
   using const_pointer = const value_type*;
   using iterator = _dequeng_iterator<dequeng>;
   using const_iterator = _dequeng_iterator<const dequeng>;
   //using reverse_iterator = std::reverse_iterator<iterator>;
   //using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
   iterator begin() _sstl_noexcept_
   {
      return iterator{ this, empty() ? nullptr : _first_pointer() };
   }

   const_iterator begin() const _sstl_noexcept_
   {
      return const_iterator{ this, empty() ? nullptr : _first_pointer() };
   }

   const_iterator cbegin() const _sstl_noexcept_
   {
      return const_iterator{ this, empty() ? nullptr : _first_pointer() };
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

   bool empty() const _sstl_noexcept_
   {
      return _size() == 0;
   }

   size_type size() const _sstl_noexcept_
   {
      return _size();
   }

   size_type capacity() const _sstl_noexcept_
   {
      return _end_storage() - _begin_storage();
   }

   void clear() _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      //FIXME
      auto crt = _first_pointer();
      while(crt <= _last_pointer())
      {
         crt->~value_type();
         ++crt;
      }
      _first_pointer() = crt;
      _set_size(0);
   }

   void push_back(const_reference value)
      _sstl_noexcept(std::is_nothrow_constructible<value_type>::value)
   {
      sstl_assert(_size() < capacity());
      _increment_pointer(_last_pointer());
      new(_last_pointer()) value_type(value);
      _increment_size();
   }

   void pop_front()
      _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      sstl_assert(!empty());
      _first_pointer()->~value_type();
      _increment_pointer(_first_pointer());
      _decrement_size();
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
      auto pos = _first_pointer()-1;
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
         _last_pointer() = pos-1;
         clear();
         throw;
      }
      #endif
      _set_size(count);
      _last_pointer() = pos;
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
   void _range_constructor(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      auto src = range_begin;
      auto dst = _begin_storage()-1;
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != range_end)
         {
            sstl_assert(_size() < capacity());
            new(++dst) value_type(*src);
            _increment_size();
            ++src;
         }
         _last_pointer() = dst;
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _last_pointer() = dst-1;
         clear();
         throw;
      }
      #endif
   }

   // member functions for derived class access
   size_type _size() const _sstl_noexcept_;
   void _set_size(size_type) _sstl_noexcept_;

   pointer& _first_pointer() _sstl_noexcept_;
   const_pointer _first_pointer() const _sstl_noexcept_;

   pointer& _last_pointer() _sstl_noexcept_;
   const_pointer _last_pointer() const _sstl_noexcept_;

   pointer _begin_storage() _sstl_noexcept_;
   const_pointer _begin_storage() const _sstl_noexcept_;

   pointer _end_storage() _sstl_noexcept_;
   const_pointer _end_storage() const _sstl_noexcept_;

   // helper functions
   void _increment_size() _sstl_noexcept_
   {
      _set_size(_size()+1);
   }

   void _decrement_size() _sstl_noexcept_
   {
      _set_size(_size()-1);
   }

   void _increment_pointer(pointer& ptr) _sstl_noexcept_
   {
      auto new_ptr = ptr + 1;
      if(new_ptr == _end_storage())
         new_ptr = _begin_storage();
      ptr = new_ptr;
   }

   void _increment_pointer(const_pointer& ptr) const _sstl_noexcept_
   {
      const_cast<dequeng&>(*this)._increment_pointer(const_cast<pointer&>(ptr));
   }

   void _decrement_pointer(pointer& ptr) _sstl_noexcept_
   {
      ptr -= 1;
      if(ptr < _begin_storage())
         ptr = _end_storage() - 1;
   }

   void _decrement_pointer(const_pointer& ptr) const _sstl_noexcept_
   {
      const_cast<dequeng&>(*this)._decrement_pointer(const_cast<pointer&>(ptr));
   }
};

template<class T, size_t CAPACITY>
class dequeng : public dequeng<T>
{
   template<class, size_t>
   friend class dequeng;

private:
   using _base = dequeng<T>;
   using _type_for_derived_class_access = typename _base::_type_for_derived_class_access;

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
      : _last_pointer(_base::_begin_storage()-1)
   {}

   explicit dequeng(size_type count, const_reference value = value_type())
      _sstl_noexcept(noexcept(std::declval<_base>()._count_constructor(std::declval<size_type>(), std::declval<value_type>())))
   {
      _base::_count_constructor(count, value);
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
   dequeng(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<TIterator>(),
                                                                        std::declval<TIterator>())))
   {
      _base::_range_constructor(range_begin, range_end);
   }

   //copy construction from any instance with same value type (capacity doesn't matter)
   dequeng(const _base& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<const_iterator>(),
                                                                        std::declval<const_iterator>())))
   {
      _base::_range_constructor(const_cast<_base&>(rhs).cbegin(), const_cast<_base&>(rhs).cend());
   }

   dequeng(const dequeng& rhs)
      _sstl_noexcept(noexcept(dequeng(std::declval<const _base&>())))
      : dequeng(static_cast<const _base&>(rhs))
   {}

   dequeng(std::initializer_list<value_type> init)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<std::initializer_list<value_type>>().begin(),
                                                                        std::declval<std::initializer_list<value_type>>().end())))
   {
      _base::_range_constructor(init.begin(), init.end());
   }

private:
   size_type _size{ 0 };
   pointer _first_pointer{ _base::_begin_storage() };
   pointer _last_pointer;
   pointer _end_storage{ _base::_begin_storage() + CAPACITY };
   std::array<typename _aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type, CAPACITY> _buffer;
};

template<class T>
typename dequeng<T>::size_type dequeng<T>::_size() const _sstl_noexcept_
{
   return reinterpret_cast<const _type_for_derived_class_access&>(*this)._size;
}

template<class T>
void dequeng<T>::_set_size(typename dequeng<T>::size_type size) _sstl_noexcept_
{
   reinterpret_cast<_type_for_derived_class_access&>(*this)._size = size;
}

template<class T>
typename dequeng<T>::pointer& dequeng<T>::_first_pointer() _sstl_noexcept_
{
   return reinterpret_cast<_type_for_derived_class_access&>(*this)._first_pointer;
}

template<class T>
typename dequeng<T>::const_pointer dequeng<T>::_first_pointer() const _sstl_noexcept_
{
   return const_cast<dequeng<T>*>(this)->_first_pointer();
}

template<class T>
typename dequeng<T>::pointer& dequeng<T>::_last_pointer() _sstl_noexcept_
{
   return reinterpret_cast<_type_for_derived_class_access&>(*this)._last_pointer;
}

template<class T>
typename dequeng<T>::const_pointer dequeng<T>::_last_pointer() const _sstl_noexcept_
{
   return const_cast<dequeng<T>*>(this)->_last_pointer();
}

template<class T>
inline bool operator==(const dequeng<T>& lhs, const dequeng<T>& rhs)
{
   return lhs.size() == rhs.size() && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template<class T>
typename dequeng<T>::pointer dequeng<T>::_begin_storage() _sstl_noexcept_
{
   auto begin_storage = reinterpret_cast<_type_for_derived_class_access&>(*this)._buffer.data();
   return static_cast<pointer>(static_cast<void*>(begin_storage));
}

template<class T>
typename dequeng<T>::const_pointer dequeng<T>::_begin_storage() const _sstl_noexcept_
{
   auto begin_storage = reinterpret_cast<const _type_for_derived_class_access&>(*this)._buffer.data();
   return static_cast<const_pointer>(static_cast<const void*>(begin_storage));
}

template<class T>
typename dequeng<T>::pointer dequeng<T>::_end_storage() _sstl_noexcept_
{
   return reinterpret_cast<_type_for_derived_class_access&>(*this)._end_storage;
}

template<class T>
typename dequeng<T>::const_pointer dequeng<T>::_end_storage() const _sstl_noexcept_
{
   return reinterpret_cast<const _type_for_derived_class_access&>(*this)._end_storage;
}

}

#endif
