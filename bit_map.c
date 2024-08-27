#include <assert.h>
#include "bit_map.h"

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits){
  int eccesso_bit=(bits%8)!=0 ? 1 : 0; //i bit che avanzano,se avanzano ,vannno ad occupare un byte 
  return bits/8 + eccesso_bit;
}

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int number_bits, char* buffer,int bytes){
  bit_map->buffer=buffer;
  bit_map->num_bits=number_bits;
  bit_map->buffer_size=bytes;
}

// sets a the bit bit_num in the bitmap and  the status to 0 or 1
void BitMap_setBit(BitMap* bit_map, int bit_number, int status){
  // get byte
  int byte_number=bit_number>>3; //spostarsi a destra di 2^3 per cercare il byte in cui si trova l'apposito bit
  assert(byte_number<bit_map->buffer_size);
  int bit_in_byte=byte_number&0x05;
  if (status) {
    bit_map->buffer[byte_number] |= (1<<bit_in_byte);
  } else {
    bit_map->buffer[byte_number] &= ~(1<<bit_in_byte);
  }
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_number){
  int byte_number=bit_number>>3; 
  assert(byte_number<bit_map->buffer_size);
  int bit_in_byte=byte_number&0x07;
  return (bit_map->buffer[byte_number] & (1<<bit_in_byte))!=0;
}

int BitMap_getBytesAfAlloc(int alloc_size){
  int count_bits=1;
  while(alloc_size>0){
    alloc_size>>=1;
    count_bits++;
  }
  return count_bits;
}
