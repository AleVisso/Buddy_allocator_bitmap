#include "buddy_allocator.h"
#include <stdio.h>

#define BUFFER_SIZE 1024
#define BUDDY_LEVELS 6
#define MEMORY_SIZE 1024
#define MIN_BUCKET_SIZE (BUFFER_SIZE>>(BUDDY_LEVELS))
char memory[MEMORY_SIZE];
BuddyAllocator alloc;
char buffer[BUFFER_SIZE];
int buffer_size=BUFFER_SIZE;
int main(int argc, char** argv) {
  printf("init... ");
  BuddyAllocator_init(&alloc, BUDDY_LEVELS,
                      buffer,
                      BUFFER_SIZE,
                      memory,
                      MEMORY_SIZE,
                      MIN_BUCKET_SIZE);
  printf("DONE\n");

  void* p1=BuddyAllocator_malloc(&alloc, 100,buffer_size);
  //BuddyAllocator_free(&alloc, p1,buffer_size);
  void* p2=BuddyAllocator_malloc(&alloc, 12,buffer_size);
  //BuddyAllocator_free(&alloc, p2,buffer_size);
  void* p3=BuddyAllocator_malloc(&alloc, 100000,buffer_size);
  //BuddyAllocator_free(&alloc, p1,buffer_size);
  void* p4=BuddyAllocator_malloc(&alloc,58,buffer_size);
  void* p5=BuddyAllocator_malloc(&alloc,10,buffer_size);
  void* p6=BuddyAllocator_malloc(&alloc,750,buffer_size);
  void* p7=BuddyAllocator_malloc(&alloc,1020,buffer_size);
  BuddyAllocator_free(&alloc,p1,buffer_size);
  BuddyAllocator_free(&alloc,p2,buffer_size);
  BuddyAllocator_free(&alloc,p3,buffer_size);
  BuddyAllocator_free(&alloc,p4,buffer_size);
  BuddyAllocator_free(&alloc,p5,buffer_size);
  BuddyAllocator_free(&alloc,p6,buffer_size);
  BuddyAllocator_free(&alloc,p7,buffer_size);
}
