/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_COUNTED_TYPE__
#define _SSTL_COUNTED_TYPE__

#include <cstddef>

namespace sstl
{
namespace test
{
   struct counted_type
   {
      counted_type()
      {
         ++constructions;
      }
      counted_type(size_t param) : member(param)
      {
         ++constructions;
      }
      counted_type(const counted_type& rhs) : member(rhs.member)
      {
         ++constructions;
         ++copy_constructions;
      }
      counted_type(counted_type&& rhs) : member(rhs.member)
      {
         ++constructions;
         ++move_constructions;
      }
      ~counted_type()
      {
         ++destructions;
      }
      static void reset_counts()
      {
         constructions = 0;
         copy_constructions = 0;
         move_constructions = 0;
         destructions = 0;
      }

      size_t member { static_cast<size_t>(-1) };

      static size_t constructions;
      static size_t copy_constructions;
      static size_t move_constructions;
      static size_t destructions;
   };
}
}

#endif
