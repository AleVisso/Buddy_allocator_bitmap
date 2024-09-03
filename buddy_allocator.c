#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "buddy_allocator.h"


int levelIdx(size_t idx){
    return (int)floor(log2(idx+1));
};

int buddyIdx(int idx){ //fratello
  if (idx == 0)
    return 0; //0 non ha fatelli
  else if (idx&0x1)
    return idx + 1; //il fratello di 1 è 2 e viceversa
  return idx - 1;
}

int parentIdx(int idx){ //padre
    return (int)(idx-1)/2;  
}

//ritorna il primo indice del livello 
int firstIdx(int level){
  return (1 << level)-1; 
}


int startIdx(int idx){
    return (idx-(firstIdx(levelIdx(idx))));
}

int count_bits(int alloc_size,BitMap *bit_map){
    int bit_rim_to_print = 0;
    int level = -1; 
    int total = levelIdx(bit_map->num_bits) - 1; 
    int counter_bits=1;
    alloc_size*=2;
    for (int i = 0; i < bit_map->num_bits; i++){  
        if (bit_rim_to_print == 0){
            if(level==total){ 
              break;
            } 
            ++level;     
            for (int j = 0; j < (1 << total) - (1 << level); j++){   
              printf(" "); 
            }
            alloc_size/=2; 
            bit_rim_to_print = 1 << level; 
        }
        if (BitMap_bit(bit_map, i)==0){ 
          counter_bits++;
        }
        else{  
          counter_bits+=BitMap_getBytesAfAlloc(alloc_size);
        }
        bit_rim_to_print--; 
    }
    return counter_bits;
}

void Bitmap_print(BitMap *bit_map,int alloc_size){
    int bit_rim_to_print = 0;
    int level = -1; 
    int total = levelIdx(bit_map->num_bits) - 1;  
    alloc_size*=2;
    for (int i = 0; i < bit_map->num_bits; i++){  
        if (bit_rim_to_print == 0){ 
            if(level==total){ 
              break;
            } 
            printf("\n\033[93mLivello %d: \t\033[0m", ++level);     
            for (int j = 0; j < (1 << total) - (1 << level); j++){  
              printf(" "); 
            }
            alloc_size/=2; 
            bit_rim_to_print = 1 << level; 
        }
        if (BitMap_bit(bit_map, i)==0){ 
          printf("\033[32m%d\033[0m ", BitMap_bit(bit_map, i));
        }
        else{ 
          printf("\033[31m%d\033[0m ", BitMap_bit(bit_map, i)*alloc_size);
        }
        bit_rim_to_print--; 
    }
    printf("\n");
};

int BuddyAllocator_init(BuddyAllocator* alloc,
                         int number_levels,
                         char* alloc_buf,
                         int alloc_buf_size,
                         char* bitmap_buf, 
                         int bitmap_buf_size,
                         int min_bucket_size) {
    if (min_bucket_size < 8000) {
        printf("Minimum bucket troppo piccolo\n");
        return 0;
    }

    if(number_levels>Level_SuperMax){
        printf("NUMERO DI LIVELLI MAGGIORE DI 8, INIZIALIZZAZIONE BUDDY ALLOCATOR TERMINATA");
        return 0;
    }
    
    int num_bits = (1 << (number_levels + 1)) - 1 ;
  
    assert ("MEMORIA PER LA BITMAP INSUFFICIENTE!" && bitmap_buf_size>=BitMap_getBytes(num_bits));
    
    if (levelIdx(alloc_buf_size) != log2(alloc_buf_size)){
      printf("LA MEMORIA DEDICATA AL BUFFER, OVVERO %d, NON E' UNA POTENZA DI DUE. \nVERRANNO UTILIZZATI SOLO %d BYTES DEI %d FORNITI.\n", alloc_buf_size, min_bucket_size << number_levels, alloc_buf_size);
      alloc_buf_size = min_bucket_size << number_levels; 
    }
    alloc->number_levels=number_levels;
    alloc->buffer = alloc_buf;
    alloc->buffer_size = alloc_buf_size;
    alloc->min_bucket_size=min_bucket_size;

    printf("\n INIZIALIZZAZIONE BUDDY ALLOCATOR. . .\n\nINFO:\n");
    printf("Memoria gestita: %d bytes\n", alloc_buf_size); //   
    printf("Memoria per la BitMap: %d bytes usati di %d bytes forniti \n", BitMap_getBytes(num_bits), bitmap_buf_size);
    printf("Bit usati per la bitmap: %d\n", num_bits);
    printf("Numero di livelli: %d\n", number_levels);
    printf("Dimensione del minimum bucket: %d\n", min_bucket_size);
    printf("zona colorata di \033[32mverde\033[0m: \033[32mlibera\033[0m\t");
    printf("\tzona colorata di \033[31mrossa\033[0m: \033[31moccupata\033[0m\n"); 
    BitMap_init(&alloc->bitmap, num_bits, bitmap_buf,bitmap_buf_size); 
    printf("\n<-------------------------------Bitmap appena allocata-------------------------------->\n");
    Bitmap_print(&alloc->bitmap,alloc_buf_size); 
    printf("\n<------------------------------------------------------------------------------------->\n");
    return 1;
};

void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size,int alloc_size) {
    if (size==0){
      printf("NON POSSO ALLOCARE 0 BYTES! NON POSSIAMO USARE IL BUDDY_ALLOCATOR\n");
      return NULL;
    }
    printf("\nTENTO DI ALLOCARE %d BYTES + %ld BYTES DEDICATI ALL'INDICE (TOT. %ld) . . .\n", size, sizeof(int), size+sizeof(int));
    size += sizeof(int);
    if(size>PAGE_SIZE){
      size-=sizeof(int);
      printf("DIMENSIONE DELLA MEMORIA RICHIESTA SUPERIORE ALLA PAGE_SIZE\n");
      printf("UTILIZZIAMO MMAP PER ALLOCARE: %d\n", size);
      return NULL;
    }
    
    if ((alloc->buffer_size) < size) {
        printf("\nIl blocco da allocare con size %d è più grande di tutta la memoria disponibile! MEMORY FAULT.\n", size);
        return NULL;
    }

    int level_new_block;
    int start_size;

    
    if (size>(alloc->buffer_size)/2){ 
      level_new_block=0;
      start_size = alloc->buffer_size; 
    } 
    else if(size<=(alloc->buffer_size)/2 && size>(alloc->buffer_size)/4){ 
      level_new_block=1;
      start_size = (alloc->buffer_size)/2; 
    }
    else{   
      level_new_block = alloc->number_levels;
      start_size = alloc->min_bucket_size;
      for (int i = 0; i<=level_new_block; i++){
        if (start_size >= size){  
          break;
        }
        else{ 
          start_size *=2;  
          level_new_block--; 
        }
      }
    }
    

    
    printf("\nLIVELLO BLOCCO SCELTO: %d", level_new_block);

    int freeidx=-1; 
    int j;
    if ( level_new_block == 0){
      if (!BitMap_bit(&alloc->bitmap, firstIdx(level_new_block))){
        freeidx = 0;
      }
    }
    else{ 
      for(j = firstIdx(level_new_block); j < firstIdx(level_new_block+1); j++){
        if (!BitMap_bit(&alloc->bitmap, j)){
          freeidx=j;
          printf("\t\t\tINDICE DEL BUDDY LIBERO: %d", freeidx);
          break;
        }
      }  
    }

    
    if(freeidx==-1){  
      printf("\nNon ci sono blocchi liberi quindi abbiamo MEMORY_FAULT. \n");
      return NULL;
    }

    Set_status_parents(&alloc->bitmap, freeidx, 1);
    Set_status_children(&alloc->bitmap, freeidx, 1);
    int number_bits=count_bits(alloc_size,&alloc->bitmap);
    char *indirizzo = alloc->buffer + startIdx(freeidx) * start_size;
    ((int *)indirizzo)[0]=freeidx; 
    printf("\nAllocato un nuovo blocco di dimensione %d al livello %d utilizzando un blocco di dimensioni %d.\nIndice %d, puntatore \033[34m%p\033[0m\n", size, level_new_block, start_size, freeidx, indirizzo+sizeof(int));
    printf("Albero BITMAP dopo l'allocazione:\n");
    printf("Numero di bit utilizzati per la rappresentazione dell'albero:%d\n",number_bits);
    int num_bytes=BitMap_getBytes(number_bits);
    printf("Numero di byte utilizzati per la rappresentazione: %d    Numero di byte a disposizione dopo la bitmap:%d\n",num_bytes,alloc->bitmap.buffer_size-num_bytes);
    Bitmap_print(&alloc->bitmap,alloc_size);
    printf("================================================================FINE ALLOCAZIONE==================================================================");
    return (void *)(indirizzo + sizeof(int));
};

void BuddyAllocator_free(BuddyAllocator *alloc, void *mem,int alloc_size){
  
  if (mem==NULL){
    printf("Non posso liberare un blocco ancora non allocato!\n");
    return;
  }
   
  printf("\nLibero il blocco puntato da \033[34m%p\033[0m . . .\n", mem);

  int *p = (int *)mem;
  int idx_to_free = p[-1];

  printf("Indice da liberare %d\n", idx_to_free);
  int dim_lvl = alloc->min_bucket_size * (1 << (alloc->number_levels - levelIdx(idx_to_free)));
  char *p_to_free = alloc->buffer + startIdx(idx_to_free) * dim_lvl;
  assert("Puntatore non allineato" && (int *)p_to_free == &p[-1]); 
  assert("Double free" && BitMap_bit(&alloc->bitmap, idx_to_free));

  Set_status_children(&alloc->bitmap, idx_to_free, 0);
  merge(&alloc->bitmap, idx_to_free); 
  int number_bits=count_bits(alloc_size,&alloc->bitmap);
  printf("\nNumero di bit utilizzati per la rappresentazione dell'albero:%d\n",number_bits);
   int num_bytes=BitMap_getBytes(number_bits);
  printf("\nNumero di byte utilizzati per la rappresentazione: %d    Numero di byte a disposizione dopo la bitmap:%d\n",num_bytes,alloc->bitmap.buffer_size-num_bytes);
  printf("Bitmap dopo la free:");
  Bitmap_print(&alloc->bitmap,alloc_size);
  printf("============================================================================MEMORIA LIBERATA=============================================================================\n");
}


void Set_status_parents(BitMap *bit_map, int bit_num, int status){
  BitMap_setBit(bit_map, bit_num, status);
  if (bit_num != 0){
    Set_status_parents(bit_map, parentIdx(bit_num), status); //ricorsiva
  }
}

void Set_status_children(BitMap *bit_map, int bit_num, int status){
  if (bit_num < bit_map->num_bits){ 
    BitMap_setBit(bit_map, bit_num, status);
    Set_status_children(bit_map, bit_num * 2 + 1, status);  
    Set_status_children(bit_map, bit_num * 2 + 2, status); 
  }
}

void merge(BitMap *bitmap, int idx){
  assert("Non puoi fare il merge su un bit libero" && !BitMap_bit(bitmap, idx));
  if (idx == 0) return; 
  int idx_buddy = buddyIdx(idx);
  if (!BitMap_bit(bitmap, idx_buddy)){ 
    printf("Il buddy di %d, ovvero %d, è libero: MERGE\n", idx, idx_buddy);
    printf("Eseguo il merge dei buddy %d e %d al livello %d . . .\n", idx, idx_buddy, levelIdx(idx));
    int parent_idx = parentIdx(idx);
    BitMap_setBit(bitmap, parent_idx, 0); 
    merge(bitmap, parent_idx);
  }
  else{ 
    printf("Il buddy di %d, ovvero %d, NON è libero: NO MERGE\n", idx, idx_buddy);
  }
}

