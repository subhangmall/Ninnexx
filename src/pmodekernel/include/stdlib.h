#include <stddef.h>

#ifndef STDLIB_H
#define STDLIB_H

void *malloc( size_t size );
void *calloc(size_t nmemb, size_t size );
void free(void* ptr);

#endif