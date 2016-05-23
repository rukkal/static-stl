/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_BITMAP_ALLOCATOR__
#define _SSTL_BITMAP_ALLOCATOR__

#include <type_traits>
#include <cstdint>
#include <array>

#include <sstl_assert.h>

#include "__internal/_except.h"
#include "__internal/_aligned_storage.h"
#include "__internal/bitset_span.h"
#include "__internal/_hacky_derived_class_access.h"

namespace sstl
{
template<class T, size_t CAPACITY=static_cast<size_t>(-1)>
class bitmap_allocator;

template<class T>
class bitmap_allocator<T>
{
public:
   using value_type = T;
   using pointer = T*;
   using const_pointer = const T*;
   using reference = T&;
   using const_reference = const T&;
   using size_type = size_t;

public:
   T* allocate() _sstl_noexcept_
   {
      auto bitmap = bitset_span(_derived()._bitmap_data.data(), _derived()._capacity);
      sstl_assert(!bitmap.all());
      auto free_block_idx = get_next_free_block_idx();
      bitmap.set(free_block_idx);
      return &_derived()._pool[free_block_idx];
   }

   void deallocate(void* p) _sstl_noexcept_
   {
      auto bitmap = bitset_span(_derived()._bitmap_data.data(), _derived()._capacity);
      pointer block = static_cast<pointer>(p);
      sstl_assert(block>=_derived()._pool && block<_derived()._pool+bitmap.size());
      auto idx = block - _derived()._pool;
      sstl_assert(bitmap.test(idx));
      bitmap.reset(idx);
      _derived()._last_allocated_block_idx = idx-1;
   }

protected:
   using _type_for_derived_class_access = bitmap_allocator<T, 11>;

   bitmap_allocator() _sstl_noexcept_ = default;
   bitmap_allocator(const bitmap_allocator&) _sstl_noexcept_ = default;
   bitmap_allocator(bitmap_allocator&&) _sstl_noexcept_ {} //MSVC (VS2013) does not support default move special member functions
   bitmap_allocator& operator=(const bitmap_allocator&) _sstl_noexcept_ = default;
   bitmap_allocator& operator=(bitmap_allocator&&) _sstl_noexcept_ {}; //MSVC (VS2013) does not support default move special member functions
   ~bitmap_allocator() = default;

private:
   _type_for_derived_class_access& _derived() _sstl_noexcept_;
   const _type_for_derived_class_access& _derived() const _sstl_noexcept_;

   size_t get_next_free_block_idx() _sstl_noexcept_
   {
      auto bitmap = bitset_span(_derived()._bitmap_data.data(), _derived()._capacity);
      auto idx = _derived()._last_allocated_block_idx;
      while(true)
      {
         if(++idx >= bitmap.size())
         {
            idx = 0;
         }
         if(!bitmap.test(idx))
            break;
      }
      return idx;
   }
};

// An allocator that uses a bitmap to keep track of the allocated blocks
template <class T, size_t CAPACITY>
class bitmap_allocator : public bitmap_allocator<T>
{
   template<class, size_t> friend class bitmap_allocator;
   
private:
   using _base = bitmap_allocator<T>;
   using _type_for_derived_class_access = typename _base::_type_for_derived_class_access;

public:
   using value_type = typename bitmap_allocator<T>::value_type;
   using pointer = typename bitmap_allocator<T>::pointer;
   using const_pointer = typename bitmap_allocator<T>::const_pointer;
   using reference = typename bitmap_allocator<T>::reference;
   using const_reference = typename bitmap_allocator<T>::const_reference;
   using size_type = typename bitmap_allocator<T>::size_type;

public:
   bitmap_allocator() _sstl_noexcept_
   {
      _assert_hacky_derived_class_access_is_valid<bitmap_allocator<value_type>, bitmap_allocator, _type_for_derived_class_access>();
      _bitmap_data.fill(0);
   }

private:
   const size_type _capacity{ CAPACITY };
   size_type _last_allocated_block_idx{ static_cast<size_type>(-1) };
   pointer _pool{ static_cast<pointer>(static_cast<void*>(_pool_data)) };
   std::array<std::uint8_t, (CAPACITY-1) / 8 + 1> _bitmap_data;
   typename _aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type _pool_data[CAPACITY];
};

template<class T>
typename bitmap_allocator<T>::_type_for_derived_class_access& bitmap_allocator<T>::_derived() _sstl_noexcept_
{
   return reinterpret_cast<_type_for_derived_class_access&>(*this);
}

template<class T>
const typename bitmap_allocator<T>::_type_for_derived_class_access& bitmap_allocator<T>::_derived() const _sstl_noexcept_
{
   return reinterpret_cast<const _type_for_derived_class_access&>(*this);
}

}

#endif
