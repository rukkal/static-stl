/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_STACK__
#define _SSTL_STACK__

#include <stack>
#include "vector.h"

namespace sstl
{
   // alias template for std::stack to allow declaring stack types
   // with consistent syntax with the rest of the library
   template<class T, size_t CAPACITY, class Container=sstl::vector<T, CAPACITY>>
   using stack = std::stack<T, Container>;
}

#endif
