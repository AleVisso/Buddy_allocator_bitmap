#pragma once

#include "used_mmap.h"
#include "bit_map.h"

#define MAX_LEVELS 16

typedef struct  {
  BitMap bitmap;
  int number_levels;
  char* buffer;
  int buffer_size; // the memory area to be managed
  int min_bucket_size; // the minimum page of RAM that can be returned
} BuddyAllocator;


// initializes the buddy allocator, and checks that the buffer is large enough
int BuddyAllocator_init(BuddyAllocator* allocator,
                         int number_levels,
                         char* buffer_alloc,
                         int buffer_size,
                         char* buffer_bitmap,
                         int buffer_size_bitmap,
                         int min_bucket_size);


//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* allocator, int size,int alloc_size);

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* allocator, void* mem,int alloc_size);

void Set_status_parents(BitMap *bit_map, int bit_num, int status);

//setta lo stato dei discendenti a status
void Set_status_children(BitMap *bit_map, int bit_num, int status);

//funzione per ricompattare i vari livelli
void merge(BitMap *bitmap, int idx);
