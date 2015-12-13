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

struct callable_type
{
   virtual void operator()(int& i) { i=EXPECTED_OUTPUT_PARAMETER; };
   void operation(int& i) { i=EXPECTED_OUTPUT_PARAMETER; };
};

void foo(int& i) { i = EXPECTED_OUTPUT_PARAMETER; }

struct base_type {};
struct derived_type : base_type {};

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
      sstl::function<void(int&)> f = callable_type{};
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
      sstl::function<void(callable_type*, int&), 2*sizeof(void*)> f = &callable_type::operation;
      callable_type c;
      int i = 3;
      f(&c, i);
      REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
   }
   #endif

   SECTION("callable is result of std::mem_fn")
   {
      sstl::function<void(callable_type*, int&), 2*sizeof(void*)> f = std::mem_fn(&callable_type::operation);
      callable_type c;
      int i = 3;
      f(&c, i);
      REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
   }

   SECTION("callable is result of std::bind")
   {
      callable_type c;
      sstl::function<void(int&), 3*sizeof(void*)> f = std::bind(&callable_type::operation, &c, std::placeholders::_1);
      int i = 3;
      f(i);
      REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
   }

   SECTION("callable with covariant return type")
   {
      SECTION("construction")
      {
         SECTION("callable is sstl::function")
         {
            auto rhs = sstl::function<derived_type()>{};
            auto f = sstl::function<base_type(), 2*sizeof(void*)>{rhs};
         }
         SECTION("callable is closure")
         {
            auto rhs = [](){ return derived_type{}; };
            auto f = sstl::function<base_type()>{rhs};
         }
      }
      SECTION("assignment")
      {
         auto f = sstl::function<base_type(), 2*sizeof(void*)>{};
         SECTION("callable is sstl::function")
         {
            auto rhs = sstl::function<derived_type()>{};
            f = rhs;
         }
         SECTION("callable is closure")
         {
            auto rhs = [](){ return derived_type{}; };
            f = rhs;
         }
      }
   }

   SECTION("return value")
   {
      sstl::function<int&(int&)> f = [](int& t) ->int& { return t; };
      int i;
      int& ri = f(i);
      REQUIRE(std::addressof(ri) == std::addressof(i));
   }

   SECTION("operator bool")
   {
      sstl::function<void()> f;
      REQUIRE(static_cast<bool>(f) == false);
      f = [](){};
      REQUIRE(static_cast<bool>(f) == true);
   }

   SECTION("number of constructions of argument")
   {
      counted_type c;
      counted_type::reset_counts();

      SECTION("by-reference parameter generates zero constructions")
      {
         sstl::function<void(counted_type&)> f = [](counted_type&){};
         f(c);
         REQUIRE(counted_type::constructions == 0);
      }

      SECTION("by-value parameter with lvalue reference argument generates one copy construction")
      {
         sstl::function<void(counted_type)> f = [](counted_type){};
         f(c);
         REQUIRE(counted_type::constructions == 1);
         REQUIRE(counted_type::copy_constructions == 1);
      }

      SECTION("by-value parameter with rvalue reference argument generates one copy construction")
      {
         sstl::function<void(counted_type)> f = [](counted_type){};
         f(std::move(c));
         REQUIRE(counted_type::constructions == 1);
         REQUIRE(counted_type::copy_constructions == 1);
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
