/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef __SSTL_BITMAP_ALLOCATOR__
#define __SSTL_BITMAP_ALLOCATOR__

#include <cassert>
#include <array>
#include <type_traits>

#include "__internal/bitset_span.h"

namespace etl
{
// Base class used to factor out capacity-indepented code
// in order to reduce code bloat
template<class T>
class __bitmap_allocator_base
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;

public:
    __bitmap_allocator_base(void* buffer, bitset_span bitmap)
        : buffer(static_cast<pointer>(buffer))
        , bitmap(bitmap)
    {
    }

    T* allocate()
    {
        assert(!bitmap.all());
        auto free_block_idx = get_next_free_block_idx();
        bitmap.set(free_block_idx);
        return &buffer[free_block_idx];
    }

    void deallocate(void* p)
    {
        T* block = static_cast<T*>(p);
        assert(block>=buffer && block<buffer+bitmap.size());
        auto idx = block - buffer;
        assert(bitmap.test(idx));
        bitmap.reset(idx);
        last_allocated_block_idx = idx-1;
    }

private:
    size_t get_next_free_block_idx()
    {
        auto idx = last_allocated_block_idx;
        while(true)
        {
            if(++idx >= bitmap.size())
            {
                idx = 0;
            }
            if(!bitmap.test(idx))
                break;
        }
        return idx;
    }

private:
    pointer buffer;
    bitset_span bitmap;
    size_t last_allocated_block_idx = static_cast<size_t>(-1);
};

// An allocator that uses a bitmap to keep track of the allocated blocks
template <class T, size_t CAPACITY>
class bitmap_allocator : public __bitmap_allocator_base<T>
{
public:
    bitmap_allocator() : __bitmap_allocator_base<T>(buffer.data(), bitset_span(bitmap.data(), CAPACITY))
    {
        bitmap.fill(0);
    }

private:
    void reset_bitmap()
    {
        for(size_t i=0; i<k_bitmap_size; ++i)
        {
            bitmap[i] = 0;
        }
    }

private:
    using bitset_block_type = unsigned char;
    static const size_t k_bits_per_block = sizeof(bitset_block_type) * 8;
    static const size_t k_bitmap_size = (CAPACITY-1) / k_bits_per_block + 1;
    std::array<bitset_block_type, k_bitmap_size> bitmap;

    std::array<typename std::aligned_storage<sizeof(T), alignof(T)>::type, CAPACITY> buffer;
};
}

#endif
