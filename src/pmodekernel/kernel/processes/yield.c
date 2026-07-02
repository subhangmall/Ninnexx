// INT 0XFF
#include <kernel/interrupts/intrStructs.h>
#include <kernel/processes/process.h>
#include <kernel/processes/contextSwitch.h>

extern struct Process* current;

void yield(struct InterruptStackFrame* isf) {
    contextSwitch(isf, current->next);
}