/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <algorithm>
#include <vector>
#include <UnitTest++/UnitTest++.h>

#include <sstl/bitmap_allocator.h>

namespace sstl
{
namespace test
{
SUITE(test_bitmap_allocator)
{
   template<class Titer>
   void check_unique(Titer begin, Titer end)
   {
      auto values = std::vector<typename Titer::value_type>(begin, end);
      std::sort(values.begin(), values.end());

      auto unique_values = std::vector<typename Titer::value_type>{};
      std::unique_copy(values.begin(), values.end(), std::back_inserter(unique_values));

      CHECK(values.size() == unique_values.size());
      CHECK(std::equal(values.begin(), values.end(), unique_values.begin()));
   }

   TEST(test_allocate_deallocate)
   {
      static const size_t capacity = 31;
      auto allocator = sstl::bitmap_allocator<int, capacity>{};
      auto allocated = std::vector<int*>{};

      // allocate all
      std::generate_n(std::back_inserter(allocated),
                     capacity,
                     [&allocator]() { return allocator.allocate(); });
      check_unique(allocated.begin(), allocated.end());

      // deallocate all
      for(auto p : allocated){ allocator.deallocate(p); };
      allocated.clear();

      // allocate all
      std::generate_n(std::back_inserter(allocated),
                     capacity,
                     [&allocator]() { return allocator.allocate(); });
      check_unique(allocated.begin(), allocated.end());
   }

   template<class T>
   void check_alignment()
   {
      static const size_t capacity = 31;
      auto allocator = sstl::bitmap_allocator<T, capacity>{};
      auto allocated = std::vector<T*>{};
      std::generate_n(std::back_inserter(allocated),
                     capacity,
                     [&allocator]() { return allocator.allocate(); });

      for(auto p : allocated)
      {
         CHECK(reinterpret_cast<size_t>(p) % alignof(T) == 0);
      }
   }

   TEST(test_alignment)
   {
      check_alignment<char>();

      check_alignment<short>();

      check_alignment<int>();

      check_alignment<long long>();

      check_alignment<long double>();

      struct alignas(16) type_align_16 {};
      check_alignment<type_align_16>();

      struct alignas(32) type_align_32 {};
      check_alignment<type_align_32>();

      struct alignas(64) type_align_64 {};
      check_alignment<type_align_64>();
   }
};
}
}
