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
#include "test_dequeng_utility.h"

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
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t(4), counted_type::copy_construction::exception);
         REQUIRE(counted_type::check{}.default_constructions(1).copy_constructions(2).destructions(3));
      }
      #endif
   }

   SECTION("range constructor")
   {
      SECTION("contained values")
      {
         auto init = {0, 1, 2, 3};
         auto d = deque_int_t(init.begin(), init.end());
         REQUIRE(are_containers_equal(d, init));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         auto init = std::initializer_list<counted_type>{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t(init.begin(), init.end()), counted_type::copy_construction::exception);
         REQUIRE(counted_type::check{}.copy_constructions(2).destructions(2));
      }
      #endif
   }

   SECTION("copy constructor")
   {
      SECTION("contained values + number of operations")
      {
         SECTION("same capacity")
         {
            auto rhs = deque_counted_type_t{0, 1, 2, 3};
            counted_type::reset_counts();
            auto lhs = rhs;
            REQUIRE(lhs == rhs);
            REQUIRE(counted_type::check().copy_constructions(4));
         }
         SECTION("different capacity")
         {
            auto rhs = sstl::dequeng<counted_type, 7>{0, 1, 2, 3};
            counted_type::reset_counts();
            auto lhs = sstl::dequeng<counted_type, 11>{ rhs };
            REQUIRE(lhs == rhs);
            REQUIRE(counted_type::check().copy_constructions(4));
         }
         SECTION("non-contiguous values")
         {
            auto rhs = make_noncontiguous_deque<counted_type>({0, 1, 2, 3});
            counted_type::reset_counts();
            auto lhs = deque_counted_type_t{ rhs };
            REQUIRE(lhs == rhs);
            REQUIRE(counted_type::check().copy_constructions(4));
         }
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         auto rhs = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t{ rhs }, counted_type::copy_construction::exception);
      }
      #endif
   }

   SECTION("move constructor")
   {
      SECTION("contained values + number of operations")
      {
         auto expected_lhs = deque_counted_type_t{0, 1, 2, 3};
         SECTION("same capacity")
         {
            auto rhs = deque_counted_type_t{0, 1, 2, 3};
            counted_type::reset_counts();
            auto lhs = deque_counted_type_t{ std::move(rhs) };
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check().move_constructions(4).destructions(4));
         }
         SECTION("different capacity")
         {
            auto rhs = sstl::dequeng<counted_type, 7>{0, 1, 2, 3};
            counted_type::reset_counts();
            auto lhs = sstl::dequeng<counted_type, 11>{ std::move(rhs) };
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check().move_constructions(4).destructions(4));
         }
         SECTION("non-contiguous values")
         {
            auto rhs = make_noncontiguous_deque<counted_type>({0, 1, 2, 3});
            counted_type::reset_counts();
            auto lhs = deque_counted_type_t{ std::move(rhs) };
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check().move_constructions(4).destructions(4));
         }
      }
      SECTION("moved-from state")
      {
         auto rhs = deque_int_t{0, 1, 2, 3};
         auto lhs = deque_int_t{ std::move(rhs) };
         REQUIRE(rhs.empty());

         rhs.push_back(10); rhs.push_back(11);
         REQUIRE(rhs == (deque_int_t{10, 11}));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         auto rhs = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t{ std::move(rhs) }, counted_type::move_construction::exception);
         REQUIRE(counted_type::check{}.move_constructions(2).destructions(4));
         REQUIRE((rhs == deque_counted_type_t{2, 3}));
      }
      #endif
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
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         auto init = std::initializer_list<counted_type>{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t{ init }, counted_type::copy_construction::exception);
         REQUIRE(counted_type::check{}.copy_constructions(2).destructions(2));
      }
      #endif
   }

   SECTION("destructor")
   {
      {
         auto d = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
      }
      REQUIRE(counted_type::check().destructions(4));
   }

   SECTION("copy assignment operator")
   {
      auto rhs = deque_counted_type_t{0, 1, 2, 3, 4};
      auto expected_lhs = deque_counted_type_t{0, 1, 2, 3, 4};

      SECTION("contained values + number of operations")
      {
         SECTION("lhs is empty")
         {
            auto lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_constructions(5));
         }
         SECTION("rhs is empty")
         {
            auto rhs = deque_counted_type_t{};
            auto lhs = deque_counted_type_t{0, 1};
            auto expected_lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs.empty());
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.destructions(2));
         }
         SECTION("lhs.size() < rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_assignments(2).copy_constructions(3));
         }
         SECTION("lhs.size() == rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_assignments(5));
         }
         SECTION("lhs.size() > rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14, 15, 16};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_assignments(5).destructions(2));
         }
         SECTION("different capacities")
         {
            auto rhs = sstl::dequeng<counted_type, 11>{0, 1, 2, 3};
            auto expected_lhs = deque_counted_type_t{0, 1, 2, 3};
            auto lhs = sstl::dequeng<counted_type, 7>{};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check().copy_constructions(4));
         }
         SECTION("non-contiguous values")
         {
            auto rhs = make_noncontiguous_deque<counted_type>({0, 1, 2, 3});
            auto expected_lhs = deque_counted_type_t{0, 1, 2, 3};
            auto lhs = make_noncontiguous_deque<counted_type>({10, 11, 12, 13});
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check().copy_assignments(4));
         }
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         SECTION("copy assignment throws")
         {
            auto lhs = deque_counted_type_t{10, 11, 12};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_assignment(2);
            REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_assignment::exception);
            REQUIRE(counted_type::check().copy_assignments(1));
            REQUIRE((lhs == deque_counted_type_t{0, 11, 12}));
         }
         SECTION("copy constructor throws")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(2);
            REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_assignments(2).copy_constructions(1));
            REQUIRE((lhs == deque_counted_type_t{0, 1, 2}));
         }
      }
      #endif
   }

   SECTION("move assignment operator")
   {
      auto rhs = deque_counted_type_t{0, 1, 2, 3, 4};
      auto expected_lhs = deque_counted_type_t{0, 1, 2, 3, 4};

      SECTION("contained values + number of operations")
      {
         SECTION("lhs is empty")
         {
            auto lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs = std::move(rhs);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(rhs.empty());
            REQUIRE(counted_type::check{}.move_constructions(5).destructions(5));
         }
         SECTION("rhs is empty")
         {
            auto rhs = deque_counted_type_t{};
            auto lhs = deque_counted_type_t{0, 1};
            auto expected_lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs = std::move(rhs);
            REQUIRE(lhs.empty());
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.destructions(2));
         }
         SECTION("lhs.size() < rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            lhs = std::move(rhs);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(rhs.empty());
            REQUIRE(counted_type::check{}.move_assignments(2).move_constructions(3).destructions(5));
         }
         SECTION("lhs.size() == rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14};
            counted_type::reset_counts();
            lhs = std::move(rhs);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(rhs.empty());
            REQUIRE(counted_type::check{}.move_assignments(5).destructions(5));
         }
         SECTION("lhs.size() > rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14, 15, 16};
            counted_type::reset_counts();
            lhs = std::move(rhs);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(rhs.empty());
            REQUIRE(counted_type::check{}.move_assignments(5).destructions(7));
         }
         SECTION("different capacities")
         {
            auto rhs = sstl::dequeng<counted_type, 11>{0, 1, 2, 3};
            auto expected_lhs = deque_counted_type_t{0, 1, 2, 3};
            auto lhs = sstl::dequeng<counted_type, 7>{};
            counted_type::reset_counts();
            lhs = std::move(rhs);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(rhs.empty());
            REQUIRE(counted_type::check().move_constructions(4).destructions(4));
         }
         SECTION("non-contiguous values")
         {
            auto rhs = make_noncontiguous_deque<counted_type>({0, 1, 2, 3});
            auto expected_lhs = deque_counted_type_t{0, 1, 2, 3};
            auto lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs = std::move(rhs);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(rhs.empty());
            REQUIRE(counted_type::check().move_constructions(4).destructions(4));
         }
      }
      SECTION("moved-from state")
      {
         auto rhs = deque_int_t{0, 1, 2, 3};
         auto lhs = deque_int_t{ };
         lhs = std::move(rhs);
         REQUIRE(rhs.empty());

         rhs.push_back(10); rhs.push_back(11);
         REQUIRE(rhs == (deque_int_t{10, 11}));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         SECTION("move assignment throws")
         {
            auto lhs = deque_counted_type_t{10, 11, 12};
            counted_type::reset_counts();
            counted_type::throw_at_nth_move_assignment(2);
            REQUIRE_THROWS_AS(lhs = std::move(rhs), counted_type::move_assignment::exception);
            REQUIRE(counted_type::check().move_assignments(1).destructions(1));
            REQUIRE((lhs == deque_counted_type_t{0, 11, 12}));
            REQUIRE((rhs == deque_counted_type_t{1, 2, 3, 4}));
         }
         SECTION("move constructor throws")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            counted_type::throw_at_nth_move_construction(2);
            REQUIRE_THROWS_AS(lhs = std::move(rhs), counted_type::move_construction::exception);
            REQUIRE(counted_type::check().move_assignments(2).move_constructions(1).destructions(3));
            REQUIRE((lhs == deque_counted_type_t{0, 1, 2}));
            REQUIRE((rhs == deque_counted_type_t{3, 4}));
         }
      }
      #endif
   }

   SECTION("initializer list assignment operator")
   {
      auto rhs = std::initializer_list<counted_type>{0, 1, 2, 3, 4};
      auto expected_lhs = deque_counted_type_t{0, 1, 2, 3, 4};

      SECTION("contained values + number of operations")
      {
         SECTION("lhs is empty")
         {
            auto lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_constructions(5));
         }
         SECTION("rhs is empty")
         {
            auto rhs = deque_counted_type_t{};
            auto lhs = deque_counted_type_t{0, 1};
            auto expected_lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs.empty());
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.destructions(2));
         }
         SECTION("lhs.size() < rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_assignments(2).copy_constructions(3));
         }
         SECTION("lhs.size() == rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_assignments(5));
         }
         SECTION("lhs.size() > rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14, 15, 16};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_assignments(5).destructions(2));
         }
         SECTION("non-contiguous values")
         {
            auto lhs = make_noncontiguous_deque<counted_type>({10, 11, 12, 13});
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check().copy_assignments(4).copy_constructions(1));
         }
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         SECTION("copy assignment throws")
         {
            auto lhs = deque_counted_type_t{10, 11, 12};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_assignment(2);
            REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_assignment::exception);
            REQUIRE(counted_type::check().copy_assignments(1));
            REQUIRE((lhs == deque_counted_type_t{0, 11, 12}));
         }
         SECTION("copy constructor throws")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(2);
            REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_assignments(2).copy_constructions(1));
            REQUIRE((lhs == deque_counted_type_t{0, 1, 2}));
         }
      }
      #endif
   }

   SECTION("count assign")
   {
      auto count = typename deque_counted_type_t::size_type{ 5 };
      auto value = counted_type{ 3 };
      auto expected_lhs = deque_counted_type_t{3, 3, 3, 3, 3};

      SECTION("contained values + number of operations")
      {
         SECTION("lhs is empty")
         {
            auto lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs.assign(count, value);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_constructions(5));
         }
         SECTION("count == 0")
         {
            auto lhs = deque_counted_type_t{0, 1};
            auto expected_lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs.assign(0, value);
            REQUIRE(lhs.empty());
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.destructions(2));
         }
         SECTION("lhs.size() < count")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            lhs.assign(count, value);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_assignments(2).copy_constructions(3));
         }
         SECTION("lhs.size() == count")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14};
            counted_type::reset_counts();
            lhs.assign(count, value);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_assignments(5));
         }
         SECTION("lhs.size() > count")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14, 15, 16};
            counted_type::reset_counts();
            lhs.assign(count, value);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check{}.copy_assignments(5).destructions(2));
         }
         SECTION("non-contiguous values")
         {
            auto lhs = make_noncontiguous_deque<counted_type>({10, 11, 12, 13});
            counted_type::reset_counts();
            lhs.assign(count, value);
            REQUIRE(lhs == expected_lhs);
            REQUIRE(counted_type::check().copy_assignments(4).copy_constructions(1));
         }
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         SECTION("copy assignment throws")
         {
            auto lhs = deque_counted_type_t{10, 11, 12};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_assignment(2);
            REQUIRE_THROWS_AS(lhs.assign(count, value), counted_type::copy_assignment::exception);
            REQUIRE(counted_type::check().copy_assignments(1));
            REQUIRE((lhs == deque_counted_type_t{value, 11, 12}));
         }
         SECTION("copy constructor throws")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(2);
            REQUIRE_THROWS_AS(lhs.assign(count, value), counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_assignments(2).copy_constructions(1));
            REQUIRE((lhs == deque_counted_type_t{value, value, value}));
         }
      }
      #endif
   }

   SECTION("at")
   {
      auto d = deque_counted_type_t{0, 1, 2, 3, 4};
      SECTION("read access")
      {
         const auto& cd = d;
         REQUIRE(d.at(0) == 0);
         REQUIRE(d.at(2) == 2);
         REQUIRE(d.at(4) == 4);
      }
      SECTION("write access")
      {
         d.at(0) = 10;
         d.at(2) = 12;
         d.at(4) = 14;
         REQUIRE(d.at(0) == 10);
         REQUIRE(d.at(2) == 12);
         REQUIRE(d.at(4) == 14);
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         REQUIRE_THROWS_AS(d.at(5), std::out_of_range);
      }
      #endif
   }

   SECTION("operator[]")
   {
      auto d = deque_counted_type_t{0, 1, 2, 3, 4};
      SECTION("read access")
      {
         const auto& cd = d;
         REQUIRE(d[0]==0);
         REQUIRE(d[2]==2);
         REQUIRE(d[4]==4);
      }
      SECTION("write")
      {
         d[0] = 10;
         d[2] = 12;
         d[4] = 14;
         REQUIRE((d == deque_counted_type_t{10, 1, 12, 3, 14}));
      }
   }

   SECTION("front")
   {
      auto d = deque_counted_type_t{0, 1, 2};
      SECTION("read access")
      {
         const auto& cd = d;
         REQUIRE(cd.front() == 0);
      }
      SECTION("write access")
      {
         d.front() = 10;
         REQUIRE(d.front() == 10);
         REQUIRE((d == deque_counted_type_t{10, 1, 2}));
      }
   }

   SECTION("back")
   {
      auto d = deque_counted_type_t{0, 1, 2};
      SECTION("read access")
      {
         const auto& cd = d;
         REQUIRE(cd.back() == 2);
      }
      SECTION("write access")
      {
         d.back() = 12;
         REQUIRE(d.back() == 12);
         REQUIRE((d == deque_counted_type_t{0, 1, 12}));
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
