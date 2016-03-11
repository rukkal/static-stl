/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_DEQUE__
#define _SSTL_DEQUE__

#include <type_traits>
#include <initializer_list>
#include <array>

#include "sstl_assert.h"
#include "__internal/_aligned_storage.h"
#include "__internal/_iterator.h"

namespace sstl
{

template<class, size_t=static_cast<size_t>(-1)>
class dequeng;

template<class T>
class dequeng<T>
{
template<class U, size_t S>
friend class dequeng; //friend declaration required for derived class' noexcept expressions

public:
   using value_type = T;
   using size_type = size_t;
   //using difference_type = ptrdiff_t;
   using reference = value_type&;
   using const_reference = const value_type&;
   using pointer = value_type*;
   using const_pointer = const value_type*;
   using iterator = value_type*;
   using const_iterator = const value_type*;
   //using reverse_iterator = std::reverse_iterator<iterator>;
   //using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
   //FIXME
   iterator begin() _sstl_noexcept_ { return _begin_pointer(); }
   const_iterator begin() const _sstl_noexcept_ { return _begin_pointer(); }
   const_iterator cbegin() const _sstl_noexcept_ { return _begin_pointer(); }
   iterator end() _sstl_noexcept_ { return _end_pointer(); }
   const_iterator end() const _sstl_noexcept_ { return _end_pointer(); }
   const_iterator cend() const _sstl_noexcept_ { return _end_pointer(); }

   void clear() _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      //FIXME
      auto crt = _end_pointer();
      while(crt != _begin_pointer())
      {
         --crt;
         crt->~value_type();
      }
      _set_end_pointer(_begin_pointer());
   }

   bool empty() const _sstl_noexcept_ { return _begin_pointer() == _end_pointer(); }

   size_t size() const _sstl_noexcept_
   {
      //FIXME
      return _end_pointer() - _begin_pointer();
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
      sstl_assert(count <=_capacity());
      auto pos = _begin_pointer();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         for(size_type i=0; i<count; ++i)
         {
            new(pos) value_type(value);
            ++pos;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end_pointer(pos);
         clear();
         throw;
      }
      #endif
      _set_end_pointer(pos);
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
   void _range_constructor(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      auto src = range_begin;
      auto dst = _begin_storage();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != range_end)
         {
            sstl_assert(dst-_begin_storage() < _capacity());
            new(dst) value_type(*src);
            ++src; ++dst;
         }
         _set_end_pointer(dst);
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end_pointer(dst);
         clear();
         throw;
      }
      #endif
   }

   // member functions for derived class access
   pointer _begin_storage() _sstl_noexcept_;
   pointer _begin_pointer() _sstl_noexcept_;
   const_pointer _begin_pointer() const _sstl_noexcept_;
   void _set_begin_pointer(pointer) _sstl_noexcept_;
   pointer _end_pointer() _sstl_noexcept_;
   const_pointer _end_pointer() const _sstl_noexcept_;
   void _set_end_pointer(pointer) _sstl_noexcept_;
   size_type _capacity() const _sstl_noexcept_;
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
      : _begin_pointer(_base::_begin_storage())
      , _end_pointer(_base::_begin_storage())
   {}

   explicit dequeng(size_type count, const_reference value = value_type())
      _sstl_noexcept(noexcept(std::declval<_base>()._count_constructor(std::declval<size_type>(), std::declval<value_type>())))
      : _begin_pointer(_base::_begin_storage())
   {
      _base::_count_constructor(count, value);
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
   dequeng(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<TIterator>(),
                                                                        std::declval<TIterator>())))
      : _begin_pointer(_base::_begin_storage())
   {
      _base::_range_constructor(range_begin, range_end);
   }

   //copy construction from any instance with same value type (capacity doesn't matter)
   dequeng(const _base& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<const_iterator>(),
                                                                        std::declval<const_iterator>())))
      : _begin_pointer(_base::_begin_storage())
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
      : _begin_pointer(_base::_begin_storage())
      , _end_pointer(_base::_begin_storage())
   {
      _base::_range_constructor(init.begin(), init.end());
   }

private:
   size_type _capacity{ CAPACITY };
   pointer _begin_pointer;
   pointer _end_pointer;
   std::array<typename _aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type, CAPACITY> _buffer;
};

template<class T>
typename dequeng<T>::pointer dequeng<T>::_begin_storage() _sstl_noexcept_
{
   auto begin_storage = reinterpret_cast<_type_for_derived_class_access&>(*this)._buffer.data();
   return static_cast<pointer>(static_cast<void*>(begin_storage));
}

template<class T>
typename dequeng<T>::pointer dequeng<T>::_begin_pointer() _sstl_noexcept_
{
   return reinterpret_cast<_type_for_derived_class_access&>(*this)._begin_pointer;
}

template<class T>
typename dequeng<T>::const_pointer dequeng<T>::_begin_pointer() const _sstl_noexcept_
{
   return const_cast<dequeng<T>*>(this)->_begin_pointer();
}

template<class T>
void dequeng<T>::_set_begin_pointer(dequeng<T>::pointer begin_pointer) _sstl_noexcept_
{
   reinterpret_cast<_type_for_derived_class_access&>(*this)._begin_pointer = begin_pointer;
}

template<class T>
typename dequeng<T>::pointer dequeng<T>::_end_pointer() _sstl_noexcept_
{
   return reinterpret_cast<_type_for_derived_class_access&>(*this)._end_pointer;
}

template<class T>
typename dequeng<T>::const_pointer dequeng<T>::_end_pointer() const _sstl_noexcept_
{
   return const_cast<dequeng<T>*>(this)->_end_pointer();
}

template<class T>
void dequeng<T>::_set_end_pointer(dequeng<T>::pointer end_pointer) _sstl_noexcept_
{
   reinterpret_cast<_type_for_derived_class_access&>(*this)._end_pointer = end_pointer;
}

template<class T>
typename dequeng<T>::size_type dequeng<T>::_capacity() const _sstl_noexcept_
{
   return reinterpret_cast<const _type_for_derived_class_access&>(*this)._capacity;
}

template<class T>
inline bool operator==(const dequeng<T>& lhs, const dequeng<T>& rhs)
{
   return lhs.size() == rhs.size() && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

}

#endif
