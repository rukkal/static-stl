/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <initializer_list>

#include <sstl/dequeng.h>
#include "utility.h"
#include "counted_type.h"

namespace sstl_test
{

struct value_type
{
   value_type() : value(0)
   {}
   value_type(int value) : value(value)
   {}
   int value;
};

template<class TValue>
sstl::dequeng<TValue, 11> make_noncontiguous_deque(std::initializer_list<TValue> init)
{
   auto d = sstl::dequeng<TValue, 11>(10);
   for(size_t i=0; i<10; ++i)
   {
      d.pop_front();
   }
   for(auto value : init)
   {
      d.push_back(value);
   }
   return d;
}

TEST_CASE("_dequeng_iterator")
{
   auto d = make_noncontiguous_deque<value_type>({0, 1, 2, 3});

   SECTION("expressions required by InputIterator concept")
   {
      REQUIRE(d.begin() != d.end());
      REQUIRE(d.begin() == d.begin());

      REQUIRE((*d.begin()).value == 0);

      REQUIRE(d.begin()->value == 0);

      {
         auto it = d.begin();
         auto& it_ref_1 = ++it;
         REQUIRE(it->value == 1);
         REQUIRE(it_ref_1->value == 1);
         auto& it_ref_2 = ++it;
         REQUIRE(it->value == 2);
         REQUIRE(it_ref_2->value == 2);
      }

      {
         auto it = d.begin();
         value_type v0 = *it++;
         REQUIRE(v0.value == 0);
         value_type v1 = *it++;
         REQUIRE(v1.value == 1);
      }
   }

   SECTION("expressions required by ForwardIterator concept")
   {
      {
         auto it = d.begin();
         auto it_0 = it++;
         REQUIRE(it_0->value == 0);
         REQUIRE(it->value == 1);

         auto it_1 = it++;
         REQUIRE(it_1->value == 1);
         REQUIRE(it->value == 2);
      }

      {
         auto it = d.begin();
         auto& value0 = *it++;
         REQUIRE(value0.value == 0);
         auto& value1 = *it++;
         REQUIRE(value1.value == 1);
      }
   }

   SECTION("expressions required by BidirectionalIterator concept")
   {
      {
         auto it = d.end();
         auto& it_ref_3 = --it;
         REQUIRE(it->value == 3);
         REQUIRE(it_ref_3->value == 3);

         auto& it_ref_2 = --it;
         REQUIRE(it->value == 2);
         REQUIRE(it_ref_2->value == 2);
      }

      {
         auto it = d.end();
         it--;
         auto it_3 = it--;
         REQUIRE(it_3->value == 3);
         REQUIRE(it->value == 2);

         auto it_2 = it--;
         REQUIRE(it_2->value == 2);
         REQUIRE(it->value == 1);
      }

      {
         auto it = d.end();
         --it;
         auto& value3 = *it--;
         REQUIRE(value3.value == 3);
         auto& value2 = *it++;
         REQUIRE(value2.value == 2);
      }
   }

   SECTION("expressions required by RandomAccessIterator concept")
   {
      {
         auto it = d.begin();
         auto& it_ref = (it+=2);
         REQUIRE(it->value == 2);
         REQUIRE(it_ref->value == 2);
      }

      {
         auto it_0 = d.begin();
         auto it_1 = it_0+1;
         REQUIRE(it_1->value == 1);
         auto it_3 = 3+it_0;
         REQUIRE(it_3->value == 3);
      }

      {
         auto it = d.end();
         auto& it_ref = (it-=2);
         REQUIRE(it->value == 2);
         REQUIRE(it_ref->value == 2);
      }

      //TODO: complete
   }

   SECTION("excpressions required by OutputIterator concept")
   {
      //TODO: complete
   }
}

}
