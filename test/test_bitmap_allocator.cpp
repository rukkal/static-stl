/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <algorithm>
#include <vector>
#include <type_traits>

#include <sstl/bitmap_allocator.h>

namespace sstl_test
{

template<class Titer>
void check_unique(Titer begin, Titer end)
{
   auto values = std::vector<typename Titer::value_type>(begin, end);
   std::sort(values.begin(), values.end());

   auto unique_values = std::vector<typename Titer::value_type> {};
   std::unique_copy(values.begin(), values.end(), std::back_inserter(unique_values));

   REQUIRE((values.size() == unique_values.size()));
   REQUIRE(std::equal(values.begin(), values.end(), unique_values.begin()));
}

TEST_CASE("bitmap_allocator - user cannot directly construct the base class")
{
   #if !_sstl_is_gcc()
      REQUIRE(!std::is_default_constructible<sstl::bitmap_allocator<int>>::value);
   #endif
   REQUIRE(!std::is_copy_constructible<sstl::bitmap_allocator<int>>::value);
   REQUIRE(!std::is_move_constructible<sstl::bitmap_allocator<int>>::value);
}

TEST_CASE("bitmap_allocator - user cannot directly destroy the base class")
{
   #if !_is_msvc() //MSVC (VS2013) has a buggy implementation of std::is_destructible
   REQUIRE(!std::is_destructible<sstl::bitmap_allocator<int>>::value);
   #endif
}

TEST_CASE("bitmap_allocator - allocate/deallocate")
{
   static const size_t capacity = 31;
   auto allocator = sstl::bitmap_allocator<int, capacity> {};
   auto allocated = std::vector<int*> {};

   // allocate all
   std::generate_n(std::back_inserter(allocated),
                 capacity,
                 [&allocator]() { return allocator.allocate(); });
   check_unique(allocated.begin(), allocated.end());

   // deallocate all
   for(auto p : allocated)
   {
     allocator.deallocate(p);
   }
   allocated.clear();

   // allocate all
   std::generate_n(std::back_inserter(allocated),
                 capacity,
                 [&allocator]() { return allocator.allocate(); });
   check_unique(allocated.begin(), allocated.end());
}

TEST_CASE("bitmap_allocator - full")
{
   static const size_t capacity = 2;
   auto allocator = sstl::bitmap_allocator < int, capacity > {};
   REQUIRE(!allocator.full());
   
   auto ptr0 = allocator.allocate();
   REQUIRE(!allocator.full());
   
   auto ptr1 = allocator.allocate();
   REQUIRE(allocator.full());
   
   allocator.deallocate(ptr1);
   REQUIRE(!allocator.full());
   
   allocator.deallocate(ptr0);
   REQUIRE(!allocator.full());
}

TEST_CASE("bitmap_allocator - memory footprint")
{
   REQUIRE(sizeof(sstl::bitmap_allocator<size_t, 1>) == (4+1)*sizeof(size_t));
   REQUIRE(sizeof(sstl::bitmap_allocator<size_t, 2>) == (4+2)*sizeof(size_t));
}

}
