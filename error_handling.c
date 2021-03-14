#include <stdio.h>
#include <stdlib.h>

void *safe_malloc(size_t size) {
    void *ptr = (void *) malloc(size);

    if (ptr == NULL) {
        perror("malloc error");
        exit(1);
    }

    return ptr;
}

void *safe_realloc(void *ptr, size_t size) {
    void *new_ptr = (void *) realloc(ptr, size);

    if (new_ptr == NULL) {
        perror("realloc error");
        exit(1);
    }
    
    return new_ptr;
}