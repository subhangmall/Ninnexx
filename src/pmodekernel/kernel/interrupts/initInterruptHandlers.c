#include <stdint.h>
#include <kernel/logging.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/intrStructs.h>
#include <kernel/interrupts/syscall.h>
#include <kernel/memory/pageFault.h>
#include <kernel/processes/yield.h>
#include <kernel/time/time.h>
#include <kernel/dosvirt/int21h.h>
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

void initInterruptHandlers() {
    setIDTHandler(0, (uint32_t) &divByZeroException, false);
    setIDTHandler(14, (uint32_t) &pageFault, false);
    setIDTHandler(0xFF, (uint32_t) &yield, true);
    setIDTHandler(0x80, (uint32_t) &sysIntr, true);
    setIDTHandler(0x21, (uint32_t)&intr21, true);
    initTimeIntrHandler();
}