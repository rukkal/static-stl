/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_DEQUE__
#define _SSTL_DEQUE__

#include <type_traits>
#include <array>

#include "sstl_assert.h"
#include "__internal/_aligned_storage.h"

namespace sstl
{

template<class, size_t=static_cast<size_t>(-1)>
class dequeng;

template<class T>
class dequeng<T>
{
template<class U, size_t S>
friend class vector; //friend declaration required for vector's noexcept expressions

public:
   using value_type = T;
   using size_type = size_t;
   //using difference_type = ptrdiff_t;
   using reference = value_type&;
   using const_reference = const value_type&;
   using pointer = value_type*;
   using const_pointer = const value_type*;
   //using iterator = value_type*;
   //using const_iterator = const value_type*;
   //using reverse_iterator = std::reverse_iterator<iterator>;
   //using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
   bool empty() const { return true; }

protected:
   dequeng() = default;
   dequeng(const dequeng&) = default;
   dequeng(dequeng&&) {}; //MSVC (VS2013) does not allow to default move special member functions
   ~dequeng() = default;
};

template<class T, size_t CAPACITY>
class dequeng : public dequeng<T>
{
private:
   using _base = dequeng<T>;
   using _type_for_derived_member_variable_access = dequeng<T, 11>;

public:
   using value_type = typename _base::value_type;
   using size_type = typename _base::size_type;
   //using difference_type = typename _base::difference_type;
   using reference = typename _base::reference;
   using const_reference = typename _base::const_reference;
   using pointer = typename _base::pointer;
   using const_pointer = typename _base::const_pointer;
   //using iterator = typename _base::iterator;
   //using const_iterator = typename _base::const_iterator;
   //using reverse_iterator = typename _base::reverse_iterator;
   //using const_reverse_iterator = typename _base::const_reverse_iterator;

public:


private:
   size_type _capacity_{ CAPACITY };
   pointer _end_;
   std::array<typename _aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type, CAPACITY> _buffer_;
};

}

#endif
