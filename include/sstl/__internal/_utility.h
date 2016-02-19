/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_UTILITY__
#define _SSTL_UTILITY__

#include <type_traits>

namespace sstl
{

template<bool Cond,
         class T,
         class _TRet = typename std::conditional<Cond, T&&, const T&>::type>
_TRet _conditional_move(T& value)
{
   return static_cast<_TRet>(value);
}

}

#endif
