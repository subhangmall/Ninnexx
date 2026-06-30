#ifndef STRING_H
#define STRING_H

#include <stddef.h>


void* memset( void* dest, int ch, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
int strncmp(const char* lhs, const char* rhs, size_t count );
char *strncpy(char *dest, const char *src, size_t count);
size_t strlen(const char* start);

#endif