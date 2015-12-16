/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <utility>
#include <memory>
#include <functional>
#include "sstl/function.h"
#include "counted_type.h"

namespace sstl
{
namespace test
{

         template<size_t> struct print;

static const int EXPECTED_OUTPUT_PARAMETER = 101;

struct callable_type
{
   void operator()(int& i) { i=EXPECTED_OUTPUT_PARAMETER; };
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

   SECTION("copy constructor")
   {
      SECTION("rhs's target invalid")
      {
         auto rhs = sstl::function<int()>{};
         auto lhs = sstl::function<int()>{ rhs };
         REQUIRE(lhs == false);
      }
      SECTION("rhs's target valid")
      {
         auto rhs = sstl::function<int()>{ [](){ return 101; } };
         auto lhs = sstl::function<int()>{ rhs };
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("number of underlying target's constructions")
      {
         auto rhs = sstl::function<void(), 2>{ counted_type{} };
         counted_type::reset_counts();
         auto lhs = rhs;
         REQUIRE(counted_type::constructions == 1);
         REQUIRE(counted_type::copy_constructions == 1);
      }
   }

   SECTION("move constructor")
   {
      SECTION("rhs's target invalid")
      {
         auto rhs = sstl::function<int()>{};
         auto lhs = sstl::function<int()>{ std::move(rhs) };
         REQUIRE(lhs == false);
      }
      SECTION("rhs's target valid")
      {
         auto rhs = sstl::function<int()>{ [](){ return 101; } };
         auto lhs = sstl::function<int()>{ std::move(rhs) };
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("number of underlying target's constructions")
      {
         auto rhs = sstl::function<void(), 2>{ counted_type{} };
         counted_type::reset_counts();
         auto lhs = std::move(rhs);
         REQUIRE(counted_type::constructions == 1);
         REQUIRE(counted_type::move_constructions == 1);
      }
   }

   SECTION("template constructor")
   {
      SECTION("target is free function")
      {
         auto f = sstl::function<void(int&), 2>{ foo };
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is function object")
      {
         auto f = sstl::function<void(int&)>{ callable_type{} };
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is closure")
      {
         auto f = sstl::function<void(int&)>{ [](int& i){ i=EXPECTED_OUTPUT_PARAMETER; } };
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is result of std::mem_fn")
      {
         auto f = sstl::function<void(callable_type*, int&), 3>{ std::mem_fn(&callable_type::operation) };
         callable_type c;
         int i = 3;
         f(&c, i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is result of std::bind")
      {
         callable_type c;
         auto target = std::bind(&callable_type::operation, &c, std::placeholders::_1);
         auto f = sstl::function<void(int&), 4>{ target };
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("number of argument target's constructions")
      {
         auto target = counted_type{};
         counted_type::reset_counts();

         SECTION("target is lvalue")
         {
            sstl::function<void(), 2>{ target };
            REQUIRE(counted_type::constructions == 1);
            REQUIRE(counted_type::copy_constructions == 1);
         }
         SECTION("target is rvalue")
         {
            sstl::function<void(), 2>{ std::move(target) };
            REQUIRE(counted_type::constructions == 1);
            REQUIRE(counted_type::move_constructions == 1);
         }
      }
   }

   SECTION("destructor")
   {
      SECTION("underlying target is destroyed")
      {
         {
            sstl::function<void(), 2> f = counted_type();
            counted_type::reset_counts();
         }
         REQUIRE(counted_type::destructions == 1);
      }
   }

   SECTION("copy assignment")
   {
      SECTION("lhs's target is invalid and rhs's target is invalid")
      {
         auto rhs = sstl::function<int()>{};
         auto lhs = sstl::function<int()>{};
         lhs = rhs;
         REQUIRE(lhs == false);
      }
      SECTION("lhs's target is valid and rhs's target is invalid")
      {
         auto rhs = sstl::function<int()>{};
         auto lhs = sstl::function<int()>{ [](){ return 101; } };
         lhs = rhs;
         REQUIRE(lhs == false);
      }
      SECTION("lhs's target is invalid and rhs's target is valid")
      {
         auto rhs = sstl::function<int()>{ [](){ return 101; } };
         auto lhs = sstl::function<int()>{};
         lhs = rhs;
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("lhs's target is valid and rhs's target is valid")
      {
         auto rhs = sstl::function<int()>{ [](){ return 101; } };
         auto lhs = sstl::function<int()>{ [](){ return 0; } };
         lhs = rhs;
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("number of underlying target's constructions")
      {
         auto rhs = sstl::function<void(), 2>{ counted_type() };
         auto lhs = sstl::function<void(), 2>{};
         counted_type::reset_counts();
         lhs = rhs;
         REQUIRE(counted_type::constructions == 1);
         REQUIRE(counted_type::copy_constructions == 1);
         REQUIRE(counted_type::destructions == 0);
      }
      SECTION("number of underlying target's destructions")
      {
         auto lhs = sstl::function<void(), 2>{ counted_type() };
         counted_type::reset_counts();
         lhs = [](){};
         REQUIRE(counted_type::destructions == 1);
         REQUIRE(counted_type::constructions == 0);
      }
   }

   SECTION("move assignment")
   {
      SECTION("lhs's target is invalid and rhs's target is invalid")
      {
         auto rhs = sstl::function<int()>{};
         auto lhs = sstl::function<int()>{};
         lhs = std::move(rhs);
         REQUIRE(lhs == false);
      }
      SECTION("lhs's target is valid and rhs's target is invalid")
      {
         auto rhs = sstl::function<int()>{};
         auto lhs = sstl::function<int()>{ [](){ return 101; } };
         lhs = std::move(rhs);
         REQUIRE(lhs == false);
      }
      SECTION("lhs's target is invalid and rhs's target is valid")
      {
         auto rhs = sstl::function<int()>{ [](){ return 101; } };
         auto lhs = sstl::function<int()>{};
         lhs = std::move(rhs);
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("lhs's target is valid and rhs's target is valid")
      {
         auto rhs = sstl::function<int()>{ [](){ return 101; } };
         auto lhs = sstl::function<int()>{ [](){ return 0; } };
         lhs = std::move(rhs);
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("number of underlying target's constructions")
      {
         auto rhs = sstl::function<void(), 2>{ counted_type() };
         auto lhs = sstl::function<void(), 2>{};
         counted_type::reset_counts();
         lhs = std::move(rhs);
         REQUIRE(counted_type::constructions == 1);
         REQUIRE(counted_type::move_constructions == 1);
         REQUIRE(counted_type::destructions == 0);
      }
      SECTION("number of underlying target's destructions")
      {
         auto rhs = sstl::function<void(), 2>{};
         auto lhs = sstl::function<void(), 2>{ counted_type() };
         counted_type::reset_counts();
         lhs = std::move(rhs);
         REQUIRE(counted_type::destructions == 1);
         REQUIRE(counted_type::constructions == 0);
      }
   }

   SECTION("template assignment")
   {
      SECTION("target is free function")
      {
         auto f = sstl::function<void(int&), 2>{};
         f = foo;
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is function object")
      {
         auto f = sstl::function<void(int&)>{};
         f = callable_type{};
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is closure")
      {
         auto f = sstl::function<void(int&)>{};
         f = [](int& i){ i=EXPECTED_OUTPUT_PARAMETER; };
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("number of argument target's constructions")
      {
         auto rhs = counted_type{};
         auto lhs = sstl::function<void(), 2>{};
         counted_type::reset_counts();

         SECTION("rhs is lvalue")
         {
            lhs = rhs;
            REQUIRE(counted_type::constructions == 1);
            REQUIRE(counted_type::copy_constructions == 1);
         }
         SECTION("ths is rvalue")
         {
            lhs = std::move(rhs);
            REQUIRE(counted_type::constructions == 1);
            REQUIRE(counted_type::move_constructions == 1);
         }
      }
   }

   SECTION("target with covariant return type")
   {
      SECTION("construction")
      {
         SECTION("target is sstl::function")
         {
            auto rhs = sstl::function<derived_type()>{};
            auto lhs = sstl::function<base_type()>{ rhs };
         }
         SECTION("target is closure")
         {
            auto rhs = [](){ return derived_type{}; };
            auto lhs = sstl::function<base_type()>{rhs};
         }
      }
      SECTION("assignment")
      {
         SECTION("target is sstl::function")
         {
            auto rhs = sstl::function<derived_type()>{};
            auto lhs = sstl::function<base_type()>{};
            lhs = rhs;
         }
         SECTION("target is closure")
         {
            auto rhs = [](){ return derived_type{}; };
            auto lhs = sstl::function<base_type()>{};
            lhs = rhs;
         }
      }
   }

   SECTION("target with contravariant parameter type")
   {
      SECTION("construction")
      {
         SECTION("target is closure")
         {
            auto rhs = [](base_type){};
            auto lhs = sstl::function<void(derived_type)>{ rhs };
         }
         SECTION("target is sstl::function")
         {
            auto rhs = sstl::function<void(base_type)>{};
            auto lhs = sstl::function<void(derived_type)>{ rhs };
         }
      }
      SECTION("assignment")
      {
         SECTION("target is closure")
         {
            auto rhs = [](base_type){};
            auto lhs = sstl::function<void(derived_type)>{ rhs };
         }
         SECTION("target is sstl::function")
         {
            auto rhs = sstl::function<void(base_type)>{};
            auto lhs = sstl::function<void(derived_type)>{ rhs };
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
      SECTION("invalid sstl::function evaluates to false")
      {
         auto f = sstl::function<void()>{};
         REQUIRE(static_cast<bool>(f) == false);
      }
      SECTION("valid sstl::function evaluates to true")
      {
         auto f = sstl::function<void()>{ [](){} };
         REQUIRE(static_cast<bool>(f) == true);
      }
      SECTION("after assignment from invalid sstl::function evaluates to false")
      {
         auto f = sstl::function<void()>{ [](){} };
         REQUIRE(static_cast<bool>(f) == true);
         f = sstl::function<void()>{};
         REQUIRE(static_cast<bool>(f) == false);
      }
      SECTION("after assignment from valid sstl::function evaluates to true")
      {
         auto f = sstl::function<void()>{ };
         REQUIRE(static_cast<bool>(f) == false);
         f = sstl::function<void()>{ [](){} };
         REQUIRE(static_cast<bool>(f) == true);
      }
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

   SECTION("size (let's keep it under control)")
   {
      using function_type = sstl::function<void()>;
      REQUIRE(sizeof(function_type) == sizeof(void*));
   }
}

}
}
