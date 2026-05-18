#include <stdint.h>
#include "../logging.h"
#include "./idt.h"

void divByZeroException(uint32_t* stack) {
    kprint("Division by zero error! Halting the system!");
    asm volatile (
        "hlt"
        :
        :
        :
    );
}

void pageFault(uint32_t* stack) {
    kprint("Page fault! Halting the system!");
    asm volatile ("hlt");
}

void initInterruptHandlers() {
    setIDTHandler(0, (uint32_t) &divByZeroException);
    setIDTHandler(14, (uint32_t) &pageFault);
}