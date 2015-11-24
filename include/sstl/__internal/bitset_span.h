/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_BITSET_SPAN__
#define _SSTL_BITSET_SPAN__

#include <cassert>
#include <limits>

namespace sstl
{
// A GSL-like (Guideline Support Library) implementation to manipulate a
// span of bits. This class is leveraged by other components to reduce
// code bloat avoiding the use of a template size parameter.
class bitset_span
{
public:
   bitset_span(void* data, size_t size)
   : blocks(static_cast<block_type*>(data))
   , num_of_bits(size)
   {
      assert(num_of_bits > 0);
   }

   void set(size_t idx)
   {
      assert(idx < num_of_bits);
      auto* byte = get_block(idx);
      auto bit_idx = get_bit_idx(idx);
      *byte |= (1<<bit_idx);
   }

   void set()
   {
      // entire blocks
      auto p = blocks;
      auto end = p+get_num_of_blocks()-1;
      while(p!=end)
      {
         *p++ = std::numeric_limits<block_type>::max();
      }
      // remaining bits
      for(size_t idx = (get_num_of_blocks()-1)*sizeof(block_type); idx<num_of_bits; ++idx)
      {
         set(idx);
      }
   }

   void reset(size_t idx)
   {
      assert(idx < num_of_bits);
      auto* byte = get_block(idx);
      auto bit_idx = get_bit_idx(idx);
      *byte &= ~(1<<bit_idx);
   }

   void reset()
   {
      // entire blocks
      auto p = blocks;
      auto end = p+get_num_of_blocks()-1;
      while(p!=end)
      {
         *p++ = 0;
      }
      // remaining bits
      for(size_t idx = (get_num_of_blocks()-1)*sizeof(block_type); idx<num_of_bits; ++idx)
      {
         reset(idx);
      }
   }

   bool test(size_t idx) const
   {
      assert(idx < num_of_bits);
      auto* byte = get_block(idx);
      auto bit_idx = get_bit_idx(idx);
      return ((*byte) & (1<<bit_idx)) != 0;
   }

   bool all() const
   {
      // entire blocks
      auto p = blocks;
      auto end = p+get_num_of_blocks()-1;
      while(p!=end)
      {
         if(*p++ != std::numeric_limits<block_type>::max())
            return false;
      }
      // remaining bits
      for(size_t idx = (get_num_of_blocks()-1)*sizeof(block_type); idx<num_of_bits; ++idx)
      {
         if(!test(idx))
            return false;
      }
      return true;
   }

   size_t size() const { return num_of_bits; }

   size_t count() const
   {
      size_t _count = 0;
      for(size_t i=0; i<num_of_bits; ++i)
      {
         if(test(i))
            ++_count;
      }
      return _count;
   }

private:
   using block_type = unsigned char;
   using const_block_type = unsigned char;

private:
   size_t get_byte_idx(size_t idx) const
   {
      return idx / (8 * sizeof(block_type));
   }

   block_type* get_block(size_t idx)
   {
      auto byte_idx = get_byte_idx(idx);
      return blocks + byte_idx;
   }

   const_block_type* get_block(size_t idx) const
   {
      auto byte_idx = get_byte_idx(idx);
      return blocks + byte_idx;
   }

   size_t get_bit_idx(size_t idx) const
   {
      auto byte_idx = get_byte_idx(idx);
      auto bit_idx = idx - 8 * byte_idx;
      return bit_idx;
   }

   size_t get_num_of_blocks() const
   {
      return (num_of_bits-1) / (8 * sizeof(block_type)) + 1;
   }

public:
   block_type* blocks;
   size_t num_of_bits;
};
}

#endif
