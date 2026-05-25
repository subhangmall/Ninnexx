#include <stdint.h>
#include <kernel/interrupts/intrStructs.h>
#include <kernel/time/time.h>
#include <kernel/interrupts/pic.h>
extern volatile uint32_t ticks;

// #define PIC1_COMMAND	0x20
// #define PIC_EOI 0x20

void pitIntr(struct InterruptStackFrame* stack) {
    ticks++;
    // __asm__ volatile ( "outb %b0, %w1" : : "a"(PIC_EOI), "Nd"(PIC1_COMMAND) : "memory"); // raw rewrite of SendEOIToPIC() cause of some previous stack issues, no longer needed
    sendEOIToPIC(0);
    return;
}