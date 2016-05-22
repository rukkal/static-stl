/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_PRIORITY_QUEUE__
#define _SSTL_PRIORITY_QUEUE__

#include <queue>
#include <functional>
#include "vector.h"

namespace sstl
{
   // alias template to provide an sstl::priority_queue type that has consistent
   // interface with the rest of the library's containers
   template<class T,
            size_t CAPACITY,
            class Container=sstl::vector<T, CAPACITY>,
            class Compare=std::less<typename  Container::value_type>>
   using priority_queue = std::priority_queue<T, Container, Compare>;
}

#endif
