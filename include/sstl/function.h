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
#include "__internal/_utility.h"
#include "__internal/_except.h"
#include "__internal/_hacky_derived_class_access.h"

namespace sstl
{

template<class TTarget, size_t CALLABLE_SIZE=static_cast<size_t>(-1)>
class function;

namespace _detail
{
   template<class T>
   struct _make_const_ref_if_value
   {
      using type = const T&;
   };

   template<class T>
   struct _make_const_ref_if_value<T&>
   {
      using type = T&;
   };

   template<class T>
   struct _make_const_ref_if_value<T&&>
   {
      using type = T&&;
   };
}

namespace _detail
{
   // true if the template parameter is an instance of sstl::function
   template<class>
   struct _is_function : std::false_type {};

   template<class TResult, class... TParams, size_t SIZE>
   struct _is_function<sstl::function<TResult(TParams...), SIZE>> : std::true_type {};
}

namespace _detail
{
   // true if types "From" are convertible to type "To"
   template<class From, class To>
   struct _are_convertible;

   template<>
   struct _are_convertible<std::tuple<>, std::tuple<>> : std::true_type {};

   template<class FromHead, class... FromTail, class ToHead, class... ToTail>
   struct _are_convertible<std::tuple<FromHead, FromTail...>, std::tuple<ToHead, ToTail...>>
   {
      static const bool value =
         std::is_convertible<FromHead, ToHead>::value
         && _are_convertible<std::tuple<FromTail...>, std::tuple<ToTail...>>::value;
   };
}

namespace _detail
{
   // true if "From" can be converted to "To",
   // i.e "To" has covariant return type and contravariant parameter types
   template<class From, class To>
   struct _is_convertible_function;

   template<class TResultFrom, class... TParamsFrom, size_t SIZE_FROM,
            class TResultTo, class... TParamsTo, size_t SIZE_TO>
   struct _is_convertible_function<
      sstl::function<TResultFrom(TParamsFrom...), SIZE_FROM>,
      sstl::function<TResultTo(TParamsTo...), SIZE_TO>>
   {
      static const bool value =
         std::is_convertible<TResultTo, TResultFrom>::value
         && _detail::_are_convertible<std::tuple<TParamsFrom...>, std::tuple<TParamsTo...>>::value;
   };

   template<class T>
   T& _get_reference(T* p) _sstl_noexcept_ { return *p; }

   template<class T>
   T& _get_reference(T& r) _sstl_noexcept_ { return r; }
}

namespace _detail
{
   template<class T, class=void>
   struct _is_inheritable
   {
      // FIXME: fix with "&& !std::is_final<T>::value" as soon as C++14 support is available
      static const bool value = std::is_class<T>::value;
   };
};

namespace _detail
{
   template<class, class>
   struct _is_member_function_pointer_compatible;

   template<class TPointerRet, class TPointerClass, class... TPointerParams,
            class TFunctionRet, class TFirstFunctionParam, class... TOtherFunctionParams, size_t SIZE>
   struct _is_member_function_pointer_compatible<
      TPointerRet (TPointerClass::*) (TPointerParams...),
      sstl::function<TFunctionRet(TFirstFunctionParam, TOtherFunctionParams...), SIZE>>
   {
      static const bool value =
         std::is_same<TPointerClass*, TFirstFunctionParam>::value || std::is_same<TPointerClass&, TFirstFunctionParam>::value;
   };
};

template<class TResult, class... TParams>
class function<TResult(TParams...)>
{
   //friend declaration required for derived class' noexcept expressions
   template<class, size_t>
   friend class function;

public:
   //required because gcc-arm-none-eabi 4.9 might not consider the forwarding-reference overload as candidate
   function& operator=(const function& rhs)
   {
      _runtime_assert_buffer_can_contain_target(rhs);
      _assign_internal_callable(rhs);
      return *this;
   }
   
   //required because gcc-arm-none-eabi 4.9 might not consider the forwarding-reference overload as candidate
   function& operator=(const function&& rhs)
   {
      _runtime_assert_buffer_can_contain_target(rhs);
      _assign_internal_callable(std::move(rhs));
      return *this;
   }

   template<class T, class TTarget = typename std::decay<T>::type>
   function& operator=(T&& rhs)
      _sstl_noexcept(!_detail::_is_function<TTarget>::value &&
                     ((std::is_lvalue_reference<T>::value && std::is_nothrow_copy_constructible<TTarget>::value)
                     || (!std::is_lvalue_reference<T>::value && std::is_nothrow_move_constructible<TTarget>::value)))
   {
      _runtime_assert_buffer_can_contain_target(rhs);
      _assign_internal_callable(std::forward<T>(rhs));
      return *this;
   }

   TResult operator()(typename _detail::_make_const_ref_if_value<TParams>::type... params) const
   {
      return _get_internal_callable()._call(std::forward<typename _detail::_make_const_ref_if_value<TParams>::type>(params)...);
   }

   operator bool() const _sstl_noexcept_
   {
      return _is_internal_callable_valid();
   }

protected:
   using _type_for_derived_class_access = function<TResult(TParams...), 0>;

   _type_for_derived_class_access& _derived() _sstl_noexcept_;
   const _type_for_derived_class_access& _derived() const _sstl_noexcept_;

   struct _internal_callable
   {
      virtual ~_internal_callable() {}
      virtual TResult _call(typename _detail::_make_const_ref_if_value<TParams>::type...) = 0;
      // std::true_type -> copy construction
      void _construct_to_buffer(std::true_type, void* buffer) const
      {
         _copy_construct_to_buffer(buffer);
      }
      // std::false_type -> move construction
      void _construct_to_buffer(std::false_type, void* buffer)
      {
         _move_construct_to_buffer(buffer);
      }
      virtual void _copy_construct_to_buffer(void*) const = 0;
      virtual void _move_construct_to_buffer(void*) = 0;
      virtual size_t _size() = 0;
   };

   template<class, class=void>
   struct _internal_callable_imp;

   //EBO specialization
   template<class TTarget>
   struct _internal_callable_imp<TTarget, typename std::enable_if<_detail::_is_inheritable<TTarget>::value>::type >
      : _internal_callable, TTarget
   {
      template<class T>
      _internal_callable_imp(T&& target) : TTarget(std::forward<T>(target))
      {
      }

      TResult _call(typename _detail::_make_const_ref_if_value<TParams>::type... params) override
      {
         return (*this)(std::forward<typename _detail::_make_const_ref_if_value<TParams>::type>(params)...);
      }

      void _copy_construct_to_buffer(void* b) const override
      {
         new(b) _internal_callable_imp(static_cast<const TTarget&>(*this));
      }

      void _move_construct_to_buffer(void* b) override
      {
         new(b) _internal_callable_imp(std::move(static_cast<TTarget&>(*this)));
      }

      size_t _size() override
      {
         return sizeof(_internal_callable_imp);
      }
   };

   //member function pointer specialization
   template<class TTarget>
   struct _internal_callable_imp<TTarget, typename std::enable_if<std::is_member_function_pointer<TTarget>::value>::type >
      : _internal_callable
   {
      template<class T>
      _internal_callable_imp(T&& target) : target(target)
      {
      }

      TResult _call(typename _detail::_make_const_ref_if_value<TParams>::type... params) override
      {
         return _call_member_function(std::forward<typename _detail::_make_const_ref_if_value<TParams>::type>(params)...);
      }

      template<class TInstance, class... TMemberFunctionParams>
      TResult _call_member_function(TInstance instance, TMemberFunctionParams&&... params)
      {
         return (_detail::_get_reference(instance).*target)(std::forward<TMemberFunctionParams>(params)...);
      }

      void _copy_construct_to_buffer(void* b) const override
      {
         new(b) _internal_callable_imp(target);
      }

      void _move_construct_to_buffer(void* b) override
      {
         new(b) _internal_callable_imp(target);
      }

      size_t _size() override
      {
         return sizeof(_internal_callable_imp);
      }

      TTarget target;

      static_assert(_detail::_is_member_function_pointer_compatible<TTarget, function>::value,
         "attempted to assign an incompatible member function pointer."
         " Are the types of the first (left most) parameters compatible?");
   };

   template<class TTarget>
   struct _internal_callable_imp<TTarget,
                                 typename std::enable_if<
                                    !_detail::_is_inheritable<TTarget>::value
                                    && !std::is_member_function_pointer<TTarget>::value>::type>
      : _internal_callable
   {
      template<class T>
      _internal_callable_imp(T&& target) : target(std::forward<T>(target))
      {
      }

      TResult _call(typename _detail::_make_const_ref_if_value<TParams>::type... params) override
      {
         return target(std::forward<typename _detail::_make_const_ref_if_value<TParams>::type>(params)...);
      }

      void _copy_construct_to_buffer(void* b) const override
      {
         new(b) _internal_callable_imp(target);
      }

      void _move_construct_to_buffer(void* b) override
      {
         new(b) _internal_callable_imp(std::move(target));
      }

      size_t _size() override
      {
         return sizeof(_internal_callable_imp);
      }

      TTarget target;
   };

protected:
   bool _is_internal_callable_valid() const _sstl_noexcept_
   {
      return std::any_of(_derived()._buffer, _derived()._buffer+_derived()._buffer_size, [](uint8_t c){ return c!=0; });
   }

   _internal_callable& _get_internal_callable() const _sstl_noexcept_
   {
      auto non_const_buffer = const_cast<void*>(static_cast<const void*>(_derived()._buffer));
      return *static_cast<_internal_callable*>(non_const_buffer);
   }

   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<_detail::_is_function<TTarget>::value>::type>
   void _construct_internal_callable(T&& rhs)
   {
      if(rhs._is_internal_callable_valid())
      {
         using is_copy_construction = std::is_lvalue_reference<T>;
         rhs._get_internal_callable()._construct_to_buffer(is_copy_construction{}, _derived()._buffer);
      }
      else
      {
         _invalidate_internal_callable();
      }
   }

   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<!_detail::_is_function<TTarget>::value>::type>
   // dummy parameter required in order not to declare an invalid overload
   void _construct_internal_callable(T&& rhs, char=0)
   {
      new(_derived()._buffer) _internal_callable_imp<TTarget>(std::forward<T>(rhs));
   }

   template<class T>
   void _assign_internal_callable(T&& rhs)
   {
      //omit self check in case of move assignment (self move assignment is UB)
      if(   std::is_lvalue_reference<T>::value 
         && static_cast<const void*>(this) == reinterpret_cast<const void*>(std::addressof(rhs)))
         return;
      if(_is_internal_callable_valid())
         _get_internal_callable().~_internal_callable();
      #if _sstl_has_exceptions()
      try
      {
      #endif
         _construct_internal_callable(std::forward<T>(rhs));
      #if _sstl_has_exceptions()
      }
      catch(...)
      {
         _invalidate_internal_callable();
         throw;
      }
      #endif
   }
   
   void _invalidate_internal_callable() _sstl_noexcept_
   {
      std::fill(_derived()._buffer, _derived()._buffer+_derived()._buffer_size, 0);
   }
   
   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<_detail::_is_function<TTarget>::value>::type>
   void _runtime_assert_buffer_can_contain_target(const T& rhs) _sstl_noexcept_
   {
      //if assertion fails specify a larger callable size (template parameter)
      sstl_assert(!rhs._is_internal_callable_valid()
               || rhs._get_internal_callable()._size() <= _derived()._buffer_size);
   }

   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<!_detail::_is_function<TTarget>::value>::type>
   // dummy parameter required in order not to declare an invalid overload
   void _runtime_assert_buffer_can_contain_target(const T&, char=0) _sstl_noexcept_
   {
      //if assertion fails specify a larger callable size (template parameter)
      sstl_assert(sizeof(_internal_callable_imp<TTarget>) <= _derived()._buffer_size);
   }
   
protected:
   function() _sstl_noexcept_ = default;
   function(const function&) _sstl_noexcept_ = default;
   function(function&&) _sstl_noexcept_ {} //MSVC (VS2013) does not support default move special member functions
   ~function() = default;
};

template<class TResult, class... TParams, size_t CALLABLE_SIZE>
class function<TResult(TParams...), CALLABLE_SIZE> final : public function<TResult(TParams...)>
{
   template<class, size_t >
   friend class function;

private:
   using _base = function<TResult(TParams...)>;
   using _type_for_derived_class_access = typename _base::_type_for_derived_class_access;
   using _internal_callable = typename _base::_internal_callable;

public:
   function() _sstl_noexcept_
   {
      _assert_hacky_derived_class_access_is_valid<_base, function, _type_for_derived_class_access>();
      _base::_invalidate_internal_callable();
   }

   //required because gcc-arm-none-eabi 4.9 might not consider the forwarding-reference overload as candidate
   function(const function& rhs)
   {
      _assert_hacky_derived_class_access_is_valid<_base, function, _type_for_derived_class_access>();
      _assert_buffer_can_contain_target(rhs);
      _base::_construct_internal_callable(rhs);
   }

   //required because gcc-arm-none-eabi 4.9 might not consider the forwarding-reference overload as candidate
   function(function&& rhs)
   {
      _assert_hacky_derived_class_access_is_valid<_base, function, _type_for_derived_class_access>();
      _assert_buffer_can_contain_target(rhs);
      _base::_construct_internal_callable(std::move(rhs));
   }

   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<_detail::_is_function<TTarget>::value>::type>
   function(T&& rhs)
   {
      _assert_hacky_derived_class_access_is_valid<_base, function, _type_for_derived_class_access>();
      _assert_buffer_can_contain_target(rhs);
      _base::_construct_internal_callable(std::forward<T>(rhs));
   }

   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<!_detail::_is_function<TTarget>::value>::type>
   // dummy parameter required in order not to declare an invalid overload
   function(T&& rhs, char = 0)
      _sstl_noexcept((std::is_lvalue_reference<T>::value && std::is_nothrow_copy_constructible<TTarget>::value)
                  || (!std::is_lvalue_reference<T>::value && std::is_nothrow_move_constructible<TTarget>::value))
   {
      _assert_hacky_derived_class_access_is_valid<_base, function, _type_for_derived_class_access>();
      _assert_buffer_can_contain_target(rhs);
      _base::_construct_internal_callable(std::forward<T>(rhs));
   }

   //required because gcc-arm-none-eabi 4.9 might not consider the forwarding-reference overload as candidate
   function& operator=(const function& rhs)
   {
      _assert_buffer_can_contain_target(rhs);
      _base::_assign_internal_callable(rhs);
      return *this;
   }

   //required because gcc-arm-none-eabi 4.9 might not consider the forwarding-reference overload as candidate
   function& operator=(function&& rhs)
   {
      _assert_buffer_can_contain_target(rhs);
      _base::_assign_internal_callable(std::move(rhs));
      return *this;
   }

   template<class T, class TTarget = typename std::decay<T>::type>
   function& operator=(T&& rhs)
      _sstl_noexcept(!_detail::_is_function<TTarget>::value &&
                     ((std::is_lvalue_reference<T>::value && std::is_nothrow_copy_constructible<TTarget>::value)
                     || (!std::is_lvalue_reference<T>::value && std::is_nothrow_move_constructible<TTarget>::value)))
   {
      _assert_buffer_can_contain_target(rhs);
      _base::_assign_internal_callable(std::forward<T>(rhs));
      return *this;
   }

   ~function()
   {
      if(_base::_is_internal_callable_valid())
      {
         _base::_get_internal_callable().~_internal_callable();
      }
   }

private:
   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<_detail::_is_function<TTarget>::value>::type>
   void _assert_buffer_can_contain_target(const T& rhs) _sstl_noexcept_
   {
      _base::_runtime_assert_buffer_can_contain_target(rhs);
   }

   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<!_detail::_is_function<TTarget>::value>::type>
   // dummy parameter required in order not to declare an invalid overload
   void _assert_buffer_can_contain_target(const T&, char = 0) _sstl_noexcept_
   {
      static_assert(sizeof(typename _base::template _internal_callable_imp<TTarget>) <= sizeof(_buffer),
         "Not enough memory available to store the wished target."
         " Hint: specify a larger callable size (template parameter).");
   }

private:
   static const size_t _VPTR_SIZE = sizeof(void*);
   size_t _buffer_size{ sizeof(_buffer) };
   mutable uint8_t _buffer[_VPTR_SIZE + CALLABLE_SIZE];
};

template<class TResult, class... TParams>
typename function<TResult(TParams...)>::_type_for_derived_class_access& function<TResult(TParams...)>::_derived() _sstl_noexcept_
{
   return reinterpret_cast<_type_for_derived_class_access&>(*this);
}

template<class TResult, class... TParams>
const typename function<TResult(TParams...)>::_type_for_derived_class_access& function<TResult(TParams...)>::_derived() const _sstl_noexcept_
{
   return reinterpret_cast<const _type_for_derived_class_access&>(*this);
}

}

#endif
