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
#include <sstl/__internal/_type_tag.h>

namespace sstl
{
namespace test
{

class counted_type
{
public:
   struct construction_exception : std::runtime_error
   {
      construction_exception() : std::runtime_error("construction exception") {}
   };

   struct default_construction_exception : std::runtime_error
   {
      default_construction_exception() : std::runtime_error("default construction exception") {}
   };

   struct parameter_construction_exception : std::runtime_error
   {
      parameter_construction_exception() : std::runtime_error("parameter construction exception") {}
   };

   struct copy_construction_exception : std::runtime_error
   {
      copy_construction_exception() : std::runtime_error("copy construction exception") {}
   };

   struct move_construction_exception : std::runtime_error
   {
      move_construction_exception() : std::runtime_error("move construction exception") {}
   };

   struct destruction_exception : std::runtime_error
   {
      destruction_exception() : std::runtime_error("destruction exception") {}
   };

   struct copy_assignment_exception : std::runtime_error
   {
      copy_assignment_exception() : std::runtime_error("copy assignment exception") {}
   };

   struct move_assignment_exception : std::runtime_error
   {
      move_assignment_exception() : std::runtime_error("move assignment exception") {}
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
      throw_if_necessary(_type_tag<construction_exception>{});
      throw_if_necessary(_type_tag<default_construction_exception>{});
      ++constructions;
      ++default_constructions;
   }
   counted_type(size_t param) : member(param)
   {
      throw_if_necessary(_type_tag<construction_exception>{});
      throw_if_necessary(_type_tag<parameter_construction_exception>{});
      ++constructions;
      ++parameter_constructions;
   }
   counted_type(const counted_type& rhs) : member(rhs.member)
   {
      throw_if_necessary(_type_tag<construction_exception>{});
      throw_if_necessary(_type_tag<copy_construction_exception>{});
      ++constructions;
      ++copy_constructions;
   }
   counted_type(counted_type&& rhs) : member(rhs.member)
   {
      throw_if_necessary(_type_tag<construction_exception>{});
      throw_if_necessary(_type_tag<move_construction_exception>{});
      ++constructions;
      ++move_constructions;
   }
   ~counted_type()
   {
      throw_if_necessary(_type_tag<destruction_exception>{});
      ++destructions;
   }
   counted_type& operator=(const counted_type& rhs)
   {
      throw_if_necessary(_type_tag<copy_assignment_exception>{});
      member = rhs.member;
      ++copy_assignments;
      return *this;
   }
   counted_type& operator=(counted_type&& rhs)
   {
      throw_if_necessary(_type_tag<move_assignment_exception>{});
      member = rhs.member;
      ++move_assignments;
      return *this;
   }

   static void throw_at_nth_construction(size_t n)
   {
      throw_expected_at_nth_construction = n;
   }
   static void throw_at_nth_default_construction(size_t n)
   {
      throw_expected_at_nth_default_construction = n;
   }
   static void throw_at_nth_parameter_construction(size_t n)
   {
      throw_expected_at_nth_parameter_construction = n;
   }
   static void throw_at_nth_copy_construction(size_t n)
   {
      throw_expected_at_nth_copy_construction = n;
   }
   static void throw_at_nth_move_construction(size_t n)
   {
      throw_expected_at_nth_move_construction = n;
   }
   static void throw_at_nth_destruction(size_t n)
   {
      throw_expected_at_nth_destruction = n;
   }
   static void throw_at_nth_copy_assigment(size_t n)
   {
      throw_expected_at_nth_copy_assignment = n;
   }
   static void throw_at_nth_move_assigment(size_t n)
   {
      throw_expected_at_nth_move_assignment = n;
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

      throw_expected_at_nth_construction = throw_disabled;
      throw_expected_at_nth_default_construction = throw_disabled;
      throw_expected_at_nth_parameter_construction = throw_disabled;
      throw_expected_at_nth_copy_construction = throw_disabled;
      throw_expected_at_nth_move_construction = throw_disabled;
      throw_expected_at_nth_destruction = throw_disabled;
      throw_expected_at_nth_copy_assignment = throw_disabled;
      throw_expected_at_nth_move_assignment = throw_disabled;
   }

   size_t member { static_cast<size_t>(-1) };

private:
   static void throw_if_necessary(_type_tag<construction_exception>)
   {
      _throw_if_necessary<construction_exception>(constructions, throw_expected_at_nth_construction);
   }

   static void throw_if_necessary(_type_tag<default_construction_exception>)
   {
      _throw_if_necessary<default_construction_exception>(default_constructions, throw_expected_at_nth_default_construction);
   }

   static void throw_if_necessary(_type_tag<parameter_construction_exception>)
   {
      _throw_if_necessary<parameter_construction_exception>(parameter_constructions, throw_expected_at_nth_parameter_construction);
   }

   static void throw_if_necessary(_type_tag<copy_construction_exception>)
   {
      _throw_if_necessary<copy_construction_exception>(copy_constructions, throw_expected_at_nth_copy_construction);
   }

   static void throw_if_necessary(_type_tag<move_construction_exception>)
   {
      _throw_if_necessary<move_construction_exception>(move_constructions, throw_expected_at_nth_move_construction);
   }

   static void throw_if_necessary(_type_tag<destruction_exception>)
   {
      _throw_if_necessary<destruction_exception>(destructions, throw_expected_at_nth_destruction);
   }

   static void throw_if_necessary(_type_tag<copy_assignment_exception>)
   {
      _throw_if_necessary<copy_assignment_exception>(copy_assignments, throw_expected_at_nth_copy_assignment);
   }

   static void throw_if_necessary(_type_tag<move_assignment_exception>)
   {
      _throw_if_necessary<move_assignment_exception>(move_assignments, throw_expected_at_nth_move_assignment);
   }

   template<class TException>
   static void _throw_if_necessary(size_t current_count, size_t& throw_expected_at_count)
   {
      if(current_count+1 == throw_expected_at_count)
      {
         throw_expected_at_count = throw_disabled;
         throw TException();
      }
   }

private:
   static size_t constructions;
   static size_t default_constructions;
   static size_t parameter_constructions;
   static size_t copy_constructions;
   static size_t move_constructions;
   static size_t destructions;
   static size_t copy_assignments;
   static size_t move_assignments;

   static size_t throw_expected_at_nth_construction;
   static size_t throw_expected_at_nth_default_construction;
   static size_t throw_expected_at_nth_parameter_construction;
   static size_t throw_expected_at_nth_copy_construction;
   static size_t throw_expected_at_nth_move_construction;
   static size_t throw_expected_at_nth_destruction;
   static size_t throw_expected_at_nth_copy_assignment;
   static size_t throw_expected_at_nth_move_assignment;

   static const size_t throw_disabled = static_cast<size_t>(-1);
};

}
}

#endif
