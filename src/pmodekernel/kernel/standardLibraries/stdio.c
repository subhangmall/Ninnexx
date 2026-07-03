#include <kernel/logging.h>
#include <stdarg.h>
#include <kernel/processes/atlock.h>

char* convert(unsigned int, int);  
atlock printLock = ATLOCK_UNLOCKED;

void puts(char* str) {
    acquireLock(&printLock);
    kprint(str);
    kprint("\n");
    releaseLock(&printLock);
}

void printf(char* strPtr, ...) {
    va_list valist;
    va_start(valist, strPtr);

    acquireLock(&printLock);
    while (*strPtr != '\0') {
        if (*strPtr != '%') {
            kputc(*strPtr);
            strPtr++;
        } else {
            strPtr++;
            switch (*strPtr) {
                case 'c':
                    // single character
                    kputc((uint8_t) va_arg(valist, int));
                    strPtr++;
                    break;
                case 's':
                    // c string
                    kprint(va_arg(valist, char*));
                    strPtr++;
                    break;
                case 'd': // signed decimal integer (just go below); of type int
                    continue;
                case 'i': // signed decimal integer; of type int
                    kprint_hex(va_arg(valist, unsigned int));
                    strPtr++;
                    break;
                case 'u': // unsigned decimal integer; everything else is uint
                    strPtr++;
                    break;
                case 'x': // hex (lowercase)
                    kprint_hexl(va_arg(valist, unsigned int));
                    strPtr++;
                    break;
                case 'X': // hex (uppercase)
                    kprint_hex(va_arg(valist, unsigned int));
                    strPtr++;
                    break;
                case 'o': // octal
                    kprint_hex(va_arg(valist, unsigned int));
                    strPtr++;
                    break;
                case '\0': // just treat it as %% then end
                    kputc('%');
                    goto end;
                case '%': // %
                    kputc('%');
                    strPtr++;
                    break;
                strPtr++;
            }
        }
    }

    end: 
    releaseLock(&printLock);

    va_end(valist);
}

