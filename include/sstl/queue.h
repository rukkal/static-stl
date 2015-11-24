/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_QUEUE__
#define _SSTL_QUEUE__

#include <queue>
#include "deque.h"

namespace sstl
{
   // alias template to provide an sstl::queue type that has consistent
   // interface with the rest of the library's containers
   template<class T, size_t CAPACITY, class Container=sstl::deque<T, CAPACITY>>
   using queue = std::queue<T, Container>;
}

#endif
