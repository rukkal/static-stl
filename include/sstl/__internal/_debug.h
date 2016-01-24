/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_DEBUG__

#define _sstl_stringify(x) #x
#define _sstl_to_string(x) _sstl_stringify(x)

#define _sstl_debug_message(message) __FILE__ ":" _sstl_to_string(__LINE__) " " message

#endif // _SSTL_DEBUG__
