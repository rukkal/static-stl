/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <UnitTest++/UnitTest++.h>
#include <bitset>
#include <array>
#include <etl/__internal/bitset_span.h>

namespace test_etl
{
SUITE(test_bitset_span)
{
   template<class T1, class T2>
   void check_equal(const T1& t1, const T2& t2)
   {
      CHECK_EQUAL(t1.size(), t2.size());
      CHECK_EQUAL(t1.count(), t2.count());
      for(size_t i=0; i<t1.size(); ++i)
      {
         CHECK_EQUAL(t1.test(i), t2.test(i));
      }
   }

   TEST(test_constructor)
   {
      auto expected = std::bitset<31> {};
      std::array<char, 5> actual_data;
      auto actual = etl::bitset_span(actual_data.data(), 31);
      check_equal(expected, actual);
   }

   TEST(test_set)
   {
      auto expected = std::bitset<31> {};
      std::array<char, 5> actual_data;
      auto actual = etl::bitset_span(actual_data.data(), 31);

      expected.set(0);
      actual.set(0);
      check_equal(expected, actual);

      expected.set(1);
      actual.set(1);
      check_equal(expected, actual);

      expected.set(29);
      actual.set(29);
      check_equal(expected, actual);

      expected.set(30);
      actual.set(30);
      check_equal(expected, actual);

      expected.set(11);
      actual.set(11);
      check_equal(expected, actual);

      expected.set();
      actual.set();
      check_equal(expected, actual);
   }

   TEST(test_reset)
   {
      auto expected = std::bitset<31> {};
      std::array<char, 5> actual_data;
      auto actual = etl::bitset_span(actual_data.data(), 31);

      expected.set();
      actual.set();
      check_equal(expected, actual);

      expected.reset(0);
      actual.reset(0);
      check_equal(expected, actual);

      expected.reset(1);
      actual.reset(1);
      check_equal(expected, actual);

      expected.reset(29);
      actual.reset(29);
      check_equal(expected, actual);

      expected.reset(30);
      actual.reset(30);
      check_equal(expected, actual);

      expected.reset(11);
      actual.reset(11);
      check_equal(expected, actual);

      expected.reset();
      actual.reset();
      check_equal(expected, actual);
   }

   TEST(test_all)
   {
      std::array<char, 5> actual_data;
      auto actual = etl::bitset_span(actual_data.data(), 31);

      CHECK_EQUAL(false, actual.all());
      actual.set();
      CHECK_EQUAL(true, actual.all());
      actual.reset(0);
      CHECK_EQUAL(false, actual.all());
      actual.set(0);
      CHECK_EQUAL(true, actual.all());
      actual.reset(30);
      CHECK_EQUAL(false, actual.all());
      actual.set(30);
      CHECK_EQUAL(true, actual.all());
   }
};
}
