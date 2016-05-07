/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_TEST_DEQUENG_UTILITY__
#define _SSTL_TEST_DEQUENG_UTILITY__

#include <initializer_list>
#include <sstl/dequeng.h>

template<class T>
sstl::dequeng<T, 11> make_noncontiguous_deque(std::initializer_list<T> init)
{
   auto d = sstl::dequeng<T, 11>{};
   for(size_t i=0; i<d.capacity()-(init.size()/2)-1; ++i)
   {
      d.push_back(0);
      d.pop_front();
   }
   for(auto& value : init)
   {
      d.push_back(value);
   }
   return d;
}

#endif
