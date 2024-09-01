#include "used_mmap.h"


void free_mmap(void *ptr,int memoria){
   if(munmap(ptr,memoria)==-1){
      perror("munmap");
      exit(EXIT_FAILURE);
}
printf("Abbiamo liberato la memoria mappata\n");
 printf("======================================================================ZONA DI MEMORIA LIBERATA===============================================================================\n");
return ;
}

void init_mmap(int memoria) {
    // Usare mmap per allocare memoria
    printf("==========================================================================INIZIO MAPPING MEMORIA====================================================================================\n");
    void *ptr = mmap(NULL, memoria, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    // Verifica se la mappatura ha avuto successo
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    // Scrittura nella memoria allocata
    printf("Scrivi una parola o una frase con al massimo (%d caratteri):  ",memoria);
    fgets((char *)ptr, memoria, stdin);
    ((char *)ptr)[memoria - 1] = '\0';  // Imposta l'ultimo carattere a '\0'
    printf("Contenuto memoria: %s", (char *)ptr);
    printf("Indirizzo di memoria con il testo: %p\n", (char *)ptr);

 printf("===========================================================================FINE MAPPING MEMORIA===========================================================================\n");
    // Deallocare la memoria
    /*if(munmap(ptr,memoria)==-1){
      perror("munmap");
      exit(EXIT_FAILURE);
}*/
    free_mmap(ptr,memoria);
    return;
}

