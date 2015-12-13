/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <bitset>
#include <array>
#include <sstl/__internal/bitset_span.h>

namespace sstl
{
namespace test
{

template<class T1, class T2>
void check_bitset_equal(const T1& actual, const T2& expected)
{
    REQUIRE((actual.size() == expected.size()));
    REQUIRE((actual.count() == expected.count()));
    for(size_t i=0; i<actual.size(); ++i)
    {
        REQUIRE((actual.test(i) == expected.test(i)));
    }
}

TEST_CASE("bitset_span")
{
   auto expected = std::bitset<31> {};
   std::array<char, 5> actual_data;
   actual_data.fill(0);
   auto actual = sstl::bitset_span(actual_data.data(), 31);

   SECTION("constructor")
   {

      check_bitset_equal(actual, expected);
   }

   SECTION("set")
   {
       expected.set(0);
       actual.set(0);
       check_bitset_equal(actual, expected);

       expected.set(1);
       actual.set(1);
       check_bitset_equal(actual, expected);

       expected.set(29);
       actual.set(29);
       check_bitset_equal(actual, expected);

       expected.set(30);
       actual.set(30);
       check_bitset_equal(actual, expected);

       expected.set(11);
       actual.set(11);
       check_bitset_equal(actual, expected);

       expected.set();
       actual.set();
       check_bitset_equal(actual, expected);
   }

   SECTION("reset")
   {
      expected.set();
      actual.set();
      check_bitset_equal(actual, expected);

      expected.reset(0);
      actual.reset(0);
      check_bitset_equal(actual, expected);

      expected.reset(1);
      actual.reset(1);
      check_bitset_equal(actual, expected);

      expected.reset(29);
      actual.reset(29);
      check_bitset_equal(actual, expected);

      expected.reset(30);
      actual.reset(30);
      check_bitset_equal(actual, expected);

      expected.reset(11);
      actual.reset(11);
      check_bitset_equal(actual, expected);

      expected.reset();
      actual.reset();
      check_bitset_equal(actual, expected);
   }

   SECTION("all")
   {
      REQUIRE(actual.all() == false);
      actual.set();
      REQUIRE(actual.all() == true);
      actual.reset(0);
      REQUIRE(actual.all() == false);
      actual.set(0);
      REQUIRE(actual.all() == true);
      actual.reset(30);
      REQUIRE(actual.all() == false);
      actual.set(30);
      REQUIRE(actual.all() == true);
   }
}
}
}
