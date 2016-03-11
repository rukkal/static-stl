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

   SECTION("count constructor")
   {
      SECTION("contained values")
      {
         auto actual = deque_int_t(3);
         auto expected = deque_int_t{0, 0, 0};
         REQUIRE(actual == expected);
      }
      SECTION("exception handling")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t(4), counted_type::copy_construction::exception);
         REQUIRE(counted_type::check{}.default_constructions(1).copy_constructions(2).destructions(3));
      }
   }

   SECTION("range constructor")
   {
      SECTION("contained values")
      {
         auto init = {0, 1, 2, 3};
         auto d = deque_int_t(init.begin(), init.end());
         REQUIRE(are_containers_equal(d, init));
      }
      SECTION("exception handling")
      {
         auto init = std::initializer_list<counted_type>{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t(init.begin(), init.end()), counted_type::copy_construction::exception);
         REQUIRE(counted_type::check{}.copy_constructions(2).destructions(2));
      }
   }

   SECTION("initializer-list constructor")
   {
      SECTION("contained values")
      {
         auto actual = deque_int_t{0, 1, 2};
         auto expected = {0, 1, 2};
         REQUIRE(actual.size() == expected.size());
         REQUIRE(std::equal(actual.cbegin(), actual.cend(), expected.begin()));
      }
      SECTION("exception handling")
      {
         auto init = std::initializer_list<counted_type>{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t{ init }, counted_type::copy_construction::exception);
         REQUIRE(counted_type::check{}.copy_constructions(2).destructions(2));
      }
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
