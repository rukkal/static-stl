/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

int main_catch(int argc, char* argv[])
{
   auto result = Catch::Session().run( argc, argv );
   return result;
}
