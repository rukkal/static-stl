/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl

Copyright(c) 2014 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include <UnitTest++/UnitTest++.h>

#include <sstl/__internal/log.h>
#include <sstl/__internal/power.h>

namespace
{
  SUITE(test_maths)
  {
    //*************************************************************************
    TEST(test_log_0_base)
    {
      int actual;

      actual = etl::log<0, 2>::value;
      CHECK_EQUAL(0, actual);

      actual = etl::log<0, 10>::value;
      CHECK_EQUAL(0, actual);
    }

    //*************************************************************************
    TEST(test_log_1_base)
    {
      int actual;

      actual = etl::log<1, 2>::value;
      CHECK_EQUAL(0, actual);

      actual = etl::log<1, 10>::value;
      CHECK_EQUAL(0, actual);
    }

    //*************************************************************************
    TEST(test_log_10_base)
    {
      int actual;

      actual = etl::log<10, 2>::value;
      CHECK_EQUAL(3, actual);

      actual = etl::log<10, 10>::value;
      CHECK_EQUAL(1, actual);
    }

    //*************************************************************************
    TEST(test_log_100_base)
    {
      int actual;

      actual = etl::log<100, 2>::value;
      CHECK_EQUAL(6, actual);

      actual = etl::log<100, 10>::value;
      CHECK_EQUAL(2, actual);
    }

    //*************************************************************************
    TEST(test_log_2)
    {
      int actual;

      actual = etl::log2<0>::value;
      CHECK_EQUAL(0, actual);

      actual = etl::log2<1>::value;
      CHECK_EQUAL(0, actual);

      actual = etl::log2<10>::value;
      CHECK_EQUAL(3, actual);

      actual = etl::log2<100>::value;
      CHECK_EQUAL(6, actual);
    }

    //*************************************************************************
    TEST(test_log_10)
    {
      int actual;

      actual = etl::log10<0>::value;
      CHECK_EQUAL(0, actual);

      actual = etl::log10<1>::value;
      CHECK_EQUAL(0, actual);

      actual = etl::log10<10>::value;
      CHECK_EQUAL(1, actual);

      actual = etl::log10<100>::value;
      CHECK_EQUAL(2, actual);

      actual = etl::log10<200>::value;
      CHECK_EQUAL(2, actual);
    }

    //*************************************************************************
    TEST(test_power)
    {
      uint64_t actual;

      // 2^1
      actual = etl::power<2, 1>::value;
      CHECK_EQUAL(2, actual);

      // 3^2
      actual = etl::power<3, 2>::value;
      CHECK_EQUAL(9, actual);

      // 4^3
      actual = etl::power<4, 3>::value;
      CHECK_EQUAL(64, actual);

      // 5^4
      actual = etl::power<5, 4>::value;
      CHECK_EQUAL(625, actual);

      // 6^5
      actual = etl::power<6, 5>::value;
      CHECK_EQUAL(7776, actual);

      // 7^6
      actual = etl::power<7, 6>::value;
      CHECK_EQUAL(117649, actual);

      // 8^7
      actual = etl::power<8, 7>::value;
      CHECK_EQUAL(2097152, actual);

      // 9^8
      actual = etl::power<9, 8>::value;
      CHECK_EQUAL(43046721, actual);

      // 10^9
      actual = etl::power<10, 9>::value;
      CHECK_EQUAL(1000000000, actual);

      // 2^16
      actual = etl::power<2, 15>::value;
      CHECK_EQUAL(0x8000, actual);

      // 2^31
      actual = etl::power<2, 31>::value;
      CHECK_EQUAL(0x80000000, actual);

      // 2^63
      actual = etl::power<2, 63>::value;
      CHECK_EQUAL(0x8000000000000000, actual);
    }

    //*************************************************************************
    TEST(test_power_of_2_round_up)
    {
      int actual;

      //
      actual = etl::power_of_2_round_up<0>::value;
      CHECK_EQUAL(2, actual);

      actual = etl::power_of_2_round_up<1>::value;
      CHECK_EQUAL(2, actual);

      actual = etl::power_of_2_round_up<2>::value;
      CHECK_EQUAL(2, actual);

      actual = etl::power_of_2_round_up<3>::value;
      CHECK_EQUAL(4, actual);

      actual = etl::power_of_2_round_up<4>::value;
      CHECK_EQUAL(4, actual);

      actual = etl::power_of_2_round_up<5>::value;
      CHECK_EQUAL(8, actual);

      actual = etl::power_of_2_round_up<127>::value;
      CHECK_EQUAL(128, actual);

      actual = etl::power_of_2_round_up<128>::value;
      CHECK_EQUAL(128, actual);

      actual = etl::power_of_2_round_up<129>::value;
      CHECK_EQUAL(256, actual);
    }

    //*************************************************************************
    TEST(test_power_of_2_round_down)
    {
      int actual;

      actual = etl::power_of_2_round_down<0>::value;
      CHECK_EQUAL(2, actual);

      actual = etl::power_of_2_round_down<1>::value;
      CHECK_EQUAL(2, actual);

      actual = etl::power_of_2_round_down<2>::value;
      CHECK_EQUAL(2, actual);

      actual = etl::power_of_2_round_down<3>::value;
      CHECK_EQUAL(2, actual);

      actual = etl::power_of_2_round_down<4>::value;
      CHECK_EQUAL(2, actual);

      actual = etl::power_of_2_round_down<5>::value;
      CHECK_EQUAL(4, actual);

      actual = etl::power_of_2_round_down<127>::value;
      CHECK_EQUAL(64, actual);

      actual = etl::power_of_2_round_down<128>::value;
      CHECK_EQUAL(64, actual);

      actual = etl::power_of_2_round_down<129>::value;
      CHECK_EQUAL(128, actual);
    }

    //*************************************************************************
    TEST(test_is_power_of_2)
    {
      bool actual;

      actual = etl::is_power_of_2<0>::value;
      CHECK_EQUAL(false, actual);

      actual = etl::is_power_of_2<1>::value;
      CHECK_EQUAL(false, actual);

      actual = etl::is_power_of_2<2>::value;
      CHECK_EQUAL(true, actual);

      actual = etl::is_power_of_2<3>::value;
      CHECK_EQUAL(false, actual);

      actual = etl::is_power_of_2<4>::value;
      CHECK_EQUAL(true, actual);

      actual = etl::is_power_of_2<5>::value;
      CHECK_EQUAL(false, actual);

      actual = etl::is_power_of_2<127>::value;
      CHECK_EQUAL(false, actual);

      actual = etl::is_power_of_2<128>::value;
      CHECK_EQUAL(true, actual);

      actual = etl::is_power_of_2<129>::value;
      CHECK_EQUAL(false, actual);
    }
  };
}
