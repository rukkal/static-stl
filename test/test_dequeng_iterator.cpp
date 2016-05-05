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
#include "test_dequeng_utility.h"

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

using iterator_type = typename sstl::dequeng<value_type>::iterator;
using const_iterator_type = typename sstl::dequeng<value_type>::const_iterator;

TEST_CASE("_dequeng_iterator")
{
   auto d = make_noncontiguous_deque<value_type>({0, 1, 2, 3});

   SECTION("default constructor")
   {
      iterator_type nonconst_it;
      const_iterator_type const_it;
   }

   SECTION("non-const iterator to const iterator conversion")
   {
      iterator_type nonconst_it;
      const_iterator_type const_it = nonconst_it;
      (void) const_it;
   }

   SECTION("non-const iterator and const iterator comparison")
   {
      REQUIRE(d.cbegin() == d.begin());
      REQUIRE(d.cbegin() != d.end());
      REQUIRE(d.cbegin() < d.end());
      REQUIRE(d.cend() > d.begin());
      REQUIRE(d.cbegin() <= d.end());
      REQUIRE(d.cend() >= d.begin());
   }

   SECTION("expressions required by InputIterator concept")
   {
      SECTION("a!=b returns 'contextually convertible to bool'")
      {
         REQUIRE(d.begin() != d.end());
      }

      SECTION("a==b returns 'contextually convertible to bool'")
      {
         REQUIRE(d.begin() == d.begin());
      }

      SECTION("*i returns 'reference or convertible to value_type'")
      {
         iterator_type it = d.begin();
         REQUIRE((*it).value == 0);
      }

      SECTION("i->m")
      {
         iterator_type it = d.begin();
         REQUIRE(it->value == 0);
      }

      SECTION("++i returns It&")
      {
         iterator_type it = d.begin();
         iterator_type& it_ref_1 = ++it;
         REQUIRE(it->value == 1);
         REQUIRE(it_ref_1->value == 1);

         iterator_type& it_ref_2 = ++it;
         REQUIRE(it->value == 2);
         REQUIRE(it_ref_2->value == 2);
      }

      SECTION("(void) i++")
      {
         iterator_type it = d.begin();
         REQUIRE(it->value == 0);

         it++;
         REQUIRE(it->value == 1);
      }

      SECTION("*i++ returns 'convertible to value_type'")
      {
         iterator_type it = d.begin();
         value_type v0 = *it++;
         REQUIRE(v0.value == 0);

         value_type v1 = *it++;
         REQUIRE(v1.value == 1);
      }
   }

   SECTION("expressions required by ForwardIterator concept")
   {
      SECTION("f++ returns It")
      {
         iterator_type it = d.begin();
         iterator_type it_0 = it++;
         REQUIRE(it_0->value == 0);
         REQUIRE(it->value == 1);

         iterator_type it_1 = it++;
         REQUIRE(it_1->value == 1);
         REQUIRE(it->value == 2);
      }

      SECTION("*f++ returns reference")
      {
         iterator_type it = d.begin();
         value_type& value0 = *it++;
         REQUIRE(value0.value == 0);

         value_type& value1 = *it++;
         REQUIRE(value1.value == 1);
      }
   }

   SECTION("expressions required by BidirectionalIterator concept")
   {
      SECTION("--b returns It&")
      {
         iterator_type it = d.end();
         iterator_type& it_ref_3 = --it;
         REQUIRE(it->value == 3);
         REQUIRE(it_ref_3->value == 3);

         iterator_type& it_ref_2 = --it;
         REQUIRE(it->value == 2);
         REQUIRE(it_ref_2->value == 2);
      }

      SECTION("b-- returns 'convertible to const It&'")
      {
         iterator_type it = d.end();
         it--;
         const iterator_type& it_3 = it--;
         REQUIRE(it_3->value == 3);
         REQUIRE(it->value == 2);

         const iterator_type& it_2 = it--;
         REQUIRE(it_2->value == 2);
         REQUIRE(it->value == 1);
      }

      SECTION("*b-- returns 'convertible to reference'")
      {
         iterator_type it = d.end();
         --it;
         value_type& value3 = *it--;
         REQUIRE(value3.value == 3);

         value_type& value2 = *it++;
         REQUIRE(value2.value == 2);
      }
   }

   SECTION("expressions required by RandomAccessIterator concept")
   {
      SECTION("r+=n returns It&")
      {
         {
            iterator_type it = d.begin();

            iterator_type& it_ref_2 = it+=2;
            REQUIRE(it->value == 2);
            REQUIRE(it_ref_2->value == 2);

            iterator_type& it_ref_end = it+=2;
            REQUIRE(it == d.end());
            REQUIRE(it_ref_end == d.end());
         }
         {
            iterator_type it = d.end();

            iterator_type& it_ref_2 = it+=(-2);
            REQUIRE(it->value == 2);
            REQUIRE(it_ref_2->value == 2);

            iterator_type& it_ref_begin = it+=(-2);
            REQUIRE(it == d.begin());
            REQUIRE(it_ref_begin == d.begin());
         }
      }

      SECTION("r+n / n+r returns It")
      {
         iterator_type it_0 = d.begin();

         iterator_type it_1 = it_0+1;
         REQUIRE(it_1->value == 1);

         iterator_type it_end = 4+it_0;
         REQUIRE(it_end == d.end());

         iterator_type it_3 = it_end+(-1);
         REQUIRE(it_3->value == 3);

         iterator_type it_begin = (-4)+it_end;
         REQUIRE(it_begin == d.begin());
      }

      SECTION("r-=n returns It&")
      {
         {
            iterator_type it = d.end();

            iterator_type& it_ref_2 = (it-=2);
            REQUIRE(it->value == 2);
            REQUIRE(it_ref_2->value == 2);

            iterator_type& it_ref_begin = (it-=2);
            REQUIRE(it == d.begin());
            REQUIRE(it_ref_begin == d.begin());
         }
         {
            iterator_type it = d.begin();

            iterator_type& it_ref_2 = (it-=(-2));
            REQUIRE(it->value == 2);
            REQUIRE(it_ref_2->value == 2);

            iterator_type& it_ref_end = (it-=(-2));
            REQUIRE(it == d.end());
            REQUIRE(it_ref_end == d.end());
         }
      }

      SECTION("r-n returns It")
      {
         iterator_type it_end = d.end();
         iterator_type it_3 = it_end-1;
         REQUIRE(it_3->value == 3);

         iterator_type it_begin = it_end-4;
         REQUIRE(it_begin == d.begin());

         iterator_type it_1 = it_begin-(-1);
         REQUIRE(it_1->value == 1);

         it_end = it_begin-(-4);
         REQUIRE(it_end == d.end());
      }

      SECTION("a-b returns difference_type")
      {
         REQUIRE(d.begin() - d.begin() == 0);
         REQUIRE(d.end() - d.end() == 0);

         REQUIRE(d.end() - d.begin() == 4);
         REQUIRE(d.begin() - d.end() == -4);

         REQUIRE((d.begin()+1) - d.begin() == 1);
         REQUIRE(d.begin() - (d.begin() + 1) == -1);

         REQUIRE(d.end() - (d.end()-1) == 1);
         REQUIRE((d.end()-1) - d.end() == -1);
      }

      SECTION("r[n] returns 'convertible to reference'")
      {
         value_type& value_type0 = d.begin()[0];
         value_type& value_type1 = d.begin()[1];
         value_type& value_type2 = d.begin()[2];
         value_type& value_type3 = d.begin()[3];

         REQUIRE(value_type0.value == 0);
         REQUIRE(value_type1.value == 1);
         REQUIRE(value_type2.value == 2);
         REQUIRE(value_type3.value == 3);

         value_type0.value = 10;
         value_type1.value = 11;
         value_type2.value = 13;
         value_type3.value = 12;

         REQUIRE(value_type0.value == 10);
         REQUIRE(value_type1.value == 11);
         REQUIRE(value_type2.value == 13);
         REQUIRE(value_type3.value == 12);
      }

      SECTION("a<b returns 'contextually convertible to bool'")
      {
         REQUIRE(d.begin() < d.begin()+1);
         REQUIRE(!(d.begin() < d.begin()));
         REQUIRE(!(d.begin()+1 < d.begin()));

         REQUIRE(d.end()-1 < d.end());
         REQUIRE(!(d.end() < d.end()));
         REQUIRE(!(d.end() < d.end()-1));
      }

      SECTION("a>b returns 'contextually convertible to bool'")
      {
         REQUIRE(d.begin()+1 > d.begin());
         REQUIRE(!(d.begin() > d.begin()));
         REQUIRE(!(d.begin() > d.begin()+1));

         REQUIRE(d.end() > d.end()-1);
         REQUIRE(!(d.end() > d.end()));
         REQUIRE(!(d.end()-1 > d.end()));
      }

      SECTION("a<=b returns 'contextually convertible to bool'")
      {
         REQUIRE(d.begin() <= d.begin());
         REQUIRE(d.begin() <= d.begin()+1);
         REQUIRE(!(d.begin()+1 <= d.begin()));

         REQUIRE(d.end() <= d.end());
         REQUIRE(d.end()-1 <= d.end());
         REQUIRE(!(d.end() <= d.end()-1));
      }

      SECTION("a>=b returns 'contextually convertible to bool'")
      {
         REQUIRE(d.begin() >= d.begin());
         REQUIRE(d.begin()+1 >= d.begin());
         REQUIRE(!(d.begin() >= d.begin()+1));

         REQUIRE(d.end() >= d.end());
         REQUIRE(d.end() >= d.end()-1);
         REQUIRE(!(d.end()-1 >= d.end()));
      }
   }

   SECTION("expressions required by OutputIterator concept")
   {
      SECTION("*o = value")
      {
         iterator_type it = d.begin();
         REQUIRE(it->value == 0);
         *it = 10;
         REQUIRE(it->value == 10);
      }

      SECTION("*r++ = value")
      {
         iterator_type it = d.begin();

         iterator_type it_10 = it;
         *it++ = 10;
         iterator_type it_11 = it;
         *it++ = 11;
         iterator_type it_2 = it;

         REQUIRE(it_10->value == 10);
         REQUIRE(it_11->value == 11);
         REQUIRE(it_2->value == 2);
      }
   }
}

}
