#include <stdint.h>
#include <kernel/logging.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/intrStructs.h>
#include <kernel/processes/yield.h>
#include <kernel/time/time.h>
#include <stdio.h>

void divByZeroException(struct InterruptStackFrame* stack) {
    printf("Division by zero error! Halting the system!\n");
    asm volatile (
        "hlt"
        :
        :
        :
    );
}

void pageFault(struct InterruptStackFrame* stack) {
    printf("Page fault! Halting the system!\n");
    asm volatile ("hlt");
}

void initInterruptHandlers() {
    setIDTHandler(0, (uint32_t) &divByZeroException);
    setIDTHandler(14, (uint32_t) &pageFault);
    setIDTHandler(0xFF, (uint32_t) &yield);
    initTimeIntrHandler();
}