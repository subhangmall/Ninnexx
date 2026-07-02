#include <kernel/processes/process.h>
#include <kernel/interrupts/intrStructs.h>

struct Process* current;

void contextSwitch(struct InterruptStackFrame* isf, struct Process* next) {
    current->kesp = (uint32_t) isf;
    current = next;
    *(uint32_t*)((uint32_t)tss + 4) = current->kernelStackTop;
    context_switch_noret();
}

