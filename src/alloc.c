#ifndef FC_LIB_ALLOC
#define FC_LIB_ALLOC

#include <stdlib.h>
#include <errno.h>
#include <string.h>

void *allocate_memory(size_t len) {
    if (len == 0) return NULL;
    void *ret;
    if ((ret = malloc(len)) == NULL) {
        fprintf(stderr, "[ALLOC ERROR] %s", strerror(errno));
        exit(-1);
    }
    return ret;
}

void *reallocate_memory(void *ptr, size_t new_len) {
    if (new_len == 0) {
        free(ptr);
        return NULL;
    }
    void *ret;
    if ((ret = realloc(ptr, new_len)) == NULL) {
        fprintf(stderr, "[ALLOC ERROR] %s", strerror(errno));
        exit(-1);
    }
}

#endif
