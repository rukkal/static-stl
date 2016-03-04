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
#include <sstl/function.h>
#include <sstl/__internal/_preprocessor.h>
#include "utility.h"
#include "counted_type.h"

namespace sstl_test
{

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
   SECTION("user cannot directly construct the base class")
   {
      #if !_sstl_is_gcc()
         REQUIRE(!std::is_default_constructible<sstl::function<void()>>::value);
      #endif
      REQUIRE(!std::is_copy_constructible<sstl::function<void()>>::value);
      REQUIRE(!std::is_move_constructible<sstl::function<void()>>::value);
   }

   SECTION("user cannot directly destroy the base class")
   {
      #if !_is_msvc() //MSVC (VS2013) has a buggy implementation of std::is_destructible
      REQUIRE(!std::is_destructible<sstl::function<void()>>::value);
      #endif
   }

   SECTION("default constructor")
   {
      {
         sstl::function<void(), 0> f;
      }
      {
         sstl::function<void(int), 0> f;
      }
      {
         sstl::function<void(int&), 0> f;
      }
      {
         sstl::function<void(int, int), 0> f;
      }
      {
         sstl::function<int(int, int), 0> f;
      }
      {
         sstl::function<int&(int&, int&), 0> f;
      }
   }

   SECTION("copy constructor")
   {
      SECTION("rhs's target invalid")
      {
         auto rhs = sstl::function<int(), 0>{};
         auto lhs = sstl::function<int(), 0>{ rhs };
         REQUIRE(lhs == false);
      }
      SECTION("rhs's target valid")
      {
         auto rhs = sstl::function<int(), 0>{ [](){ return 101; } };
         auto lhs = sstl::function<int(), 0>{ rhs };
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("number of underlying target's constructions")
      {
         auto rhs = sstl::function<void(), sizeof(counted_type)>{ counted_type{} };
         counted_type::reset_counts();
         auto lhs = rhs;
         REQUIRE(counted_type::check().copy_constructions(1));
      }
   }

   SECTION("move constructor")
   {
      SECTION("rhs's target invalid")
      {
         auto rhs = sstl::function<int(), 0>{};
         auto lhs = sstl::function<int(), 0>{ std::move(rhs) };
         REQUIRE(lhs == false);
      }
      SECTION("rhs's target valid")
      {
         auto rhs = sstl::function<int(), 0>{ [](){ return 101; } };
         auto lhs = sstl::function<int(), 0>{ std::move(rhs) };
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("number of underlying target's constructions")
      {
         auto rhs = sstl::function<void(), sizeof(counted_type)>{ counted_type{} };
         counted_type::reset_counts();
         auto lhs = std::move(rhs);
         REQUIRE(counted_type::check().move_constructions(1));
      }
   }

   SECTION("template constructor")
   {
      SECTION("target is free function")
      {
         auto f = sstl::function<void(int&), sizeof(&foo)>{ foo };
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is function object")
      {
         auto f = sstl::function<void(int&), sizeof(callable_type)>{ callable_type{} };
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is closure")
      {
         auto f = sstl::function<void(int&), 0>{ [](int& i){ i=EXPECTED_OUTPUT_PARAMETER; } };
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is pointer to member function")
      {
         callable_type c;
         SECTION("first parameter is 'this' pointer")
         {
            auto f = sstl::function<void(callable_type*, int&), sizeof(&callable_type::operation)>{ &callable_type::operation };
            int i = 3;
            f(&c, i);
            REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
         }
         SECTION("first parameter is 'this' reference")
         {
            auto f = sstl::function<void(callable_type&, int&), sizeof(&callable_type::operation)>{ &callable_type::operation };
            int i = 3;
            f(c, i);
            REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
         }
      }
      SECTION("target is result of std::mem_fn")
      {
         auto target = std::mem_fn(&callable_type::operation);
         auto f = sstl::function<void(callable_type*, int&), sizeof(target)>{ target };
         callable_type c;
         int i = 3;
         f(&c, i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is result of std::bind")
      {
         callable_type c;
         auto target = std::bind(&callable_type::operation, &c, std::placeholders::_1);
         auto f = sstl::function<void(int&), sizeof(target)>{ target };
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
            sstl::function<void(), sizeof(target)>{ target };
            REQUIRE(counted_type::check().copy_constructions(1));
         }
         SECTION("target is rvalue")
         {
            sstl::function<void(), sizeof(target)>{ std::move(target) };
            REQUIRE(counted_type::check().move_constructions(1));
         }
      }
   }

   SECTION("destructor")
   {
      SECTION("underlying target is destroyed")
      {
         {
            sstl::function<void(), sizeof(counted_type)> f = counted_type();
            counted_type::reset_counts();
         }
         REQUIRE(counted_type::check().destructions(1));
      }
   }

   SECTION("copy assignment")
   {
      SECTION("lhs's target is invalid and rhs's target is invalid")
      {
         auto rhs = sstl::function<int(), 0>{};
         auto lhs = sstl::function<int(), 0>{};
         lhs = rhs;
         REQUIRE(lhs == false);
      }
      SECTION("lhs's target is valid and rhs's target is invalid")
      {
         auto rhs = sstl::function<int(), 0>{};
         auto lhs = sstl::function<int(), 0>{ [](){ return 101; } };
         lhs = rhs;
         REQUIRE(lhs == false);
      }
      SECTION("lhs's target is invalid and rhs's target is valid")
      {
         auto rhs = sstl::function<int(), 0>{ [](){ return 101; } };
         auto lhs = sstl::function<int(), 0>{};
         lhs = rhs;
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("lhs's target is valid and rhs's target is valid")
      {
         auto rhs = sstl::function<int(), 0>{ [](){ return 101; } };
         auto lhs = sstl::function<int(), 0>{ [](){ return 0; } };
         lhs = rhs;
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      #if !_is_msvc()
      //note: the test breaks because MSVC elides the assignment (although the optimizer is turned off)
      //even when the sstl::function instances are escaped (compiler is forced not to optimize them)
      SECTION("number of underlying target's constructions")
      {
         auto rhs = sstl::function<void(), sizeof(counted_type)>{ counted_type() };
         auto lhs = sstl::function<void(), sizeof(counted_type)>{};
         counted_type::reset_counts();
         lhs = rhs;
         REQUIRE(counted_type::check().copy_constructions(1).destructions(0));
      }
      #endif
      SECTION("number of underlying target's destructions")
      {
         auto lhs = sstl::function<void(), sizeof(counted_type)>{ counted_type() };
         counted_type::reset_counts();
         lhs = [](){};
         REQUIRE(counted_type::check().constructions(0).destructions(1));
      }
   }

   SECTION("move assignment")
   {
      SECTION("lhs's target is invalid and rhs's target is invalid")
      {
         auto rhs = sstl::function<int(), 0>{};
         auto lhs = sstl::function<int(), 0>{};
         lhs = std::move(rhs);
         REQUIRE(lhs == false);
      }
      SECTION("lhs's target is valid and rhs's target is invalid")
      {
         auto rhs = sstl::function<int(), 0>{};
         auto lhs = sstl::function<int(), 0>{ [](){ return 101; } };
         lhs = std::move(rhs);
         REQUIRE(lhs == false);
      }
      SECTION("lhs's target is invalid and rhs's target is valid")
      {
         auto rhs = sstl::function<int(), 0>{ [](){ return 101; } };
         auto lhs = sstl::function<int(), 0>{};
         lhs = std::move(rhs);
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("lhs's target is valid and rhs's target is valid")
      {
         auto rhs = sstl::function<int(), 0>{ [](){ return 101; } };
         auto lhs = sstl::function<int(), 0>{ [](){ return 0; } };
         lhs = std::move(rhs);
         REQUIRE(lhs == true);
         REQUIRE(lhs() == 101);
      }
      SECTION("number of underlying target's constructions")
      {
         auto rhs = sstl::function<void(), sizeof(counted_type)>{ counted_type() };
         auto lhs = sstl::function<void(), sizeof(counted_type)>{};
         counted_type::reset_counts();
         lhs = std::move(rhs);
         REQUIRE(counted_type::check().move_constructions(1));
      }
      SECTION("number of underlying target's destructions")
      {
         auto rhs = sstl::function<void(), sizeof(counted_type)>{};
         auto lhs = sstl::function<void(), sizeof(counted_type)>{ counted_type() };
         counted_type::reset_counts();
         lhs = std::move(rhs);
         REQUIRE(counted_type::check().destructions(1));
      }
   }

   SECTION("template assignment")
   {
      SECTION("target is free function")
      {
         auto f = sstl::function<void(int&), sizeof(&foo)>{};
         f = foo;
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is function object")
      {
         auto f = sstl::function<void(int&), sizeof(callable_type)>{};
         f = callable_type{};
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is pointer to member function")
      {
         auto f = sstl::function<void(callable_type&, int&), sizeof(&callable_type::operation)>{};
         f = &callable_type::operation;
         callable_type c;
         int i = 3;
         f(c, i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("target is closure")
      {
         auto f = sstl::function<void(int&), 0>{};
         f = [](int& i){ i=EXPECTED_OUTPUT_PARAMETER; };
         int i = 3;
         f(i);
         REQUIRE(i == EXPECTED_OUTPUT_PARAMETER);
      }
      SECTION("number of argument target's constructions")
      {
         auto rhs = counted_type{};
         auto lhs = sstl::function<void(), sizeof(counted_type)>{};
         counted_type::reset_counts();

         SECTION("rhs is lvalue")
         {
            lhs = rhs;
            REQUIRE(counted_type::check().copy_constructions(1));
         }
         SECTION("ths is rvalue")
         {
            lhs = std::move(rhs);
            REQUIRE(counted_type::check().move_constructions(1));
         }
      }
   }

   SECTION("target with covariant return type")
   {
      SECTION("construction")
      {
         SECTION("target is sstl::function")
         {
            auto rhs = sstl::function<derived_type(), 0>{};
            auto lhs = sstl::function<base_type(), 0>{ rhs };
         }
         SECTION("target is closure")
         {
            auto rhs = [](){ return derived_type{}; };
            auto lhs = sstl::function<base_type(), 0>{rhs};
         }
      }
      SECTION("assignment")
      {
         SECTION("target is sstl::function")
         {
            auto rhs = sstl::function<derived_type(), 0>{};
            auto lhs = sstl::function<base_type(), 0>{};
            lhs = rhs;
         }
         SECTION("target is closure")
         {
            auto rhs = [](){ return derived_type{}; };
            auto lhs = sstl::function<base_type(), 0>{};
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
            auto lhs = sstl::function<void(derived_type), 0>{ rhs };
         }
         SECTION("target is sstl::function")
         {
            auto rhs = sstl::function<void(base_type), 0>{};
            auto lhs = sstl::function<void(derived_type), 0>{ rhs };
         }
      }
      SECTION("assignment")
      {
         SECTION("target is closure")
         {
            auto rhs = [](base_type){};
            auto lhs = sstl::function<void(derived_type), 0>{ rhs };
         }
         SECTION("target is sstl::function")
         {
            auto rhs = sstl::function<void(base_type), 0>{};
            auto lhs = sstl::function<void(derived_type), 0>{ rhs };
         }
      }
   }

   SECTION("return value")
   {
      sstl::function<int&(int&), 0> f = [](int& t) ->int& { return t; };
      int i;
      int& ri = f(i);
      REQUIRE(&ri == &i);
   }

   SECTION("operator bool")
   {
      SECTION("invalid sstl::function evaluates to false")
      {
         auto f = sstl::function<void(), 0>{};
         REQUIRE(static_cast<bool>(f) == false);
      }
      SECTION("valid sstl::function evaluates to true")
      {
         auto f = sstl::function<void(), 0>{ [](){} };
         REQUIRE(static_cast<bool>(f) == true);
      }
      SECTION("after assignment from invalid sstl::function evaluates to false")
      {
         auto f = sstl::function<void(), 0>{ [](){} };
         REQUIRE(static_cast<bool>(f) == true);
         f = sstl::function<void(), 0>{};
         REQUIRE(static_cast<bool>(f) == false);
      }
      SECTION("after assignment from valid sstl::function evaluates to true")
      {
         auto f = sstl::function<void(), 0>{ };
         REQUIRE(static_cast<bool>(f) == false);
         f = sstl::function<void(), 0>{ [](){} };
         REQUIRE(static_cast<bool>(f) == true);
      }
   }

   SECTION("number of constructions of argument")
   {
      counted_type c;
      counted_type::reset_counts();

      SECTION("by-reference parameter generates zero constructions")
      {
         sstl::function<void(counted_type&), 0> f = [](counted_type&){};
         f(c);
         REQUIRE(counted_type::check().constructions(0));
      }
      SECTION("by-value parameter with lvalue reference argument generates one copy construction")
      {
         sstl::function<void(counted_type), 0> f = [](counted_type){};
         f(c);
         REQUIRE(counted_type::check().copy_constructions(1));
      }
      SECTION("by-value parameter with rvalue reference argument generates one copy construction")
      {
         sstl::function<void(counted_type), 0> f = [](counted_type){};
         f(std::move(c));
         REQUIRE(counted_type::check().copy_constructions(1));
      }
   }

   SECTION("constness")
   {
      struct nonconst_call_operator_type
      {
         void operator()() {};
      };

      struct const_call_operator_type
      {
         void operator()() const {};
      };

      SECTION("sstl::function with const-call-operator target")
      {
         auto f = sstl::function<void(), 0>{ const_call_operator_type{} };
         f();
      }
      SECTION("const sstl::function with non-const-call-operator target")
      {
         const auto f = sstl::function<void(), 0>{ nonconst_call_operator_type{} };
         f();
      }
   }

   SECTION("size (let's keep it under control)")
   {
      using function_type = sstl::function<void(), 0>;
      REQUIRE(sizeof(function_type) == sizeof(void*));
   }
}

}
