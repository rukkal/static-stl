/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <algorithm>
#include <sstl/__internal/_except.h>
#include <sstl/dequeng.h>

#include "utility.h"
#include "counted_type.h"

namespace sstl_test
{

using deque_int_base_t = sstl::dequeng<int>;
using deque_int_t = sstl::dequeng<int, 11>;
using deque_counted_type_t = sstl::dequeng<counted_type, 11>;

TEST_CASE("dequeng")
{
   SECTION("user cannot directly construct the base class")
   {
      #if !_sstl_is_gcc()
         REQUIRE(!std::is_default_constructible<deque_int_base_t>::value);
      #endif
      REQUIRE(!std::is_copy_constructible<deque_int_base_t>::value);
      REQUIRE(!std::is_move_constructible<deque_int_base_t>::value);
   }

   SECTION("user cannot directly destroy the base class")
   {
      #if !_is_msvc() //MSVC (VS2013) has a buggy implementation of std::is_destructible
      REQUIRE(!std::is_destructible<deque_int_base_t>::value);
      #endif
   }

   SECTION("default constructor")
   {
      auto d = deque_int_t();
      REQUIRE(d.empty());
   }

   #if 0
   SECTION("count constructor")
   {
      auto actual = deque_int_t(3);
      auto expected = deque_int_t{0, 0, 0};
      REQUIRE(actual == expected);
   }
   #endif

   SECTION("initializer-list constructor")
   {
      auto actual = deque_int_t{0, 1, 2};
      auto expected = {0, 1, 2};
      REQUIRE(actual.size() == expected.size());
      REQUIRE(std::equal(actual.cbegin(), actual.cend(), expected.begin()));
   }

   SECTION("non-member relative operators")
   {
      SECTION("lhs < rhs")
      {
         {
            auto lhs = deque_int_t{0, 1, 2};
            auto rhs = deque_int_t{0, 1, 2, 3};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(lhs < rhs);
            //REQUIRE(lhs <= rhs);
            //REQUIRE(!(lhs > rhs));
            //REQUIRE(!(lhs >= rhs));
         }
         {
            auto lhs = deque_int_t{0, 1, 2, 3};
            auto rhs = deque_int_t{0, 1, 3, 3};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(lhs < rhs);
            //REQUIRE(lhs <= rhs);
            //REQUIRE(!(lhs > rhs));
            //REQUIRE(!(lhs >= rhs));
         }
      }
      SECTION("lhs == rhs")
      {
         {
            auto lhs = deque_int_t{0, 1, 2};
            auto rhs = deque_int_t{0, 1, 2};
            REQUIRE(lhs == rhs);
            //REQUIRE(!(lhs != rhs));
            //REQUIRE(!(lhs < rhs));
            //REQUIRE(lhs <= rhs);
            //REQUIRE(!(lhs > rhs));
            //REQUIRE(lhs >= rhs);
         }
      }
      SECTION("lhs > rhs")
      {
         {
            auto lhs = deque_int_t{0, 1, 2, 3};
            auto rhs = deque_int_t{0, 1, 2};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(!(lhs < rhs));
            //REQUIRE(!(lhs <= rhs));
            //REQUIRE(lhs > rhs);
            //REQUIRE(lhs >= rhs);
         }
         {
            auto lhs = deque_int_t{0, 1, 3, 3};
            auto rhs = deque_int_t{0, 1, 2, 3};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(!(lhs < rhs));
            //REQUIRE(!(lhs <= rhs));
            //REQUIRE(lhs > rhs);
            //REQUIRE(lhs >= rhs);
         }
      }
   }
}

}
