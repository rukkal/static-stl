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
#include <stdexcept>

#include "sstl_assert.h"
#include "__internal/_except.h"
#include "__internal/_type_tag.h"
#include "__internal/_utility.h"
#include "__internal/_iterator.h"
#include "__internal/aligned_storage.h"
#include "__internal/_debug.h"

namespace sstl
{

template<class T>
class _vector_base
{
template<class U, size_t S>
friend class vector; //friend declaration required for vector's noexcept expressions

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
   static const bool _is_copy = true;

protected:
   void _count_constructor(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      #if _sstl_has_exceptions()
      try
      {
      #endif
         for(size_t i=0; i<count; ++i)
            _emplace_back(value);
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _clear();
         throw;
      }
      #endif
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
   void _range_constructor(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      auto src = range_begin;
      auto dst = _begin();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != range_end)
         {
            new(dst) value_type(*src);
            ++src; ++dst;
         }
         _set_end(dst);
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end(dst);
         _clear();
         throw;
      }
      #endif
   }

   void _move_constructor(_vector_base&& rhs)
      _sstl_noexcept((std::is_nothrow_move_constructible<value_type>::value
                     || std::is_nothrow_copy_constructible<value_type>::value)
                     && std::is_nothrow_destructible<value_type>::value)
   {
      auto src = rhs._begin();
      auto dst = _begin();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != rhs._end())
         {
            new(dst) value_type(_move_construct_if_noexcept(*src));
            #if !_sstl_has_exceptions()
            src->~value_type();
            #endif
            ++src; ++dst;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end(dst);
         _clear();
         throw;
      }
      src = rhs._begin();
      while(src != rhs._end())
      {
         src->~value_type();
         ++src;
      }
      #endif
      _set_end(dst);
      rhs._set_end(rhs._begin());
   }

   void _destructor() _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      auto begin = _begin();
      auto end = _end();
      while(begin != end)
         (begin++)->~value_type();
   }

   template<class TIterator>
   void _copy_assign(TIterator rhs_begin, TIterator rhs_end)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value
                     && std::is_nothrow_destructible<value_type>::value)
   {
      auto src = rhs_begin;
      auto dest = _begin();
      auto end = _end();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != rhs_end)
         {
            if(dest < end)
               *dest = *src;
            else
               new(dest) value_type(*src);
            ++dest; ++src;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end(std::max(dest, _end()));
         _clear();
         throw;
      }
      #endif
      _set_end(dest);
      while(dest < end)
      {
         dest->~value_type();
         ++dest;
      }
   }

   template<class TIterator>
   void _move_assign(TIterator rhs_begin, TIterator rhs_end)
      _sstl_noexcept((std::is_nothrow_move_assignable<value_type>::value || std::is_nothrow_copy_assignable<value_type>::value)
                     && (std::is_nothrow_move_constructible<value_type>::value || std::is_nothrow_copy_constructible<value_type>::value)
                     && std::is_nothrow_destructible<value_type>::value)
   {
      auto src = rhs_begin;
      auto dest = _begin();
      auto end = _end();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != rhs_end)
         {
            if(dest < end)
               *dest = _move_assign_if_noexcept(*src);
            else
               new(dest) value_type(_move_assign_if_noexcept(*src));
            #if !_sstl_has_exceptions()
            src->~value_type();
            #endif
            ++dest; ++src;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end(std::max(dest, _end()));
         _clear();
         throw;
      }
      #endif

      _set_end(dest);
      while(dest < end)
      {
         dest->~value_type();
         ++dest;
      }

      #if _sstl_has_exceptions()
      while(src-- > rhs_begin)
      {
         src->~value_type();
      }
      #endif
   }

   void _count_assign(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value
                     && std::is_nothrow_destructible<value_type>::value)
   {
      auto dest = _begin();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(count > 0)
         {
            if(dest < _end())
               *dest = value;
            else
               new(dest) value_type(value);
            ++dest; --count;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end(std::max(dest, _end()));
         _clear();
         throw;
      }
      #endif
      auto new_end = dest;
      while(dest < _end())
      {
         dest->~value_type();
         ++dest;
      }
      _set_end(new_end);
   }

   reference _at(size_type idx) _sstl_noexcept_if_doesnt_have_exceptions_
   {
      #if _sstl_has_exceptions()
      if(idx >= _size())
      {
         throw std::out_of_range(_sstl_debug_message("vector access out of range"));
      }
      #endif
      auto pos = _begin() + idx;
      sstl_assert(pos < _end());
      return *pos;
   }

   reference _subscript_operator(size_type idx) _sstl_noexcept_
   {
      auto pos = _begin() + idx;
      sstl_assert(pos < _end());
      return *pos;
   }

   reference _front() _sstl_noexcept_
   {
      sstl_assert(!_empty());
      return *_begin();
   }
   reference _back() _sstl_noexcept_
   {
      sstl_assert(!_empty());
      return *(_end()-1);
   }

   pointer _begin() _sstl_noexcept_;
   pointer _end() _sstl_noexcept_;
   void _set_end(pointer) _sstl_noexcept_;
   reverse_iterator _rbegin() _sstl_noexcept(std::is_nothrow_constructible<reverse_iterator, iterator>::value)
   {
      return reverse_iterator(_end());
   }
   reverse_iterator _rend() _sstl_noexcept(std::is_nothrow_constructible<reverse_iterator, iterator>::value)
   {
      return reverse_iterator(_begin());
   }

   bool _empty() const _sstl_noexcept_
   {
      return const_cast<_vector_base&>(*this)._begin() == const_cast<_vector_base&>(*this)._end();
   }

   size_type _size() const _sstl_noexcept_
   {
      return std::distance(const_cast<_vector_base&>(*this)._begin(),
                           const_cast<_vector_base&>(*this)._end());
   }

   void _clear() _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      auto begin = _begin();
      auto pos = begin;
      auto end = _end();
      while(pos != end)
         (pos++)->~value_type();
      _set_end(begin);
   }

   template<bool is_copy_insertion>
   iterator _insert(iterator pos, reference value)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value
                     && (!is_copy_insertion || std::is_nothrow_copy_constructible<value_type>::value))
   {
      auto end = _end();
      if(pos != end)
      {
         auto last = end-1;
         #if _sstl_has_exceptions()
         try
         {
         #endif
            new(end) value_type(std::move(*last));
         #if _sstl_has_exceptions()
         }
         catch(...)
         {
            _clear();
            throw;
         }

         try
         {
         #endif
            std::move_backward(pos, last, end);
            *pos = _conditional_move<!is_copy_insertion>(value);
         #if _sstl_has_exceptions()
         }
         catch(...)
         {
            _set_end(end+1);
            _clear();
            throw;
         }
         #endif
      }
      else
      {
         new(end) value_type(_conditional_move<!is_copy_insertion>(value));
      }
      _set_end(end+1);
      return pos;
   }

   iterator _insert(iterator pos, size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value
                     && std::is_nothrow_copy_assignable<value_type>::value)
   {
      auto end = _end();
      auto new_end = end + count;
      auto src = end - 1;
      auto dst = new_end - 1;

      #if _sstl_has_exceptions()
      try
      {
      #endif
         auto end_src_move_construction = std::max(pos-1, end-count-1);
         while(src > end_src_move_construction)
         {
            new(dst) value_type(std::move(*src));
            --src; --dst;
         }

         auto end_src_move_assignment = pos - 1;
         while(src > end_src_move_assignment)
         {
            *dst = std::move(*src);
            --src; --dst;
         }

         auto end_dst_copy_construction = end - 1;
         while(dst > end_dst_copy_construction)
         {
            new(dst) value_type(value);
            --dst;
         }

         auto end_dst_copy_assignment = pos - 1;
         while(dst > end_dst_copy_assignment)
         {
            *dst = value;
            --dst;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         for(auto p=new_end-1; p>dst; --p)
            p->~value_type();
         if(pos != end)
         {
            _set_end(std::min(end, dst+1));
            _clear();
         }
         throw;
      }
      #endif
      _set_end(new_end);

      return pos;
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
   iterator _insert(iterator pos, TIterator range_begin, TIterator range_end)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value
                     && noexcept(value_type(*std::declval<TIterator&>()))
                     && noexcept(std::declval<value_type&>() = *std::declval<TIterator&>()))
   {
      auto count = std::distance(range_begin, range_end);
      auto end = _end();
      auto new_end = end + count;
      auto src_range = range_end - 1;
      auto src_vector = end - 1;
      auto dst_vector = new_end - 1;

      #if _sstl_has_exceptions()
      try
      {
      #endif
         auto end_src_move_construction = std::max(pos-1, end-count-1);
         while(src_vector > end_src_move_construction)
         {
            new(dst_vector) value_type(std::move(*src_vector));
            --src_vector; --dst_vector;
         }

         auto end_src_move_assignment = pos - 1;
         while(src_vector > end_src_move_assignment)
         {
            *dst_vector = std::move(*src_vector);
            --src_vector; --dst_vector;
         }

         auto end_dst_construction = end - 1;
         while(dst_vector > end_dst_construction)
         {
            new(dst_vector) value_type(*src_range);
            --dst_vector; --src_range;
         }

         auto end_dst_assignment = pos - 1;
         while(dst_vector > end_dst_assignment)
         {
            *dst_vector = *src_range;
            --dst_vector; --src_range;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         for(auto p=new_end-1; p>dst_vector; --p)
            p->~value_type();
         if(pos != end)
         {
            _set_end(std::min(end, dst_vector+1));
            _clear();
         }
         throw;
      }
      #endif
      _set_end(new_end);

      return pos;
   }

   template<class... Args>
   iterator _emplace(iterator pos, Args&&... args)
      _sstl_noexcept(std::is_nothrow_constructible<value_type, typename std::add_rvalue_reference<Args>::type...>::value
                     && noexcept(std::declval<_vector_base>().template _insert<!_is_copy>(std::declval<iterator>(),
                                                                                 std::declval<value_type&>())))
   {
      value_type value(std::forward<Args>(args)...);
      return _insert<!_is_copy>(pos, value);
   }

   iterator _erase(iterator pos) _sstl_noexcept(std::is_nothrow_move_assignable<value_type>::value
                                                && std::is_nothrow_destructible<value_type>::value)
   {
      auto end = _end();
      auto current = pos;
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(current+1 != end)
         {
            *current = std::move(*(current+1));
            ++current;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _clear();
         throw;
      }
      #endif
      current->~value_type();
      _set_end(current);
      return pos;
   }

   iterator _erase(iterator range_begin, iterator range_end)
      _sstl_noexcept(std::is_nothrow_move_assignable<value_type>::value && std::is_nothrow_destructible<value_type>::value)
   {
      auto end = _end();
      auto dst = range_begin;
      auto src = range_end;

      while(src != end)
      {
         *dst = std::move(*src);
         ++src; ++dst;
      }
      auto new_end = dst;

      while(dst != end)
      {
         dst->~value_type();
         ++dst;
      }
      _set_end(new_end);
      return range_begin;
   }

   template<class... Args>
   void _emplace_back(Args&&... args)
      _sstl_noexcept(std::is_nothrow_constructible<value_type, typename std::add_rvalue_reference<Args>::type...>::value)
   {
      auto end = _end();
      new(end) value_type(std::forward<Args>(args)...);
      _set_end(end+1);
   }

   void _pop_back() _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      sstl_assert(!_empty());
      auto end = _end();
      (--end)->~value_type();
      _set_end(end);
   }

   void _swap(_vector_base& rhs)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_destructible<value_type>::value)
   {
      _vector_base *large, *small;

      if(_size() < rhs._size())
      {
         large = &rhs;
         small = this;
      }
      else
      {
         large = this;
         small = &rhs;
      }

      auto large_pos = large->_begin();
      auto large_end_swaps = large->_begin() + small->_size();
      auto large_end = large->_end();
      auto small_pos = small->_begin();

      while(large_pos != large_end_swaps)
      {
         std::iter_swap(large_pos, small_pos);
         ++large_pos; ++small_pos;
      }
      large->_set_end(large_pos);

      while(large_pos != large_end)
      {
         new(small_pos) value_type(std::move(*large_pos));
         large_pos->~value_type();
         ++large_pos; ++small_pos;
      }
      small->_set_end(small_pos);
   }
};

template<class T, size_t Capacity>
class vector : public _vector_base<T>
{
   friend T* _vector_base<T>::_begin() _sstl_noexcept_;
   friend T* _vector_base<T>::_end() _sstl_noexcept_;
   friend void _vector_base<T>::_set_end(T*) _sstl_noexcept_;

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
      #if IS_MSVC()
      // MSVC can't compute derived-to-base pointer conversion at compile-time
      sstl_assert(static_cast<_base*>(static_cast<_type_for_derived_member_variable_access*>(0)) == static_cast<_base*>(0));
      sstl_assert(static_cast<_base*>(static_cast<vector*>(0)) == static_cast<_base*>(0));
      #else
      static_assert( static_cast<_base*>(static_cast<_type_for_derived_member_variable_access*>(0))
                     == static_cast<_base*>(0),
                     "base and derived vector classes must have the same address, such property"
                     " is exploited by the base class to access the derived member variables");

      static_assert( static_cast<_base*>(static_cast<vector*>(0))
                     == static_cast<_base*>(0),
                     "base and derived vector classes must have the same address, such property"
                     " is exploited by the base class to access the derived member variables");
      #endif
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
      : _end_(begin())
   {
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
   }

   explicit vector(size_type count, const_reference value=value_type())
      _sstl_noexcept(noexcept(std::declval<_base>()._count_constructor(std::declval<size_type>(), std::declval<const_reference>())))
      : _end_(begin())
   {
      sstl_assert(count <= Capacity);
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_count_constructor(count, value);
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
   vector(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<TIterator>(),
                                                                        std::declval<TIterator>())))
   {
      sstl_assert(std::distance(range_begin, range_end) <= Capacity);
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_range_constructor(range_begin, range_end);
   }

   //copy construction from any vector with same value type (capacity doesn't matter)
   vector(const _base& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<const_iterator>(),
                                                                        std::declval<const_iterator>())))
   {
      sstl_assert(rhs._size() <= Capacity);
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_range_constructor(const_cast<_base&>(rhs)._begin(), const_cast<_base&>(rhs)._end());
   }

   vector(const vector& rhs)
      _sstl_noexcept(noexcept(vector(std::declval<const _base&>())))
      : vector(static_cast<const _base&>(rhs))
   {}

   //move construction from any vector with same value type (capacity doesn't matter)
   vector(_base&& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._move_constructor(std::declval<_base>())))
   {
      sstl_assert(rhs._size() <= Capacity);
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_move_constructor(std::move(rhs));
   }

   vector(vector&& rhs)
      _sstl_noexcept(noexcept(vector(std::declval<_base>())))
      : vector(static_cast<_base&&>(rhs))
   {}

   vector(std::initializer_list<value_type> init)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor(
         std::declval<std::initializer_list<value_type>>().begin(),
         std::declval<std::initializer_list<value_type>>().end())))
   {
      sstl_assert(init.size() <= Capacity);
      _assert_vector_derived_member_variable_access_is_valid(_type_tag<vector>{});
      _base::_range_constructor(init.begin(), init.end());
   }

   ~vector() _sstl_noexcept(noexcept(std::declval<_base>()._destructor()))
   {
      _base::_destructor();
   }

   //copy assignment from vectors with same value type (capacity doesn't matter)
   vector& operator=(const _base& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._copy_assign( std::declval<iterator>(),
                                                                  std::declval<iterator>())))
   {
      if(this != &rhs)
      {
         sstl_assert(rhs._size() <= Capacity);
         _base::_copy_assign(const_cast<_base&>(rhs)._begin(), const_cast<_base&>(rhs)._end());
      }
      return *this;
   }

   vector& operator=(const vector& rhs)
      _sstl_noexcept(noexcept(std::declval<vector>().operator=(std::declval<const _base&>())))
   {
      return operator=(static_cast<const _base&>(rhs));
   }

   //move assignment from vectors with same value type (capacity doesn't matter)
   vector& operator=(_base&& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._move_assign( std::declval<iterator>(),
                                                                  std::declval<iterator>())))
   {
      if(this != &rhs)
      {
         sstl_assert(rhs._size() <= Capacity);
         _base::_move_assign(rhs._begin(), rhs._end());
         rhs._set_end(rhs._begin());
      }
      return *this;
   }

   vector& operator=(vector&& rhs)
      _sstl_noexcept(noexcept(std::declval<vector>().operator=(std::declval<_base>())))
   {
      return operator=(static_cast<_base&&>(rhs));
   }

   vector& operator=(std::initializer_list<value_type> init)
      _sstl_noexcept(noexcept(std::declval<_base>()._copy_assign(
            std::declval<std::initializer_list<value_type>>().begin(),
            std::declval<std::initializer_list<value_type>>().end())))
   {
      _base::_copy_assign(init.begin(), init.end());
      return *this;
   }

   void assign(size_type count, const_reference value)
      _sstl_noexcept(noexcept(std::declval<_base>()._count_assign(std::declval<size_type>(), std::declval<const_reference>())))
   {
      sstl_assert(count <= Capacity);
      _base::_count_assign(count, value);
   }

   template<class TIterator,
            class = _enable_if_input_iterator_t<TIterator>>
   void assign(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(noexcept(std::declval<_base>()._copy_assign( std::declval<TIterator>(),
                                                                  std::declval<TIterator>())))
   {
      sstl_assert(std::distance(range_begin, range_end) <= Capacity);
      _base::_copy_assign(range_begin, range_end);
   }

   reference at(size_type idx)
      _sstl_noexcept(noexcept(std::declval<_base>()._at(size_type{})))
   {
      return _base::_at(idx);
   }
   const_reference at(size_type idx) const
      _sstl_noexcept(noexcept(std::declval<_base>()._at(size_type{})))
   {
      return const_cast<vector&>(*this)._base::_at(idx);
   }

   reference operator[](size_type idx)
      _sstl_noexcept(noexcept(std::declval<_base>()._subscript_operator(size_type{})))
   {
      return _base::_subscript_operator(idx);
   }
   const_reference operator[](size_type idx) const
      _sstl_noexcept(noexcept(std::declval<_base>()._subscript_operator(size_type{})))
   {
      return const_cast<vector&>(*this)._base::_subscript_operator(idx);
   }

   reference front() _sstl_noexcept_
   {
      return _base::_front();
   }
   const_reference front() const _sstl_noexcept_
   {
      return const_cast<vector&>(*this)._base::_front();
   }

   reference back() _sstl_noexcept_
   {
      return _base::_back();
   }
   const_reference back() const _sstl_noexcept_
   {
      return const_cast<vector&>(*this)._base::_back();
   }

   pointer data() _sstl_noexcept_ { return _base::_begin(); }
   const_pointer data() const _sstl_noexcept_ { return const_cast<vector&>(this)._base::_begin(); }

   iterator begin() _sstl_noexcept_ { return _base::_begin(); }
   const_iterator begin() const _sstl_noexcept_ { return const_cast<vector&>(*this)._base::_begin(); }
   const_iterator cbegin() const _sstl_noexcept_ { return const_cast<vector&>(*this)._base::_begin(); }

   iterator end() _sstl_noexcept_ { return _end_; }
   const_iterator end() const _sstl_noexcept_ { return _end_; }
   const_iterator cend() const _sstl_noexcept_ { return _end_; }

   reverse_iterator rbegin() _sstl_noexcept(noexcept(std::declval<_base>()._rbegin()))
   {
      return _base::_rbegin();
   }
   const_reverse_iterator rbegin() const _sstl_noexcept(noexcept(std::declval<_base>()._rbegin()))
   {
      return const_cast<vector&>(*this)._base::_rbegin();
   }
   const_reverse_iterator crbegin() const _sstl_noexcept(noexcept(std::declval<_base>()._rbegin()))
   {
      return const_cast<vector&>(*this)._base::_rbegin();
   }

   reverse_iterator rend() _sstl_noexcept(noexcept(std::declval<_base>()._rend()))
   {
      return _base::_rend();
   }
   const_reverse_iterator rend() const _sstl_noexcept(noexcept(std::declval<_base>()._rend()))
   {
      return const_cast<vector&>(*this)._base::_rend();
   }
   const_reverse_iterator crend() const _sstl_noexcept(noexcept(std::declval<_base>()._rend()))
   {
      return const_cast<vector&>(*this)._base::_rend();
   }

   bool empty() const _sstl_noexcept_ { return _base::_empty(); }
   size_type size() const _sstl_noexcept_ { return _base::_size(); }
   size_type max_size() const _sstl_noexcept_ { return Capacity; }
   size_type capacity() const _sstl_noexcept_ { return Capacity; }

   void clear() _sstl_noexcept(noexcept(std::declval<_base>()._clear()))
   {
      _base::_clear();
   }

   iterator insert(const_iterator pos, const_reference value)
      _sstl_noexcept(noexcept(std::declval<_base>().template _insert<_base::_is_copy>( std::declval<iterator>(),
                                                                                       std::declval<reference>())))
   {
      sstl_assert(size() < Capacity);
      return _base::template _insert<_base::_is_copy>(const_cast<iterator>(pos), const_cast<reference>(value));
   }

   iterator insert(const_iterator pos, value_type&& value)
      _sstl_noexcept(noexcept(std::declval<_base>().template _insert<!_base::_is_copy>(std::declval<iterator>(),
                                                                                       std::declval<reference>())))
   {
      sstl_assert(size() < Capacity);
      return _base::template _insert<!_base::_is_copy>(const_cast<iterator>(pos), const_cast<reference>(value));
   }

   iterator insert(const_iterator pos, size_type count, const_reference value)
      _sstl_noexcept(noexcept(std::declval<_base>()._insert(std::declval<iterator>(),
                                                            std::declval<size_type>(),
                                                            std::declval<const_reference>())))
   {
      sstl_assert(size() + count <= Capacity);
      return _base::_insert(const_cast<iterator>(pos), count, value);
   }

   template<class TIterator, class = _enable_if_input_iterator_t<TIterator>>
   iterator insert(const_iterator pos, TIterator range_begin, TIterator range_end)
      _sstl_noexcept(noexcept(std::declval<_base>()._insert(std::declval<iterator>(),
                                                            std::declval<TIterator>(),
                                                            std::declval<TIterator>())))
   {
      sstl_assert(size() + std::distance(range_begin, range_end) <= Capacity);
      return _base::_insert(const_cast<iterator>(pos), range_begin, range_end);
   }

   iterator insert(const_iterator pos, std::initializer_list<value_type> init)
      _sstl_noexcept(noexcept(std::declval<_base>()._insert( std::declval<iterator>(),
                                                            std::declval<std::initializer_list<value_type>>().begin(),
                                                            std::declval<std::initializer_list<value_type>>().end())))
   {
      sstl_assert(size() + init.size() <= Capacity);
      return _base::_insert(const_cast<iterator>(pos), init.begin(), init.end());
   }

   template<class... Args>
   iterator emplace(const_iterator pos, Args&&... args)
      _sstl_noexcept(std::is_nothrow_constructible<value_type, typename std::add_rvalue_reference<Args>::type...>::value
                     && noexcept(std::declval<_base>()._emplace(std::declval<iterator>(), std::declval<Args>()...)))
   {
      sstl_assert(size() < Capacity);
      return _base::_emplace(const_cast<iterator>(pos), std::forward<Args>(args)...);
   }

   iterator erase(const_iterator pos)
      _sstl_noexcept(noexcept(std::declval<_base>()._erase(std::declval<iterator>())))
   {
      sstl_assert(pos >= begin() && pos < end());
      return _base::_erase(const_cast<iterator>(pos));
   }

   iterator erase(const_iterator range_begin, const_iterator range_end)
      _sstl_noexcept(noexcept(std::declval<_base>()._erase(std::declval<iterator>(), std::declval<iterator>())))
   {
      sstl_assert(range_begin <= range_end);
      sstl_assert(range_begin >= begin() && range_end <= end());
      return _base::_erase(const_cast<iterator>(range_begin), const_cast<iterator>(range_end));
   }

   void push_back(const_reference value)
      _sstl_noexcept(noexcept(std::declval<_base>()._emplace_back(std::declval<const_reference>())))
   {
      sstl_assert(size() < Capacity);
      _base::_emplace_back(value);
   }

   void push_back(value_type&& value)
      _sstl_noexcept(noexcept(std::declval<_base>()._emplace_back(std::declval<value_type>())))
   {
      sstl_assert(size() < Capacity);
      _base::_emplace_back(std::move(value));
   }

   template<class... Args>
   void emplace_back(Args&&... args)
      _sstl_noexcept(noexcept(std::declval<_base>()._emplace_back(std::forward<Args>(std::declval<Args>())...)))
   {
      sstl_assert(size() < Capacity);
      _base::_emplace_back(std::forward<Args>(args)...);
   }

   void pop_back() _sstl_noexcept(noexcept(std::declval<_base>()._pop_back()))
   {
      _base::_pop_back();
   }

   template<size_type CapacityRhs>
   void swap(vector<value_type, CapacityRhs>& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._swap(std::declval<_base&>())))
   {
      sstl_assert(rhs.size() <= Capacity);
      sstl_assert(size() <= CapacityRhs);
      _base::_swap(rhs);
   }

private:
   pointer _end_;
   std::array<typename aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type, Capacity> _buffer_;
};

template<class T>
T* _vector_base<T>::_begin() _sstl_noexcept_
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   return reinterpret_cast<T*>(reinterpret_cast<type_for_derived_member_variable_access&>(*this)._buffer_.data());
}

template<class T>
T* _vector_base<T>::_end() _sstl_noexcept_
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   return reinterpret_cast<type_for_derived_member_variable_access&>(*this)._end_;
}

template<class T>
void _vector_base<T>::_set_end(T* value) _sstl_noexcept_
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   reinterpret_cast<type_for_derived_member_variable_access&>(*this)._end_ = value;
}

template <class T, size_t CapacityLhs, size_t CapacityRhs>
inline bool operator==(const vector<T, CapacityLhs>& lhs, const vector<T, CapacityRhs>& rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template <class T, size_t CapacityLhs, size_t CapacityRhs>
inline bool operator!=(const vector<T, CapacityLhs>& lhs, const vector<T, CapacityRhs>& rhs)
{
    return !(lhs == rhs);
}

template <class T, size_t CapacityLhs, size_t CapacityRhs>
inline bool operator<(const vector<T, CapacityLhs>& lhs, const vector<T, CapacityRhs>& rhs)
{
   return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <class T, size_t CapacityLhs, size_t CapacityRhs>
inline bool operator<=(const vector<T, CapacityLhs>& lhs, const vector<T, CapacityRhs>& rhs)
{
   return !(rhs < lhs);
}

template <class T, size_t CapacityLhs, size_t CapacityRhs>
inline bool operator>(const vector<T, CapacityLhs>& lhs, const vector<T, CapacityRhs>& rhs)
{
   return rhs < lhs;
}

template <class T, size_t CapacityLhs, size_t CapacityRhs>
inline bool operator>=(const vector<T, CapacityLhs>& lhs, const vector<T, CapacityRhs>& rhs)
{
   return !(lhs < rhs);
}

template<class T, size_t Capacity>
void swap(vector<T, Capacity>& lhs, vector<T, Capacity>& rhs)
{
   lhs.swap(rhs);
}

template<class T, size_t CapacityLhs, size_t CapacityRhs>
void swap(vector<T, CapacityLhs>& lhs, vector<T, CapacityRhs>& rhs)
{
   lhs.swap(rhs);
}

}

#endif
