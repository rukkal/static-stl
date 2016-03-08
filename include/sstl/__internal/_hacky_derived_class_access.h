/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/
#ifndef _SSTL_HACKY_DERIVED_CLASS_ACCESS
#define _SSTL_HACKY_DERIVED_CLASS_ACCESS

template<class TBase, class TDerived, class TDerivedForHackyAccess>
void _assert_hacky_derived_class_access_is_valid()
{
   //assert that the specified classes (template parameters) have the same addresses,
   //such property is exploited to access the member variables of a derived
   //class from its base (quite hackish!).

   //note: the address value used in the assertion cannot be null, because
   //static_cast never applies an offset to a given pointer if it is null!
   void* non_null_address = reinterpret_cast<void*>(size_t(1)<<(sizeof(void*)*8-1));
   sstl_assert(static_cast<TBase*>(static_cast<TDerived*>(non_null_address)) == static_cast<TBase*>(non_null_address));
   sstl_assert(static_cast<TBase*>(static_cast<TDerivedForHackyAccess*>(non_null_address)) == static_cast<TBase*>(non_null_address));
}

#endif
