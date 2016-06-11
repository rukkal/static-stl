/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/
#ifndef _SSTL_HACKY_DERIVED_CLASS_ACCESS
#define _SSTL_HACKY_DERIVED_CLASS_ACCESS

#include <type_traits>
#include <sstl_assert.h>

namespace sstl
{
namespace _detail
{
   template<class TMember>
   struct _void_ptr_to_member_type
   {
      static TMember& convert(void* ptr)
      {
         return *static_cast<TMember*>(ptr);
      }
   };
   
   //the arrays declared in the dummy types used to do the "hacky derived class access"
   //are likely to have dummy/invalid sizes. This template specialization converts
   //all such arrays to pointers, thus avoiding to violate the strict-aliasing rule.
   //Moreover, using arrays with dummy sizes would probably also trigger UB
   //because of out-of-bounds accesses.
   template<class TElement, size_t size>
   struct _void_ptr_to_member_type<TElement[size]>
   {
      static TElement* convert(void* ptr)
      {
         return static_cast<TElement*>(ptr);
      }
   };
}
}

#define _sstl_member_of_derived_class(this_pointer, member_name) \
   (sstl::_detail::_void_ptr_to_member_type<\
         decltype(std::declval<_type_for_hacky_derived_class_access>().member_name)\
      >::convert( \
         static_cast<void*>( \
            static_cast<char*>(const_cast<void*>(static_cast<const void*>(this_pointer))) \
            + offsetof(_type_for_hacky_derived_class_access, member_name))))

template<class TBase, class TDerived, class TDerivedForHackyAccess>
void _assert_hacky_derived_class_access_is_valid()
{
   //assert that:
   //-instances of the specified base and derived types have the same addresses
   //-the derived types are standard layout types
   //such properties are exploited to access the member variables of a derived
   //class from its base (very hackish!)

   //note: the address value used in the assertion cannot be null, because
   //static_cast never applies an offset to a given pointer if it is null!
   void* non_null_address = reinterpret_cast<void*>(size_t(1)<<(sizeof(void*)*8-1));
   (void) non_null_address;
   sstl_assert(static_cast<TBase*>(static_cast<TDerived*>(non_null_address)) == static_cast<TBase*>(non_null_address));
   sstl_assert(static_cast<TBase*>(static_cast<TDerivedForHackyAccess*>(non_null_address)) == static_cast<TBase*>(non_null_address));
   
   static_assert(std::is_standard_layout<TDerived>::value && std::is_standard_layout<TDerivedForHackyAccess>::value,
                "Non-standard-layout types used for hacky derived class access. "
                "Hint: all non-static member variables should be declared in the "
                "derived class (and only there) with the same access control.");
}

#endif
