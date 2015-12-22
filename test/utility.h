/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_TEST_UTILITY__
#define _SSTL_TEST_UTILITY__

namespace sstl
{
namespace test
{
   template<class TContainerActual, class TContainerExpected>
   bool are_containers_equal(const TContainerActual& actual, const TContainerExpected& expected)
   {
      if(actual.size() != expected.size())
         return false;
      if(!std::equal(actual.begin(), actual.end(), expected.begin()))
         return false;
      return true;
   }
};
};

#endif
