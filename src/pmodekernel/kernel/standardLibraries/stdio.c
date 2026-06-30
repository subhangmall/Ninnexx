#include <kernel/logging.h>
#include <stdarg.h>

char* convert(unsigned int, int);  

void puts(char* str) {
    kprint(str);
    kprint("\n");
}

// printf function from https://www.firmcodes.com/write-printf-function-c/
void printf(char* strPtr, uint32_t num, ...) {
    va_list valist;
    va_start(valist, num);

    while (*strPtr != '\0') {
        if (*strPtr != '%') {
            kputc(*strPtr);
            strPtr++;
            continue;
        }

        // whatever; make this actually not trash later
        if (*strPtr == '%') {
            strPtr++;
            switch (*strPtr) {
                case 'c':
                    // single character
                    kputc((uint8_t) va_arg(valist, int));
                    break;
                case 's':
                    // c string
                    kprint(va_arg(valist, char*));
                    break;
                case 'd': // signed decimal integer (just go below); of type int
                    continue;
                case 'i': // signed decimal integer; of type int
                    kprint_hex(va_arg(valist, unsigned int));
                    break;
                case 'u': // unsigned decimal integer; everything else is uint
                    break;
                case 'x': // hex (lowercase)
                    kprint_hexl(va_arg(valist, unsigned int));
                    break;
                case 'X': // hex (uppercase)
                    kprint_hex(va_arg(valist, unsigned int));
                    break;
                case 'o': // octal
                    kprint_hex(va_arg(valist, unsigned int));
                    break;
                case '\0': // just treat it as %% then end
                    kputc('%');
                    goto end;
                case '%': // %
                    kputc('%');
                    break;
                strPtr++;
            }

            end:
                break;
        }
    }

    va_end(valist);
}

