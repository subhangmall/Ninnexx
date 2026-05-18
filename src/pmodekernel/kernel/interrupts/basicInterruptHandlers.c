#include <stdint.h>
#include "../logging.h"

void divByZeroException(uint32_t* stack) {
    kprint("Division by zero error! Halting the system!");
    asm volatile (
        "hlt"
        :
        :
        :
    );
}