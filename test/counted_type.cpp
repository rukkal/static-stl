/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include "counted_type.h"

namespace sstl
{
namespace test
{
   size_t counted_type::construction::count;
   size_t counted_type::default_construction::count;
   size_t counted_type::parameter_construction::count;
   size_t counted_type::copy_construction::count;
   size_t counted_type::move_construction::count;
   size_t counted_type::destruction::count;
   size_t counted_type::copy_assignment::count;
   size_t counted_type::move_assignment::count;

   size_t counted_type::construction::throw_expected_at_count;
   size_t counted_type::default_construction::throw_expected_at_count;
   size_t counted_type::parameter_construction::throw_expected_at_count;
   size_t counted_type::copy_construction::throw_expected_at_count;
   size_t counted_type::move_construction::throw_expected_at_count;
   size_t counted_type::destruction::throw_expected_at_count;
   size_t counted_type::copy_assignment::throw_expected_at_count;
   size_t counted_type::move_assignment::throw_expected_at_count;
}
}
