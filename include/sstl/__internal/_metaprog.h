/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_METAPROGRAMMING__
#define _SSTL_METAPROGRAMMING__

namespace sstl
{
namespace _metaprog
{
   template < size_t A, size_t B, bool SelectA >
   struct select
   {
      static const size_t value = A;
   };

   template<size_t A, size_t B>
   struct select < A, B, false >
   {
      static const size_t value = B;
   };

   template<size_t A, size_t B>
   struct max
   {
      static const size_t value = select<A, B, (A>B)>::value;
   };
}
}

#endif
