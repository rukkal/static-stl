/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <functional>
#include <sstl/priority_queue.h>
#include <sstl/vector.h>

namespace sstl_test
{
   
TEST_CASE("priority queue - constructors_can_be_instantiated")
{
   static const auto CAPACITY = 5;
   auto compare = std::less<int> {};
   auto container = sstl::vector<int, CAPACITY> {};

   sstl::priority_queue<int, CAPACITY> {};

   sstl::priority_queue<int, CAPACITY>(compare, container);

   auto other_priority_queue = sstl::priority_queue<int, CAPACITY> {};
   sstl::priority_queue<int, CAPACITY> {other_priority_queue};
}

TEST_CASE("priority queue - member_functions_can_be_instantiated")
{
   auto priority_queue = sstl::priority_queue<int, 5> {};
   auto priority_queue2 = priority_queue;

   priority_queue.push(3);
   priority_queue.emplace(5);
   priority_queue.top();
   priority_queue.pop();
   priority_queue.empty();
   priority_queue.size();
   priority_queue.swap(priority_queue2);
}

}
