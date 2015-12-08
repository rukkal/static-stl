/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_FUNCTION__
#define _SSTL_FUNCTION__

#include <cstddef>
#include <type_traits>
#include <new>

namespace sstl
{
template<class TCallable, size_t CALLABLE_SIZE_BYTES = sizeof(void*)>
class function;

template<class TResult, class... TParams, size_t CALLABLE_SIZE_BYTES>
class function<TResult(TParams...), CALLABLE_SIZE_BYTES> final
{
public:
   function()
   {
      TResult(*dummy)(TParams...) = nullptr;
      new(buffer) internal_callable_imp<decltype(dummy)>(dummy);
   }

   function(const function& rhs)
   {
      rhs.get_internal_callable().copy_to_buffer(buffer);
   }

   function(function&& rhs)
   {
      rhs.get_internal_callable().move_to_buffer(buffer);
   }

   function& operator=(const function& rhs)
   {
      if (this != &rhs)
      {
         get_internal_callable().~internal_callable();
         rhs.get_internal_callable().copy_to_buffer(buffer);
      }
      return *this;
   }

   function& operator=(function&& rhs)
   {
      if (this != &rhs)
      {
         get_internal_callable().~internal_callable();
         rhs.get_internal_callable().move_to_buffer(buffer);
      }
      return *this;
   }

   template<
      class T,
      class = typename std::enable_if <
         !std::is_same<function, typename std::decay<T>::type>::value
   > ::type>
   function(T&& callable)
   {
      using TCallable = typename std::decay<T>::type;
      static_assert(
         sizeof(internal_callable_imp<TCallable>) <= sizeof(buffer),
         "Not enough memory available to store the wished callable target."
         "Hint: specify size of the callable target as an extra template argument");
      new(buffer)internal_callable_imp<TCallable>(std::forward<T>(callable));
   }

   ~function()
   {
      get_internal_callable().~internal_callable();
   }

   template<class... TArgs>
   TResult operator()(TArgs&&... args)
   {
      return get_internal_callable()(std::forward<TArgs>(args)...);
   }

private:
   struct internal_callable
   {
      virtual ~internal_callable() {}
      virtual TResult operator()(TParams...) = 0;
      virtual void copy_to_buffer(void*) const = 0;
      virtual void move_to_buffer(void*) = 0;
   };

   template<class T>
   struct internal_callable_imp : internal_callable
   {
      template<
         class U,
         class = typename std::enable_if<
            !std::is_same<internal_callable_imp, typename std::decay<U>::type>::value
      >::type>
      internal_callable_imp(U&& callable) : callable(std::forward<U>(callable))
      {
      }

      TResult operator()(TParams... params) override
      {
         // note: here by-value parameters are forwarded as rvalue references
         // (even if forwarding reference type deduction doesn't occur,
         // see reference collapsing rules),
         // whereas lvalue references are unchanged
         return callable(std::forward<TParams>(params)...);
      }

      void copy_to_buffer(void* b) const override
      {
         new(b) internal_callable_imp(callable);
      }

      void move_to_buffer(void* b) override
      {
         new(b) internal_callable_imp(std::move(callable));
      }

      T callable;
   };

private:
   internal_callable& get_internal_callable()
   {
      return *static_cast<internal_callable*>(static_cast<void*>(buffer));
   }

   const internal_callable& get_internal_callable() const
   {
      return *static_cast<const internal_callable*>(static_cast<const void*>(buffer));
   }

private:
   // the default storage size is large enough to store common callables such as
   // stateless closures, stateless function objects and function pointers
   static constexpr size_t VPTR_SIZE_BYTES{ sizeof(void*) };
   static constexpr size_t INTERNAL_CALLABLE_SIZE_BYTES{ CALLABLE_SIZE_BYTES + VPTR_SIZE_BYTES };
   uint8_t buffer[INTERNAL_CALLABLE_SIZE_BYTES];
};
}

#endif
