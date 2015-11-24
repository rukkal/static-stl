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

#include <cstdint>
#include <limits>
#include <type_traits>

#include <sstl/__internal/binary.h>
#include <sstl/fnv_1.h>

#undef max

namespace sstl
{
namespace test
{
   // Count bits the easy way.
   template <typename T>
   size_t test_count(T value)
   {
     size_t count = 0;

     for (size_t i = 0; i < std::numeric_limits<T>::digits; ++i)
     {
       if ((value & (T(1) << i)) != 0)
       {
         ++count;
       }
     }

     return count;
   }

   // Check parity the easy way.
   template <typename T>
   size_t test_parity(T value)
   {
     size_t count = test_count(value);

     return count & 1;
   }

   // Power of 2.
   uint64_t test_power_of_2(int power)
   {
     uint64_t result = 1;

     for (int i = 0; i < power; ++i)
     {
       result *= 2;
     }

     return result;
   }

   // Fold bits.
   template <typename TReturn>
   TReturn test_fold_bits(uint64_t value, int size)
   {
     int bits_remaining = 64;
     uint64_t mask = test_power_of_2(size) - 1;
     TReturn  result = 0;

     while (bits_remaining > size)
     {
       result = result ^ (value & mask);
       value  = value >> size;
       bits_remaining -= size;
     }

     result = result ^ (value & mask);

     return result;
   }

  SUITE(test_binary)
  {
    //*************************************************************************
    TEST(test_rotate_left8)
    {
      uint8_t value;

      value = 0x00;
      value = sstl::rotate_left(value);
      CHECK_EQUAL(0, int(value));

      value = 0x21;
      value = sstl::rotate_left(value);
      CHECK_EQUAL(0x42, int(value));

      value = 0x42;
      value = sstl::rotate_left(value);
      CHECK_EQUAL(0x84, int(value));

      value = 0x84;
      value = sstl::rotate_left(value);
      CHECK_EQUAL(0x09, int(value));

      value = 0xB7;
      value = sstl::rotate_left(value, 2);
      CHECK_EQUAL(0xDE, int(value));

      value = 0xB7;
      value = sstl::rotate_left(value, 3);
      CHECK_EQUAL(0xBD, int(value));

      value = 0xB7;
      value = sstl::rotate_left(value, 4);
      CHECK_EQUAL(0x7B, int(value));

      value = 0xB7;
      value = sstl::rotate_left(value, 5);
      CHECK_EQUAL(0xF6, int(value));

      value = 0xB7;
      value = sstl::rotate_left(value, 6);
      CHECK_EQUAL(0xED, int(value));

      value = 0xB7;
      value = sstl::rotate_left(value, 7);
      CHECK_EQUAL(0xDB, int(value));

      value = 0xB7;
      value = sstl::rotate_left(value, 8);
      CHECK_EQUAL(0xB7, int(value));

      value = 0xB7;
      value = sstl::rotate_left(value, 9);
      CHECK_EQUAL(0x6F, int(value));
    }

    //*************************************************************************
    TEST(test_rotate_left16)
    {
      uint16_t value;

      value = 0x0000;
      value = sstl::rotate_left(value);
      CHECK_EQUAL(0, value);

      value = 0x8421;
      value = sstl::rotate_left(value);
      CHECK_EQUAL(0x0843, value);

      value = 0x0843;
      value = sstl::rotate_left(value);
      CHECK_EQUAL(0x1086, value);

      value = 0x1086;
      value = sstl::rotate_left(value);
      CHECK_EQUAL(0x210C, value);

      value = 0xB73C;
      value = sstl::rotate_left(value, 2);
      CHECK_EQUAL(0xDCF2, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 3);
      CHECK_EQUAL(0xB9E5, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 4);
      CHECK_EQUAL(0x73CB, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 5);
      CHECK_EQUAL(0xE796, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 6);
      CHECK_EQUAL(0xCF2D, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 7);
      CHECK_EQUAL(0x9E5B, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 8);
      CHECK_EQUAL(0x3CB7, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 9);
      CHECK_EQUAL(0x796E, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 9);
      CHECK_EQUAL(0x796E, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 10);
      CHECK_EQUAL(0xF2DC, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 11);
      CHECK_EQUAL(0xE5B9, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 12);
      CHECK_EQUAL(0xCB73, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 13);
      CHECK_EQUAL(0x96E7, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 14);
      CHECK_EQUAL(0x2DCF, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 15);
      CHECK_EQUAL(0x5B9E, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 16);
      CHECK_EQUAL(0xB73C, int(value));

      value = 0xB73C;
      value = sstl::rotate_left(value, 17);
      CHECK_EQUAL(0x6E79, int(value));
    }

    //*************************************************************************
    TEST(test_rotate_right8)
    {
      uint8_t value;

      value = 0x00;
      value = sstl::rotate_right(value);
      CHECK_EQUAL(0, int(value));

      value = 0x21;
      value = sstl::rotate_right(value);
      CHECK_EQUAL(0x90, int(value));

      value = 0x42;
      value = sstl::rotate_right(value);
      CHECK_EQUAL(0x21, int(value));

      value = 0x84;
      value = sstl::rotate_right(value);
      CHECK_EQUAL(0x42, int(value));

      value = 0xB7;
      value = sstl::rotate_right(value, 2);
      CHECK_EQUAL(0xED, int(value));

      value = 0xB7;
      value = sstl::rotate_right(value, 3);
      CHECK_EQUAL(0xF6, int(value));

      value = 0xB7;
      value = sstl::rotate_right(value, 4);
      CHECK_EQUAL(0x7B, int(value));

      value = 0xB7;
      value = sstl::rotate_right(value, 5);
      CHECK_EQUAL(0xBD, int(value));

      value = 0xB7;
      value = sstl::rotate_right(value, 6);
      CHECK_EQUAL(0xDE, int(value));

      value = 0xB7;
      value = sstl::rotate_right(value, 7);
      CHECK_EQUAL(0x6F, int(value));

      value = 0xB7;
      value = sstl::rotate_right(value, 8);
      CHECK_EQUAL(0xB7, int(value));

      value = 0xB7;
      value = sstl::rotate_right(value, 9);
      CHECK_EQUAL(0xDB, int(value));
    }

    //*************************************************************************
    TEST(test_rotate_right16)
    {
      uint16_t value;

      value = 0x0000;
      value = sstl::rotate_right(value);
      CHECK_EQUAL(0, value);

      value = 0x8421;
      value = sstl::rotate_right(value);
      CHECK_EQUAL(0xC210, value);

      value = 0xC210;
      value = sstl::rotate_right(value);
      CHECK_EQUAL(0x6108, value);

      value = 0x6108;
      value = sstl::rotate_right(value);
      CHECK_EQUAL(0x3084, value);

      value = 0xB73C;
      value = sstl::rotate_right(value, 2);
      CHECK_EQUAL(0x2DCF, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 3);
      CHECK_EQUAL(0x96E7, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 4);
      CHECK_EQUAL(0xCB73, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 5);
      CHECK_EQUAL(0xE5B9, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 6);
      CHECK_EQUAL(0xF2DC, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 7);
      CHECK_EQUAL(0x796E, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 8);
      CHECK_EQUAL(0x3CB7, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 9);
      CHECK_EQUAL(0x9E5B, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 9);
      CHECK_EQUAL(0x9E5B, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 10);
      CHECK_EQUAL(0xCF2D, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 11);
      CHECK_EQUAL(0xE796, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 12);
      CHECK_EQUAL(0x73CB, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 13);
      CHECK_EQUAL(0xB9E5, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 14);
      CHECK_EQUAL(0xDCF2, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 15);
      CHECK_EQUAL(0x6E79, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 16);
      CHECK_EQUAL(0xB73C, int(value));

      value = 0xB73C;
      value = sstl::rotate_right(value, 17);
      CHECK_EQUAL(0x5B9E, int(value));
    }

    //*************************************************************************
    TEST(test_rotate16)
    {
      uint16_t value;

      value = 0xB73C;
      value = sstl::rotate(value, 4);
      CHECK_EQUAL(0x73CB, int(value));

      value = 0xB73C;
      value = sstl::rotate(value, -4);
      CHECK_EQUAL(0xCB73, int(value));
    }

    //*************************************************************************
    TEST(test_reverse_bits8)
    {
      uint8_t value;

      value = 0xF0;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x0F, int(value));

      value = 0x0F;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0xF0, int(value));

      value = 0xA5;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0xA5, int(value));

      value = 0x5A;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x5A, int(value));

      value = 0xCA;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x53, int(value));

      value = 0xAC;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x35, int(value));
    }

    //*************************************************************************
    TEST(test_reverse_bits16)
    {
      uint16_t value;

      value = 0xFC5A;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x5A3F, value);

      value = 0x5A5A;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x5A5A, value);

      value = 0xA5A5;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0xA5A5, value);
    }

    //*************************************************************************
    TEST(test_reverse_bits32)
    {
      uint32_t value;

      value = 0xF0C3A55A;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x5AA5C30F, value);

      value = 0xA5A5A5A5;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0xA5A5A5A5, value);

      value = 0x5A5A5A5A;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x5A5A5A5A, value);
    }

    //*************************************************************************
    TEST(test_reverse_bits64)
    {
      uint64_t value;

      value = 0x1234ABCDF0C3A55A;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x5AA5C30FB3D52C48, value);

      value = 0xA5A5A5A5A5A5A5A5;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0xA5A5A5A5A5A5A5A5, value);

      value = 0x5A5A5A5A5A5A5A5A;
      value = sstl::reverse_bits(value);
      CHECK_EQUAL(0x5A5A5A5A5A5A5A5A, value);
    }

    //*************************************************************************
    TEST(test_reverse_bytes16)
    {
      uint16_t value;

      value = 0xFC5A;
      value = sstl::reverse_bytes(value);
      CHECK_EQUAL(0x5AFC, value);

      value = 0x5AA5;
      value = sstl::reverse_bytes(value);
      CHECK_EQUAL(0xA55A, value);

      value = 0xA55A;
      value = sstl::reverse_bytes(value);
      CHECK_EQUAL(0x5AA5, value);
    }

    //*************************************************************************
    TEST(test_reverse_bytes32)
    {
      uint32_t value;

      value = 0xF0C3A55A;
      value = sstl::reverse_bytes(value);
      CHECK_EQUAL(0x5AA5C3F0, value);

      value = 0xA5A55A5A;
      value = sstl::reverse_bytes(value);
      CHECK_EQUAL(0x5A5AA5A5, value);

      value = 0x5A5AA5A5;
      value = sstl::reverse_bytes(value);
      CHECK_EQUAL(0xA5A55A5A, value);
    }

    //*************************************************************************
    TEST(test_binary_to_gray8)
    {
      uint8_t last_gray = 0;

      for (size_t i = 1; i <= std::numeric_limits<uint8_t>::max(); ++i)
      {
        uint8_t gray = sstl::binary_to_gray(uint8_t(i));
        uint8_t result = gray ^ last_gray;
        last_gray = gray;

        bool pass = ((result != 0) && !(result & (result - 1)));

        // Only one bit should be set.
        CHECK(pass);
      }
    }

    //*************************************************************************
    TEST(test_binary_to_gray16)
    {
      uint16_t last_gray = 0;

      for (size_t i = 1; i <= std::numeric_limits<uint16_t>::max(); ++i)
      {
        uint16_t gray   = sstl::binary_to_gray(uint16_t(i));
        uint16_t result = gray ^ last_gray;
        last_gray       = gray;

        bool pass = ((result != 0) && !(result & (result - 1)));

        // Only one bit should be set.
        CHECK(pass);
      }
    }

    //*************************************************************************
    TEST(test_binary_to_gray32)
    {
      sstl::fnv_1a_32 hash;

      hash.add(1);

      for (size_t i = 1; i < 1000000; ++i)
      {
        uint32_t value = hash.value();

        uint32_t last_gray = sstl::binary_to_gray(uint32_t(value));
        uint32_t gray      = sstl::binary_to_gray(uint32_t(value + 1));
        uint32_t result    = gray ^ last_gray;

        bool pass = ((result != 0) && !(result & (result - 1)));

        hash.add(1);

        // Only one bit should be set.
        CHECK(pass);
      }
    }

    //*************************************************************************
    TEST(test_binary_to_gray64)
    {
      sstl::fnv_1a_64 hash;

      hash.add(1);

      for (size_t i = 1; i < 1000000; ++i)
      {
        uint64_t value = hash.value();

        uint64_t last_gray = sstl::binary_to_gray(uint64_t(value));
        uint64_t gray = sstl::binary_to_gray(uint64_t(value + 1));
        uint64_t result = gray ^ last_gray;

        bool pass = ((result != 0) && !(result & (result - 1)));

        hash.add(1);

        // Only one bit should be set.
        CHECK(pass);
      }
    }

    //*************************************************************************
    TEST(test_count_bits_8)
    {
      for (size_t i = 1; i <= std::numeric_limits<uint16_t>::max(); ++i)
      {
        CHECK_EQUAL(test_count(i), sstl::count_bits(i));
      }
    }

    //*************************************************************************
    TEST(test_count_bits_16)
    {
      for (size_t i = 1; i <= std::numeric_limits<uint16_t>::max(); ++i)
      {
        CHECK_EQUAL(test_count(i), sstl::count_bits(i));
      }
    }

    //*************************************************************************
    TEST(test_count_bits_32)
    {
      sstl::fnv_1a_32 hash;

      for (size_t i = 0; i < 1000000; ++i)
      {
        hash.add(1);

        uint32_t value = hash.value();

        CHECK_EQUAL(test_count(value), sstl::count_bits(value));
      }
    }

    //*************************************************************************
    TEST(test_count_bits_64)
    {
      sstl::fnv_1a_64 hash;

      for (size_t i = 0; i < 1000000; ++i)
      {
        hash.add(1);

        uint64_t value = hash.value();

        CHECK_EQUAL(test_count(value), sstl::count_bits(value));
      }
    }

    //*************************************************************************
    TEST(test_parity_8)
    {
      for (size_t i = 1; i <= std::numeric_limits<uint8_t>::max(); ++i)
      {
        CHECK_EQUAL(test_parity(i), sstl::parity(i));
      }
    }

    //*************************************************************************
    TEST(test_parity_16)
    {
      for (size_t i = 1; i <= std::numeric_limits<uint16_t>::max(); ++i)
      {
        CHECK_EQUAL(test_parity(i), sstl::parity(i));
      }
    }

    //*************************************************************************
    TEST(test_parity_32)
    {
      sstl::fnv_1a_32 hash;

      for (size_t i = 0; i < 1000000; ++i)
      {
        hash.add(1);

        uint32_t value = hash.value();

        CHECK_EQUAL(test_parity(value), sstl::parity(value));
      }
    }

    //*************************************************************************
    TEST(test_parity_64)
    {
      sstl::fnv_1a_64 hash;

      for (size_t i = 0; i < 1000000; ++i)
      {
        hash.add(1);

        uint64_t value = hash.value();

        CHECK_EQUAL(test_parity(value), sstl::parity(value));
      }
    }

    //*************************************************************************
    TEST(test_fold_bits)
    {
      const uint64_t data = 0xE8C9AACCBC3D9A8F;

      CHECK_EQUAL(test_fold_bits<uint64_t>(data,  1), (sstl::fold_bits<uint64_t,  1>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data,  2), (sstl::fold_bits<uint64_t,  2>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data,  3), (sstl::fold_bits<uint64_t,  3>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data,  4), (sstl::fold_bits<uint64_t,  4>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data,  5), (sstl::fold_bits<uint64_t,  5>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data,  6), (sstl::fold_bits<uint64_t,  6>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data,  7), (sstl::fold_bits<uint64_t,  7>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data,  8), (sstl::fold_bits<uint64_t,  8>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data,  9), (sstl::fold_bits<uint64_t,  9>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 10), (sstl::fold_bits<uint64_t, 10>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 11), (sstl::fold_bits<uint64_t, 11>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 12), (sstl::fold_bits<uint64_t, 12>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 13), (sstl::fold_bits<uint64_t, 13>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 14), (sstl::fold_bits<uint64_t, 14>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 15), (sstl::fold_bits<uint64_t, 15>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 16), (sstl::fold_bits<uint64_t, 16>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 17), (sstl::fold_bits<uint64_t, 17>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 18), (sstl::fold_bits<uint64_t, 18>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 19), (sstl::fold_bits<uint64_t, 19>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 20), (sstl::fold_bits<uint64_t, 20>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 21), (sstl::fold_bits<uint64_t, 21>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 22), (sstl::fold_bits<uint64_t, 22>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 23), (sstl::fold_bits<uint64_t, 23>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 24), (sstl::fold_bits<uint64_t, 24>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 25), (sstl::fold_bits<uint64_t, 25>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 26), (sstl::fold_bits<uint64_t, 26>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 27), (sstl::fold_bits<uint64_t, 27>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 28), (sstl::fold_bits<uint64_t, 28>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 29), (sstl::fold_bits<uint64_t, 29>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 30), (sstl::fold_bits<uint64_t, 30>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 31), (sstl::fold_bits<uint64_t, 31>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 32), (sstl::fold_bits<uint64_t, 32>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 33), (sstl::fold_bits<uint64_t, 33>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 34), (sstl::fold_bits<uint64_t, 34>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 35), (sstl::fold_bits<uint64_t, 35>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 36), (sstl::fold_bits<uint64_t, 36>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 37), (sstl::fold_bits<uint64_t, 37>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 38), (sstl::fold_bits<uint64_t, 38>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 39), (sstl::fold_bits<uint64_t, 39>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 40), (sstl::fold_bits<uint64_t, 40>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 41), (sstl::fold_bits<uint64_t, 41>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 42), (sstl::fold_bits<uint64_t, 42>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 43), (sstl::fold_bits<uint64_t, 43>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 44), (sstl::fold_bits<uint64_t, 44>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 45), (sstl::fold_bits<uint64_t, 45>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 46), (sstl::fold_bits<uint64_t, 46>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 47), (sstl::fold_bits<uint64_t, 47>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 48), (sstl::fold_bits<uint64_t, 48>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 49), (sstl::fold_bits<uint64_t, 49>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 50), (sstl::fold_bits<uint64_t, 50>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 51), (sstl::fold_bits<uint64_t, 51>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 52), (sstl::fold_bits<uint64_t, 52>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 53), (sstl::fold_bits<uint64_t, 53>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 54), (sstl::fold_bits<uint64_t, 54>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 55), (sstl::fold_bits<uint64_t, 55>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 56), (sstl::fold_bits<uint64_t, 56>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 57), (sstl::fold_bits<uint64_t, 57>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 58), (sstl::fold_bits<uint64_t, 58>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 59), (sstl::fold_bits<uint64_t, 59>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 60), (sstl::fold_bits<uint64_t, 60>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 61), (sstl::fold_bits<uint64_t, 61>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 62), (sstl::fold_bits<uint64_t, 62>(data)));
      CHECK_EQUAL(test_fold_bits<uint64_t>(data, 63), (sstl::fold_bits<uint64_t, 63>(data)));
    }

    //*************************************************************************
    TEST(test_max_value_for_bits)
    {
        // Check that the values are correct.
        CHECK_EQUAL(0, sstl::max_value_for_nbits<0>::value);
        CHECK_EQUAL(1, sstl::max_value_for_nbits<1>::value);
        CHECK_EQUAL(3, sstl::max_value_for_nbits<2>::value);
        CHECK_EQUAL(7, sstl::max_value_for_nbits<3>::value);
        CHECK_EQUAL(15, sstl::max_value_for_nbits<4>::value);
        CHECK_EQUAL(31, sstl::max_value_for_nbits<5>::value);
        CHECK_EQUAL(63, sstl::max_value_for_nbits<6>::value);
        CHECK_EQUAL(127, sstl::max_value_for_nbits<7>::value);
        CHECK_EQUAL(255, sstl::max_value_for_nbits<8>::value);
        CHECK_EQUAL(511, sstl::max_value_for_nbits<9>::value);
        CHECK_EQUAL(1023, sstl::max_value_for_nbits<10>::value);
        CHECK_EQUAL(2047, sstl::max_value_for_nbits<11>::value);
        CHECK_EQUAL(4095, sstl::max_value_for_nbits<12>::value);
        CHECK_EQUAL(8191, sstl::max_value_for_nbits<13>::value);
        CHECK_EQUAL(16383, sstl::max_value_for_nbits<14>::value);
        CHECK_EQUAL(32767, sstl::max_value_for_nbits<15>::value);
        CHECK_EQUAL(65535, sstl::max_value_for_nbits<16>::value);
        CHECK_EQUAL(131071, sstl::max_value_for_nbits<17>::value);
        CHECK_EQUAL(262143, sstl::max_value_for_nbits<18>::value);
        CHECK_EQUAL(524287, sstl::max_value_for_nbits<19>::value);
        CHECK_EQUAL(1048575, sstl::max_value_for_nbits<20>::value);
        CHECK_EQUAL(2097151, sstl::max_value_for_nbits<21>::value);
        CHECK_EQUAL(4194303, sstl::max_value_for_nbits<22>::value);
        CHECK_EQUAL(8388607, sstl::max_value_for_nbits<23>::value);
        CHECK_EQUAL(16777215, sstl::max_value_for_nbits<24>::value);
        CHECK_EQUAL(33554431, sstl::max_value_for_nbits<25>::value);
        CHECK_EQUAL(67108863, sstl::max_value_for_nbits<26>::value);
        CHECK_EQUAL(134217727, sstl::max_value_for_nbits<27>::value);
        CHECK_EQUAL(268435455, sstl::max_value_for_nbits<28>::value);
        CHECK_EQUAL(536870911, sstl::max_value_for_nbits<29>::value);
        CHECK_EQUAL(1073741823, sstl::max_value_for_nbits<30>::value);
        CHECK_EQUAL(2147483647, sstl::max_value_for_nbits<31>::value);
        CHECK_EQUAL(4294967295, sstl::max_value_for_nbits<32>::value);
        CHECK_EQUAL(8589934591, sstl::max_value_for_nbits<33>::value);
        CHECK_EQUAL(17179869183, sstl::max_value_for_nbits<34>::value);
        CHECK_EQUAL(34359738367, sstl::max_value_for_nbits<35>::value);
        CHECK_EQUAL(68719476735, sstl::max_value_for_nbits<36>::value);
        CHECK_EQUAL(137438953471, sstl::max_value_for_nbits<37>::value);
        CHECK_EQUAL(274877906943, sstl::max_value_for_nbits<38>::value);
        CHECK_EQUAL(549755813887, sstl::max_value_for_nbits<39>::value);
        CHECK_EQUAL(1099511627775, sstl::max_value_for_nbits<40>::value);
        CHECK_EQUAL(2199023255551, sstl::max_value_for_nbits<41>::value);
        CHECK_EQUAL(4398046511103, sstl::max_value_for_nbits<42>::value);
        CHECK_EQUAL(8796093022207, sstl::max_value_for_nbits<43>::value);
        CHECK_EQUAL(17592186044415, sstl::max_value_for_nbits<44>::value);
        CHECK_EQUAL(35184372088831, sstl::max_value_for_nbits<45>::value);
        CHECK_EQUAL(70368744177663, sstl::max_value_for_nbits<46>::value);
        CHECK_EQUAL(140737488355327, sstl::max_value_for_nbits<47>::value);
        CHECK_EQUAL(281474976710655, sstl::max_value_for_nbits<48>::value);
        CHECK_EQUAL(562949953421311, sstl::max_value_for_nbits<49>::value);
        CHECK_EQUAL(1125899906842623, sstl::max_value_for_nbits<50>::value);
        CHECK_EQUAL(2251799813685247, sstl::max_value_for_nbits<51>::value);
        CHECK_EQUAL(4503599627370495, sstl::max_value_for_nbits<52>::value);
        CHECK_EQUAL(9007199254740991, sstl::max_value_for_nbits<53>::value);
        CHECK_EQUAL(18014398509481983, sstl::max_value_for_nbits<54>::value);
        CHECK_EQUAL(36028797018963967, sstl::max_value_for_nbits<55>::value);
        CHECK_EQUAL(72057594037927935, sstl::max_value_for_nbits<56>::value);
        CHECK_EQUAL(144115188075855871, sstl::max_value_for_nbits<57>::value);
        CHECK_EQUAL(288230376151711743, sstl::max_value_for_nbits<58>::value);
        CHECK_EQUAL(576460752303423487, sstl::max_value_for_nbits<59>::value);
        CHECK_EQUAL(1152921504606846975, sstl::max_value_for_nbits<60>::value);
        CHECK_EQUAL(2305843009213693951, sstl::max_value_for_nbits<61>::value);
        CHECK_EQUAL(4611686018427387903, sstl::max_value_for_nbits<62>::value);
        CHECK_EQUAL(9223372036854775807, sstl::max_value_for_nbits<63>::value);
        CHECK_EQUAL(static_cast<uint64_t>(18446744073709551615u), sstl::max_value_for_nbits<64>::value);

        // Check that the value types are correct.
        CHECK((std::is_same<uint8_t,  sstl::max_value_for_nbits<0>::value_type>::value));
        CHECK((std::is_same<uint8_t,  sstl::max_value_for_nbits<1>::value_type>::value));
        CHECK((std::is_same<uint8_t,  sstl::max_value_for_nbits<2>::value_type>::value));
        CHECK((std::is_same<uint8_t,  sstl::max_value_for_nbits<3>::value_type>::value));
        CHECK((std::is_same<uint8_t,  sstl::max_value_for_nbits<4>::value_type>::value));
        CHECK((std::is_same<uint8_t,  sstl::max_value_for_nbits<5>::value_type>::value));
        CHECK((std::is_same<uint8_t,  sstl::max_value_for_nbits<6>::value_type>::value));
        CHECK((std::is_same<uint8_t,  sstl::max_value_for_nbits<7>::value_type>::value));
        CHECK((std::is_same<uint8_t,  sstl::max_value_for_nbits<8>::value_type>::value));
        CHECK((std::is_same<uint16_t, sstl::max_value_for_nbits<9>::value_type>::value));
        CHECK((std::is_same<uint16_t, sstl::max_value_for_nbits<10>::value_type>::value));
        CHECK((std::is_same<uint16_t, sstl::max_value_for_nbits<11>::value_type>::value));
        CHECK((std::is_same<uint16_t, sstl::max_value_for_nbits<12>::value_type>::value));
        CHECK((std::is_same<uint16_t, sstl::max_value_for_nbits<13>::value_type>::value));
        CHECK((std::is_same<uint16_t, sstl::max_value_for_nbits<14>::value_type>::value));
        CHECK((std::is_same<uint16_t, sstl::max_value_for_nbits<15>::value_type>::value));
        CHECK((std::is_same<uint16_t, sstl::max_value_for_nbits<16>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<17>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<18>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<19>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<20>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<21>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<22>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<23>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<24>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<25>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<26>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<27>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<28>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<29>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<30>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<31>::value_type>::value));
        CHECK((std::is_same<uint32_t, sstl::max_value_for_nbits<32>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<33>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<34>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<35>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<36>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<37>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<38>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<39>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<40>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<41>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<42>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<43>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<44>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<45>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<46>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<47>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<48>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<49>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<50>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<51>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<52>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<53>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<54>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<55>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<56>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<57>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<58>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<59>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<60>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<61>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<62>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<63>::value_type>::value));
        CHECK((std::is_same<uint64_t, sstl::max_value_for_nbits<64>::value_type>::value));
    }
  };
}
}
