/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_TYPE_TAG__
#define _SSTL_TYPE_TAG__

namespace sstl
{

template<class T>
struct _type_tag
{
   using _type = T;
};

}

#endif
