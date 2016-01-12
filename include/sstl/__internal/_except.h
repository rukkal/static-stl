/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_EXCEPT__

#include "preprocessor.h"

#if !defined(__EXCEPTIONS) || defined(_SSTL_NOEXCEPTIONS_TEST)
   #define _sstl_has_exceptions() 0
#else
   #define _sstl_has_exceptions() 1
#endif

#if IS_MSVC() && _MSC_VER < 1900
   #define _sstl_noexcept_
   #define _sstl_noexcept(...)
#else
   #define _sstl_noexcept_ noexcept
   #define _sstl_noexcept(...) noexcept(__VA_ARGS__)
#endif

#endif // _SSTL_EXCEPT__
