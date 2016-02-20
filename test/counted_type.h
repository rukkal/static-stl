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
#include <stdexcept>
#include <sstl/__internal/_except.h>

namespace sstl_test
{

class counted_type
{
public:
   struct construction
   {
      struct exception : std::runtime_error
      {
         exception() : std::runtime_error("construction exception") {}
      };
      static size_t count;
      static size_t throw_expected_at_count;
   };

   struct default_construction
   {
      struct exception : std::runtime_error
      {
         exception() : std::runtime_error("default construction exception") {}
      };
      static size_t count;
      static size_t throw_expected_at_count;
   };

   struct parameter_construction
   {
      struct exception : std::runtime_error
      {
         exception() : std::runtime_error("parameter construction exception") {}
      };
      static size_t count;
      static size_t throw_expected_at_count;
   };

   struct copy_construction
   {
      struct exception : std::runtime_error
      {
         exception() : std::runtime_error("copy construction exception") {}
      };
      static size_t count;
      static size_t throw_expected_at_count;
   };

   struct move_construction
   {
      struct exception : std::runtime_error
      {
         exception() : std::runtime_error("move construction exception") {}
      };
      static size_t count;
      static size_t throw_expected_at_count;
   };

   struct destruction
   {
      struct exception : std::runtime_error
      {
         exception() : std::runtime_error("destruction exception") {}
      };
      static size_t count;
      static size_t throw_expected_at_count;
   };

   struct copy_assignment
   {
      struct exception : std::runtime_error
      {
         exception() : std::runtime_error("copy assignment exception") {}
      };
      static size_t count;
      static size_t throw_expected_at_count;
   };

   struct move_assignment
   {
      struct exception : std::runtime_error
      {
         exception() : std::runtime_error("move assignment exception") {}
      };
      static size_t count;
      static size_t throw_expected_at_count;
   };

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
         if(expected_move_assignments==invalid_count)
            expected_move_assignments=0;
         return *this;
      }

      check& move_assignments(size_t count)
      {
         expected_move_assignments=count;
         if(expected_copy_assignments==invalid_count)
            expected_copy_assignments=0;
         return *this;
      }

      operator bool() const
      {
         bool detailed_construction_checks_are_either_all_valid_or_all_invalid =
            is_valid_count(expected_default_constructions) == is_valid_count(expected_parameter_constructions)
            && is_valid_count(expected_default_constructions) == is_valid_count(expected_copy_constructions)
            && is_valid_count(expected_default_constructions) == is_valid_count(expected_move_constructions);
         assert(detailed_construction_checks_are_either_all_valid_or_all_invalid);

         bool if_any_construction_check_is_performed_it_is_either_a_detailed_one_or_a_general_one =
            (!is_valid_count(expected_default_constructions) && !is_valid_count(expected_constructions))
            || (is_valid_count(expected_default_constructions) && !is_valid_count(expected_constructions))
            || (!is_valid_count(expected_default_constructions) && is_valid_count(expected_constructions));
         assert(if_any_construction_check_is_performed_it_is_either_a_detailed_one_or_a_general_one);

         bool assignment_checks_are_either_all_valid_or_all_invalid =
            is_valid_count(expected_copy_assignments) == is_valid_count(expected_move_assignments);
         assert(assignment_checks_are_either_all_valid_or_all_invalid);

         return check_value(counted_type::construction::count, expected_constructions)
            && check_value(counted_type::default_construction::count, expected_default_constructions)
            && check_value(counted_type::parameter_construction::count, expected_parameter_constructions)
            && check_value(counted_type::copy_construction::count, expected_copy_constructions)
            && check_value(counted_type::move_construction::count, expected_move_constructions)
            && check_value(counted_type::destruction::count, expected_destructions)
            && check_value(counted_type::copy_assignment::count, expected_copy_assignments)
            && check_value(counted_type::move_assignment::count, expected_move_assignments);
      }

   private:
      void set_to_zero_if_invalid(size_t& a, size_t& b, size_t& c)
      {
         if(!is_valid_count(a))
            a = 0;
         if(!is_valid_count(b))
            b = 0;
         if(!is_valid_count(c))
            c = 0;
      }

      static bool check_value(size_t actual, size_t expected)
      {
         if(expected!=invalid_count && actual!=expected)
            return false;
         else
            return true;
      }

   private:
      size_t expected_constructions{ invalid_count };
      size_t expected_default_constructions{ invalid_count };
      size_t expected_parameter_constructions{ invalid_count };
      size_t expected_copy_constructions{ invalid_count };
      size_t expected_move_constructions{ invalid_count };
      size_t expected_destructions{ invalid_count };
      size_t expected_copy_assignments{ invalid_count };
      size_t expected_move_assignments{ invalid_count };
   };

public:
   counted_type()
   {
      throw_if_necessary<construction>();
      throw_if_necessary<default_construction>();
      ++construction::count;
      ++default_construction::count;
   }
   counted_type(size_t param) : member(param)
   {
      throw_if_necessary<construction>();
      throw_if_necessary<parameter_construction>();
      ++construction::count;
      ++parameter_construction::count;
   }
   counted_type(const counted_type& rhs) : member(rhs.member)
   {
      throw_if_necessary<construction>();
      throw_if_necessary<copy_construction>();
      ++construction::count;
      ++copy_construction::count;
   }
   counted_type(counted_type&& rhs) : member(rhs.member)
   {
      throw_if_necessary<construction>();
      throw_if_necessary<move_construction>();
      ++construction::count;
      ++move_construction::count;
   }
   ~counted_type()
   {
      throw_if_necessary<destruction>();
      ++destruction::count;
   }
   counted_type& operator=(const counted_type& rhs)
   {
      throw_if_necessary<copy_assignment>();
      member = rhs.member;
      ++copy_assignment::count;
      return *this;
   }
   counted_type& operator=(counted_type&& rhs)
   {
      throw_if_necessary<move_assignment>();
      member = rhs.member;
      ++move_assignment::count;
      return *this;
   }

   static void throw_at_nth_construction(size_t n)
   {
      construction::throw_expected_at_count = n;
   }
   static void throw_at_nth_default_construction(size_t n)
   {
      default_construction::throw_expected_at_count = n;
   }
   static void throw_at_nth_parameter_construction(size_t n)
   {
      parameter_construction::throw_expected_at_count = n;
   }
   static void throw_at_nth_copy_construction(size_t n)
   {
      copy_construction::throw_expected_at_count = n;
   }
   static void throw_at_nth_move_construction(size_t n)
   {
      move_construction::throw_expected_at_count = n;
   }
   static void throw_at_nth_destruction(size_t n)
   {
      destruction::throw_expected_at_count = n;
   }
   static void throw_at_nth_copy_assignment(size_t n)
   {
      copy_assignment::throw_expected_at_count = n;
   }
   static void throw_at_nth_move_assignment(size_t n)
   {
      move_assignment::throw_expected_at_count = n;
   }

   void operator()() const {} // make type also usable as function object
   bool operator==(const counted_type& rhs) const { return member == rhs.member; }
   static void reset_counts()
   {
      construction::count = 0;
      default_construction::count = 0;
      parameter_construction::count = 0;
      copy_construction::count = 0;
      move_construction::count = 0;
      destruction::count = 0;
      copy_assignment::count = 0;
      move_assignment::count = 0;

      construction::throw_expected_at_count = invalid_count;
      default_construction::throw_expected_at_count = invalid_count;
      parameter_construction::throw_expected_at_count = invalid_count;
      copy_construction::throw_expected_at_count = invalid_count;
      move_construction::throw_expected_at_count = invalid_count;
      destruction::throw_expected_at_count = invalid_count;
      copy_assignment::throw_expected_at_count = invalid_count;
      move_assignment::throw_expected_at_count = invalid_count;
   }

   size_t member { static_cast<size_t>(-1) };

private:
   template<class TOperation>
   static void throw_if_necessary()
   {
      #if _sstl_has_exceptions()
      if(TOperation::count+1 == TOperation::throw_expected_at_count)
      {
         TOperation::throw_expected_at_count = invalid_count;
         throw typename TOperation::exception();
      }
      #endif
   }

   static bool is_valid_count(size_t value)
   {
      return value!=invalid_count;
   }

private:
   static const size_t invalid_count = static_cast<size_t>(-1);
};

}

#endif
