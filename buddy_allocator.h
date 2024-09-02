#pragma once

#include "used_mmap.h"
#include "bit_map.h"

#define Level_SuperMax 8
#define PAGE_SIZE 1024

typedef struct  {
  BitMap bitmap;
  int number_levels;
  char* buffer;
  int buffer_size;
  int min_bucket_size; 
} BuddyAllocator;



int BuddyAllocator_init(BuddyAllocator* allocator,
                         int number_levels,
                         char* buffer_alloc,
                         int buffer_size,
                         char* buffer_bitmap,
                         int buffer_size_bitmap,
                         int min_bucket_size);


void* BuddyAllocator_malloc(BuddyAllocator* allocator, int size,int alloc_size);

void BuddyAllocator_free(BuddyAllocator* allocator, void* mem,int alloc_size);

void Set_status_parents(BitMap *bit_map, int bit_num, int status);

void Set_status_children(BitMap *bit_map, int bit_num, int status);

void merge(BitMap *bitmap, int idx);
