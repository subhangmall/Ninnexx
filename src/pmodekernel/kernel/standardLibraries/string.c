#include <stdint.h>
#include <stddef.h>

void* memcpy(void* dest, const void* src, size_t count) {
    for (size_t i = 0; i < count; i++) {
        *(uint8_t*)(dest + i) = *(uint8_t*)(src + i);
    }
    return dest;
}

void* memset( void* dest, int ch, size_t count) {
    for (size_t i = 0; i < count; i++) {
        *(uint8_t*)(dest + i) = (uint8_t) ch;
    }
    return dest;
}

// MAKE THIS MORE SPECIFIC LATER
int strncmp(const char* lhs, const char* rhs, size_t count ) {
    for (size_t i = 0; i < count; i++) {
        if (lhs[i] == '\0' && rhs[i] == '\0')
            return 0;
        
        if (lhs[i] != rhs[i])
            return (unsigned char)lhs[i] - (unsigned char)rhs[i];
    }

    return 0;
}

char *strncpy(char *dest, const char *src, size_t count) {
    size_t i;
    for (i = 0; i < count; i++) {
        if (*(char*)(src+i) == '\0') break;
        *(char*)(dest + i) = *(char*)(src + i);
    }

    while (i < count) {
        *(char*)(dest + i++) = '\0';
    }

    return dest;
}

size_t strlen(const char* start)  {
    size_t i = 0;
    while (*(uint8_t*)(start + i++) != '\0') {}
    return i - 1;
}