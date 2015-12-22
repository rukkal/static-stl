/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_ITERATOR__
#define _SSTL_ITERATOR__

#include <type_traits>
#include <iterator>

namespace sstl
{

template<class TIterator>
using _enable_if_input_iterator_t =
   typename std::enable_if<std::is_base_of<  std::input_iterator_tag,
                                             typename std::iterator_traits<TIterator>::iterator_category
                                          >::value
                           >::type;

}

#endif
