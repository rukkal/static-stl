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
