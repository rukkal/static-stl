/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <sstl/stack.h>

namespace sstl_test
{
TEST_CASE("stack")
{
   SECTION("constructors can be instantiated")
   {
      static const auto CAPACITY = 5;
      auto container = sstl::vector<int, CAPACITY> {};

      sstl::stack<int, CAPACITY> {};

      sstl::stack<int, CAPACITY> {container};

      auto other_stack = sstl::stack<int, CAPACITY> {};
      sstl::stack<int, CAPACITY> {other_stack};
   }

   SECTION("member functions can be instantiated")
   {
      auto stack = sstl::stack<int, 5> {};
      auto stack2 = stack;

      stack.push(3);
      stack.emplace(3);
      stack.top();
      stack.pop();
      stack.empty();
      stack.size();
      stack.swap(stack2);
   }
}
}
