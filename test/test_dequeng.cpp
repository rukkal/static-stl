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
#include "counted_type_stream_iterator.h"
#include "test_dequeng_utility.h"

namespace sstl_test
{
using deque_counted_type_base_t = sstl::dequeng<counted_type>;
using deque_counted_type_t = sstl::dequeng<counted_type, 11>;

TEST_CASE("dequeng")
{
   SECTION("user cannot directly construct the base class")
   {
      #if !_sstl_is_gcc()
         REQUIRE(!std::is_default_constructible<deque_counted_type_base_t>::value);
      #endif
      REQUIRE(!std::is_copy_constructible<deque_counted_type_base_t>::value);
      REQUIRE(!std::is_move_constructible<deque_counted_type_base_t>::value);
   }

   SECTION("user cannot directly destroy the base class")
   {
      #if !_is_msvc() //MSVC (VS2013) has a buggy implementation of std::is_destructible
      REQUIRE(!std::is_destructible<deque_counted_type_base_t>::value);
      #endif
   }

   SECTION("default constructor")
   {
      counted_type::reset_counts();
      auto d = deque_counted_type_t();
      REQUIRE(counted_type::check{}.constructions(0));
      REQUIRE(d.empty());
   }

   SECTION("count constructor")
   {
      SECTION("contained values")
      {
         auto expected = deque_counted_type_t{counted_type(), counted_type(), counted_type()};
         counted_type::reset_counts();
         auto actual = deque_counted_type_t(3);
         REQUIRE(counted_type::check{}.default_constructions(1).copy_constructions(3));
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
      auto values = std::initializer_list<counted_type>{0, 1, 2, 3};
      counted_type::reset_counts();
      SECTION("contained values")
      {
         auto d = deque_counted_type_t(values.begin(), values.end());
         REQUIRE(are_containers_equal(d, values));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t(values.begin(), values.end()), counted_type::copy_construction::exception);
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
            REQUIRE(counted_type::check().move_constructions(4).destructions(4));
            REQUIRE(lhs == expected_lhs);
         }
         SECTION("different capacity")
         {
            auto rhs = sstl::dequeng<counted_type, 7>{0, 1, 2, 3};
            counted_type::reset_counts();
            auto lhs = sstl::dequeng<counted_type, 11>{ std::move(rhs) };
            REQUIRE(counted_type::check().move_constructions(4).destructions(4));
            REQUIRE(lhs == expected_lhs);
         }
         SECTION("non-contiguous values")
         {
            auto rhs = make_noncontiguous_deque<counted_type>({0, 1, 2, 3});
            counted_type::reset_counts();
            auto lhs = deque_counted_type_t{ std::move(rhs) };
            REQUIRE(counted_type::check().move_constructions(4).destructions(4));
            REQUIRE(lhs == expected_lhs);
         }
      }
      SECTION("moved-from state")
      {
         auto rhs = deque_counted_type_t{0, 1, 2, 3};
         auto lhs = deque_counted_type_t{ std::move(rhs) };
         REQUIRE(rhs.empty());

         rhs.push_back(10); rhs.push_back(11);
         REQUIRE(rhs == (deque_counted_type_t{10, 11}));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         auto rhs = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t{ std::move(rhs) }, counted_type::move_construction::exception);
         REQUIRE(counted_type::check{}.move_constructions(2).destructions(4));
         REQUIRE(rhs == (deque_counted_type_t{2, 3}));
      }
      #endif
   }

   SECTION("initializer-list constructor")
   {
      auto init = std::initializer_list<counted_type>{0, 1, 2, 3};
      counted_type::reset_counts();
      SECTION("contained values")
      {
         auto d = deque_counted_type_t{0, 1, 2, 3};
         REQUIRE(counted_type::check{}.parameter_constructions(4).copy_constructions(4));
         REQUIRE(are_containers_equal(d, init));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
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
            REQUIRE(lhs == deque_counted_type_t({0, 11, 12}));
         }
         SECTION("copy constructor throws")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(2);
            REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_assignments(2).copy_constructions(1));
            REQUIRE(lhs == (deque_counted_type_t{0, 1, 2}));
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
         auto rhs = deque_counted_type_t{0, 1, 2, 3};
         auto lhs = deque_counted_type_t{ };
         lhs = std::move(rhs);
         REQUIRE(rhs.empty());

         rhs.push_back(10); rhs.push_back(11);
         REQUIRE(rhs == (deque_counted_type_t{10, 11}));
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
            REQUIRE(lhs == (deque_counted_type_t{0, 11, 12}));
            REQUIRE(rhs == (deque_counted_type_t{1, 2, 3, 4}));
         }
         SECTION("move constructor throws")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            counted_type::throw_at_nth_move_construction(2);
            REQUIRE_THROWS_AS(lhs = std::move(rhs), counted_type::move_construction::exception);
            REQUIRE(counted_type::check().move_assignments(2).move_constructions(1).destructions(3));
            REQUIRE(lhs == (deque_counted_type_t{0, 1, 2}));
            REQUIRE(rhs == (deque_counted_type_t{3, 4}));
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
            REQUIRE(lhs == (deque_counted_type_t{0, 11, 12}));
         }
         SECTION("copy constructor throws")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(2);
            REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_assignments(2).copy_constructions(1));
            REQUIRE(lhs == (deque_counted_type_t{0, 1, 2}));
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
            REQUIRE(lhs == (deque_counted_type_t{value, 11, 12}));
         }
         SECTION("copy constructor throws")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(2);
            REQUIRE_THROWS_AS(lhs.assign(count, value), counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_assignments(2).copy_constructions(1));
            REQUIRE(lhs == (deque_counted_type_t{value, value, value}));
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
         REQUIRE(d == (deque_counted_type_t{10, 1, 12, 3, 14}));
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
         REQUIRE(d == (deque_counted_type_t{10, 1, 2}));
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
         REQUIRE(d == (deque_counted_type_t{0, 1, 12}));
      }
   }

   SECTION("iterators")
   {
      SECTION("zero elements")
      {
         auto d = deque_counted_type_t{};
         const auto& cd = d;

         REQUIRE(d.begin() == d.end());
         REQUIRE(cd.cbegin() == cd.cend());
         REQUIRE(d.rbegin() == d.rend());
         REQUIRE(cd.crbegin() == cd.crend());
      }
      SECTION("one elements")
      {
         auto d = deque_counted_type_t{1};
         const auto& cd = d;

         REQUIRE(std::distance(d.begin(), d.end()) == 1);
         REQUIRE(std::distance(cd.cbegin(), cd.cend()) == 1);
         REQUIRE(std::distance(d.rbegin(), d.rend()) == 1);
         REQUIRE(std::distance(cd.crbegin(), cd.crend()) == 1);

         REQUIRE(*d.begin() == 1);
         REQUIRE(*cd.cbegin() == 1);
         REQUIRE(*d.rbegin() == 1);
         REQUIRE(*cd.crbegin() == 1);
      }
      SECTION("many elements")
      {
         auto l = std::initializer_list<counted_type>{1, 2, 3, 4, 5};
         auto lbegin = l.begin();
         auto lend = l.end();
         auto lrbegin = std::reverse_iterator<decltype(lend)>{ lend };
         auto lrend = std::reverse_iterator<decltype(lbegin)>{ lbegin };

         auto d = deque_counted_type_t{ l };
         const auto& cd = d;

         REQUIRE(std::distance(d.begin(), d.end()) == 5);
         REQUIRE(std::distance(cd.cbegin(), cd.cend()) == 5);
         REQUIRE(std::distance(d.rbegin(), d.rend()) == 5);
         REQUIRE(std::distance(cd.crbegin(), cd.crend()) == 5);

         REQUIRE(std::equal(lbegin, lend, d.begin()));
         REQUIRE(std::equal(lbegin, lend, cd.cbegin()));
         REQUIRE(std::equal(lrbegin, lrend, d.rbegin()));
         REQUIRE(std::equal(lrbegin, lrend, cd.crbegin()));
      }
   }

   SECTION("empty")
   {
      auto d = deque_counted_type_t{};
      REQUIRE(d.empty());
      d.push_back(0);
      REQUIRE(!d.empty());
      d.clear();
      REQUIRE(d.empty());
   }

   SECTION("full")
   {
      auto d = sstl::dequeng<counted_type, 3>{0, 1, 2};
      REQUIRE(d.full());
      d.pop_back();
      REQUIRE(!d.full());
      d.push_back(0);
      REQUIRE(d.full());
   }

   SECTION("size")
   {
      auto d = deque_counted_type_t{};
      REQUIRE(d.size() == 0);
      d.push_back(0);
      REQUIRE(d.size() == 1);
      d.clear();
      REQUIRE(d.size() == 0);
   }

   SECTION("max_size")
   {
      {
         auto d = sstl::dequeng<counted_type, 1>{};
         REQUIRE(d.max_size() == 1);
      }
      {
         auto d = sstl::dequeng<counted_type, 11>{};
         REQUIRE(d.max_size() == 11);
      }
   }

   SECTION("clear")
   {
      SECTION("contiguous values")
      {
         auto d = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
         d.clear();
         REQUIRE(counted_type::check().destructions(4));
         REQUIRE(d.empty());
      }
      SECTION("non-contiguous values")
      {
         auto d = make_noncontiguous_deque<counted_type>({0, 1, 2, 3});
         counted_type::reset_counts();
         d.clear();
         REQUIRE(counted_type::check().destructions(4));
         REQUIRE(d.empty());
      }
   }

   SECTION("insert (lvalue reference + rvalue reference) + emplace")
   {
      auto d = make_noncontiguous_deque<counted_type>({0, 1, 2, 3, 4});
      auto value = counted_type{ 10 };
      counted_type::reset_counts();
      SECTION("begin")
      {
         SECTION("lvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cbegin(), value);
            REQUIRE(counted_type::check().copy_constructions(1));
            REQUIRE(it == d.begin());
            REQUIRE(d == (deque_counted_type_t{10, 0, 1, 2, 3, 4}));         
         }
         SECTION("rvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cbegin(), std::move(value));
            REQUIRE(counted_type::check().move_constructions(1));
            REQUIRE(it == d.begin());
            REQUIRE(d == (deque_counted_type_t{10, 0, 1, 2, 3, 4}));
         }
         SECTION("emplace")
         {
            deque_counted_type_t::iterator it = d.emplace(d.cbegin(), 10);
            REQUIRE(counted_type::check().parameter_constructions(1).move_constructions(1));
            REQUIRE(it == d.begin());
            REQUIRE(d == (deque_counted_type_t{10, 0, 1, 2, 3, 4}));
         }
      }
      SECTION("begin+1")
      {
         SECTION("lvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, value);
            REQUIRE(counted_type::check().move_constructions(1).copy_assignments(1));
            REQUIRE(it == d.begin()+1);
            REQUIRE(d == (deque_counted_type_t{0, 10, 1, 2, 3, 4}));
         }
         SECTION("rvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, std::move(value));
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(1));
            REQUIRE(it == d.begin()+1);
            REQUIRE(d == (deque_counted_type_t{0, 10, 1, 2, 3, 4}));
         }
         SECTION("emplace")
         {
            deque_counted_type_t::iterator it = d.emplace(d.cbegin()+1, 10);
            REQUIRE(counted_type::check().parameter_constructions(1).move_constructions(1).move_assignments(1));
            REQUIRE(it == d.begin()+1);
            REQUIRE(d == (deque_counted_type_t{0, 10, 1, 2, 3, 4}));
         }
      }
      SECTION("begin+2")
      {
         SECTION("lvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, value);
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(1).copy_assignments(1));
            REQUIRE(it == d.begin()+2);
            REQUIRE(d == (deque_counted_type_t{0, 1, 10, 2, 3, 4}));
         }
         SECTION("rvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, std::move(value));
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(2));
            REQUIRE(it == d.begin()+2);
            REQUIRE(d == (deque_counted_type_t{0, 1, 10, 2, 3, 4}));
         }
         SECTION("emplace")
         {
            deque_counted_type_t::iterator it = d.emplace(d.cbegin()+2, 10);
            REQUIRE(counted_type::check().parameter_constructions(1).move_constructions(1).move_assignments(2));
            REQUIRE(it == d.begin()+2);
            REQUIRE(d == (deque_counted_type_t{0, 1, 10, 2, 3, 4}));
         }
      }
      SECTION("end")
      {
         SECTION("lvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cend(), value);
            REQUIRE(counted_type::check().copy_constructions(1));
            REQUIRE(it == d.end()-1);
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10}));
         }
         SECTION("rvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cend(), std::move(value));
            REQUIRE(counted_type::check().move_constructions(1));
            REQUIRE(it == d.end()-1);
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10}));
         }
         SECTION("emplace")
         {
            deque_counted_type_t::iterator it = d.emplace(d.cend(), 10);
            REQUIRE(counted_type::check().parameter_constructions(1).move_constructions(1));
            REQUIRE(it == d.end()-1);
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10}));
         }
      }
      SECTION("end-1")
      {
         SECTION("lvalue_reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cend()-1, value);
            REQUIRE(counted_type::check().move_constructions(1).copy_assignments(1));
            REQUIRE(it == d.end()-2);
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 4}));
         }
         SECTION("rvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cend()-1, std::move(value));
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(1));
            REQUIRE(it == d.end()-2);
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 4}));
         }
         SECTION("emplace")
         {
            deque_counted_type_t::iterator it = d.emplace(d.cend()-1, 10);
            REQUIRE(counted_type::check().parameter_constructions(1).move_constructions(1).move_assignments(1));
            REQUIRE(it == d.end()-2);
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 4}));
         }
      }
      SECTION("end-2")
      {
         SECTION("lvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cend()-2, value);
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(1).copy_assignments(1));
            REQUIRE(it == d.end()-3);
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 3, 4}));
         }
         SECTION("rvalue reference")
         {
            deque_counted_type_t::iterator it = d.insert(d.cend()-2, std::move(value));
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(2));
            REQUIRE(it == d.end()-3);
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 3, 4}));
         }
         SECTION("emplace")
         {
            deque_counted_type_t::iterator it = d.emplace(d.cend()-2, 10);
            REQUIRE(counted_type::check().parameter_constructions(1).move_constructions(1).move_assignments(2));
            REQUIRE(it == d.end()-3);
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 3, 4}));
         }
      }
      #if _sstl_has_exceptions()
      //note only the lvalue-reference version is tested here, because the rvalue-version
      //and emplace share the same exception handling code
      SECTION("exception handling")
      {
         SECTION("construction of new element throws")
         {
            SECTION("begin")
            {
               counted_type::throw_at_nth_copy_construction(1);
               REQUIRE_THROWS_AS(d.insert(d.begin(), value), counted_type::copy_construction::exception);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("end")
            {
               counted_type::throw_at_nth_copy_construction(1);
               REQUIRE_THROWS_AS(d.insert(d.end(), value), counted_type::copy_construction::exception);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
         }
         SECTION("assignment of new element throws")
         {
            counted_type::throw_at_nth_copy_assignment(1);
            REQUIRE_THROWS_AS(d.insert(d.end()-2, value), counted_type::copy_assignment::exception);
            REQUIRE(counted_type::check{}.move_constructions(1).move_assignments(1).copy_assignments(0).destructions(0));
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 3, 4}));
         }
         SECTION("move construction throws (during internal shift)")
         {
            SECTION("begin region")
            {
               counted_type::throw_at_nth_move_construction(1);
               REQUIRE_THROWS_AS(d.insert(d.begin()+1, value), counted_type::move_construction::exception);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("end region")
            {
               counted_type::throw_at_nth_move_construction(1);
               REQUIRE_THROWS_AS(d.insert(d.end()-1, value), counted_type::move_construction::exception);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
         }
         SECTION("move assignment throws (during internal shift)")
         {
            SECTION("end region")
            {
               counted_type::throw_at_nth_move_assignment(1);
               REQUIRE_THROWS_AS(d.insert(d.end()-2, value), counted_type::move_assignment::exception);
               REQUIRE(counted_type::check{}.move_constructions(1).move_assignments(0).destructions(0));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 4}));
            }
            SECTION("begin region")
            {
               auto d = make_noncontiguous_deque<counted_type>({0, 1, 2, 3, 4, 5});
               counted_type::reset_counts();
               counted_type::throw_at_nth_move_assignment(1);
               REQUIRE_THROWS_AS(d.insert(d.begin()+2, value), counted_type::move_assignment::exception);
               REQUIRE(counted_type::check{}.move_constructions(1).move_assignments(0).destructions(0));
               REQUIRE(d == (deque_counted_type_t{0, 0, 1, 2, 3, 4, 5}));
            }
         }
      }
      #endif
   }

   SECTION("insert (count + range versions)")
   {
      auto d = make_noncontiguous_deque<counted_type>({0, 1, 2, 3, 4});
      auto value = counted_type{ 10 };
      auto values = std::initializer_list<counted_type>{ 10, 11, 12, 13, 14 };
      SECTION("begin")
      {
         SECTION("number of new elements=0")
         {
            auto values = std::initializer_list<counted_type>{};
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin(), 0, value);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.begin());
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin());
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin(), values.begin(), values.end());
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.begin());
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
         }
         SECTION("number of new elements=1")
         {
            auto values = std::initializer_list<counted_type>{ 10 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin(), 1, value);
               REQUIRE(counted_type::check{}.copy_constructions(1));
               REQUIRE(it == d.begin());
               REQUIRE(d == (deque_counted_type_t{10, 0, 1, 2, 3, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin());
               REQUIRE(d == (deque_counted_type_t{10, 0, 1, 2, 3, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin(), values.begin(), values.end());
               REQUIRE(counted_type::check{}.copy_constructions(1));
               REQUIRE(it == d.begin());
               REQUIRE(d == (deque_counted_type_t{10, 0, 1, 2, 3, 4}));
            }
         }
         SECTION("number of new elements=2")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin(), 2, value);
               REQUIRE(counted_type::check{}.copy_constructions(2));
               REQUIRE(it == d.begin());
               REQUIRE(d == (deque_counted_type_t{10, 10, 0, 1, 2, 3, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin());
               REQUIRE(d == (deque_counted_type_t{10, 11, 0, 1, 2, 3, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin(), values.begin(), values.end());
               REQUIRE(counted_type::check{}.copy_constructions(2));
               REQUIRE(it == d.begin());
               REQUIRE(d == (deque_counted_type_t{10, 11, 0, 1, 2, 3, 4}));
            }
         }
      }
      SECTION("begin+1")
      {
         SECTION("number of new elements=0")
         {
            auto values = std::initializer_list<counted_type>{};
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, 0, value);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, values.begin(), values.end());
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
         }
         SECTION("number of new elements=1")
         {
            auto values = std::initializer_list<counted_type>{ 10 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, 1, value);
               REQUIRE(counted_type::check{}.move_constructions(1).copy_assignments(1));
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 10, 1, 2, 3, 4}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 10, 1, 2, 3, 4}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(1).copy_assignments(1));
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 10, 1, 2, 3, 4}));
            }
         }
         SECTION("number of new elements=2")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, 2, value);
               REQUIRE(counted_type::check{}.move_constructions(1).copy_constructions(1).copy_assignments(1));
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 10, 10, 1, 2, 3, 4}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 10, 11, 1, 2, 3, 4}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(1).copy_constructions(1).copy_assignments(1));
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 10, 11, 1, 2, 3, 4}));            
            }
         }
         SECTION("number of new elements=3")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11, 12 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, 3, value);
               REQUIRE(counted_type::check{}.move_constructions(1).copy_constructions(2).copy_assignments(1));
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 10, 10, 10, 1, 2, 3, 4}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 10, 11, 12, 1, 2, 3, 4}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+1, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(1).copy_constructions(2).copy_assignments(1));
               REQUIRE(it == d.begin()+1);
               REQUIRE(d == (deque_counted_type_t{0, 10, 11, 12, 1, 2, 3, 4}));
            }
         }
      }
      SECTION("begin+2")
      {
         SECTION("number of new elements=0")
         {
            auto values = std::initializer_list<counted_type>{};
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, 0, value);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, values.begin(), values.end());
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
         }
         SECTION("number of new elements=1")
         {
            auto values = std::initializer_list<counted_type>{ 10 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, 1, value);
               REQUIRE(counted_type::check{}.move_constructions(1).move_assignments(1).copy_assignments(1));
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 10, 2, 3, 4}));
            
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 10, 2, 3, 4}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(1).move_assignments(1).copy_assignments(1));
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 10, 2, 3, 4}));
            }
         }
         SECTION("number of new elements=2")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, 2, value);
               REQUIRE(counted_type::check{}.move_constructions(2).copy_assignments(2));
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 10, 10, 2, 3, 4}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 10, 11, 2, 3, 4}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(2).copy_assignments(2));
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 10, 11, 2, 3, 4}));            
            }
         }
         SECTION("number of new elements=3")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11, 12 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, 3, value);
               REQUIRE(counted_type::check{}.move_constructions(2).copy_constructions(1).copy_assignments(2));
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 10, 10, 10, 2, 3, 4}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 10, 11, 12, 2, 3, 4}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cbegin()+2, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(2).copy_constructions(1).copy_assignments(2));
               REQUIRE(it == d.begin()+2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 10, 11, 12, 2, 3, 4}));
            }
         }
      }
      SECTION("end")
      {
         SECTION("number of new elements=0")
         {
            auto values = std::initializer_list<counted_type>{};
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end(), 0, value);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.end());
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.end());
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.end(), values.begin(), values.end());
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.end());
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
         }
         SECTION("number of new elements=1")
         {
            auto values = std::initializer_list<counted_type>{ 10 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end(), 1, value);
               REQUIRE(counted_type::check{}.copy_constructions(1));
               REQUIRE(it == d.end()-1);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.end()-1);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.end(), values.begin(), values.end());
               REQUIRE(counted_type::check{}.copy_constructions(1));
               REQUIRE(it == d.end()-1);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10}));
            }
         }
         SECTION("number of new elements=2")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end(), 2, value);
               REQUIRE(counted_type::check{}.copy_constructions(2));
               REQUIRE(it == d.end()-2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10, 10}));
            }
            SECTION("input iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.end()-2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10, 11}));
            }
            SECTION("forward iterator")
            {
               deque_counted_type_t::iterator it = d.insert(d.end(), values.begin(), values.end());
               REQUIRE(counted_type::check{}.copy_constructions(2));
               REQUIRE(it == d.end()-2);
               counted_type v2;
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10, 11}));
            }
         }
      }
      SECTION("end-1")
      {
         SECTION("number of new elements=0")
         {
            auto values = std::initializer_list<counted_type>{};
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-1, 0, value);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.end()-1);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend()-1, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.cend()-1);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-1, values.begin(), values.end());
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.end()-1);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
         }
         SECTION("number of new elements=1")
         {
            auto values = std::initializer_list<counted_type>{ 10 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-1, 1, value);
               REQUIRE(counted_type::check{}.move_constructions(1).copy_assignments(1));
               REQUIRE(it == d.end()-2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend()-1, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.cend()-2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-1, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(1).copy_assignments(1));
               REQUIRE(it == d.end()-2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 4}));
            }
         }
         SECTION("number of new elements=2")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-1, 2, value);
               REQUIRE(counted_type::check{}.move_constructions(1).copy_constructions(1).copy_assignments(1));
               REQUIRE(it == d.end()-3);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 10, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend()-1, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.cend()-3);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 11, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-1, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(1).copy_constructions(1).copy_assignments(1));
               REQUIRE(it == d.end()-3);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 11, 4}));
            }
         }
         SECTION("number of new elements=3")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11, 12 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-1, 3, value);
               REQUIRE(counted_type::check{}.move_constructions(1).copy_constructions(2).copy_assignments(1));
               REQUIRE(it == d.end()-4);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 10, 10, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend()-1, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.cend()-4);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 11, 12, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-1, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(1).copy_constructions(2).copy_assignments(1));
               REQUIRE(it == d.end()-4);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 10, 11, 12, 4}));
            }
         }
      }
      SECTION("end-2")
      {
         SECTION("number of new elements=0")
         {
            auto values = std::initializer_list<counted_type>{};
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-2, 0, value);
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.end()-2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend()-2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.cend()-2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-2, values.begin(), values.end());
               REQUIRE(counted_type::check{}.constructions(0).destructions(0));
               REQUIRE(it == d.end()-2);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
         }
         SECTION("number of new elements=1")
         {
            auto values = std::initializer_list<counted_type>{ 10 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-2, 1, value);
               REQUIRE(counted_type::check{}.move_constructions(1).move_assignments(1).copy_assignments(1));
               REQUIRE(it == d.end()-3);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 3, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend()-2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.cend()-3);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 3, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-2, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(1).move_assignments(1).copy_assignments(1));
               REQUIRE(it == d.end()-3);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 3, 4}));
            }
         }
         SECTION("number of new elements=2")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11 };
            counted_type::reset_counts();
            SECTION("count")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-2, 2, value);
               REQUIRE(counted_type::check{}.move_constructions(2).copy_assignments(2));
               REQUIRE(it == d.end()-4);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 10, 3, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend()-2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.cend()-4);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 11, 3, 4}));
            }
            SECTION("range (forward iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.end()-2, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(2).copy_assignments(2));
               REQUIRE(it == d.end()-4);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 11, 3, 4}));
            }
         }
         SECTION("number of new elements=3")
         {
            auto values = std::initializer_list<counted_type>{ 10, 11, 12 };
            counted_type::reset_counts();
            SECTION("count")
            {
               counted_type::reset_counts();
               deque_counted_type_t::iterator it = d.insert(d.end()-2, 3, value);
               REQUIRE(counted_type::check{}.move_constructions(2).copy_constructions(1).copy_assignments(2));
               REQUIRE(it == d.end()-5);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 10, 10, 3, 4}));
            }
            SECTION("range (input iterator)")
            {
               deque_counted_type_t::iterator it = d.insert(d.cend()-2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
               REQUIRE(it == d.cend()-5);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 11, 12, 3, 4}));
            }
            SECTION("range (forward iterator)")
            {
               counted_type::reset_counts();
               deque_counted_type_t::iterator it = d.insert(d.end()-2, values.begin(), values.end());
               REQUIRE(counted_type::check{}.move_constructions(2).copy_constructions(1).copy_assignments(2));
               REQUIRE(it == d.end()-5);
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 10, 11, 12, 3, 4}));
            }
         }
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling (copy construction throws)")
      {
         auto values = std::initializer_list<counted_type>{ 10, 11, 12, 13, 14 };
         counted_type::reset_counts();
         SECTION("begin region")
         {
            SECTION("count")
            {
               counted_type::throw_at_nth_copy_construction(2);
               REQUIRE_THROWS_AS(d.insert(d.cbegin()+2, 5, value), counted_type::copy_construction::exception);
               REQUIRE(counted_type::check{}.move_constructions(2).copy_constructions(1).destructions(3));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
            SECTION("range (input iterator)")
            {
               counted_type::throw_at_nth_copy_construction(4);
               REQUIRE_THROWS_AS(d.insert(d.cbegin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{})
                                 , counted_type::copy_construction::exception);
               REQUIRE(counted_type::check{}.copy_constructions(3).destructions(0));
               REQUIRE(d == (deque_counted_type_t{12, 11, 10, 0, 1, 2, 3, 4}));
            }
            SECTION("range (forward iterator)")
            {
               counted_type::throw_at_nth_copy_construction(2);
               REQUIRE_THROWS_AS(d.insert(d.cbegin()+2, values.begin(), values.begin()+5), counted_type::copy_construction::exception);
               REQUIRE(counted_type::check{}.move_constructions(2).copy_constructions(1).destructions(3));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));
            }
         }
         SECTION("end region")
         {
            SECTION("count")
            {
               counted_type::throw_at_nth_copy_construction(2);
               REQUIRE_THROWS_AS(d.insert(d.cend()-2, 5, value), counted_type::copy_construction::exception);
               REQUIRE(counted_type::check{}.move_constructions(2).copy_constructions(1).destructions(3));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));  
            }
            SECTION("range (input iterator)")
            {
               counted_type::throw_at_nth_copy_construction(4);
               REQUIRE_THROWS_AS(d.insert(d.cend()-2, counted_type_stream_iterator{values}, counted_type_stream_iterator{})
                                 , counted_type::copy_construction::exception);
               REQUIRE(counted_type::check{}.copy_constructions(3).destructions(0));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 10, 11, 12}));
            }
            SECTION("range (forward iterator)")
            {             
               counted_type::throw_at_nth_copy_construction(2);
               REQUIRE_THROWS_AS(d.insert(d.cend()-2, 5, value), counted_type::copy_construction::exception);
               REQUIRE(counted_type::check{}.move_constructions(2).copy_constructions(1).destructions(3));
               REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4}));  
            }
         }
      }
      #endif
   }

   SECTION("erase (position)")
   {
      auto d = make_noncontiguous_deque<counted_type>({0, 1, 2, 3, 4, 5, 6});
      counted_type::reset_counts();
      SECTION("begin")
      {
         deque_counted_type_t::iterator it = d.erase(d.cbegin());
         REQUIRE(counted_type::check{}.move_assignments(0).destructions(1));
         REQUIRE(it == d.begin());
         REQUIRE(d == (deque_counted_type_t{1, 2, 3, 4, 5, 6}));
      }
      SECTION("begin+1")
      {
         deque_counted_type_t::iterator it = d.erase(d.cbegin()+1);
         REQUIRE(counted_type::check{}.move_assignments(1).destructions(1));
         REQUIRE(it == d.begin()+1);
         REQUIRE(d == (deque_counted_type_t{0, 2, 3, 4, 5, 6}));
      }
      SECTION("begin+2")
      {
         deque_counted_type_t::iterator it = d.erase(d.cbegin()+2);
         REQUIRE(counted_type::check{}.move_assignments(2).destructions(1));
         REQUIRE(it == d.begin()+2);
         REQUIRE(d == (deque_counted_type_t{0, 1, 3, 4, 5, 6}));
      }
      SECTION("end-1")
      {
         deque_counted_type_t::iterator it = d.erase(d.cend()-1);
         REQUIRE(counted_type::check{}.move_assignments(0).destructions(1));
         REQUIRE(it == d.end());
         REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 5}));
      }
      SECTION("end-2")
      {
         deque_counted_type_t::iterator it = d.erase(d.cend()-2);
         REQUIRE(counted_type::check{}.move_assignments(1).destructions(1));
         REQUIRE(it == d.end()-1);
         REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 4, 6}));
      }
      SECTION("end-3")
      {
         deque_counted_type_t::iterator it = d.erase(d.cend()-3);
         REQUIRE(counted_type::check{}.move_assignments(2).destructions(1));
         REQUIRE(it == d.end()-2);
         REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 5, 6}));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         SECTION("begin region")
         {
            counted_type::throw_at_nth_move_assignment(2);
            REQUIRE_THROWS_AS(d.erase(d.cbegin()+2), counted_type::move_assignment::exception);
            REQUIRE(counted_type::check{}.move_assignments(1).destructions(0));
            REQUIRE(d == (deque_counted_type_t{0, 1, 1, 3, 4, 5, 6}));
         }
         SECTION("end region")
         {
            counted_type::throw_at_nth_move_assignment(2);
            REQUIRE_THROWS_AS(d.erase(d.cend()-3), counted_type::move_assignment::exception);
            REQUIRE(counted_type::check{}.move_assignments(1).destructions(0));
            REQUIRE(d == (deque_counted_type_t{0, 1, 2, 3, 5, 5, 6}));
         }
      }
      #endif
   }

   SECTION("non-member relative operators")
   {
      SECTION("lhs < rhs")
      {
         {
            auto lhs = deque_counted_type_t{0, 1, 2};
            auto rhs = deque_counted_type_t{0, 1, 2, 3};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(lhs < rhs);
            //REQUIRE(lhs <= rhs);
            //REQUIRE(!(lhs > rhs));
            //REQUIRE(!(lhs >= rhs));
         }
         {
            auto lhs = deque_counted_type_t{0, 1, 2, 3};
            auto rhs = deque_counted_type_t{0, 1, 3, 3};
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
            auto lhs = deque_counted_type_t{0, 1, 2};
            auto rhs = deque_counted_type_t{0, 1, 2};
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
            auto lhs = deque_counted_type_t{0, 1, 2, 3};
            auto rhs = deque_counted_type_t{0, 1, 2};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(!(lhs < rhs));
            //REQUIRE(!(lhs <= rhs));
            //REQUIRE(lhs > rhs);
            //REQUIRE(lhs >= rhs);
         }
         {
            auto lhs = deque_counted_type_t{0, 1, 3, 3};
            auto rhs = deque_counted_type_t{0, 1, 2, 3};
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
