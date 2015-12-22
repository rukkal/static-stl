/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_VECTOR__
#define _SSTL_VECTOR__

#include <cstddef>
#include <utility>
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <algorithm>
#include <array>

#include "sstl_assert.h"
#include "__internal/noexcept.h"
#include "__internal/_utility.h"
#include "__internal/_iterator.h"
#include "__internal/aligned_storage.h"

namespace sstl
{

template<class T>
class _vector_base
{
public:
   using value_type = T;
   using size_type = size_t;
   using difference_type = ptrdiff_t;
   using reference = value_type&;
   using const_reference = const value_type&;
   using pointer = value_type*;
   using const_pointer = const value_type*;
   using iterator = value_type*;
   using const_iterator = const value_type*;
   using reverse_iterator = std::reverse_iterator<iterator>;
   using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
   _vector_base(pointer begin) _sstl_noexcept_
      : _end(begin)
   {}

   _vector_base(pointer begin, size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      : _end(begin)
   {
      for(size_t i=0; i<count; ++i)
         _push_back(value);
   }

   // const-qualified TIterator -> copy values
   // non const-qualified TIterator -> move values
   template<class TIterator,
            class = _enable_if_input_iterator_t<TIterator>>
   _vector_base(pointer begin, TIterator first, TIterator last)
      _sstl_noexcept((std::is_const<TIterator>::value && std::is_nothrow_copy_constructible<value_type>::value)
                     || (std::is_const<TIterator>::value && std::is_nothrow_move_constructible<value_type>::value))
      : _end(begin)
   {
      using source_reference = typename std::iterator_traits<TIterator>::reference;
      std::for_each(first, last, [this](source_reference value){ this->_push_back(std::move(value)); });
   }

   _vector_base(pointer begin, std::initializer_list<value_type> init)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      : _end(begin)
   {
      std::for_each(init.begin(), init.end(), [this](const_reference value){ this->_push_back(value); });
   }

   void _destroy(pointer begin)
   {
      while(begin != _end)
         (begin++)->~value_type();
   }

   template<bool is_copy_assignment, class TIterator>
   void _assign(pointer begin, TIterator rhs_begin, TIterator rhs_end)
      _sstl_noexcept((is_copy_assignment
                     &&std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
                     ||
                     (!is_copy_assignment
                     && std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_move_constructible<value_type>::value))
   {
      auto src = rhs_begin;
      auto dest = begin;
      while(src != rhs_end)
      {
         if(dest < _end)
            *dest = _conditional_move<!is_copy_assignment>(*src);
         else
            new(dest) value_type(_conditional_move<!is_copy_assignment>(*src));
         ++dest; ++src;
      }
      auto new_end = dest;
      while(dest < _end)
      {
         dest->~value_type();
         ++dest;
      }
      _end = new_end;
   }

   void _assign(pointer begin, size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      auto dest = begin;
      while(count > 0)
      {
         if(dest < _end)
            *dest = value;
         else
            new(dest) value_type(value);
         ++dest; --count;
      }
      auto new_end = dest;
      while(dest < _end)
      {
         dest->~value_type();
         ++dest;
      }
      _end = new_end;
   }

   void _push_back(const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      new(_end++) value_type(value);
   }

   void _push_back(value_type&& value)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value)
   {
      new(_end++) value_type(std::move(value));
   }

public:
   static const bool _is_copy = true;
   pointer _end;
};

template<class T, size_t Capacity>
class vector : private _vector_base<T>
{
private:
   using _base = _vector_base<T>;
public:
   using value_type = typename _base::value_type;
   using size_type = typename _base::size_type;
   using difference_type = typename _base::difference_type;
   using reference = typename _base::reference;
   using const_reference = typename _base::const_reference;
   using pointer = typename _base::pointer;
   using const_pointer = typename _base::const_pointer;
   using iterator = typename _base::iterator;
   using const_iterator = typename _base::const_iterator;
   using reverse_iterator = typename _base::reverse_iterator;
   using const_reverse_iterator = typename _base::const_reverse_iterator;

public:
   vector() _sstl_noexcept_
      : _base(begin())
   {}

   explicit vector(size_type count, const_reference value=value_type())
      _sstl_noexcept(std::is_nothrow_default_constructible<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
      : _base(begin(), count, value)
   {
      sstl_assert(count <= Capacity);
   }

   template<class TIterator,
            class = _enable_if_input_iterator_t<TIterator>,
            class TConstIterator = typename std::add_const<TIterator>::type>
   vector(TIterator first, TIterator last)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      : _base(begin(), static_cast<TConstIterator>(first), static_cast<TConstIterator>(last))
   {
      sstl_assert(std::distance(first, last) <= Capacity);
   }

   vector(const vector& rhs)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      : _base(begin(), rhs.cbegin(), rhs.cend())
   {}

   vector(vector&& rhs)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value)
      : _base(begin(), rhs.begin(), rhs.end())
   {}

   vector(std::initializer_list<value_type> init)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      : _base(begin(), init)
   {
      sstl_assert(init.size() <= Capacity);
   }

   ~vector()
   {
      _base::_destroy(begin());
   }

   vector& operator=(const vector& rhs)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      if(this != &rhs)
      {
         _base::template _assign<!_base::_is_copy>(begin(), rhs.cbegin(), rhs.cend());
      }
      return *this;
   }

   vector& operator=(vector&& rhs)
      _sstl_noexcept(std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_move_constructible<value_type>::value)
   {
      if(this != &rhs)
         _base::template _assign<!_base::_is_copy>(begin(), rhs.begin(), rhs.end());
      return *this;
   }

   vector& operator=(std::initializer_list<value_type> init)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      _base::template _assign<_base::_is_copy>(begin(), init.begin(), init.end());
      return *this;
   }

   void assign(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      sstl_assert(count <= Capacity);
      _base::_assign(begin(), count, value);
   }

   template<class TIterator,
            class = _enable_if_input_iterator_t<TIterator>>
   void assign(TIterator first, TIterator last)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      sstl_assert(std::distance(first, last) <= Capacity);
      _base::template _assign<_base::_is_copy>(begin(), first, last);
   }

   reference at(size_type idx) _sstl_noexcept_
   {
      auto pos = begin() + idx;
      sstl_assert(pos < end());
      return *pos;
   }
   const_reference at(size_type idx) const _sstl_noexcept_
   {
      return const_cast<vector&>(*this).at(idx);
   }

   reference operator[](size_type idx) _sstl_noexcept_
   {
      return at(idx);
   }
   const_reference operator[](size_type idx) const _sstl_noexcept_
   {
      return const_cast<vector&>(*this).at(idx);
   }

   reference front() _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *begin();
   }
   const_reference front() const _sstl_noexcept_ { return const_cast<vector&>(*this).front(); }

   reference back() _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *(_base::_end-1);
   }
   const_reference back() const _sstl_noexcept_ { return const_cast<vector&>(*this).back(); }

   pointer data() _sstl_noexcept_ { return std::addressof(*begin()); }
   const_pointer data() const _sstl_noexcept_ { return const_cast<vector&>(this).data(); }


   iterator begin() _sstl_noexcept_ { return static_cast<iterator>(static_cast<void*>(_buffer.data())); }
   const_iterator begin() const _sstl_noexcept_ { return const_cast<vector&>(*this).begin(); }
   const_iterator cbegin() const _sstl_noexcept_ { return const_cast<vector&>(*this).begin(); }

   iterator end() _sstl_noexcept_ { return _base::_end; }
   const_iterator end() const _sstl_noexcept_ { return _base::_end; }
   const_iterator cend() const _sstl_noexcept_ { return _base::_end; }

   reverse_iterator rbegin() _sstl_noexcept_ { return reverse_iterator( end() ); }
   const_reverse_iterator rbegin() const _sstl_noexcept_ { return const_cast<vector&>(*this).rbegin(); }
   const_reverse_iterator crbegin() const _sstl_noexcept_ { return const_cast<vector&>(*this).rbegin(); }

   reverse_iterator rend() _sstl_noexcept_ { return reverse_iterator( begin() ); }
   const_reverse_iterator rend() const _sstl_noexcept_ { return const_cast<vector&>(*this).rend(); }
   const_reverse_iterator crend() const _sstl_noexcept_ { return const_cast<vector&>(*this).rend(); }

   bool empty() const _sstl_noexcept_ { return cbegin()==cend(); }
   size_type size() const _sstl_noexcept_ { return std::distance(cbegin(), cend()); }
   size_type max_size() const _sstl_noexcept_ { return Capacity; }
   size_type capacity() const _sstl_noexcept_ { return Capacity; }




   void push_back(const_reference value) _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      sstl_assert(size() < Capacity);
      _push_back(value);
   }

   void push_back(value_type&& value) _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value)
   {
      sstl_assert(size() < Capacity);
      _base::_push_back(std::move(value));
   }

   template<class... Args>
   void emplace_back(Args&&... args)
      _sstl_noexcept(std::is_nothrow_constructible<value_type, typename std::add_rvalue_reference<Args>::type...>::value)
   {
      sstl_assert(size() < Capacity);
      new(_base::_end++) value_type(std::forward<Args>(args)...);
   }

   void pop_back()
   {
      sstl_assert(!empty());
      (--_base::_end)->~value_type();
   }

private:
   std::array<typename aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type, Capacity> _buffer;
};

}

#endif
