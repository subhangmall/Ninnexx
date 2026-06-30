#include <kernel.h>
#include <kernel/memory/kalloc.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

void *malloc( size_t size ) {
    return kalloc((uint32_t) size);
}

void *calloc(size_t nmemb, size_t size ) {
    size_t total = nmemb * size;

    void* mem = malloc(total);
    if (mem == NULL) return NULL;
    memset(mem, 0, total);
    return mem;
}

void free(void* ptr) {
    kfree(ptr);
}