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
#include <algorithm>
#include <new>
#include <memory>

namespace sstl
{

namespace detail
{
   template<class T>
   struct make_const_ref_if_value
   {
      using type = const T&;
   };

   template<class T>
   struct make_const_ref_if_value<T&>
   {
      using type = T&;
   };

   template<class T>
   struct make_const_ref_if_value<T&&>
   {
      using type = T&&;
   };
}

template<class TTarget, size_t TARGET_SIZE_BYTES = sizeof(void*)>
class function;

template<class TResult, class... TParams, size_t TARGET_SIZE_BYTES>
class function<TResult(TParams...), TARGET_SIZE_BYTES> final
{
public:
   function() noexcept
   {
      clear_internal_callable();
   }

   function(const function& rhs)
   {
      construct_internal_callable(rhs);
   }

   function(function&& rhs)
   {
      construct_internal_callable(std::move(rhs));
   }

   template<
      class T,
      class TTarget = typename std::decay<T>::type,
      class = typename std::enable_if<
         !std::is_same<function, TTarget>::value
   >::type>
   function(T&& target) noexcept( (std::is_lvalue_reference<T>::value && std::is_nothrow_copy_constructible<TTarget>::value)
                                 || (!std::is_lvalue_reference<T>::value && std::is_nothrow_move_constructible<TTarget>::value))
   {
      static_assert(
         sizeof(internal_callable_imp<TTarget>) <= sizeof(buffer),
         "Not enough memory available to store the wished target."
         "Hint: specify size of the target as an extra template argument");
      new(buffer)internal_callable_imp<TTarget>(std::forward<T>(target));
   }

   function& operator=(const function& rhs)
   {
      assign_internal_callable(rhs);
      return *this;
   }

   function& operator=(function&& rhs)
   {
      assign_internal_callable(std::move(rhs));
      return *this;
   }

   ~function()
   {
      if(!is_internal_callable_cleared())
      {
         get_internal_callable().~internal_callable();
      }
   }

   TResult operator()(typename detail::make_const_ref_if_value<TParams>::type... params)
   {
      return get_internal_callable()(std::forward<typename detail::make_const_ref_if_value<TParams>::type>(params)...);
   }

   operator bool() const noexcept
   {
      return !is_internal_callable_cleared();
   }

private:
   struct internal_callable
   {
      virtual ~internal_callable() {}
      virtual TResult operator()(typename detail::make_const_ref_if_value<TParams>::type...) = 0;
      // std::true_type -> copy construction
      void construct_to_buffer(std::true_type, void* buffer) const
      {
         copy_construct_to_buffer(buffer);
      }
      // std::false_type -> move construction
      void construct_to_buffer(std::false_type, void* buffer)
      {
         move_construct_to_buffer(buffer);
      }
      virtual void copy_construct_to_buffer(void*) const = 0;
      virtual void move_construct_to_buffer(void*) = 0;
   };

   template<class T>
   struct internal_callable_imp : internal_callable
   {
      template<
         class U,
         class = typename std::enable_if<
            !std::is_same<internal_callable, typename std::decay<U>::type>::value
      >::type>
      internal_callable_imp(U&& target) : target(std::forward<U>(target))
      {
      }

      TResult operator()(typename detail::make_const_ref_if_value<TParams>::type... params) override
      {
         return target(std::forward<typename detail::make_const_ref_if_value<TParams>::type>(params)...);
      }

      void copy_construct_to_buffer(void* b) const override
      {
         new(b) internal_callable_imp(target);
      }

      void move_construct_to_buffer(void* b) override
      {
         new(b) internal_callable_imp(std::move(target));
      }

      T target;
   };

private:
   template<class TFunction>
   void construct_internal_callable(TFunction&& rhs)
   {
      if(!rhs.is_internal_callable_cleared())
      {
         using is_copy_construction = std::is_lvalue_reference<TFunction>;
         rhs.get_internal_callable().construct_to_buffer(is_copy_construction{}, buffer);
      }
      else
      {
         clear_internal_callable();
      }
   }

   template<class TFunction>
   void assign_internal_callable(TFunction&& rhs)
   {
      if(this == std::addressof(rhs))
         return;
      if(is_internal_callable_cleared())
      {
         if(!rhs.is_internal_callable_cleared())
         {
            using is_copy_construction = std::is_lvalue_reference<TFunction>;
            rhs.get_internal_callable().construct_to_buffer(is_copy_construction{}, buffer);
         }
      }
      else
      {
         get_internal_callable().~internal_callable();
         construct_internal_callable(std::forward<TFunction>(rhs));
      }
   }

   internal_callable& get_internal_callable()
   {
      return *static_cast<internal_callable*>(static_cast<void*>(buffer));
   }

   const internal_callable& get_internal_callable() const
   {
      return *static_cast<const internal_callable*>(static_cast<const void*>(buffer));
   }

   void clear_internal_callable() noexcept
   {
      std::fill(std::begin(buffer), std::end(buffer), 0);
   }

   bool is_internal_callable_cleared() const noexcept
   {
      return std::all_of(std::begin(buffer), std::end(buffer), [](uint8_t c){ return c==0; });
   }

private:
   // the default storage size is large enough to store targets such as
   // stateless closures, stateless function objects and function pointers
   static const size_t VPTR_SIZE_BYTES{ sizeof(void*) };
   static const size_t INTERNAL_CALLABLE_SIZE_BYTES{ TARGET_SIZE_BYTES + VPTR_SIZE_BYTES };
   uint8_t buffer[INTERNAL_CALLABLE_SIZE_BYTES];
};
}

#endif
