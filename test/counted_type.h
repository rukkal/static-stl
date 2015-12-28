/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_COUNTED_TYPE__
#define _SSTL_COUNTED_TYPE__

#include <cstddef>
#include <cassert>

namespace sstl
{
namespace test
{

class counted_type
{
public:
   class check
   {
   public:
      check& constructions(size_t count)
      {
         expected_constructions=count;
         return *this;
      }

      check& default_constructions(size_t count)
      {
         expected_default_constructions = count;
         set_to_zero_if_invalid( expected_parameter_constructions,
                                 expected_copy_constructions,
                                 expected_move_constructions);
         return *this;
      }

      check& parameter_constructions(size_t count)
      {
         expected_parameter_constructions = count;
         set_to_zero_if_invalid( expected_default_constructions,
                                 expected_copy_constructions,
                                 expected_move_constructions);
         return *this;
      }

      check& copy_constructions(size_t count)
      {
         expected_copy_constructions=count;
         set_to_zero_if_invalid( expected_default_constructions,
                                 expected_parameter_constructions,
                                 expected_move_constructions);
         return *this;
      }

      check& move_constructions(size_t count)
      {
         expected_move_constructions=count;
         set_to_zero_if_invalid( expected_default_constructions,
                                 expected_parameter_constructions,
                                 expected_copy_constructions);
         return *this;
      }

      check& destructions(size_t count)
      {
         expected_destructions=count;
         return *this;
      }

      check& copy_assignments(size_t count)
      {
         expected_copy_assignments=count;
         if(expected_move_assignments==invalid)
            expected_move_assignments=0;
         return *this;
      }

      check& move_assignments(size_t count)
      {
         expected_move_assignments=count;
         if(expected_copy_assignments==invalid)
            expected_copy_assignments=0;
         return *this;
      }

      operator bool() const
      {
         bool detailed_construction_checks_are_either_all_valid_or_all_invalid =
            is_valid(expected_default_constructions) == is_valid(expected_parameter_constructions)
            && is_valid(expected_default_constructions) == is_valid(expected_copy_constructions)
            && is_valid(expected_default_constructions) == is_valid(expected_move_constructions);
         assert(detailed_construction_checks_are_either_all_valid_or_all_invalid);

         bool if_any_construction_check_is_performed_it_is_either_a_detailed_one_or_a_general_one =
            (!is_valid(expected_default_constructions) && !is_valid(expected_constructions))
            || (is_valid(expected_default_constructions) && !is_valid(expected_constructions))
            || (!is_valid(expected_default_constructions) && is_valid(expected_constructions));
         assert(if_any_construction_check_is_performed_it_is_either_a_detailed_one_or_a_general_one);

         bool assignment_checks_are_either_all_valid_or_all_invalid =
            is_valid(expected_copy_assignments) == is_valid(expected_move_assignments);
         assert(assignment_checks_are_either_all_valid_or_all_invalid);

         return check_value(counted_type::constructions, expected_constructions)
            && check_value(counted_type::default_constructions, expected_default_constructions)
            && check_value(counted_type::parameter_constructions, expected_parameter_constructions)
            && check_value(counted_type::copy_constructions, expected_copy_constructions)
            && check_value(counted_type::move_constructions, expected_move_constructions)
            && check_value(counted_type::destructions, expected_destructions)
            && check_value(counted_type::copy_assignments, expected_copy_assignments)
            && check_value(counted_type::move_assignments, expected_move_assignments);
      }

   private:
      void set_to_zero_if_invalid(size_t& a, size_t& b, size_t& c)
      {
         if(!is_valid(a))
            a = 0;
         if(!is_valid(b))
            b = 0;
         if(!is_valid(c))
            c = 0;
      }

      static bool check_value(size_t actual, size_t expected)
      {
         if(expected!=invalid && actual!=expected)
            return false;
         else
            return true;
      }

      static bool is_valid(size_t value)
      {
         return value!=invalid;
      }

   private:
      static const size_t invalid = static_cast<size_t>(-1);
      size_t expected_constructions{ invalid };
      size_t expected_default_constructions{ invalid };
      size_t expected_parameter_constructions{ invalid };
      size_t expected_copy_constructions{ invalid };
      size_t expected_move_constructions{ invalid };
      size_t expected_destructions{ invalid };
      size_t expected_copy_assignments{ invalid };
      size_t expected_move_assignments{ invalid };
   };

public:
   counted_type()
   {
      ++constructions;
      ++default_constructions;
   }
   counted_type(size_t param) : member(param)
   {
      ++constructions;
      ++parameter_constructions;
   }
   counted_type(const counted_type& rhs) : member(rhs.member)
   {
      ++constructions;
      ++copy_constructions;
   }
   counted_type(counted_type&& rhs) : member(rhs.member)
   {
      ++constructions;
      ++move_constructions;
   }
   ~counted_type()
   {
      ++destructions;
   }
   counted_type& operator=(const counted_type& rhs)
   {
      member = rhs.member;
      ++copy_assignments;
      return *this;
   }
   counted_type& operator=(counted_type&& rhs)
   {
      member = rhs.member;
      ++move_assignments;
      return *this;
   }
   void operator()() const {} // make type also usable as function object
   bool operator==(const counted_type& rhs) const { return member == rhs.member; }
   static void reset_counts()
   {
      constructions = 0;
      default_constructions = 0;
      parameter_constructions = 0;
      copy_constructions = 0;
      move_constructions = 0;
      destructions = 0;
      copy_assignments = 0;
      move_assignments = 0;
   }

   size_t member { static_cast<size_t>(-1) };

public:
   static size_t constructions;
   static size_t default_constructions;
   static size_t parameter_constructions;
   static size_t copy_constructions;
   static size_t move_constructions;
   static size_t destructions;
   static size_t copy_assignments;
   static size_t move_assignments;
};

}
}

#endif
