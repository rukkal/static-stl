/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_WARNINGS__

#define STRINGIFY(a) #a

#ifdef __clang__
   #define warnings_clang_push_ignore(warning_name) \
   _Pragma(STRINGIFY(clang diagnostic push)) \
   _Pragma(STRINGIFY(clang diagnostic ignored warning_name))
#else
   #define warnings_clang_push_ignore(warning_name)
#endif

#ifdef __clang__
   #define warnings_clang_pop_ignore() \
   _Pragma(STRINGIFY(clang diagnostic pop))
#else
   #define warnings_clang_pop_ignore()
#endif

#endif // _SSTL_WARNINGS__
