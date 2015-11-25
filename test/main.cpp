/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <iostream>

int main_catch(int argc, char* argv[]);
int main_unittestcpp();

int main(int argc, char* argv[])
{
   std::cout<<"================="<<std::endl;
   std::cout<<"catch unit tests:"<<std::endl;
   std::cout<<"================="<<std::endl;
   auto result_catch = main_catch(argc, argv);

   std::cout<<"======================="<<std::endl;
   std::cout<<"unittestcpp unit tests:"<<std::endl;
   std::cout<<"======================="<<std::endl;
   auto result_unittestcpp = main_unittestcpp();

   if(result_catch != 0)
      return result_catch;
   else
      return result_unittestcpp;
}
