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
#include "__internal/_type_tag.h"
#include "__internal/_utility.h"
#include "__internal/_iterator.h"
#include "__internal/aligned_storage.h"

namespace sstl
{

template<class T>
class _vector_base
{
template<class U, size_t S>
friend class vector;

protected:
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

protected:
   void _count_constructor(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      for(size_t i=0; i<count; ++i)
         _push_back(value);
   }

   // const-qualified TIterator -> copy values
   // non const-qualified TIterator -> move values
   template<class TIterator,
            class = _enable_if_input_iterator_t<TIterator>>
   void _range_constructor(TIterator first, TIterator last)
      _sstl_noexcept((std::is_const<TIterator>::value && std::is_nothrow_copy_constructible<value_type>::value)
                     || (std::is_const<TIterator>::value && std::is_nothrow_move_constructible<value_type>::value))
   {
      using source_reference = typename std::iterator_traits<TIterator>::reference;
      std::for_each(first, last, [this](source_reference value){ this->_push_back(std::move(value)); });
   }

   void _initializer_list_constructor(std::initializer_list<value_type> init)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      std::for_each(init.begin(), init.end(), [this](const_reference value){ this->_push_back(value); });
   }

   void _destructor()
   {
      auto begin = _get_begin();
      auto end = _get_end();
      while(begin != end)
         (begin++)->~value_type();
   }

   template<bool is_copy_assignment, class TIterator>
   void _assign(TIterator rhs_begin, TIterator rhs_end)
      _sstl_noexcept((is_copy_assignment
                     &&std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
                     ||
                     (!is_copy_assignment
                     && std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_move_constructible<value_type>::value))
   {
      auto src = rhs_begin;
      auto dest = _get_begin();
      auto end = _get_end();
      while(src != rhs_end)
      {
         if(dest < end)
            *dest = _conditional_move<!is_copy_assignment>(*src);
         else
            new(dest) value_type(_conditional_move<!is_copy_assignment>(*src));
         ++dest; ++src;
      }
      auto new_end = dest;
      while(dest < end)
      {
         dest->~value_type();
         ++dest;
      }
      _set_end(new_end);
   }

   void _assign(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      auto dest = _get_begin();
      auto end = _get_end();
      while(count > 0)
      {
         if(dest < end)
            *dest = value;
         else
            new(dest) value_type(value);
         ++dest; --count;
      }
      auto new_end = dest;
      while(dest < end)
      {
         dest->~value_type();
         ++dest;
      }
      _set_end(new_end);
   }

   void _push_back(const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      auto end = _get_end();
      new(end++) value_type(value);
      _set_end(end);
   }

   void _push_back(value_type&& value)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value)
   {
      auto end = _get_end();
      new(end++) value_type(std::move(value));
      _set_end(end);
   }

   pointer _get_begin() noexcept;
   pointer _get_end() noexcept;
   void _set_end(pointer) noexcept;

protected:
   static const bool _is_copy = true;
};

template<class T, size_t Capacity>
class vector : public _vector_base<T>
{
   friend T* _vector_base<T>::_get_begin() noexcept;
   friend T* _vector_base<T>::_get_end() noexcept;
   friend void _vector_base<T>::_set_end(T*) noexcept;

private:
   using _base = _vector_base<T>;
   using _type_for_derived_member_variable_access = vector<T, 11>;

private:
   //function's signature must depend on instantiated vector type
   //in order not to generate multiple definitions
   //note: the instantiated vector type is used through a non-member function
   //(would be an incomplete type inside a member function)
   friend void _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>)
   {
      static_assert( static_cast<_base*>(static_cast<_type_for_derived_member_variable_access*>(0))
                     == static_cast<_base*>(0),
                     "base and derived vector classes must have the same address, such property"
                     " is exploited by the base class to access the derived member variables");

      static_assert( static_cast<_base*>(static_cast<vector*>(0))
                     == static_cast<_base*>(0),
                     "base and derived vector classes must have the same address, such property"
                     " is exploited by the base class to access the derived member variables");
   }

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
      : _end(begin())
   {
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
   }

   explicit vector(size_type count, const_reference value=value_type())
      _sstl_noexcept(std::is_nothrow_default_constructible<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
      : _end(begin())
   {
      sstl_assert(count <= Capacity);
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_count_constructor(count, value);
   }

   template<class TIterator,
            class = _enable_if_input_iterator_t<TIterator>,
            class TConstIterator = typename std::add_const<TIterator>::type>
   vector(TIterator first, TIterator last)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      : _end(begin())
   {
      sstl_assert(std::distance(first, last) <= Capacity);
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_range_constructor(static_cast<TConstIterator>(first), static_cast<TConstIterator>(last));
   }

   vector(const vector& rhs)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      : _end(begin())
   {
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_range_constructor(rhs.cbegin(), rhs.cend());
   }

   vector(vector&& rhs)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value)
      : _end(begin())
   {
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_range_constructor(rhs.begin(), rhs.end());
   }

   vector(std::initializer_list<value_type> init)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      : _end(begin())
   {
      sstl_assert(init.size() <= Capacity);
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_initializer_list_constructor(init);
   }

   ~vector()
   {
      _base::_destructor();
   }

   vector& operator=(const vector& rhs)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      if(this != &rhs)
      {
         _base::template _assign<!_base::_is_copy>(rhs.cbegin(), rhs.cend());
      }
      return *this;
   }

   vector& operator=(vector&& rhs)
      _sstl_noexcept(std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_move_constructible<value_type>::value)
   {
      if(this != &rhs)
         _base::template _assign<!_base::_is_copy>(rhs.begin(), rhs.end());
      return *this;
   }

   vector& operator=(std::initializer_list<value_type> init)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      _base::template _assign<_base::_is_copy>(init.begin(), init.end());
      return *this;
   }

   void assign(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      sstl_assert(count <= Capacity);
      _base::_assign(count, value);
   }

   template<class TIterator,
            class = _enable_if_input_iterator_t<TIterator>>
   void assign(TIterator first, TIterator last)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value)
   {
      sstl_assert(std::distance(first, last) <= Capacity);
      _base::template _assign<_base::_is_copy>(first, last);
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
      return *(_end-1);
   }
   const_reference back() const _sstl_noexcept_ { return const_cast<vector&>(*this).back(); }

   pointer data() _sstl_noexcept_ { return std::addressof(*begin()); }
   const_pointer data() const _sstl_noexcept_ { return const_cast<vector&>(this).data(); }


   iterator begin() _sstl_noexcept_ { return static_cast<iterator>(static_cast<void*>(_buffer)); }
   const_iterator begin() const _sstl_noexcept_ { return const_cast<vector&>(*this).begin(); }
   const_iterator cbegin() const _sstl_noexcept_ { return const_cast<vector&>(*this).begin(); }

   iterator end() _sstl_noexcept_ { return _end; }
   const_iterator end() const _sstl_noexcept_ { return _end; }
   const_iterator cend() const _sstl_noexcept_ { return _end; }

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
      new(_end++) value_type(std::forward<Args>(args)...);
   }

   void pop_back()
   {
      sstl_assert(!empty());
      (--_end)->~value_type();
   }

private:
   pointer _end;
   typename aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type _buffer[Capacity];
};

template<class T>
T* _vector_base<T>::_get_begin() noexcept
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   return reinterpret_cast<T*>(reinterpret_cast<type_for_derived_member_variable_access&>(*this)._buffer);
}

template<class T>
T* _vector_base<T>::_get_end() noexcept
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   return reinterpret_cast<type_for_derived_member_variable_access&>(*this)._end;
}

template<class T>
void _vector_base<T>::_set_end(T* value) noexcept
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   reinterpret_cast<type_for_derived_member_variable_access&>(*this)._end = value;
}

}

#endif
