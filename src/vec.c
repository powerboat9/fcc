#ifndef FC_LIB_VEC
#define FC_LIB_VEC

#include <stdlib.h>
#include <string.h>
#include "alloc.c"

struct vector {
    char *data;
    size_t len;
    size_t size;
};

void vector_free(struct vector *v) {
    free(v->data);
    v->data = NULL;
    v->len = 0;
    v->size = 0;
}

size_t _vector_get_next_size(size_t old_size) {
    if (old_size == 0) return 16;
    size_t new_size;
    if (__builtin_mul_overflow(old_size, 2, &new_size)) {
        fprintf(stderr, "[ALLOC OVERFLOW ERROR] %s", strerror(errno));
        exit(-1);
    }
    return new_size;
}

void vector_extend_uninit(struct vector *v, size_t len) {
    if (len == 0) return;
    if (v->size > v->len) {
        size_t easy_change = v->size - v->len;
        if (easy_change >= len) {
            v->len += len;
            return;
        }
        len -= easy_change;
    }
    size_t next_size = v->size;
    do {
        next_size = _vector_get_next_size(next_size);
    } while ((next_size - v->size) < len);
    v->data = reallocate_memory(v->data, next_size);
    v->len += len;
    v->size = next_size;
}

void vector_extend(struct vector *v, char *data, size_t len) {
    size_t old_len = v->len;
    vector_extend_uninit(v, len);
    memcpy(v->data + old_len, data, len);
}

void push_byte_to_vector(struct vector *v, char val) {
    vector_extend(v, &val, 1);
}

int vector_new(struct vector *v, size_t initial_size) {
    v->data = NULL;
    v->len = 0;
    v->size = 0;
    if (initial_size != 0) {
        vector_extend_uninit(v, initial_size);
        v->len = 0;
    }
}

int vector_pop_chunk(struct vector *v, char *out, size_t out_len) {
    if (out_len > v->len) return -1;
    v->len -= out_len;
    memcpy(out, v->data + v->len, out_len);
    return 0;
}

int vector_pop(struct vector *v) {
    if (v->len == 0) return -1;
    v->len--;
    return (int) ((unsigned char) v->data[v->len]);
}

#endif
