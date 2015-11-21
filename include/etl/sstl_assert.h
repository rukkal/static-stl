/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_ASSERT
#define _SSTL_ASSERT

#include <cassert>

#ifndef sstl_assert
// simply uses the standard library's assert
// feel free to change this header to customize your assertion behavior
#define sstl_assert(condition) assert(condition)
#endif

#endif
