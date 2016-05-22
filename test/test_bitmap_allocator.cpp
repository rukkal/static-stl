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

TEST_CASE("bitmap_allocator")
{
   SECTION("allocate/deallocate")
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
   
   SECTION("memory footprint")
   {
      REQUIRE(sizeof(sstl::bitmap_allocator<size_t, 1>) == (4+1)*sizeof(size_t));
      REQUIRE(sizeof(sstl::bitmap_allocator<size_t, 2>) == (4+2)*sizeof(size_t));
   }
}

}
