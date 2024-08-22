#include "used_mmap.h"

int used_mmap(int memoria) {
    // Dimensione della memoria da allocare (2KB)

    // Usare mmap per allocare memoria
    void *ptr = mmap(NULL, memoria, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    // Verifica se la mappatura ha avuto successo
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Scrittura nella memoria allocata
    strcpy(ptr, "Ciao, questa è una memoria allocata usando mmap!");

    // Stampa il contenuto della memoria
    printf("%s\n", (char *)ptr);
    printf("indirizzo di memoria con il testo: %p\n", (char *)ptr);

    // Deallocare la memoria
    if (munmap(ptr, memoria) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    return 0;
}
