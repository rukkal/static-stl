/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_FREELIST_ALLOCATOR__
#define _SSTL_FREELIST_ALLOCATOR__

#include <type_traits>
#include <cstdint>
#include <array>

#include <sstl_assert.h>

#include "__internal/_except.h"
#include "__internal/_aligned_storage.h"
#include "__internal/_hacky_derived_class_access.h"

namespace sstl
{
template<class T, size_t CAPACITY=static_cast<size_t>(-1)>
class freelist_allocator;

template<class T>
class freelist_allocator<T>
{
public:
   using value_type = T;
   using pointer = T*;
   using const_pointer = const T*;
   using reference = T&;
   using const_reference = const T&;
   using size_type = size_t;

public:
   pointer allocate() _sstl_noexcept_
   {
      sstl_assert(_derived()._next_free != nullptr);
      auto new_next_free = *reinterpret_cast<void**>(_derived()._next_free);
      auto ret = static_cast<pointer>(_derived()._next_free);
      _derived()._next_free = new_next_free;
      return ret;
   }

   void deallocate(pointer p) _sstl_noexcept_
   {
      *reinterpret_cast<void**>(p) = _derived()._next_free;
      _derived()._next_free = p;
   }

   bool full() const
   {
      return _derived()._next_free == nullptr;
   }

protected:
   using _type_for_derived_class_access = freelist_allocator<T, 11>;

   freelist_allocator() _sstl_noexcept_ = default;
   freelist_allocator(const freelist_allocator&) _sstl_noexcept_ = default;
   freelist_allocator(freelist_allocator&&) _sstl_noexcept_{} //MSVC (VS2013) does not support default move special member functions
   freelist_allocator& operator=(const freelist_allocator&)_sstl_noexcept_ = default;
   freelist_allocator& operator=(freelist_allocator&&)_sstl_noexcept_{}; //MSVC (VS2013) does not support default move special member functions
   ~freelist_allocator() = default;

   void _initialize_pool(size_type capacity) _sstl_noexcept_
   {
      auto crt = _derived()._pool;
      auto last = _derived()._pool+capacity-1;
      while(crt != last)
      {
         *reinterpret_cast<void**>(crt) = static_cast<void*>(crt+1);
         ++crt;
      }
      *reinterpret_cast<void**>(last) = nullptr;
   }

private:
   _type_for_derived_class_access& _derived() _sstl_noexcept_;
   const _type_for_derived_class_access& _derived() const _sstl_noexcept_;
};

template <class T, size_t CAPACITY>
class freelist_allocator : public freelist_allocator<T>
{
   template<class, size_t> friend class freelist_allocator;
   
private:
   using _base = freelist_allocator<T>;
   using _type_for_derived_class_access = typename _base::_type_for_derived_class_access;

public:
   using value_type = typename freelist_allocator<T>::value_type;
   using pointer = typename freelist_allocator<T>::pointer;
   using const_pointer = typename freelist_allocator<T>::const_pointer;
   using reference = typename freelist_allocator<T>::reference;
   using const_reference = typename freelist_allocator<T>::const_reference;
   using size_type = typename freelist_allocator<T>::size_type;

public:
   freelist_allocator() _sstl_noexcept_
   {
      _assert_hacky_derived_class_access_is_valid<freelist_allocator<value_type>, freelist_allocator, _type_for_derived_class_access>();
      _base::_initialize_pool(CAPACITY);
   }

private:
   static const size_type _pool_block_size =
      _metaprog::max<sizeof(void*), sizeof(value_type)>::value;
   static const size_type _pool_block_align =
      _metaprog::max<std::alignment_of<void*>::value, std::alignment_of<value_type>::value>::value;

private:
   void* _next_free{ _pool };
   typename _aligned_storage<_pool_block_size, _pool_block_align>::type _pool[CAPACITY];
};

template<class T>
typename freelist_allocator<T>::_type_for_derived_class_access& freelist_allocator<T>::_derived() _sstl_noexcept_
{
   return reinterpret_cast<_type_for_derived_class_access&>(*this);
}

template<class T>
const typename freelist_allocator<T>::_type_for_derived_class_access& freelist_allocator<T>::_derived() const _sstl_noexcept_
{
   return reinterpret_cast<const _type_for_derived_class_access&>(*this);
}

}

#endif
