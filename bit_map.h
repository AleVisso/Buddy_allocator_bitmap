#pragma once
#include <stdint.h>

// simple bit array
typedef struct  {
  char *buffer;  // externally allocated buffer
  int buffer_size;
  int num_bits; 
} BitMap;

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits);

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int num_bits, char* buffer);

// sets bit_number in the bitmap and 
void BitMap_setBit(BitMap* bit_map, int bit_number, int status);

//check what state bit_number is on
int BitMap_bit(const BitMap* bit_map, int bit_number);
//calculates bit numbers after allocating memory
int BitMap_getBytesAfAlloc(int alloc_size);
