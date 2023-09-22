
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* mem_alloc(size_t size) {

    if(size == 0)
        size = 1;

    void* ptr = malloc(size);
    if(ptr == NULL) {
        fprintf(stderr, "MEMORY: Cannot allocate %lu bytes\n", size);
        exit(1);
    }

    memset(ptr, 0, size);
    return ptr;
}

void* mem_realloc(void* ptr, size_t size) {

    void* nptr = realloc(ptr, size);
    if(nptr == NULL) {
        fprintf(stderr, "MEMORY: Cannot re-allocate %lu bytes\n", size);
        exit(1);
    }

    return nptr;
}

void* mem_dup(void* ptr, size_t size) {

    void* nptr = mem_alloc(size);

    memcpy(nptr, ptr, size);
    return nptr;
}

char* mem_dup_str(const char* str) {

    return (char*)mem_dup((void*)str, strlen(str)+1);
}

void mem_free(void* ptr) {

    if(ptr != NULL)
        free(ptr);
}
