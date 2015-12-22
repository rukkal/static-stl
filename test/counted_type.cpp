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
   size_t counted_type::constructions;
   size_t counted_type::copy_constructions;
   size_t counted_type::move_constructions;
   size_t counted_type::destructions;
   size_t counted_type::copy_assignments;
   size_t counted_type::move_assignments;
}
}
