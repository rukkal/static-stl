/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <memory>
#include <functional>
#include "sstl/function.h"
#include "counted_type.h"

namespace sstl
{
namespace test
{

static const int EXPECTED_OUTPUT_PARAMETER = 101;

class Callable
{
public:
   void operator()(int& i) { i=EXPECTED_OUTPUT_PARAMETER; };

   void operation(int& i) { i=EXPECTED_OUTPUT_PARAMETER; };
};

void foo(int& i) { i = EXPECTED_OUTPUT_PARAMETER; }

TEST_CASE("function")
{
   SECTION("default constructor")
   {
      {
         sstl::function<void()> f;
      }
      {
         sstl::function<void(int)> f;
      }
      {
         sstl::function<void(int&)> f;
      }
      {
         sstl::function<void(int, int)> f;
      }
      {
         sstl::function<int(int, int)> f;
      }
      {
         sstl::function<int&(int&, int&)> f;
      }
   }

   SECTION("callable is free function")
   {
      sstl::function<void(int&)> f = foo;
      int i = 3;
      f(i);
      REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
   }

   SECTION("callable is function object")
   {
      sstl::function<void(int&)> f = Callable{};
      int i = 3;
      f(i);
      REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
   }

   SECTION("callable is closure")
   {
      sstl::function<void(int&)> f = [](int& i){ i=EXPECTED_OUTPUT_PARAMETER; };
      int i = 3;
      f(i);
      REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
   }

   #if 0
   SECTION("callable is member function pointer")
   {
      REQUIRE(false); // implement
      sstl::function<void(Callable*, int&), 2*sizeof(void*)> f = &Callable::operation;
      Callable c;
      int i = 3;
      f(&c, i);
      REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
   }
   #endif

   SECTION("callable is result of std::mem_fn")
   {
      sstl::function<void(Callable*, int&), 2*sizeof(void*)> f = std::mem_fn(&Callable::operation);
      Callable c;
      int i = 3;
      f(&c, i);
      REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
   }

   SECTION("callable is result of std::bind")
   {
      Callable c;
      sstl::function<void(int&), 3*sizeof(void*)> f = std::bind(&Callable::operation, &c, std::placeholders::_1);
      int i = 3;
      f(i);
      REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
   }

   SECTION("return value")
   {
      sstl::function<int&(int&)> f = [](int& t) ->int& { return t; };
      int i;
      int& ri = f(i);
      REQUIRE(std::addressof(ri) == std::addressof(i));
   }

   SECTION("number of constructions of argument")
   {
      {
         sstl::function<void(counted_type&)> f = [](counted_type&){};

         counted_type c;
         counted_type::reset_counts();
         f(c);

         REQUIRE(counted_type::copy_constructions == 0);
         REQUIRE(counted_type::move_constructions == 0);
      }

      {
         sstl::function<void(counted_type)> f = [](counted_type){};

         counted_type c;
         counted_type::reset_counts();
         f(c);

         REQUIRE(counted_type::copy_constructions == 1);
         REQUIRE(counted_type::move_constructions == 1);
      }
   }

   SECTION("number of constructions of underlying callable object")
   {
      {
         counted_type c;
         counted_type::reset_counts();

         sstl::function<void()> f = c;

         REQUIRE(counted_type::copy_constructions == 1);
         REQUIRE(counted_type::move_constructions == 0);
      }

      {
         counted_type::reset_counts();

         sstl::function<void()> f = counted_type{};

         REQUIRE(counted_type::copy_constructions == 0);
         REQUIRE(counted_type::move_constructions == 1);
      }
   }

   SECTION("deep construction")
   {
      sstl::function<void()> f = counted_type();

      {
         counted_type::reset_counts();
         auto f2 = f;
         REQUIRE(counted_type::copy_constructions == 1);
         REQUIRE(counted_type::move_constructions == 0);
      }

      {
         counted_type::reset_counts();
         auto f2 = std::move(f);
         REQUIRE(counted_type::copy_constructions == 0);
         REQUIRE(counted_type::move_constructions == 1);
      }
   }

   SECTION("deep assignment")
   {
      sstl::function<void()> f = counted_type();

      {
         sstl::function<void()> f2;
         counted_type::reset_counts();
         f2 = f;
         REQUIRE(counted_type::copy_constructions == 1);
         REQUIRE(counted_type::move_constructions == 0);
      }

      {
         sstl::function<void()> f2;
         counted_type::reset_counts();
         f2 = std::move(f);
         REQUIRE(counted_type::copy_constructions == 0);
         REQUIRE(counted_type::move_constructions == 1);
      }
   }

   SECTION("deep destruction")
   {
      {
         sstl::function<void()> f = counted_type();
         counted_type::reset_counts();
      }
      REQUIRE(counted_type::destructions == 1);

      {
         sstl::function<void()> f = counted_type();
         counted_type::reset_counts();
         f = [](){};
         REQUIRE(counted_type::destructions == 1);
      }

      {
         sstl::function<void()> to = counted_type();
         sstl::function<void()> from = counted_type();
         counted_type::reset_counts();
         to = std::move(from);
         REQUIRE(counted_type::destructions == 1);
      }
   }

   SECTION("size (let's keep it under control)")
   {
      using function_type = sstl::function<void()>;
      REQUIRE(sizeof(function_type) == 2*sizeof(void*));
   }
}

}
}
