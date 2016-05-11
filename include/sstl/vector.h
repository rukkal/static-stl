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

#include <sstl_assert.h>

#include "__internal/_except.h"
#include "__internal/_utility.h"
#include "__internal/_iterator.h"
#include "__internal/_aligned_storage.h"
#include "__internal/_hacky_derived_class_access.h"
#include "__internal/_debug.h"

namespace sstl
{

template<class, size_t=static_cast<size_t>(-1)>
class vector;

template<class T>
class vector<T>
{
template<class U, size_t S>
friend class vector; //friend declaration required for vector's noexcept expressions

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
   vector& operator=(const vector& rhs)
      _sstl_noexcept(noexcept(std::declval<vector>()._copy_assign(std::declval<iterator>(), std::declval<iterator>())))
   {
      sstl_assert(rhs.size() <= capacity());
      if(this != &rhs)
      {
         _copy_assign(const_cast<vector&>(rhs).begin(), const_cast<vector&>(rhs).end());
      }
      return *this;
   }

   vector& operator=(vector&& rhs)
      _sstl_noexcept(noexcept(std::declval<vector>()._move_assign(std::declval<iterator>(), std::declval<iterator>())))
   {
      sstl_assert(rhs.size() <= capacity());
      _move_assign(rhs.begin(), rhs.end());
      rhs._set_end(rhs.begin());
      return *this;
   }

   vector& operator=(std::initializer_list<value_type> init)
      _sstl_noexcept(noexcept(std::declval<vector>()._copy_assign(
            std::declval<std::initializer_list<value_type>>().begin(),
            std::declval<std::initializer_list<value_type>>().end())))
   {
      sstl_assert(init.size() <= _capacity());
      _copy_assign(init.begin(), init.end());
      return *this;
   }

   void assign(size_type count, const_reference value)
      _sstl_noexcept(noexcept(std::declval<vector>()._count_assign(std::declval<size_type>(), std::declval<const_reference>())))
   {
      sstl_assert(count <= _capacity());
      _count_assign(count, value);
   }

   template<class TIterator,
            class = typename std::enable_if<_is_input_iterator<TIterator>::value>::type>
   void assign(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(noexcept(std::declval<vector>()._copy_assign(std::declval<TIterator>(),
                                                                  std::declval<TIterator>())))
   {
      _copy_assign(range_begin, range_end);
   }

   void assign(std::initializer_list<value_type> ilist)
      _sstl_noexcept(noexcept(std::declval<vector>()._copy_assign(
         std::declval<std::initializer_list<value_type>>().begin(),
         std::declval<std::initializer_list<value_type>>().end())))
   {
      sstl_assert(ilist.size() <= _capacity());
      _copy_assign(ilist.begin(), ilist.end());
   }

   reference at(size_type idx) _sstl_noexcept(!_sstl_has_exceptions())
   {
      #if _sstl_has_exceptions()
      if(idx >= size())
      {
         throw std::out_of_range(_sstl_debug_message("vector access out of range"));
      }
      #endif
      auto pos = begin() + idx;
      sstl_assert(pos < end());
      return *pos;
   }

   const_reference at(size_type idx) const
      _sstl_noexcept(noexcept(std::declval<vector>().at(size_type{})))
   {
      return const_cast<vector&>(*this).at(idx);
   }

   reference operator[](size_type idx) _sstl_noexcept_
   {
      auto pos = begin() + idx;
      sstl_assert(pos < end());
      return *pos;
   }

   const_reference operator[](size_type idx) const
      _sstl_noexcept(noexcept(std::declval<vector>().operator[](size_type{})))
   {
      return const_cast<vector&>(*this).operator[](idx);
   }

   reference front() _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *begin();
   }

   const_reference front() const
      _sstl_noexcept(noexcept(std::declval<vector>().front()))
   {
      return const_cast<vector&>(*this).front();
   }

   reference back() _sstl_noexcept_
   {
      sstl_assert(!empty());
      return *(end()-1);
   }

   const_reference back() const
      _sstl_noexcept(noexcept(std::declval<vector>().back()))
   {
      return const_cast<vector&>(*this).back();
   }

   pointer data() _sstl_noexcept_
   {
      return begin();
   }

   const_pointer data() const _sstl_noexcept(std::declval<vector>().data())
   {
      return const_cast<vector&>(this).data();
   }

   iterator begin() _sstl_noexcept_
   {
      return _begin();
   }

   const_iterator begin() const _sstl_noexcept(noexcept(std::declval<vector>().begin()))
   {
      return const_cast<vector&>(*this).begin();
   }

   const_iterator cbegin() const _sstl_noexcept(noexcept(std::declval<vector>().begin()))
   {
      return const_cast<vector&>(*this).begin();
   }

   iterator end() _sstl_noexcept_
   {
      return _end();
   }

   const_iterator end() const _sstl_noexcept(noexcept(std::declval<vector>().end()))
   {
      return const_cast<vector&>(*this).end();
   }

   const_iterator cend() const _sstl_noexcept(noexcept(std::declval<vector>().end()))
   {
      return const_cast<vector&>(*this).end();
   }

   reverse_iterator rbegin()
      _sstl_noexcept(std::is_nothrow_constructible<reverse_iterator, iterator>::value)
   {
      return reverse_iterator(end());
   }

   const_reverse_iterator rbegin() const
      _sstl_noexcept(std::declval<vector>().rbegin())
   {
      return const_cast<vector&>(*this).rbegin();
   }

   const_reverse_iterator crbegin() const
      _sstl_noexcept(noexcept(std::declval<vector>().rbegin()))
   {
      return const_cast<vector&>(*this).rbegin();
   }

   reverse_iterator rend()
      _sstl_noexcept(std::is_nothrow_constructible<reverse_iterator, iterator>::value)
   {
      return reverse_iterator(begin());
   }

   const_reverse_iterator rend() const
      _sstl_noexcept(noexcept(std::declval<vector>().rend()))
   {
      return const_cast<vector&>(*this).rend();
   }

   const_reverse_iterator crend() const
      _sstl_noexcept(noexcept(std::declval<vector>().rend()))
   {
      return const_cast<vector&>(*this).rend();
   }

   bool empty() const _sstl_noexcept_
   {
      return const_cast<vector&>(*this).begin() == const_cast<vector&>(*this).end();
   }

   size_type size() const _sstl_noexcept_
   {
      return std::distance(const_cast<vector&>(*this).begin(),
                           const_cast<vector&>(*this).end());
   }

   size_type max_size() const _sstl_noexcept_
   {
      return capacity();
   }

   size_type capacity() const _sstl_noexcept_
   {
      return _capacity();
   }

   void clear() _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      auto pos = begin();
      while(pos != end())
         (pos++)->~value_type();
      _set_end(begin());
   }

   iterator insert(const_iterator pos, const_reference value)
      _sstl_noexcept(noexcept(std::declval<vector>().template _insert<vector::_is_copy>(  std::declval<iterator>(),
                                                                                          std::declval<reference>())))
   {
      sstl_assert(pos >= begin() && pos <= end());
      sstl_assert(size() < capacity());
      return _insert<_is_copy>(const_cast<iterator>(pos), const_cast<reference>(value));
   }

   iterator insert(const_iterator pos, value_type&& value)
      _sstl_noexcept(noexcept(std::declval<vector>().template _insert<!vector::_is_copy>( std::declval<iterator>(),
                                                                                          std::declval<reference>())))
   {
      sstl_assert(pos >= begin() && pos <= end());
      sstl_assert(size() < capacity());
      return _insert<!_is_copy>(const_cast<iterator>(pos), const_cast<reference>(value));
   }

   iterator insert(const_iterator pos, size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value
                     && std::is_nothrow_copy_assignable<value_type>::value)
   {
      sstl_assert(pos >= begin() && pos <= end());
      sstl_assert(size() + count <= capacity());
      auto new_end = end() + count;
      auto src = end() - 1;
      auto dst = new_end - 1;

      #if _sstl_has_exceptions()
      try
      {
      #endif
         auto end_src_move_construction = std::max(const_cast<pointer>(pos)-1, end()-count-1);
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

         auto end_dst_copy_construction = end() - 1;
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
         if(pos != end())
         {
            _set_end(std::min(end(), dst+1));
            clear();
         }
         throw;
      }
      #endif
      _set_end(new_end);
      return const_cast<iterator>(pos);
   }

   template<class TIterator>
   iterator insert(  const_iterator pos,
                     TIterator range_begin,
                     TIterator range_end,
                     typename std::enable_if<_is_forward_iterator<TIterator>::value>::type* = 0)
      _sstl_noexcept(noexcept(std::declval<vector>()._insert(  std::declval<iterator>(),
                                                               std::declval<TIterator>(),
                                                               std::declval<TIterator>())))
   {
      sstl_assert(pos >= begin() && pos <= end());
      sstl_assert(size() + std::distance(range_begin, range_end) <= capacity());
      return _insert(const_cast<iterator>(pos), range_begin, range_end);
   }

   template<class TIterator>
   iterator insert(  const_iterator pos,
                     TIterator range_begin,
                     TIterator range_end,
                     typename std::enable_if< _is_input_iterator<TIterator>::value
                                          && !_is_forward_iterator<TIterator>::value>::type* = 0)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value
                  && noexcept(std::rotate(std::declval<iterator>(),
                                          std::declval<iterator>(),
                                          std::declval<iterator>())))
   {
      auto nonconst_pos = const_cast<iterator>(pos);
      auto old_end = end();

      #if _sstl_has_exceptions()
      try
      {
      #endif
         std::copy(range_begin, range_end, std::back_inserter(*this));
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         while(end() > old_end)
         {
            (end()-1)->~value_type();
            _set_end(end()-1);
         }
         throw;
      }
      #endif

      std::rotate(nonconst_pos, old_end, end());
      return nonconst_pos;
   }

   iterator insert(const_iterator pos, std::initializer_list<value_type> init)
      _sstl_noexcept(noexcept(std::declval<vector>()._insert(  std::declval<iterator>(),
                                                               std::declval<std::initializer_list<value_type>>().begin(),
                                                               std::declval<std::initializer_list<value_type>>().end())))
   {
      sstl_assert(pos >= begin() && pos <= end());
      sstl_assert(size() + init.size() <= capacity());
      return _insert(const_cast<iterator>(pos), init.begin(), init.end());
   }

   template<class... Args>
   iterator emplace(const_iterator pos, Args&&... args)
      _sstl_noexcept(std::is_nothrow_constructible<value_type, typename std::add_rvalue_reference<Args>::type...>::value
                     && noexcept(std::declval<vector>().template _insert<!vector::_is_copy>( std::declval<iterator>(),
                                                                                             std::declval<value_type&>())))
   {
      sstl_assert(pos >= begin() && pos <= end());
      sstl_assert(size() < capacity());
      value_type value(std::forward<Args>(args)...);
      return _insert<!_is_copy>(const_cast<iterator>(pos), value);
   }

   iterator erase(const_iterator pos)
      _sstl_noexcept(std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_destructible<value_type>::value)
   {
      sstl_assert(pos >= begin() && pos < end());
      auto current = const_cast<pointer>(pos);
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(current+1 != end())
         {
            *current = std::move(*(current+1));
            ++current;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         clear();
         throw;
      }
      #endif
      current->~value_type();
      _set_end(current);
      return const_cast<iterator>(pos);
   }

   iterator erase(const_iterator range_begin, const_iterator range_end)
      _sstl_noexcept(std::is_nothrow_move_assignable<value_type>::value && std::is_nothrow_destructible<value_type>::value)
   {
      sstl_assert(range_begin <= range_end);
      sstl_assert(range_begin >= begin() && range_end <= end());
      auto dst = const_cast<pointer>(range_begin);
      auto src = const_cast<pointer>(range_end);

      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != end())
         {
            *dst = std::move(*src);
            ++src; ++dst;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         clear();
         throw;
      }
      #endif
      auto new_end = dst;

      while(dst != end())
      {
         dst->~value_type();
         ++dst;
      }
      _set_end(new_end);
      return const_cast<iterator>(range_begin);
   }

   void push_back(const_reference value)
      _sstl_noexcept(noexcept(std::declval<vector>().emplace_back(std::declval<const_reference>())))
   {
      sstl_assert(size() < capacity());
      emplace_back(value);
   }

   void push_back(value_type&& value)
      _sstl_noexcept(noexcept(std::declval<vector>().emplace_back(std::declval<value_type>())))
   {
      sstl_assert(size() < capacity());
      emplace_back(std::move(value));
   }

   template<class... Args>
   void emplace_back(Args&&... args)
      _sstl_noexcept(std::is_nothrow_constructible<value_type, typename std::add_rvalue_reference<Args>::type...>::value)
   {
      sstl_assert(size() < capacity());
      new(end()) value_type(std::forward<Args>(args)...);
      _set_end(end()+1);
   }

   void pop_back()
      _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      sstl_assert(!empty());
      (end()-1)->~value_type();
      _set_end(end()-1);
   }

   void swap(vector& rhs)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value
                     && std::is_nothrow_destructible<value_type>::value)
   {
      sstl_assert(rhs.size() <= capacity());
      sstl_assert(size() <= rhs.capacity());
      vector *large, *small;

      if(size() < rhs.size())
      {
         large = &rhs;
         small = this;
      }
      else
      {
         large = this;
         small = &rhs;
      }

      auto large_pos = large->begin();
      auto large_end_swaps = large->begin() + small->size();
      auto large_end = large->end();
      auto small_pos = small->begin();

      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(large_pos != large_end_swaps)
         {
            std::iter_swap(large_pos, small_pos);
            ++large_pos; ++small_pos;
         }
         while(large_pos != large_end)
         {
            new(small_pos) value_type(std::move(*large_pos));
            large_pos->~value_type();
            ++large_pos; ++small_pos;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         if(large_pos >= large_end_swaps)
         {
            while(large_pos != large_end)
            {
               large_pos->~value_type();
               ++large_pos;
            }
            large->_set_end(large_end_swaps);
            small->_set_end(small_pos);
         }
         large->clear();
         small->clear();
         throw;
      }
      #endif

      large->_set_end(large_end_swaps);
      small->_set_end(small_pos);
   }

protected:
   static const bool _is_copy = true;

protected:
   vector() = default;
   vector(const vector&) = default;
   vector(vector&&) {}; //MSVC (VS2013) does not allow to default move special member functions
   ~vector() = default;

   void _count_constructor(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      #if _sstl_has_exceptions()
      try
      {
      #endif
         for(size_t i=0; i<count; ++i)
            emplace_back(value);
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         clear();
         throw;
      }
      #endif
   }

   template<class TIterator, class = typename std::enable_if<_is_input_iterator<TIterator>::value>::type>
   void _range_constructor(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(std::is_nothrow_copy_constructible<value_type>::value)
   {
      auto src = range_begin;
      auto dst = begin();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != range_end)
         {
            sstl_assert(dst-begin() < capacity());
            new(dst) value_type(*src);
            ++src; ++dst;
         }
         _set_end(dst);
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end(dst);
         clear();
         throw;
      }
      #endif
   }

   template<class TIterator>
   void _copy_assign(TIterator rhs_begin, TIterator rhs_end)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value
                     && std::is_nothrow_destructible<value_type>::value)
   {
      auto src = rhs_begin;
      auto dest = begin();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != rhs_end)
         {
            if(dest < end())
               *dest = *src;
            else
               new(dest) value_type(*src);
            ++dest; ++src;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end(std::max(dest, end()));
         clear();
         throw;
      }
      #endif
      auto old_end = end();
      _set_end(dest);
      while(dest < old_end)
      {
         dest->~value_type();
         ++dest;
      }
   }

   void _move_constructor(vector&& rhs)
      _sstl_noexcept((std::is_nothrow_move_constructible<value_type>::value
                     || std::is_nothrow_copy_constructible<value_type>::value)
                     && std::is_nothrow_destructible<value_type>::value)
   {
      auto src = rhs.begin();
      auto dst = begin();
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
         _set_end(dst);
         clear();
         while(src != rhs.end())
         {
            src->~value_type();
            ++src;
         }
         rhs._set_end(rhs.begin());
         throw;
      }
      #endif
      _set_end(dst);
      rhs._set_end(rhs.begin());
   }

   void _destructor() _sstl_noexcept(std::is_nothrow_destructible<value_type>::value)
   {
      auto b = begin();
      auto e = end();
      while(b != e)
         (b++)->~value_type();
   }

   template<class TIterator>
   void _move_assign(TIterator rhs_begin, TIterator rhs_end)
      _sstl_noexcept((std::is_nothrow_move_assignable<value_type>::value || std::is_nothrow_copy_assignable<value_type>::value)
                     && (std::is_nothrow_move_constructible<value_type>::value || std::is_nothrow_copy_constructible<value_type>::value)
                     && std::is_nothrow_destructible<value_type>::value)
   {
      auto src = rhs_begin;
      auto dest = begin();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(src != rhs_end)
         {
            if(dest < end())
               *dest = std::move(*src);
            else
               new(dest) value_type(std::move(*src));
            src->~value_type();
            ++dest; ++src;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end(std::max(dest, end()));
         clear();
         while(src < rhs_end)
         {
            src->~value_type();
            ++src;
         }
         throw;
      }
      #endif

      auto old_end = end();
      _set_end(dest);
      while(dest < old_end)
      {
         dest->~value_type();
         ++dest;
      }
   }

   void _count_assign(size_type count, const_reference value)
      _sstl_noexcept(std::is_nothrow_copy_assignable<value_type>::value
                     && std::is_nothrow_copy_constructible<value_type>::value
                     && std::is_nothrow_destructible<value_type>::value)
   {
      auto dest = begin();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         while(count > 0)
         {
            if(dest < end())
               *dest = value;
            else
               new(dest) value_type(value);
            ++dest; --count;
         }
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _set_end(std::max(dest, end()));
         clear();
         throw;
      }
      #endif
      auto new_end = dest;
      while(dest < end())
      {
         dest->~value_type();
         ++dest;
      }
      _set_end(new_end);
   }

   pointer _begin() _sstl_noexcept_;
   pointer _end() _sstl_noexcept_;
   void _set_end(pointer) _sstl_noexcept_;
   size_type _capacity() const _sstl_noexcept_;

   template<bool is_copy_insertion>
   iterator _insert(iterator pos, reference value)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value
                     && (!is_copy_insertion || std::is_nothrow_copy_constructible<value_type>::value))
   {
      if(pos != end())
      {
         auto last = end()-1;
         #if _sstl_has_exceptions()
         try
         {
         #endif
            new(end()) value_type(std::move(*last));
         #if _sstl_has_exceptions()
         }
         catch(...)
         {
            clear();
            throw;
         }

         try
         {
         #endif
            std::move_backward(pos, last, end());
            *pos = _conditional_move<!is_copy_insertion>(value);
         #if _sstl_has_exceptions()
         }
         catch(...)
         {
            _set_end(end()+1);
            clear();
            throw;
         }
         #endif
      }
      else
      {
         new(end()) value_type(_conditional_move<!is_copy_insertion>(value));
      }
      _set_end(end()+1);
      return pos;
   }

   template<class TIterator, class = typename std::enable_if<_is_bidirectional_iterator<TIterator>::value>::type>
   iterator _insert(iterator pos, TIterator range_begin, TIterator range_end)
      _sstl_noexcept(std::is_nothrow_move_constructible<value_type>::value
                     && std::is_nothrow_move_assignable<value_type>::value
                     && noexcept(value_type(*std::declval<TIterator&>()))
                     && noexcept(std::declval<value_type&>() = *std::declval<TIterator&>()))
   {
      auto count = std::distance(range_begin, range_end);
      auto new_end = end() + count;
      auto src_range = range_end - 1;
      auto src_vector = end() - 1;
      auto dst_vector = new_end - 1;

      #if _sstl_has_exceptions()
      try
      {
      #endif
         auto end_src_move_construction = std::max(pos-1, end()-count-1);
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

         auto end_dst_construction = end() - 1;
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
         if(pos != end())
         {
            _set_end(std::min(end(), dst_vector+1));
            clear();
         }
         throw;
      }
      #endif
      _set_end(new_end);

      return pos;
   }
};

template<class T, size_t Capacity>
class vector : public vector<T>
{
   friend T* vector<T>::_begin() _sstl_noexcept_;
   friend T* vector<T>::_end() _sstl_noexcept_;
   friend void vector<T>::_set_end(T*) _sstl_noexcept_;
   friend size_t vector<T>::_capacity() const _sstl_noexcept_;

private:
   using _base = vector<T>;
   using _type_for_derived_member_variable_access = vector<T, 11>;

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
      : _end_(_base::begin())
   {
      _assert_hacky_derived_class_access_is_valid<vector<value_type>, vector, _type_for_derived_member_variable_access>();
   }

   explicit vector(size_type count, const_reference value=value_type())
      _sstl_noexcept(noexcept(std::declval<_base>()._count_constructor(std::declval<size_type>(), std::declval<const_reference>())))
      : _end_(_base::begin())
   {
      sstl_assert(count <= Capacity);
      _assert_hacky_derived_class_access_is_valid<vector<value_type>, vector, _type_for_derived_member_variable_access>();
      _base::_count_constructor(count, value);
   }

   template<class TIterator, class = typename std::enable_if<_is_input_iterator<TIterator>::value>::type>
   vector(TIterator range_begin, TIterator range_end)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<TIterator>(),
                                                                        std::declval<TIterator>())))
   {
      _assert_hacky_derived_class_access_is_valid<vector<value_type>, vector, _type_for_derived_member_variable_access>();
      _base::_range_constructor(range_begin, range_end);
   }

   //copy construction from any vector with same value type (capacity doesn't matter)
   vector(const _base& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._range_constructor( std::declval<const_iterator>(),
                                                                        std::declval<const_iterator>())))
   {
      _assert_hacky_derived_class_access_is_valid<vector<value_type>, vector, _type_for_derived_member_variable_access>();
      _base::_range_constructor(const_cast<_base&>(rhs).begin(), const_cast<_base&>(rhs).end());
   }

   vector(const vector& rhs)
      _sstl_noexcept(noexcept(vector(std::declval<const _base&>())))
      : vector(static_cast<const _base&>(rhs))
   {}

   //move construction from any vector with same value type (capacity doesn't matter)
   vector(_base&& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>()._move_constructor(std::declval<_base>())))
   {
      sstl_assert(rhs.size() <= Capacity);
      _assert_hacky_derived_class_access_is_valid<vector<value_type>, vector, _type_for_derived_member_variable_access>();
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
      _assert_hacky_derived_class_access_is_valid<vector<value_type>, vector, _type_for_derived_member_variable_access>();
      _base::_range_constructor(init.begin(), init.end());
   }

   ~vector() _sstl_noexcept(noexcept(std::declval<_base>()._destructor()))
   {
      _base::_destructor();
   }

   vector& operator=(const vector<value_type>& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<_base>())))
   {
      return reinterpret_cast<vector&>(_base::operator=(rhs));
   }

   vector& operator=(const vector& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<_base>())))
   {
      return reinterpret_cast<vector&>(_base::operator=(rhs));
   }

   //move assignment from vectors with same value type (capacity doesn't matter)
   vector& operator=(_base&& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<_base>())))
   {
      return reinterpret_cast<vector&>(_base::operator=(std::move(rhs)));
   }

   vector& operator=(vector&& rhs)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<_base>())))
   {
      return reinterpret_cast<vector&>(_base::operator=(std::move(rhs)));
   }

   vector& operator=(std::initializer_list<value_type> init)
      _sstl_noexcept(noexcept(std::declval<_base>().operator=(std::declval<std::initializer_list<value_type>>())))
   {
      _base::operator=(init);
      return *this;
   }

private:
   size_type _capacity_{ Capacity };
   pointer _end_;
   std::array<typename _aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type, Capacity> _buffer_;
};

template<class T>
T* vector<T>::_begin() _sstl_noexcept_
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   return reinterpret_cast<T*>(reinterpret_cast<type_for_derived_member_variable_access&>(*this)._buffer_.data());
}

template<class T>
T* vector<T>::_end() _sstl_noexcept_
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   return reinterpret_cast<type_for_derived_member_variable_access&>(*this)._end_;
}

template<class T>
void vector<T>::_set_end(T* value) _sstl_noexcept_
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   reinterpret_cast<type_for_derived_member_variable_access&>(*this)._end_ = value;
}

template<class T>
size_t vector<T>::_capacity() const _sstl_noexcept_
{
   using type_for_derived_member_variable_access = typename vector<T, 1>::_type_for_derived_member_variable_access;
   return reinterpret_cast<const type_for_derived_member_variable_access&>(*this)._capacity_;
}

template <class T>
inline bool operator==(const vector<T>& lhs, const vector<T>& rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template <class T>
inline bool operator!=(const vector<T>& lhs, const vector<T>& rhs)
{
    return !(lhs == rhs);
}

template <class T>
inline bool operator<(const vector<T>& lhs, const vector<T>& rhs)
{
   return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <class T>
inline bool operator<=(const vector<T>& lhs, const vector<T>& rhs)
{
   return !(rhs < lhs);
}

template <class T>
inline bool operator>(const vector<T>& lhs, const vector<T>& rhs)
{
   return rhs < lhs;
}

template <class T>
inline bool operator>=(const vector<T>& lhs, const vector<T>& rhs)
{
   return !(lhs < rhs);
}

template<class T>
void swap(vector<T>& lhs, vector<T>& rhs)
{
   lhs.swap(rhs);
}

}

#endif
