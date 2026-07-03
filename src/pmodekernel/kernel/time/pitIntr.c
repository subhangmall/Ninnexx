#include <stdint.h>
#include <kernel/interrupts/intrStructs.h>
#include <kernel/time/time.h>
#include <kernel/interrupts/pic.h>
#include <kernel/processes/yield.h>
extern volatile uint32_t ticks;

// #define PIC1_COMMAND	0x20
// #define PIC_EOI 0x20

void pitIntr(struct InterruptStackFrame* stack) {
    ticks++;
    sendEOIToPIC(0);
    if (ticks % 100 == 0) {
        yield(stack);
    }

    return;
}