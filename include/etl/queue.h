/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef __SSTL_QUEUE__
#define __SSTL_QUEUE__

#include <queue>
#include "deque.h"

namespace etl
{
   // alias template to provide an etl::queue type that has consistent
   // interface with the rest of the library's containers
   template<class T, size_t CAPACITY, class Container=etl::deque<T, CAPACITY>>
   using queue = std::queue<T, Container>;
}

#endif
