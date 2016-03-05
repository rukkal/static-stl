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
#include "__internal/_except.h"

namespace sstl
{

template<class TTarget, size_t BUFFER_SIZE=static_cast<size_t>(-1)>
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
   T& _get_reference(T* p) { return *p; }

   template<class T>
   T& _get_reference(T& r) { return r; }
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

namespace _detail
{
   void* _get_internal_callable_address(void*) _sstl_noexcept_;
};

template<class TResult, class... TParams>
class function<TResult(TParams...)>
{
   //friend declaration required for derived class' noexcept expressions
   template<class, size_t>
   friend class function;

public:
   TResult operator()(typename _detail::_make_const_ref_if_value<TParams>::type... params) const
   {
      return _get_internal_callable()._call(std::forward<typename _detail::_make_const_ref_if_value<TParams>::type>(params)...);
   }

protected:
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

      TTarget target;
   };

private:
   _internal_callable& _get_internal_callable() const _sstl_noexcept_
   {
      auto type_erased_this = const_cast<void*>(static_cast<const void*>(this));
      auto type_erased_internal_callable = _detail::_get_internal_callable_address(type_erased_this);
      return *static_cast<_internal_callable*>(type_erased_internal_callable);
   }

protected:
   function() = default;
   function(const function&) = default;
   function(function&&) = default;
   ~function() = default;
   function& operator=(const function&) = default;
   function& operator=(function&&) = default;
};

template<class TResult, class... TParams, size_t BUFFER_SIZE>
class function<TResult(TParams...), BUFFER_SIZE> final : public function<TResult(TParams...)>
{
   friend void* _detail::_get_internal_callable_address(void*) _sstl_noexcept_;
   template<class, size_t > friend class function;

private:
   using _base_type = function<TResult(TParams...)>;

public:
   function() _sstl_noexcept_
   {
      _clear_internal_callable();
   }

   template<
      class T,
      class TTarget = typename std::decay<T>::type,
      class = typename std::enable_if<_detail::_is_function<TTarget>::value>::type>
   function(T&& rhs)
   {
      static_assert( _detail::_is_convertible_function<function, TTarget>::value,
                     "the instance of sstl::function passed as argument"
                     "must have covariant return type and contravariant"
                     "parameter types in order to be assigned");
      _construct_internal_callable(std::forward<T>(rhs));
   }

   template<
      class T,
      class TTarget = typename std::decay<T>::type,
      class = typename std::enable_if<!_detail::_is_function<TTarget>::value>::type>
   // dummy parameter required in order not to declare an invalid overload
   function(T&& rhs, char=0) _sstl_noexcept( (std::is_lvalue_reference<T>::value && std::is_nothrow_copy_constructible<TTarget>::value)
                                             || (!std::is_lvalue_reference<T>::value && std::is_nothrow_move_constructible<TTarget>::value))
   {
      _construct_internal_callable(std::forward<T>(rhs));
   }

   template<class T, class TTarget = typename std::decay<T>::type>
   function& operator=(T&& rhs) _sstl_noexcept( !_detail::_is_function<TTarget>::value &&
                                                ((std::is_lvalue_reference<T>::value && std::is_nothrow_copy_constructible<TTarget>::value)
                                                || (!std::is_lvalue_reference<T>::value && std::is_nothrow_move_constructible<TTarget>::value)))
   {
      _assign_internal_callable(std::forward<T>(rhs));
      return *this;
   }

   ~function()
   {
      if(!_is_internal_callable_cleared())
      {
         _base_type::_get_internal_callable().~_internal_callable();
      }
   }

   operator bool() const _sstl_noexcept_
   {
      return !_is_internal_callable_cleared();
   }

private:
   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<_detail::_is_function<TTarget>::value>::type>
   void _construct_internal_callable(T&& rhs)
   {
      static_assert(_detail::_is_function<TTarget>::value, "");
      if(!rhs._is_internal_callable_cleared())
      {
         using is_copy_construction = std::is_lvalue_reference<T>;
         rhs._get_internal_callable()._construct_to_buffer(is_copy_construction{}, _buffer);
      }
      else
      {
         _clear_internal_callable();
      }
   }

   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<!_detail::_is_function<TTarget>::value>::type>
   // dummy parameter required in order not to declare an invalid overload
   void _construct_internal_callable(T&& rhs, char=0)
   {
      static_assert(
         sizeof(typename _base_type::template _internal_callable_imp<TTarget>) <= sizeof(_buffer),
         "Not enough memory available to store the wished target."
         "Hint: specify size of the target as extra template argument");
      new(_buffer) typename _base_type::template _internal_callable_imp<TTarget>(std::forward<T>(rhs));
   }

   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<_detail::_is_function<TTarget>::value>::type>
   void _assign_internal_callable(T&& rhs)
   {
      if(static_cast<void*>(this) == static_cast<void*>(std::addressof(rhs)))
         return;
      if(_is_internal_callable_cleared())
      {
         if(!rhs._is_internal_callable_cleared())
         {
            using is_copy_construction = std::is_lvalue_reference<T>;
            rhs._get_internal_callable()._construct_to_buffer(is_copy_construction{}, _buffer);
         }
      }
      else
      {
         _base_type::_get_internal_callable().~_internal_callable();
         _construct_internal_callable(std::forward<T>(rhs));
      }
   }

   template<class T,
            class TTarget = typename std::decay<T>::type,
            class = typename std::enable_if<!_detail::_is_function<TTarget>::value>::type>
   // dummy parameter required in order not to declare an invalid overload
   void _assign_internal_callable(T&& rhs, char=0)
   {
      if(!_is_internal_callable_cleared())
      {
         _base_type::_get_internal_callable().~_internal_callable();
      }
      _construct_internal_callable(std::forward<T>(rhs));
   }

   void _clear_internal_callable()
   {
      std::fill(std::begin(_buffer), std::end(_buffer), 0);
   }

   bool _is_internal_callable_cleared() const
   {
      return std::all_of(std::begin(_buffer), std::end(_buffer), [](uint8_t c){ return c==0; });
   }

private:
   static const size_t _VPTR_SIZE = sizeof(void*);
   mutable uint8_t _buffer[_VPTR_SIZE + BUFFER_SIZE];
};

namespace _detail
{
   void* _get_internal_callable_address(void* base_address) _sstl_noexcept_
   {
      using type_for_derived_member_variable_access = const function<void(), 0>;
      return reinterpret_cast<type_for_derived_member_variable_access*>(base_address)->_buffer;
   }
}

}

#endif
